/*************************************************************************
	> File Name: xcp.c
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年06月12日 星期日 16时26分56秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>
#include <signal.h>
#include <stdarg.h>
#include <error.h>
#include <time.h>

#define _FILE_OFFSET_BITS 64
#define BOOL int
#define TRUE 1
#define FALSE 0
#define MAX_FMTSTR_LENGTH 2048
#define COPY_BUF_SIZE 4096
#define MAX_PATH_LENGTH(PATH_MAX + 1)
#define GBYTES(1024 * 1024 *1024)
#define KBYTES(1024 * 1024)
#define HOUR (60 * 60)
#define MINUTE 60
#define OPP_CONTINUE 0
#define OPP_SKIP 1
#define OPP_CANCEL 2

#define MSGT_PROMPT 0
#define MSGT_WARNING 1
#define MSGT_ERROR 2
#define MSGT_VERBOSE 3
#define MSGT_DEMO 4

typedef int (*each_opp_t)(const char*,const char*,const char*,const struct stat* st);
typedef void (*sig_handler_t)(int);

int sum_file = 0;
int sum_dir = 0;
long long sum_size = 0;
int copied_file = 0;
int copied_dir = 0;
long long copied_size = 0;
time_t copy_start_time = 0;
BOOL status_pause = FALSE;
BOOL opt_d = FALSE;
BOOL opt_f = FALSE;
BOOL opt_q = FALSE;
BOOL opt_r = FALSE;
BOOL opt_v = FALSE;

int action(const char *path_from,const char *path_to,const char *path_tree,const struct stat* st)
{
    int ret_val = OPP_CONTINUE;
    char path_from_full[MAX_PATH_LENGTH];
    char path_to_full[MAX_PATH_LENGTH];
    size_t rd,wr,swr;
    char buf[COPY_BUF_SIZE];
    FILE *src_file,*dest_file;
    BOOL over_write = FALSE;
    int cmd;
    BOOL skip = FALSE;
    struct stat st_dest;

    make_path(path_from_full,path_from,path_tree);
    make_path(path_to_full,path_to,path_tree);

    if(S_ISREG(st->st_mode))
    {
        if(opt_v)
        {
            printf_message(MSGT_VERBOSE,"cp \"%s\" -> \"%s\". \n",path_from_full,path_to_full);
        }

        if(strcmp(path_from_full,path_to_full) == 0)
        {
            ret_val = OPP_SKIP;
        }
    }
}

void usage()
{
    printf("xcp :\n");

    printf("description:cp with progress\n");  
    printf("synopsis: xcp [OPTIONS] src1 [src2 ... srcn] dest\n\n");  
    printf("[OPTIONS]\n");  
    printf("-r:recusive copy sub directories.\n");  
    printf("-f:force overwrite without prompt.\n");  
    printf("-q:quiet no warning/error message.\n");  
    printf("-d:demo,do not copy,output message only.\n");  
    printf("-v:verbos output.\n");  
    printf("-h:show usage message.\n");  
}

BOOL is_self_copy(const char* src,const char* dest)
{
    char c;
    char *sub = strstr(dest,src);

    if(sub)
    {
        c = sub[strlen(src)];
        return c=='\0' || c=='/' || src[strlen(src) -1]=='/';
    }
    else
    {
        return FALSE;
    }
}

int main(int argc,char *args[])
{
    int i = 0;
    char *path_from=NULL,*path_to=NULL,*file_name=NULL;
    char *path_to_fixed[MAX_PATH_LENGTH];
    struct stat st_src,st_dest;
    char human_readal_size[200];
    int opt;
    BOOL help = FALSE;

    while((opt=getopt(argc,args,"rfqdhv")) != -1)
    {
        switch(opt)
        {
            case 'r':
                opt_r = TRUE;
                break;
            case 'f':
                opt_r = TRUE;
                break;
            case 'q':
                opt_q = TRUE;
                break;
            case 'd':
                opt_d = TRUE;
                break;
            case 'h':
                help = TRUE;
                break;
            case 'v':
                opt_v = TRUE;
                break;
            case '?':
                printf("unknown option:%c\n",optopt);
                break;
            default:
                break;
        }
    }

    if(help || optind+2 > argc)
    {
        usage();
        return 1;
    }
    
    sum_file = 0;
    sum_dir = 0;
    sum_size = 0;
    
    time(copy_start_time);
    show_status(FALSE);
    install_timer(1,time_handler):
    
    for(i=optind;i<argc-1;++i)
    {
        path_from = args[i];
        path_to  = args[argc - 1];

        if(-1 == stat(path_from,&st_src))
        {
            printf_message(MSGT_ERROR,"\"%s\" doesn't exit.\n",path_from);
            continue;
        }
        if(S_ISREG(st_src.st_mode))
        {
            if((0 == stat(path_to,&st_dest)) && S_ISDIR(st_dest.st_mode))
            {
                file_name = strrchr(path_from,'/');
                path_to = make_path(path_to_fixed,path_to,file_name?file_name+1:path_from)
            }
        }else if(S_ISDIR(st_src.st_mode))
        {
            if(opt_r && is_self_copy(path_from,path_to))
            {
                printf_message(MSGT_ERROR,"can't xcp \"%s\" -> \"%s\"\n",path_from,path_to);
                continue;
            }
        }
        else
        {
            printf_message(MSGT_WARNING,"skip \"%s\" not a file nor a directory.\n",path_from);
            continue;
        }

        if(opt_d)
        {
            walk(path_from,path_to,NULL,demo);
        }
        else
        {
            walk(path_from,path_to,NULL,action);
        }
    }
    install_timer(0,NULL);
    show_status(TRUE);

    return 0;
}

