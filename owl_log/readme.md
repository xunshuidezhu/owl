## 如何使用owl日志
###  日志分为五个级别分别为debug, info, warn, err, fatal,以及查看堆栈信息bt_log
###　１．定义全局变量 owllog::Log* OWL,该变量声明在log.h中
###  2. 定义owllog::Log对象，构造函数参数为日志文件路径，并将该对象地址赋值给OWL
###  3. 开启log线程，log.start()
###  详情可查看日志测试代码