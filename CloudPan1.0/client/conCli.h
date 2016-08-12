/*************************************************************************
	> File Name: conCli.h
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月19日 星期二 22时50分11秒
 ************************************************************************/

#ifndef CONCLI_H
#define CONCLI_H
#include "../public.h"

class conCli
{
public:
    conCli(int port,const char *addr);
    ~conCli();
    void Run();
    void wellcome();
    void function();
    void do_work();
    void sendmsg();
    void getinfor();
private:
    int sock;
    //int trans_fd;
    //int check_fd;
    int port;
    const char *m_addr;
    //msgInfo msg;
    string msg;
    //list<string> msg;
    //list<string>::iterator it;
    struct sockaddr_in seraddr;
};

#endif
