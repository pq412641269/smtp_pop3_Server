#ifndef NET_H
#define NET_H

#include "comm.h"
#include "parsemail.h"
#include "decode64.h"
#include "mailrecv.h"

/*----------------------------------------
 *描述：完成smtp 锁步流程
 *参数：sockfd 套接字 pmail 邮件结构体 usertable 结构体存放用户名和密码
 *返回值：成功返回0 失败返回 1
----------------------------------------*/
int handleconnection(int sockfd, struct mail * pmail,table_t *usertable);

/*----------------------------------------
 *描述：完成pop 锁步流程
 *参数：cfd 套接字
 *返回值：成功返回0 失败返回 1
----------------------------------------*/
int pop3connection(int cfd);	//pop3锁步

#endif

