#ifndef DEVICE_H
#define DEVICE_H

#include "comm.h"
#include "mailrecv.h"
#include "net.h"

/*----------------------------------------
 *描述：base64 编码
 *参数：data:需要编码的数据
 *返回值：返回编码好的数据
----------------------------------------*/
int subjectcontrol(const struct mail * pmail,struct subject_ctl * subject);

/*----------------------------------------
 *描述：根据主题字完成设备控制或者文件更新
 *参数：pmail:邮件信息 subject:根据邮件信息解析传出来的控制信息
 *返回值：成功返回0 失败返回1
----------------------------------------*/
int emitupdate(const struct mail * pmail,struct subject_ctl * subject); //changtable

/*----------------------------------------
 *描述：更新数据库
 *参数：pmail:邮件信息 subject:控制信息
 *返回值：成功返回0 失败返回1
----------------------------------------*/
int squpdate(const struct mail * pmail,struct subject_ctl * subject); //changtable

/*----------------------------------------
 *描述：命令控制
 *参数： subject:控制信息
 *返回值：成功返回0 失败返回1
----------------------------------------*/
int emitcommand(struct subject_ctl * subject); 	//8LED 7SHU MOTO

/*----------------------------------------
 *描述：新建邮件文件名
 *参数：username:用户名 filenname:传出文件名
 *返回值：成功返回0 失败返回1
----------------------------------------*/
int getcreatemailname(const char *username,char *filename);	//控制完成后，要根据控制结果组装邮件要保存文件，此函数用于获取要创建的邮件文件名，规则:zhangsan_0.mail

/*----------------------------------------
 *描述：获取邮件文件名
 *参数：pmail:用户名 filenme:传出要发送的文件名
 *返回值：成功返回0 失败返回1
----------------------------------------*/
int getsendmailname(const char *username,char *filename);	//在pop3流程中，要发送控制结果邮件，而发送的邮件在文件中，先获得这封邮件的文件名zhangsan_0.mail,最后要删除这个文件

/*----------------------------------------
 *描述：根据控制结果组装邮件
 *参数：pmail:邮件信息 subject:控制信息 filename:传入要发送的文件名 table:用户名密码表格
 *返回值：成功返回0 失败返回1
----------------------------------------*/
int createmail(const struct mail *pmail,struct subject_ctl *subject,const char *filename,table_t table);	//控制完成后，要根据控制结果组装邮件,邮件格式按照mail.doc文件上邮件格式组装strcat(),sprintf()

/*----------------------------------------
 *描述：发送控制结果邮件
 *参数：cfd:传入用户套接字 pmail:邮件信息
 *返回值：成功返回0 失败返回1
----------------------------------------*/
int sendemail(int cfd, const char *pmail);

/*----------------------------------------
 *描述：获取系统时间
 *参数：
 *返回值：返回系统时间
----------------------------------------*/
char *getCurrtime();



#endif
