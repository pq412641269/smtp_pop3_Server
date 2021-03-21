#ifndef COMM_H
#define COMM_H

#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>
#include <sys/stat.h>            
#include <pthread.h>
#include <semaphore.h>
#include <sqlite3.h>

#define SYSERR(X,OPTION,Y,K,Z) if((X)OPTION(Y))\
                                {\
                                    printf("%s:%s:%d:%s\n",__FILE__,__func__,__LINE__,(K));\
                                    return Z;\
                                }
//安全检查宏

#define MAX_MAIL (8 * 1024)         //邮件大小
#define MAX_MAIL_NUM 20             //最大邮件数量
#define MAX_ATTACHMENT (7 * 1024)   //最大附件大小

#define USERFILE "table.txt"
#define SQLITE3 "table.db"


typedef struct mail
{
	char send [128];	        //发送者地址
	char recv[128];	            //接收者地址
	char subject[128];	        //主题字
 	char filename[128];	        //文件名
	char raw [MAX_MAIL];        //邮件内容
	char atta [MAX_ATTACHMENT]; //附件内容
	int len;		            //附件长度
}mail_t;

typedef struct subject_ctl
{
	char command[16];	//命令字	8LED 7SHU MOTO CHANGETABLE
	int bulb;	        //灯的号数(1~8)
	int signal;		    //七段数码管显示信号(0~99)
	int revo;   	    //电机转数(0,1000)
    int bulb_ctl;	    //控制信号（0关灯，1开灯）
	int result;         //0表示控制成功，－1表示控制失败
}ctrl_t;

typedef struct table
{
    int num;                    //存放邮件数量
    char username[10];          //邮件用户名
    char password[10];			//用户密码
}table_t;

#endif
