# Simple_Server
 使用线程池实现的简Web服务器，主线程当中使用epoll来监听所有的描述符，并将工作分给woker线程。
 服务器运行时需要将请求的根目录变换为主机上存在的某个文件夹下面。

