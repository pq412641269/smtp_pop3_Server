#include "devicecontrol.h"

int subjectcontrol(const struct mail * pmail,struct subject_ctl * subject)
{
    SYSERR(NULL,==,pmail,"mail == NULL",-1);
    SYSERR(NULL,==,subject,"subject == NULL",-1);
    
    if(!strcmp(subject->command,"8LED"))
    {
        emitcommand(subject);
    }
    else if(!strcmp(subject->command,"7SHU"))
    {
        emitcommand(subject);
    }
    else if(!strcmp(subject->command,"MOTO"))
    {
        emitcommand(subject);
    }
    else if(!strcmp(subject->command,"CHANGETABLE"))
    {
        if(0 == pmail->len)
        {
            printf("atta is void or no atta\n");
            subject->result = -2;      
            return -1;
        }
        squpdate(pmail,subject);
    }
    else
    {
        subject->result = -1;      
        printf("CMD Control error\n");
        return -1;
    }

    return 0;
}

int emitcommand(struct subject_ctl * subject)
{
    if(!strcmp(subject->command,"8LED"))
    {
        if(subject->bulb > 8 || subject->bulb < 1 || subject->bulb_ctl > 1 || subject->bulb_ctl < 0)
        {
            subject->result = -1;      
            printf("8LED Control error\n");
            return -1;
        }
        printf("8LED Control data: %s:%d %d\n",subject->command,subject->bulb,subject->bulb_ctl);
    }
    else if(!strcmp(subject->command,"7SHU"))
    {
        if(subject->signal < 0 || subject->signal > 99)
        {
            subject->result = -1;      
            printf("7SHU Control error\n");
            return -1;
        }
        printf("7SHU Control data: %s: %d\n",subject->command,subject->signal);
    }
    else if(!strcmp(subject->command,"MOTO"))
    {
        if(subject->revo >1000 || subject->revo < 0)
        {
            subject->result = -1;      
            printf("MOTO Control error\n");
            return -1;
        }
        printf("MOTO Control data: %s: %d\n",subject->command,subject->revo);
    }
    else
    {
        subject->result = -1;      
        printf("CMD Control error\n");
        return -1;
    }
    return 0;
}

int emitupdate(const struct mail * pmail,struct subject_ctl * subject)
{
    FILE *fp = fopen(USERFILE,"w");
    {
        if(NULL == fp)
        {
            printf("FILE open error! \n");
            subject->result = -2;      
            return -1;
        }
    }
    fprintf(fp,"%s",pmail->atta);
    fclose(fp);
    return 0;
}

int squpdate(const struct mail * pmail,struct subject_ctl * subject)
{
#if 1
    sqlite3 *db = NULL;
    if(sqlite3_open(SQLITE3,&db) != SQLITE_OK)
    {
        syslog(LOG_ERR,"open sqlite3 error %s %d\n",__func__,getpid());
        return -1;
    }

    const char *select_sql = "delete from usertable where num > -1";
    char *errmsg = NULL;
    if(sqlite3_exec(db,select_sql,NULL,NULL,&errmsg) != SQLITE_OK)
    {
        syslog(LOG_ERR,"sqlite3_exec error %s : %s : %d\n",errmsg,__func__,getpid());
        sqlite3_close(db);
        return -1;
    }
#endif

    int num = 0;
    table_t table_atta[100] = {0};
    char s[2] = " ";
    char c[2] = "\n";
    char buf[2048] = "";
    strncpy(buf,pmail->atta,strlen(pmail->atta));

    table_atta[num].num = num;
    char *str = strtok(buf,s);
    if(str == NULL)
    {
        syslog(LOG_ERR,"strtok error %s %d\n",__func__,getpid());
        sqlite3_close(db);
        return -1;
    }
    strncpy(table_atta[num].username,str,strlen(str));

    str = strtok(NULL,c);
    if(str == NULL)
    {
        syslog(LOG_ERR,"strtok error %s %d\n",__func__,getpid());
        sqlite3_close(db);
        return -1;
    }
    strncpy(table_atta[num].password,str,strlen(str)-1);

     while(1)                                                                                               
     {   
         num++;
         str = strtok(NULL,s);
         if(str == NULL)
         {   
             break;
         }
         strncpy(table_atta[num].username,str,strlen(str));
         
         table_atta[num].num = num;
         
         str = strtok(NULL,c);
         if(str == NULL)
         {   
             break;
         }
         strncpy(table_atta[num].password,str,strlen(str)-1);
     }
 
     int i;
     const char *insert_format = "insert into usertable(num,name,pass) values(%d,'%s','%s');";
     char insert_sql[128] = "";
     for(i = 0; i < num; i++)
     {
         sprintf(insert_sql,insert_format,table_atta[i].num,table_atta[i].username,table_atta[i].password);
         if(sqlite3_exec(db,insert_sql,NULL,NULL,&errmsg) != SQLITE_OK)
         {
            syslog(LOG_ERR,"sqlite3_exec error %s : %s : %d\n",errmsg,__func__,getpid());
            sqlite3_close(db);
            return -1;
         }
     }
     sqlite3_exec(db,"COMMIT;",NULL,NULL,&errmsg); 
     sqlite3_close(db);
     return 0;
 }

int getcreatemailname(const char *username,char *filename)
{
    if(NULL == username)
    {
        SYSERR(NULL,==,username,"username == NULL",-1);
        return -1;
    }

    int i;
    char buf[50] = "";
    for(i = 0;i < MAX_MAIL_NUM;i++)
    {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"%s_%d.mail",username,i);
        if(access(buf,F_OK))
        {
            strncpy(filename,buf,strlen(buf));
            break;
        }
    }
    return 0;
}

int getsendmailname(const char *username,char *filename)
{
    if(username == NULL || filename == NULL )    
    {
        perror("username or filename == NULL");
        return -1;
    }

    int i;
    char buf[50] = "";
    for(i = 0;i < MAX_MAIL_NUM;i++)
    {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"%s_%d.mail",username,i);
        if(!access(buf,F_OK))
        {
            strncpy(filename,buf,strlen(buf));
            return 0;
        }
    }
    return -1;
}

int createmail(const struct mail *pmail,struct subject_ctl *subject,const char *filename,table_t table)
{
    int fd = open(filename,O_RDWR | O_CREAT | O_TRUNC,0666); 
    if(fd < 0)
    {
        perror("open file error");
        return -1;
    }

    char str[MAX_MAIL] = "Message-ID: <001101c93cc4$1ed2ae30$5400a8c0@liuxiaoforever>\r\n";
    char buf[128] = "";    
    sprintf(buf,"From: \"%s\" <%s>\r\n",table.username,pmail->recv);
    strcat(str,buf);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"To: <%s>\r\n",pmail->send);
    strcat(str,buf);
    memset(buf,0,sizeof(buf));

    if(!strncmp(subject->command,"8LED",4))
    {
        if(subject->result < 0)
        {
            sprintf(buf,"Subject:%s %d %d Control failed! \r\n",subject->command,subject->bulb,subject->bulb_ctl);
        }
        else
        {
            sprintf(buf,"Subject:%s %d %d  Control Success! \r\n",subject->command,subject->bulb,subject->bulb_ctl);
        }
    }
    else if(!strncmp(subject->command,"7SHU",4))
    {
        if(subject->result < 0)
        {
            sprintf(buf,"Subject:%s %d Control failed! \r\n",subject->command,subject->signal);
        }
        else
        {
            sprintf(buf,"Subject:%s %d  Control Success! \r\n",subject->command,subject->signal);
        }
    }
    else if(!strncmp(subject->command,"MOTO",4))
    {
        if(subject->result < 0)
        {
            sprintf(buf,"Subject:%s %d  Control failed! \r\n",subject->command,subject->revo);
        }
        else
        {
            sprintf(buf,"Subject:%s %d  Control Success! \r\n",subject->command,subject->revo);
        }
    }
    else if(!strncmp(subject->command,"CHANGETABLE",11))
    {
        if(subject->result < 0)
        {
            sprintf(buf,"Subject:%s  Control failed! \r\n",subject->command);
        }
        else
        {
            sprintf(buf,"Subject:%s  Control Success! \r\n",subject->command);
        }
    }
    else
    {
        sprintf(buf,"Subject:cmd error  Control failed! \r\n");
    }

    strcat(str,buf);
    memset(buf,0,sizeof(buf));
    char *time = getCurrtime();
    sprintf(buf,"Date: %s",time);
    strcat(str,buf);

    memset(buf,0,sizeof(buf));
    sprintf(buf,"MIME-Version: 1.0\r\n");
    strcat(str,buf);

    memset(buf,0,sizeof(buf));
    sprintf(buf,"Content-Type: multipart/alternative; \r\n");
    strcat(str,buf);

    memset(buf,0,sizeof(buf));
    sprintf(buf,"X-Priority: 3..X-MSMail-Priority: Normal\r\n");
    strcat(str,buf);

    memset(buf,0,sizeof(buf));
    sprintf(buf,"X-Mailer: Microsoft Outlook Express 6.00.2900.3138\r\n");
    strcat(str,buf);

    memset(buf,0,sizeof(buf));
    sprintf(buf,"X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2900.3198\r\n");
    strcat(str,buf);

    memset(buf,0,sizeof(buf));
    sprintf(buf,"\r\n");
    strcat(str,buf);

    memset(buf,0,sizeof(buf));
    sprintf(buf,"This is response of %s's device!\r\n",subject->command);
    strcat(str,buf);

    if(write(fd,str,strlen(str)) == -1)
    {
        perror("FILE write error!");
        return -1;
    }
    close(fd);
    return 0;
}

char *getCurrtime()
{
    time_t tim = 0;
	time(&tim);	//1获取秒数
    char *time = ctime(&tim);
    return time;
}

int sendemail(int cfd, const char *pmail)
{
    SYSERR(NULL,==,pmail,"pmail == NULL",-1);
    int fd = open(pmail,O_RDONLY);
    SYSERR(-1,==,fd,"open error",-1);
    
    char buf[MAX_MAIL] = "";
    read(fd,buf,sizeof(buf) -1);
    if(write(cfd,buf,strlen(buf)) == -1)
    {
        perror("write mail error");
        close(fd);   
        return -1;
    }
    
    close(fd);   
    unlink(pmail);
    return 0;
}
