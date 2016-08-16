##这里包含一些编程过程中的记录，遇到的问题
....
```cpp
服务器端安全可靠的后台运行工具：
nohup ./ser > serLog.out 2>&1 &
nohup ./FTS > FTSLog.out 2>&1 &

服务器端输出重定向到：serLog.out 和 FTSLog.out 
使用 jobs 查看任务
使用 fg %n　关闭


//查找用户‘qqq’的文件列表
select user_id,filename,size,type from user,files where user.user_id='qqq' and user.file_id=files.id;
//查找用户‘qqq’的好友列表
select friends.user_id from user,friends where user.user_id='qqq' and user.friends_id=friends.id;

//注册用户
insert into user(user_id,password) select '用户名','密码' from dual where not exists(select *from user where user.user_id='用户名');

//编译mysql需要加上
-L/usr/lib/mysql -lmysqlclient

//编译mysql连接池时，使用的时libmysqlcppconn接口,需要加上
-L/usr/lib/mysql -lmysqlcppconn

//用于 用户登录 查询，存在返回记录条数，不存在返回0
select exists(select *from user where user_id='用户名' and password='密码');

//mysql编译
g++ -o MyDB MyDB.cpp MyDB.h public.h -L/usr/lib/mysql -lmysqlclient

//问题1：undefined reference to `conCli::do_work()'
//需将引用到的.cpp文件生成.a库文件
  g++ -c conCli.cpp   //.o
  ar -rc conCli.a conCli.o
//编译时加上生成的.a库文件

//客户端生成可执行文件
g++ -o Cli testcli.cpp conCli.a
//服务器端生成可执行文件
g++ -c conSer.cpp
ar -rc conCli.a conCli.o
g++ -o Ser testser.cpp conSer.a


//问题2： multiple definition of `msg'
 在所以全局变量前加static关键字或在public.h头文件声明extern关键字

//问题3：错误：no match 为‘operator<’在‘std::bind(_Functor, _ArgTypes ...) [with _Functor = int, _ArgTypes = sockaddr*, unsigned int](((sockaddr*)(&((conSer*)this)->conSer::seraddr)), 16u) < 0’中
 由于memcached的调用涉及到C++11标准（即在编译时需要加上--std=gnu++0x），所以Socket下的bind()函数和STL下的bind()函数发生冲突
解决方法：
	1.在使用socket下的bind()函数所在的类中禁止使用using namespace std;由于调试需要，这种办法不可取
	2.在使用socket下的bind()函数时，在bind()前加“::”运算符

memcached启动：
/usr/local/bin/memcached -m 10 -u root -l 127.0.0.1 -p 11211 -c 256 -vv
```

```cpp
"Segmentation Fault" 
  段错误是指访问的内存超出了系统给这个程序所设定的内存：
     1、访问不存在的内存地址
     2、访问系统保护的内存地址
     3、访问只读的内存地址
     4、栈溢出
   
 查看详细的段错误信息：
   1、dmesg: 通过dmesg命令可以查看发送段错误的程序名称、引起段错误发生的内存地址、指令指针地址、堆栈指针地址、错误代码、错误原因等
   2、gcc/g++编译程序源码加上-g参数，用于gdb调试，显示错误信息（但是多进程下gdb调试比较困难，我一般用cout在关键地方输出信息）
   3、nm  ./可执行程序：列出二进制文件中的符号表（符号地址、类型、名），可以帮助定位错误地方
   4、ldd ./可执行程序: 查看二进制程序的共享链接库依赖，包括库的名称、起始地址，可确定段错误发生的库（自己的库还是依赖的库） 
   5、objdump: 先用dmesg找出最近发生的段错误输出信息（段错误地址、指令指针地址）
               然后使用objdump生成二进制的相关信息：objdump -d ./可执行程序 > dumpfile
               dumpfile重定向文件包含可执行程序的汇编代码，从中查找dmesg输出的发生错误的地址，定位原程序
 注意事项 
    段错误主要出现在指针的使用地方，定义时初始化，使用时判 NULL，Glibc库中基本所有的函数默认形参指针为非空
    数组下标是否越界，数组元素是否存在
    是否访问了已经释放的空间
    printf/scanf的格式控制是否合理
```







