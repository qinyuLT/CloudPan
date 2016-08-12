/*************************************************************************
	> File Name: login.h
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月19日 星期二 22时55分11秒
 ************************************************************************/
#ifndef LOGIN_H
#define LOGIN_H

list<string> msg;
string user;
string passwd;

enum OP_TYPE{ENTER=1,REGISTER=2,UPLODE=3,DOWNLOAD=4,CHAT=5};

void wellcome()
{
    cout<<"===================================="<<endl;
    cout<<"=   Welcome To My Network Disk!    ="<<endl;
    cout<<"===================================="<<endl;
    cout<<"=                                  ="<<endl;
    cout<<"=       1.登录       2.注册        ="<<endl;
    cout<<"=                                  ="<<endl;
    cout<<"===================================="<<endl;
    cout<<"Your Choice: ";
}

void function()
{
    cout<<"===================================="<<endl;
    cout<<"=   Welcome To My Network Disk!    ="<<endl;
    cout<<"===================================="<<endl;
    cout<<"=                                  ="<<endl;
    cout<<"=    1.查看好友      2.个人信息    ="<<endl;
    cout<<"=                                  ="<<endl;
    cout<<"===================================="<<endl;
    cout<<"Your Choice: ";
}

void do_work()
{
    int select;
    
    wellcome();
    cin>>select;
    switch(select)
    {
    case ENTER:
        cout<<"用户名：";
        cin>>user;
        cout<<"密  码：";
        cin>>paaword;
        msg.push_back("ENTER");
        msg.push_back(user);
        msg.push_back(passwd);
        
        send();
        break;

    case REGISTER:
        cout<<"用户名：";
        cin>>user;
        cout<<"密  码：";
        cin>>paaword;
        msg.push_back("REGISTER");
        msg.push_back(user);
        msg.push_back(passwd);
        
        send();
        break;

    case CHAT:
        break;

    case UPLODE:
        break;
    
    case DOWNLOAD:
        break;

    default:
        break;
    }
}

#endif
