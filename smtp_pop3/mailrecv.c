#include "mailrecv.h"
#include "devicecontrol.h"
#include "net.h"
#include "comm.h"

sem_t pop_sem = {0};
sem_t smtp_sem = {0};

int getusername(int sockfd,struct table *p)
{
    char buf[128] = "";
    int len = read(sockfd,buf,sizeof(buf)-1);
    buf[len - 2] = 0;
	char *buf1 = base64_decode(buf);
    strncpy(p->username,buf1,strlen(buf1));
    return 0;
}

int getpassword(int sockfd,struct table *p)
{
    char buf[128] = "";
    int len = read(sockfd,buf,sizeof(buf)-1);
    buf[len - 2] = 0;
	char *buf1 = base64_decode(buf);
    strncpy(p->password,buf1,strlen(buf1));
    return 0;
}

int getfromaddress(int sockfd,struct mail *pmail) 
{
    char buf[128] = "";
    read(sockfd,buf,sizeof(buf)-1);
    char *start = strstr(buf,"<") +1;
    char *end = strstr(start,">");
    int len = end - start;
    strncpy(pmail->send,start,len); 
    return 0;
}

int gettoaddress(int sockfd,struct mail *pmail)
{
    char buf[128] = "";
    read(sockfd,buf,sizeof(buf)-1);
    char *start = strstr(buf,"<") +1;
    char *end = strstr(start,">");
    int len = end - start;
    strncpy(pmail->recv,start,len); 
    return 0;
}

int getbody(int sockfd,struct mail *pmail)
{
    read(sockfd,pmail->raw,sizeof(pmail->raw)-1);
    read(sockfd,pmail->raw,sizeof(pmail->raw)-1);
    getslave(sockfd,pmail);
    return 0;
}

int getslave(int sockfd,struct mail *pmail) 
{
    char *start = strstr(pmail->raw,"filename=");
    if(NULL == start)
    {
        return -1;
    }

    char *mid = strstr(start,"\r\n\r\n") + 4;
    char *end = strstr(mid,"-") - 2;
    int len = end - mid;
    strncpy(pmail->atta,mid,len); 
    pmail->len = len;
    return 0;
}

int getuser_pop(int sockfd,struct table *p)
{
    char buf[128] = "";
    read(sockfd,buf,sizeof(buf)-1);
    char *start = strstr(buf," ") + 1;
    strncpy(p->username,start,strlen(start) - 2); 
    return 0;
}

int getpass_pop(int sockfd, struct table *p)
{
    char buf[128] = "";
    read(sockfd,buf,sizeof(buf)-1);
    char *start = strstr(buf," ") + 1;
    strncpy(p->password,start,strlen(start) - 2); 
    return 0;
}

void *pop_pthread_fun(void *args)
{
    pthread_detach(pthread_self());
    int *p = (int *)args;
    int pop_cid = *p;
    extern sem_t pop_sem;
    sem_wait(&pop_sem);
    if(pop3connection(pop_cid))	
    {
        printf("pop3 error!");
        close(pop_cid);
        sem_post(&pop_sem);
        shutdown(pop_cid,SHUT_RDWR);
        pthread_exit(NULL);
    }
    sem_post(&pop_sem);
    shutdown(pop_cid,SHUT_RDWR);
    close(pop_cid);
    pthread_exit(NULL);
}

void *smtp_pthread_fun(void *args)
{
    pthread_detach(pthread_self());
    int *p = (int *)args;
    int smtp_cid = *p;
    mail_t *mail = (mail_t *)malloc(sizeof(mail_t));
    table_t usertable = {0};
    extern sem_t smtp_sem;
    sem_wait(&smtp_sem);
    if(handleconnection(smtp_cid,mail,&usertable))
    {
        free(mail);
        mail = NULL;
        sem_post(&smtp_sem);
        shutdown(smtp_cid,SHUT_RDWR);
        close(smtp_cid);
        pthread_exit(NULL);
    }
    sem_post(&smtp_sem);
    ctrl_t ctrl = {0};
    char filename[20] = "";
    parsemail(mail,&ctrl);
    subjectcontrol(mail,&ctrl);
    getcreatemailname(usertable.username,filename);	//控制完成后，要根据控制结果组装邮件要保存文件，此函数用于获取要创建的邮件文件名，规则:zhangsan_0.mail
    createmail(mail,&ctrl,filename,usertable);	//控制完成后，要根据控制结果组装邮件,邮件格式按照mail.doc文件上邮件格式组装strcat(),sprintf()

    free(mail);
    mail = NULL;
    close(smtp_cid);
    shutdown(smtp_cid,SHUT_RDWR);
    pthread_exit(NULL);
}

