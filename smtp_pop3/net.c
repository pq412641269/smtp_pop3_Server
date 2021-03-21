#include "comm.h"
#include "devicecontrol.h"
#include "net.h"

int handleconnection(int sockfd, struct mail * pmail,table_t *usertable)
{
    SYSERR(sockfd,<,2,"sockfd < 2",-1);
    SYSERR(pmail,==,NULL,"pmail == NULL",-1);

    char * response220 = "220 192.168.144.128 ok\r\n";
    char * response250_HELO = "250-192.168.144.128\r\n250-PIPELINING\r\n250-SIZE 52428800\r\n250-AUTH LOGIN PLAIN\r\n250-AUTH=LOGIN\r\n250-MAILCOMPRESS\r\n250 BITMIME\r\n";
    char * response334_user = "334 VXNlcm5hbWU6\r\n";
    char * response334_pass = "334 UGFzc3dvcmQ6\r\n";
    char * response235 = "235 Authenticatin successful\r\n";
    char * response250_ok = "250 OK\r\n";
    char * response_354 = "354 End data with <CR><LF>.<CR><LF>\r\n";
    char * response_221 = "221 Bye\r\n";

    write(sockfd,response220,strlen(response220));
    char buf[1024] = "";
    read(sockfd,buf,sizeof(buf) - 1);
    if(strncmp(buf,"HELO",4) && strncmp(buf,"EHLO",4))
    {
        perror("HELO or EHlO error");
        return -1;
    }

    write(sockfd,response250_HELO,strlen(response250_HELO));
    read(sockfd,buf,sizeof(buf) - 1);
    if(strncmp(buf,"AUTH LOGIN",10))
    {
        perror("AUTH LOGIN error");
        return -1;
    }

    write(sockfd,response334_user,strlen(response334_user));
    if(getusername(sockfd,usertable))
    {
        syslog(LOG_ERR,"getname error %s %d\n",__func__,getpid());
        perror("getusername error");
        return -1;
    }

    write(sockfd,response334_pass,strlen(response334_pass));
    if(getpassword(sockfd,usertable))
    {
        syslog(LOG_ERR,"getpass error %s %d\n",__func__,getpid());
        return -1;
    }

    if(checkusername(usertable))
    {
        syslog(LOG_ERR,"pass or name error %s %d\n",__func__,getpid());
        return -1;
    }

    write(sockfd,response235,strlen(response235));
    if(getfromaddress(sockfd,pmail))
    {
        perror("getfromaddress error");
        return -1;
    }

    write(sockfd,response250_ok,strlen(response250_ok));
    if(gettoaddress(sockfd,pmail))
    {
        perror("gettoaddress error");
        return -1;
    }

    write(sockfd,response250_ok,strlen(response250_ok));
    read(sockfd,buf,sizeof(buf)-1);
    if(strncmp(buf,"DATA",4))
    {
        perror("DATA error");
        return -1;
    }

    write(sockfd,response_354,strlen(response_354));
    if(getbody(sockfd,pmail))
    {
        perror("getboyd error");
        return -1;
    }
    
    write(sockfd,response250_ok,strlen(response250_ok));
    read(sockfd,buf,sizeof(buf)-1);
    if(strncasecmp(buf,"QUIT",4))
    {
        perror("QUIT error");
        return -1;
    }

    write(sockfd,response_221,strlen(response_221));
    return 0;

}
int pop3connection(int cfd)
{

    char * pop_cmd_1 = "+OK	pop3 server\r\n";
    char * pop_cmd_2 = "+OK\r\n";
    char * pop_cmd_3 = "+OK 1 300\r\n";
    char * pop_cmd_4 = "+OK 1 message\r\n1 300\r\n.\r\n";
    char * pop_cmd_5 = "+OK 120 octets\r\n";
    char * pop_cmd_6 = "\r\n.\r\n";

    char buf[1024] = "";
    write(cfd,pop_cmd_1,strlen(pop_cmd_1));
    table_t pop_table = {0};
    getuser_pop(cfd,&pop_table);
    write(cfd,pop_cmd_2,strlen(pop_cmd_2));
    getpass_pop(cfd,&pop_table);

    if(checkusername(&pop_table))
    {
        syslog(LOG_ERR,"pass or name error %s %d\n",__func__,getpid());
        return -1;
    }

    write(cfd,pop_cmd_2,strlen(pop_cmd_2));
    read(cfd,buf,sizeof(buf)-1);
    if(strncmp(buf,"STAT",4))
    {
        perror("STAT error");
        return -1;
    }

    write(cfd,pop_cmd_3,strlen(pop_cmd_3));
    read(cfd,buf,sizeof(buf)-1);
    if(strncmp(buf,"LIST",4))
    {
        perror("LIST error");
        return -1;
    }

    write(cfd,pop_cmd_4,strlen(pop_cmd_4));
    read(cfd,buf,sizeof(buf)-1);
    if(strncmp(buf,"RETR 1",6))
    {
        perror("RETR 1 error");
        return -1;
    }

    write(cfd,pop_cmd_5,strlen(pop_cmd_5));
    char file[20] = "";
    if(getsendmailname(pop_table.username,file) < 0)
    {
        perror("find fail");
        return -1;
    }
    sendemail(cfd,file);

    write(cfd,pop_cmd_6,strlen(pop_cmd_6));
    read(cfd,buf,sizeof(buf)-1);
    if(strncmp(buf,"DELE 1",6))
    {
        perror("DELE 1 error");
        return -1;
    }

    write(cfd,pop_cmd_2,strlen(pop_cmd_2));
    read(cfd,buf,sizeof(buf)-1);
    if(strncmp(buf,"QUIT",4))
    {
        perror("QUIT error");
        return -1;
    }

    write(cfd,pop_cmd_2,strlen(pop_cmd_2));
    return 0;
}
