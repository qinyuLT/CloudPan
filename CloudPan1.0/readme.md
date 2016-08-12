##这里包含一些变成过程中的记录，遇到的问题
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
解决方法：1.在使用socket下的bind()函数所在的类中禁止使用using namespace std;由于调试需要，这种办法不可取
	2.在使用socket下的bind()函数时，在bind()前加“::”运算符

memcached启动：
/usr/local/bin/memcached -m 10 -u root -l 127.0.0.1 -p 11211 -c 256 -vv
```








