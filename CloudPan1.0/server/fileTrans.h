/*************************************************************************
	> File Name: fileTrans.h
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月21日 星期三 12时54分19秒
 ************************************************************************/

#ifndef _FILETRANS_H
#define _FILETRANS_H

#include "../public.h"

class FileTrans
{
public:
    FileTrans(int port,const char*addr);
    ~FileTrans();
    void Run();
private:
    bool sendFile();
    bool recvFile();
    bool createFile();
    bool removeFile(string path);
    void sendFileInfo();
    void getFileInfo();
    void showFileList();
    void getCmd();

private:
    int trans_fd;
    int connfd;
    string user_id;
    string msg;
    list<string> sql;
    struct fileinfo m_fileinfo;
    struct filedata m_filedata;
    struct sockaddr_in seraddr,peeraddr;
};

#endif
