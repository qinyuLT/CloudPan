/*************************************************************************
	> File Name: conCli.cpp
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月19日 星期二 22时48分39秒
 ************************************************************************/
#include "md5.h"
#include "conCli.h"

conCli::conCli(int port,const char *addr)
{
    m_addr = addr;
    if((sock=socket(PF_INET,SOCK_STREAM,0)) < 0)
         ERR_EXIT("socket");
    memset(&seraddr,0,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(port);
    seraddr.sin_addr.s_addr = inet_addr(m_addr);//htonl(INADDR_ANY);
    //cout<<"conCli successful!"<<endl;
}

conCli::~conCli(){}

void conCli::Run()
{
     if(connect(sock,(struct sockaddr*)&seraddr,sizeof(seraddr)) < 0)
          ERR_EXIT("connect");
    while(1)
    {
       // wellcome();
        do_work();
    }
}

void conCli::wellcome()
{
    //system("clear");
    cout<<"===================================="<<endl;
    cout<<"=   Welcome To My Network Disk!    ="<<endl;
    cout<<"===================================="<<endl;
    cout<<"=                                  ="<<endl;
    cout<<"=       1.登录       2.注册       ="<<endl;
    cout<<"=                                  ="<<endl;
    cout<<"===================================="<<endl;
    cout<<"Your Choice >: ";
}

void conCli::function()
{
    system("clear");
    cout<<"===================================="<<endl;
    cout<<"=   Welcome To My Network Disk!    ="<<endl;
    cout<<"===================================="<<endl;
    cout<<"=                                  ="<<endl;
    cout<<"=    1.查看好友      2.个人信息   ="<<endl;
    cout<<"=                                  ="<<endl;
    cout<<"===================================="<<endl;
    cout<<"Your Choice >: ";
}

void conCli::sendmsg()
{
    while(1)
    {
        int res = send(sock,(msg.c_str()),msg.size()+1,0);
        //cout<<"res = "<<res<<endl;
        
        recv(sock,(void *)msg.c_str(),msg.size(),0);
        //cout<<"recv msg:"<<msg.c_str()<<endl;
        if(strncmp("1",msg.c_str(),1) == 0 ||
           strncmp("register success!",msg.c_str(),10) == 0)
            function();
        else if(strncmp("The user existed",msg.c_str(),17)==0)
        {
            cout<<msg.c_str()<<endl;
            do_work();
        }else
        {
            system("clear");
            cout<<"用户名或密码错误！"<<endl;
            //cout<<msg.c_str()<<endl;
            do_work();
        }
        while(1){}
    }
}

void conCli::getinfor()
{
    string user,passwd;
    
    cout<<"用户名：";
    cin>>user;
    msg += user;
    cout<<"密  码：";
    cin>>passwd;
    //msg = msg + " "+passwd;
    msg = msg + " " +MD5(passwd).toString();
    //cout<<"send msg = "<<msg<<endl;
    sendmsg();
}

void conCli::do_work()
{
    int select;
    system("clear");
    wellcome();
    cin>>select;
    
    switch(select)
    {
    case ENTER://登录
        msg = "1 ";
        getinfor();
        break;

    case REGISTER://注册
        msg = "2 ";
        getinfor();
        break;

    case CHART://聊天
        msg = "5 ";
        break;

    case UPLODE://上传文件
        msg = "3 ";
        break;
    
    case DOWNLOAD://下载文件
        msg = "4 ";
        break;

    default:
        msg = "-1 ";
        break;
    }
}

int main()
{
    conCli cli(CPT,"127.0.0.1");
    cli.Run();
    return 0;
}
