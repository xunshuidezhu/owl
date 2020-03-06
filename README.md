# OWL

OWL是一个基于Reactor模式的Linux C++网络服务器框架，基于one loop per thread，配合epoll LT，线程池加协程的并发模型框架

#### 压力测试

>服务器参数：
>CPU个数：4   内存：8GB 
>
>测试程序：
>echo pingpong服务，每次传递100字节内容，见目录test/

| TCP服务线程数 |  benchmark情况 |  QPS |
| :-----: | :-----: | :-----: |
|1线程|100个benchmark，建立1000个连接| `34482/s` |
|4线程|100个benchmark，建立1000个连接| `52631/s` |
|8线程|100个benchmark，建立1000个连接| `62500/s` |

### 如何开始
进入到owl目录下执行
```
make
```
会在lib目录下生成静态库，进入到test目录下执行
```
make
```
会生成pingpong服务端与客户端程序，执行开启服务端
```
./server.prog
```
客户端开启需要输入参数，包括ip, 端口，客户端数量，发送数据的大小

## 介绍

### IO Event: 基于epoll

一切IO事件设置为非阻塞，由Linux epoll进行管理，且TCP、UDP的可读、可写事件均以默认方式即水平模式注册

为什么不用EPOLLET边缘触发模式？
边缘触发模式需要每次read到EAGAIN

### Timer Event: Timer Queue设计

- 以最小堆管理Timers（注册Timer、删除Timer），以每个Timer的发生时间在最小堆中排序
- 以timerfd作为通知方式，交给eventLoop监听，将超时事件转为IO事件
- timerfd所设置的时间总是最小堆的堆顶Timer的发生时间

### EventLoop

IO Event与Timer Event全部在EventLoop中注册、监听、运行，一个EventLoop独占一个线程,
每个socketfd只在一个线程中消费，可避免多线程争用问题


### TCP服务器架构
#### 多线程Reactor＋协程

多线程TCP服务器采用了one loop per thread模型同时配合协程执行任务，一个eventloop只在一个线程中运行，线程池中的每个工作线程挂起在epollwait上，主线程作为Accepter角色，线程池作为实际连接操作者TCPConnection，主线程收到fd后，通过robin-bound方式分发给线程池中的线程，线程中的线程拥有消息队列其实现为一个阻塞队列，将fd send到工作线程，其中唤醒线程通过给eventfd写一个字节的数据，而将fd投放到消息队列通过push即可，消息队列与工作线程共用一个index用来绑定线程与其消息队列，其生命周期由threadpool掌管．线程池每个线程运行EventLoop维护一定量的连接，管理这些连接的IO Event与Timer Event，线程池中每个线程初始时监听自己队列的eventfd，便于与主线程通信．

当Accepter获取fd后，会注册此fd与可读事件的回调函数到主线程的event上，之后此fd有数据可读就可以将此fd按上文方式发给线程池中的一个工作线程，会将socketfd的数据读到缓冲区，客端注册任务回调函数通过消息分发器，一个id和func的哈希表，此id可自己注册，执行任务回调函数，这个回调函数的执行发生在一个协程上，协程的生命周期为一次任务毁掉函数的过程．

#### 协程模型
每个协程跑在线程池中的一个线程上，每个线程拥有一个协程调度器，协程调度器掌管保存的协程上下文，线程池中的每个线程每个时刻只有一个协程在工作，由于线程的分配是round-robind的，因此协程也是均衡分配的，协程的开销很小，即用即消．

关于跨协程的问题，golang的协程模型是m:n的，支持跨协程，也就是当某个线程跑了很多的协程而其他线程可能空闲时，能否steal协程，通过消息队列是可以将协程调度器发送给其他线程的，因此是可以实现这个技术的．


#### Accepter细节
Accepter收到新连接，以Round-Robin轮询方法将连接发送到一个线程的队列，以交给此线程管理这个连接

#### 粘包处理

tcp造成粘包的原因一般有两种，一种是由于nagle算法，将较小的内容拼接成大内容，一次性发送到服务端，也就是所谓的＂糊涂窗口综合征＂，另一种则是由于缓冲区过小，数据造成分片
应用层一般解决的方法有三种:

1. 发送固定长度的字节长度，比如服务端和客户端约定以100字节长度发送
2.　固定特殊字符结尾，比如http以 /r /n结尾
3.　包头封装序列号加这个包的长度，按照包头的长度拼包
OWL采用的是第三种方法，为要发送的数据包打上包头

#### 消息分发

当数据从socketfd读到缓冲区之后，用户注册的任务回调函数执行

#### 内存池管理
预先分配各种大小的缓冲区若干：
4K：200个，8KB：100个，16KB：50个，32KB：20个，64KB：10个，128KB：5个
##### 读缓冲区
先会利用`ioctl+FIONREAD`获取可读数据大小
如果有读缓冲区（说明上次粘包了，没归还），则看剩余缓冲区空间是否放得下新数据，能就放，不能就跟管理器要个新缓冲区替代了旧的，copy了旧缓冲区的数据后把旧的归还，把新可读数据放进来
如果没有读缓冲区（说明上次没粘包，归还了），则跟管理器要个缓冲区，把新可读数据放进来
当业务回调处理后发现缓冲区没更多数据了，归还；否则，将剩余数据移动到缓冲区头部，继续持有等待下次使用

##### 写缓冲区
用户调用sendData，如果有写缓冲区（说明上次每写完，不归还），则追加数据，如果放不下了，就跟管理器要个新缓冲区替代了旧的，copy了旧缓冲区的数据后把旧的归还，把新待发数据放进来
一次性把写缓冲区的数据发送给socket后，如果发送完成，则归还缓冲区；否则把剩余待发数据移动到缓冲区头部，继续持有等待下次使用

对于读缓冲区，当读完全部数据且被处理后，读缓冲区就可以释放了，除非有粘包发生
对于写缓冲区，当全部数据被发送到socket，写缓冲区就可以释放了，除非有数据没发完

### UDP服务器架构：单线程Reactor

由于UDP是无连接的，多线程操作同一个UDP socket效率未必很高，故选择了单线程模型

### 使用方法

server端:
```
void buz(const char* data, uint32_t len, int cmdid, net_commu* commu, void* usr_data)
{
    EchoString req, rsp;
    req.ParseFromArray(data, len); //解包，data[0:len)保证是一个完整包
    rsp.set_id(req.id());
    rsp.set_content(req.content());
    string rspStr;
    rsp.SerializeToString(&rspStr);
    commu->send_data(rspStr.c_str(), rspStr.size(), cmdid); //回复消息
}

int main()
{
    event_loop loop;

    config_reader::setPath("myconf.ini");
    string ip = config_reader::ins()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_reader::ins()->GetNumber("reactor", "port", 12315);

    tcp_server server(&loop, ip.c_str(), port); //创建TCP服务器
    server.add_msg_cb(1, buz); //设置：当收到消息id = 1的消息调用的回调函数
        //我们约定EchoString消息的ID是1
    loop.process_evs();
    return 0;
}
```
client端:
```
void echoBack(const char* data, uint32_t len, int msgId, net_commu* commu, void* usr_data)
{
    long* count = (long*)usr_data;
    echo::EchoString req, rsp;
    if (rsp.ParseFromArray(data, len) && rsp.content() == "I miss you i miss you i miss you i miss you i miss you i miss you i miss you i miss you i miss you!")
        *count = *count + 1;

    if (*count >= config.total) {
        endTs = getCurrentMills();
        printf("communicate %ld times\n", *count);
        printf("time use %ldms\n", endTs - startTs);
        printf("qps %.2f\n", (*count * 1000.0) / (endTs - startTs));
        exit(1);
    }
    req.set_id(rsp.id() + 1);
    req.set_content(rsp.content());
    std::string reqStr;
    req.SerializeToString(&reqStr);
    commu->send_data(reqStr.c_str(), reqStr.size(), 1); //回复消息
}

void onConnectionCb(tcp_client* client, void* args)
{
    unsigned long* startTsPtr = (unsigned long*)args;
    if (!*startTsPtr)
        *startTsPtr = getCurrentMills();
    //连接建立后，主动发送消息
    echo::EchoString req;
    req.set_id(100);
    req.set_content("I miss you i miss you i miss you i miss you i miss you i miss you i miss you i miss you i miss you!");
    std::string reqStr;
    req.SerializeToString(&reqStr);
    client->send_data(reqStr.c_str(), reqStr.size(), 1); //主动发送消息
}

int main(int argc, char** argv)
{
    parseOption(argc, argv);
    long done = 0;
    event_loop loop;
    std::vector<tcp_client*> clients;
    for (int i = 0; i < config.concurrency; ++i) {
        tcp_client* cli = new tcp_client(&loop, config.hostip, config.hostPort); //创建TCP客户端
        if (!cli)
            exit(1);
        cli->add_msg_cb(1, echoBack, &done); //设置：当收到消息id=GetRouteByAgentRspId的消息时的回调函数
        cli->onConnection(onConnectionCb, &startTs); //当连接建立后，执行函数onConnectionCb
        clients.push_back(cli);
    }

    loop.process_evs();

    for (int i = 0; i < config.concurrency; ++i) {
        tcp_client* cli = clients[i];
        delete cli;
    }
    return 0;
}
```
