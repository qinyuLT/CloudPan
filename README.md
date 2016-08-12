# CloudPan
##version 1.0
####系统架构图

![](https://github.com/qinyuLT/Images/blob/master/cloudpan.png)</br>

###负载均衡服务器的作用：
- 1.接收Cli端客户请求消息Request Msg，并作相应处理，回传Respone Msg;
- 2.负载均衡服务器负责客户的登录、注册功能;
- 3.负载均衡服务器管理后端的功能处理服务器（FileTransServer、ChartServer）,选择合适服务器处理业务

###FileTransServer：
- 负责文件处理相关的操作

###ChartServer
- 负责聊天相关的操作

##version 2.0
相对version1.0版本的改进是：使用[G6](http://www.oschina.net/p/G6)开源负载均衡器，并加入相应的进程池、连接池；</br>

该项目是在linux下采用C/C++编程，基于TCP协议，服务器端采用多进程长连接的socket的通信方式，并采用开源的负载均衡器G6作为客户端请求与服务端响应的中间件分发器，[G6](http://www.oschina.net/p/G6)是该开发者在[G5](http://www.oschina.net/p/g5)的基础上的升级版，相比G5多了心跳报告和命令管理的功能，所以对于服务器的状态就不需要关心了，包括服务器的组装，只需要修改G6的配置文件。考虑到服务器端如果每接收到一个请求fork一个进程处理方式的效率低，所以服务器端采用进程池方式，结合[libevent](http://libevent.org/)网络库，可以处理并发的TCP连接请求，同时对数据库的访问会带来同样的问题，所以实现了一个基于[mysqlclient](https://pypi.python.org/pypi/mysqlclient)接口的连接池。

###一共使用三个端口：[详情 public.h](https://github.com/qinyuLT/CloudPan/blob/master/CloudPan1.0/public.h)
```c++
#define CPT  6666    //管理端口
#define TPT  6667    //传输端口
#define MPT  6668    //聊天端口
```
###数据的处理
[memcached](http://php.net/manual/zh/book.memcached.php)的采用是为了减轻对mysql数据库的访问，比如在用户注册的时候，需要插入mysql中并在memcached中基于key-value的键值对用户名和密码进行存储，这样对于用户之后的登录只需要在memcached中查询，而不需要访问数据库，因memcached是基于内存的缓存系统，所以在这样查询效率就会很高，但是不好的一点就是如果系统宕机或者重启，数据就没有了；对于这种情况我的处理方式是，定期将memcached中的数据写入硬盘文件，每次重启导入该文件，这里的定期采用的是libevent中统一事件源中的定时器事件。

memchache和mysql对于文件的存储，只存储文件在磁盘上的地址。对于用户密码的安全性已经文件的一致性，采用了MD5算法，客户端发送的密码是经过MD5算法生成的32位随机字符串，对于文件，这32位字符串可以唯一标识，用它可以实现秒传功能，即便是相同内容的不同文件名也可以分辨出来。

断点续传功能的实现跟大文件的传输有关，对小文件一次传输不作记录；对于大文件的传输采用两个结构体：分别传输文件头部信息和文件数据内容
```c++
//文件头部信息
struct fileinfo
{
    string file_name;    //文件名
    string file_MD5;     //文件MD5值
    string file_path;    //文件存储路径
    int file_size;       //文件大小
    int file_chunk_size; //分块个数
    int chunk_size;      //分块标准
    bool trans_status;   //传输状态
};
//文件内容
struct filedata
{
    string file_name;   //文件名 
    int id;             //分块id
    int offset;         //该块在文件的偏移量
    bool trans_status;  //传输状态
};
```

###linux下常用的文件传输方式：
- FTP: ftp 主机名 /ip   常用命令：ls,cd,get,put,open</br>
    	 使用ftp脚本可以自动完成文件传输任务（堆积命令）</br>
         不保留文件的属性（文档的保护模式或者修改次数），不能递归的拷贝子目录</br>

- rcp:
    	 保留文件的属性(-p)，可以递归的拷贝子目录(-r),提供DES加密传输(-x)</br>

- scp: secure copy
     	 安全拷贝的缩写，使用ssh来传输数据，并使用与ssh相同的认证模式</br>
     	 scp命令用法和rcp命令用法非常类似，但是scp更安全</br>

- wget: 支持HTTP、HTTPS、FTP协议，支持代理服务器以及断点续传功能</br>
    	  可以实现递归下载</br>

- curl: 是对libcurl库的一个命令行工具包，功能很强大，80多个下载参数</br>
    	  支持FTP、FTPS、HTTP、HTTPS、TFTP、SFTP、TELNET等各种协议</br>
    	  支持代理、用户认证、FTP传输、HTTP post、SSL连接、文件续传等</br>

- rsync:高效的远程数据备份和镜像工具、可以快速同步多台主机间的文件</br>
    	  支持文件详细属性的拷贝（所有者、组信息、权限信息等）</br>
    	  通过远程shell(ssh/rsh)进程传输</br>
    	  流水线式文件传输模式，文件传输效率高</br>
    	  
        rsync算法能在相当短的时间内计算出需要备份的数据</br>
    	  只对源、目的文件的不同进行传输</br>
    	  从而降低网络中传输的数据量，以此达到快速备份镜像的目的</br>

