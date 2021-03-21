#ifndef PARSEMAIL_H
#define PARSEMAIL_H

#include "comm.h"
#include <stdio.h>
#include <string.h>
#include "decode64.h"

/*----------------------------------------
 *描述: 校验用户名
 *参数：username 用户名 usertable 结构体存放用户名和密码
 *返回值：成功返回0 失败返回 1
----------------------------------------*/
int verusername(const char *username,struct table *usertable);

/*----------------------------------------
 *描述: 校验密码
 *参数：username 用户名 usertable 结构体存放用户名和密码
 *返回值：成功返回0 失败返回 1
----------------------------------------*/
int verpassword(const char *password,struct table *usertable);

/*----------------------------------------
 *描述: 邮件解析
 *参数：pmail 邮件结构体  subject 控制信息结构体
 *返回值：成功返回0 失败返回 1
----------------------------------------*/
int parsemail(struct mail * pmail,struct subject_ctl * subject);

/*----------------------------------------
 *描述: 读取数据库信息校验用户名和密码
 *参数：usertable 结构体存放用户名和密码
 *返回值：成功返回0 失败返回 1
----------------------------------------*/
int checkusername(struct table *usertable);

#endif
