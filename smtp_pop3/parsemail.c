#include "parsemail.h"

int checkusername(struct table *usertable)
{ 
    sqlite3 *db = NULL;
    if(sqlite3_open(SQLITE3,&db) != SQLITE_OK)                             
    {
        syslog(LOG_ERR,"open sqlite3 error %s %d\n",__func__,getpid());
        return -1;
    }

    char *errmsg = NULL;
    const char *select_sql = "select * from usertable";
    char **resultp = NULL;
    int nrow = 0,ncolumn = 0;

    if(sqlite3_get_table(db,select_sql,&resultp,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
    {
        printf("open sqlite3_get_table error\n");
        sqlite3_close(db);
        return -1;
    }
    int i,j,index = ncolumn;
    for(i = 0; i < nrow; i++)
    {
        for(j = 0; j < ncolumn; j++)
        {
            if(!strcmp(resultp[j], "name") && !strcmp(resultp[index],usertable->username))
            {
                j++;
                index++;
                if(!strcmp(resultp[j],"pass") && !strcmp(resultp[index],usertable->password))
                {
                    sqlite3_free_table(resultp);
                    resultp = NULL;
                    sqlite3_close(db);
                    return 0;
                }
            }
        index++;
        }
    }
    sqlite3_free_table(resultp);
    resultp = NULL;
    sqlite3_close(db);
    return -1;
}


int verusername(const char *username,struct table *usertable)
{
    if(NULL == username || NULL == usertable) 
        return -1;

    FILE *fp = fopen(USERFILE,"r");
    if(NULL == fp)
    {
        syslog(LOG_ERR,"open error %s %d\n",__func__,getpid());
        perror("fopen error");
        return -1;
    }
    struct table temp = {0};

    while(fscanf(fp,"%s %s\n",temp.username,temp.password) != EOF)
    {
        if(!strcmp(username,temp.username))
        {
            memcpy(usertable,&temp,sizeof(temp));
            fclose(fp);
            return 0;
        }
    }
        syslog(LOG_ERR,"name error %s %d\n",__func__,getpid());
        perror("fopen error");
    fclose(fp);
    return -1;
}

int verpassword(const char * password,struct table *usertable)
{
    if(NULL == password || usertable == NULL)
        return -1;
    
    if(!strcmp(password,usertable->password))
        return 0;
    syslog(LOG_ERR,"password error %s %d\n",__func__,getpid());
    return -1;
}

int parsemail(struct mail * pmail,struct subject_ctl * subject)
{
    if(NULL == pmail || NULL == subject)
        return -1;

    const char *sub = "Subject:";
    char *start = strstr(pmail->raw,sub) + 9;
    if(NULL == start)
        return -1;
    char *end = strstr(start,"\r\n") - 1;
    if(NULL == end)
        return -1;
    int len = end - start + 1;

    char buf[128] = "";
    strncpy(buf,start,len);
    const char *command = strtok(buf," ");
    if(NULL == command)
        return -1;

    if(!strcmp(command,"8LED"))
    {
        strcpy(subject->command,command);
        const char *bulb = strtok(NULL," ");
        subject->bulb = atoi(bulb);
        const char *bulb_ctrl = strtok(NULL," ");
        subject->bulb_ctl = atoi(bulb_ctrl);
    }
    else if(!strcmp(command,"7SHU"))
    {
        strcpy(subject->command,command);
        const char *num = strtok(NULL," ");
        subject->signal = atoi(num);
    }
    else if(!strcmp(command,"MOTO"))
    {
        strcpy(subject->command,command);
        const char *revo = strtok(NULL," ");
        subject->revo = atoi(revo);
    }
    else if(!strcmp(command,"CHANGETABLE"))
        strcpy(subject->command,command);
    else
        return -1;

    return 0;
}

