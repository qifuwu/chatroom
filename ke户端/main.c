#include "myhead.h" 

//普通用户读线程
void* read_msg(void*arg)
{
    int i;
    int ret;
    struct send_msg s_msg;
    while(1)
    {
        bzero(&s_msg, sizeof(s_msg) );//清空                    
        ret = read(sockfd,&s_msg,sizeof(s_msg));       
        if (ret != 0)      //接收信息
        {
            if(s_msg.cmd == CMD_SHOW_FRIEND)                  //1:查看在线好友
            {
                printf("在线好友ID:\n");
                for(i = 0;i<10;i++)
                {
                    printf("|\t%s\t|\n",s_msg.friend[i].id);
                }
                printf("以上是在线好友\n");
                
            }
            else if(s_msg.cmd == CMD_USER_CHAT_SUCCESS)                  //2:私聊
            {
                printf("来自%s(%s)私聊信息:\"%s\"\n",s_msg.from_name,s_msg.from,s_msg.massage);
            }
            else if(s_msg.cmd == CMD_USER_CHAT_FAIL)
            {
                printf("对方不在线\n");
            }
            else if(s_msg.cmd == CMD_USER_CHATALL)                      //3:群聊接收
            {
                printf("来自%s(%s)群聊信息:\"%s\"\n",s_msg.from_name,s_msg.from,s_msg.massage);
            }
            else if(s_msg.cmd == CMD_USER_CHATALLERR)                      //3:群聊接收
            {
                printf("%s\n",s_msg.massage);
            }
            else if(s_msg.cmd == CMD_USER_CHATALLERRMINGAN)                      //3:群聊接收
            {
                printf("%s\n",s_msg.massage);
            }
            else if(s_msg.cmd == CMD_LOOK_RECORD_ALL)                      //4:群聊记录接收
            {
                printf("%s\n",s_msg.record);
            }
            else if(s_msg.cmd == CMD_LOOK_RECORD_ONE)                      //4:私聊记录接收
            {
                printf("%s\n",s_msg.record);
            }
            else if(s_msg.cmd == CMD_JINYAN)                      //5:禁言
            {
                printf("%s已被禁言!\n",s_msg.id);
            }
            else if(s_msg.cmd == CMD_JINYANERR)                      //5:禁言
            {
                printf("禁言失败,id非法!\n");
            }
            else if(s_msg.cmd == CMD_JIEJIN)                      //6:解禁
            {
                printf("%s已成功解禁!\n",s_msg.id);
            }
            else if(s_msg.cmd == CMD_JIEJINERR)                      //6:解禁失败
            {
                printf("解禁失败,id非法!\n");
            }
            else if(s_msg.cmd == CMD_DELETE_ACCOUNT)                           //10:注销
            {
                sleep(1);
                sem_post(&sem);
                return NULL;
            }
            else if(s_msg.cmd == CMD_OUTLINE)                           //11:下线
            {
                
                printf("下线成功\n");
                return NULL;
            }
            else if(s_msg.cmd == CMD_HEART_CHECK)                       //心跳检测
            {
                s_msg.cmd = CMD_HEART_CHECK;
                write(sockfd,&s_msg,sizeof(s_msg));
            }
        }
        else
        {
            break;
        }
    }
}

//root用户读线程,多此一举可以不用的
void* read2_msg(void*arg)
{
    int i;
    int ret;
    struct send_msg s_msg;
    while(1)
    {
        bzero(&s_msg, sizeof(s_msg) );//清空                    
        ret = read(sockfd,&s_msg,sizeof(s_msg));       
        if (ret != 0)      //接收信息
        {
            if(s_msg.cmd == CMD_SHOW_FRIEND)                  //1:查看在线好友
            {
                printf("在线好友ID:\n");
                for(i = 0;i<10;i++)
                {
                    printf("|\t%s\t|\n",s_msg.friend[i].id);
                }
                printf("以上是在线好友\n");
                
            }
            else if(s_msg.cmd == CMD_USER_CHAT_SUCCESS)                  //2:私聊
            {
                printf("来自%s私聊信息:\"%s\"\n",s_msg.id,s_msg.massage);
            }
            else if(s_msg.cmd == CMD_USER_CHAT_FAIL)
            {
                printf("对方不在线\n");
            }
            else if(s_msg.cmd == CMD_USER_CHATALL)                      //3:群聊接收
            {
                printf("来自%s群聊信息:\"%s\"\n",s_msg.from,s_msg.massage);
            }
            else if(s_msg.cmd == CMD_OUTLINE)                           //11:下线
            {
                
                printf("下线成功\n");
                return NULL;
            }
            else if(s_msg.cmd == CMD_HEART_CHECK)                       //心跳检测
            {
                s_msg.cmd = CMD_HEART_CHECK;
                write(sockfd,&s_msg,sizeof(s_msg));
            }
        }
        else
        {
            break;
        }
    }
}

int main()
{
    /*局部变量*/
    pthread_t tid;    //读线程id
    pthread_t root_tid;
    
    int ret = 0;    //connect返回值
    struct sockaddr_in s_addr;      /* 定义服务器的结构体 */
        
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(PORT);  //端口转换为大端
        s_addr.sin_addr.s_addr = inet_addr(SERVERADDR); //地址转换，自动转化为网络字节序
    socklen_t s_addrlen = sizeof(struct sockaddr_in);
    pthread_mutex_init(&mutex,NULL);//初始化锁
    pthread_cond_init(&cond,NULL);
    sem_init(&sem,0,0);//第一个0是本进程中所有线程，第二个0是初始未或得信号量
/*********************以上为tcp模板，靠sockfd和服务器通信**********************************/
    
            /*建立socket通信连接*/
        if ( ( sockfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1 )
        {
            perror("socket");
            exit(-1);
        }
            /*设置端口复用*/
        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        /*connect连接到服务器*/
        ret = connect(sockfd, (struct sockaddr*)&s_addr, s_addrlen);
        if(ret == -1)    // zero is returned.  On error, -1
        {
            perror("connect");
            exit(-1);
        }

        printf("\033c连接服务器成功\n");
    while(1)
    {
        ret = 0;
        ret = login();
        if(ret == 1)//普通用户
        {
            
            pthread_create(&tid,NULL,(void*)read_msg,NULL);
            pthread_detach(tid);
            int val = 0;
            val = user();
            if(val == 11)//我一登录成功啥都不做返回登录，
            {
                struct send_msg msg;
                bzero(&msg,sizeof(msg));
                msg.cmd = CMD_OUTLINE;
                printf("正在下线...\n");
                sleep(1);
                write(sockfd,&msg,sizeof(msg));
                sleep(1);
                //返回登录
            }
            else if(val == 12)//我一登录成功啥都不做返回登录，
            {
                //等到注销成功跳出user函数就能返回到主界面了
            }
        }
        else if(ret == 2)//root用户
        {
            pthread_create(&tid,NULL,(void*)read_msg,NULL);
            pthread_detach(tid);
            int val = 0;
            val = manager();
            if(val == 11)//我一登录成功啥都不做返回登录，
            {
                struct send_msg msg;
                bzero(&msg,sizeof(msg));
                msg.cmd = CMD_OUTLINE;
                printf("正在下线...\n");
                sleep(1);
                write(sockfd,&msg,sizeof(msg));
                sleep(1);
                //返回登录
            }
        }
        if (ret == 3)//全部退出，close(fd)写在login()中
        {
            printf("\033c");
            printf("byebye\n");
                            //sleep(100);
            return 0;
        }
    }
    
    return 0;
}