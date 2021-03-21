#include "comm.h"
#include "parsemail.h"
#include "devicecontrol.h"

int main()
{
    pid_t pid = fork();
    if(pid < 0)
    {
        return -1;
    }
    else if(pid > 0)
    {
        exit(0);
    }
    else if(0 == pid)
    {
            pid_t sid_t = setsid();
            if(sid_t < 0)
            {
                syslog(LOG_ERR,"tcp server setsid error %d\n",getpid());
                return -1;
            }
            
            if(chdir("/home/pengqi/hqyj/sys/smtp_pop3") < 0)
            {
                syslog(LOG_ERR,"tcp server chdir error %d\n",getpid());
                return -1;
            }
            if(umask(0022) < 0)
            {
                return -1;
            }
            int newfd = open("/dev/null",O_RDWR);
            if(newfd < 0)
            {
                return -1;
            }
            
            dup2(newfd,STDIN_FILENO);
            dup2(newfd,STDOUT_FILENO);
            dup2(newfd,STDERR_FILENO);

            pid_t stmp_pid = fork();
            if(stmp_pid < 0)
            {
                return -1;
            }
            else if(0 == stmp_pid)
            {

            int sid = socket(AF_INET,SOCK_STREAM,0);
            if(sid < 0)
            {
                perror("socket error");
                return -1;
            }
            
            int optval = 1; 
            setsockopt(sid,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
            struct sockaddr_in saddr = {0};
            saddr.sin_family = AF_INET;
            saddr.sin_port = htons(10000);
            saddr.sin_addr.s_addr = inet_addr("192.168.144.128");

            if(bind(sid,(struct sockaddr *)&saddr,sizeof(saddr)))
            {
                perror("bind error");
                close(sid);
                return -1;
            }

            if(listen(sid,10))
            {
                perror("listen error");
                close(sid);
                return -1;
            }

            pthread_t smtp_tid = -1; 
            extern sem_t smtp_sem;
            int cid = -1;
            sem_init(&smtp_sem,0,1);
            while(1)
            {
                cid = accept(sid,NULL,NULL);
                if(cid < 0)
                {
                    syslog(LOG_ERR,"smtp_cid error %s %d\n",__func__,getpid());
                }
                pthread_create(&smtp_tid,NULL,smtp_pthread_fun,&cid);

            }
            sem_destroy(&smtp_sem);
            close(sid);
        }
        else if(stmp_pid > 0)
        {
            pid_t pop_pid = fork();
            if(pop_pid < 0) 
            {
                return -1;
            }
            else if(pop_pid == 0)
            {
         
                int pop_sid = socket(AF_INET,SOCK_STREAM,0);
                if(pop_sid < 0)
                {
                    perror("socket error");
                    return -1;
                }
                
                int optval_1 = 1; 
                setsockopt(pop_sid,SOL_SOCKET,SO_REUSEADDR,&optval_1,sizeof(optval_1));
                struct sockaddr_in saddr1 = {0};
                saddr1.sin_family = AF_INET;
                saddr1.sin_port = htons(10001);
                saddr1.sin_addr.s_addr = inet_addr("192.168.144.128");

                if(bind(pop_sid,(struct sockaddr *)&saddr1,sizeof(saddr1)))
                {
                    perror("bind error");
                    close(pop_sid);
                    return -1;
                }

                if(listen(pop_sid,10))
                {
                    perror("listen error");
                    close(pop_sid);
                    return -1;
                }
                                
                pthread_t pop_tid = -1; 
                int pop_cid = -1;
                extern sem_t pop_sem;
                sem_init(&pop_sem,0,1);
                while(1)
                {
                    pop_cid = accept(pop_sid,NULL,NULL);
                    if(pop_cid < 0)
                    {
                        syslog(LOG_ERR,"pop_cid error %s %d\n",__func__,getpid());
                    }
                    pthread_create(&pop_tid,NULL,pop_pthread_fun,&pop_cid);
                }
                sem_destroy(&pop_sem);
                close(pop_sid);
            }
        }
    }
          return 0;
}


