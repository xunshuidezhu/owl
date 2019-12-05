## owl
### 博客地址：https://blog.csdn.net/weixin_41783335/article/details/103405964
### owl is a c++ net framework 
### owl是一个c++网络库，基于事件驱动
### 支持　http 模块，封装了更轻量的协程以代替多线程并发
### 完成reactor部分代码,为网络库核心部分，分为reactor, eventhandler, demultiplexer三个部分
### 完成日志系统，日志会定时（定次数）刷新缓冲区落盘
### 增加http模块
### 增加小跟堆计时器任务，利用gettimeofday计时，利用epoll_wait定时
### 增加协程库，用来并发读取event事件（暂不支持遇到io切换cpu，暂时改用线程池方案，更好的基于协程的异步io可参考libgo, libco, go的goroutine,python的gevent，我封装的协程仅支持保存协程堆栈切换上下文）
### 增加线程池，基于生产者消费者模型与任务队列，用来处理事件
