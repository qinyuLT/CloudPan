/*************************************************************************
	> File Name: public.h
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月19日 星期二 22时30分55秒
 ************************************************************************/
#ifndef _PUBLIC_H
#define _PUBLIC_H

#include <string.h>
#include <string>
#include <list>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <sstream>//stringstream
#include <iostream>
#include <signal.h>
#include <errno.h>

using namespace std;

#define CPT   6666    //管理端口
#define TPT   6667    //传输端口
#define MPT   6668    //聊天端口
#define MCA   11211   //memcached端口

#define SERADDR "127.0.0.1"
#define DBUSER  "root"
#define DBPSSW  "root"
#define DBNAME  "CloudPan"

#define SER_HOME_PATH "/home/yourname/CloudPan/serdir" //服务端文件存储路径
#define CLI_HOME_PATH "/home/yourname/CloudPan/clidir" //客户端文件存储路径

#define BUFFSIZE 128

#define ERR_EXIT(m) \
        do \
        { \
           perror(m); \
           exit(EXIT_FAILURE); \
        }while(0)

#define EPOLL_SIZE 50

//用户操作类型
enum OP_TYPE{ERR=-1,ENTER=1,REGISTER=2,UPLODE=3,DOWNLOAD=4,CHART=5,RM=6,LS=7};

//用户信息
typedef struct Msg
{
    OP_TYPE type;
    string user_id;
    string password; 
}msgInfo;

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
//文件信息
struct filedata
{
    string file_name;   //文件名 
    int id;             //分块id
    int offset;         //该块在文件的偏移量
    bool trans_status;  //传输状态
};

#endif
