/*************************************************************************
	> File Name: conSer.h
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月19日 星期二 22时51分03秒
 ************************************************************************/

#ifndef CONSER_H
#define CONSER_H
#include "../public.h"
#include "MyDB.h"
#include "sqlConnPool.h"

class conSer
{
public:

    conSer(int port,const char *addr);
    ~conSer();
    void Socket();
    void Bind();
    void Listen();
    int accept_timeout(unsigned int wait_seconds);//select
    void epoll_work();                            //epoll
    void Run();

private:
    void selectFTServer();
    //selectCServer();
    void do_service(int conn);
    void get_cmd();

private:
    int listenfd;
    int port;
    char *addr;
    list<string> sql;
    string msg;
    //msgInfo msg;
    struct sockaddr_in seraddr,peeraddr;
    //MyDB *db;
};

#endif
