#include"myhead.h"

struct Node
{
    int cfd;
    char id[15];
    int flag;//是否被禁言,被禁言就置为1.
    time_t last_second;//该客户端最后一次操作时间
    struct Node * next;
};

int init(struct Node **head)
{
    struct Node *newnode = (struct Node*)malloc(sizeof(struct Node));
    if(newnode == NULL)
    {
        return 0;
    }
    newnode->cfd = 0;
    strcpy(newnode->id,"0");
    newnode->next = NULL;
    *head = newnode;
}

struct Node* freeall(struct Node*head)
{
    while (head->next != NULL)
    {
        struct Node*ptr = head;
        head = head->next;
        free(ptr);
    }
    free(head);
    head = NULL;
    return head;
}

int print(struct Node*head)
{
    if(head->next ==NULL)
    {
        return 0;
    }
    head = head->next;
    while(head->next!=NULL)
    {
        printf("%d ",head->cfd);
        printf("%s\n",head->id);
        head = head->next;
    }
    printf("%d ",head->cfd);
    printf("%s\n",head->id);
}

int insert_tail(struct Node *head,int cfd,char * id)
{
    struct Node *newnode = (struct Node*)malloc(sizeof(struct Node));
    if(newnode == NULL)
    {
        printf("insert tail!\n");
        return 0;
    }
    newnode->cfd = cfd;
    strcpy(newnode->id,id);
    newnode->next = NULL;
    while(head->next != NULL)
    {
        head = head->next;
    }
    head->next = newnode;
}

int len(struct Node *head)
{
    int i = 0;
    while(head->next != NULL)
    {
        head = head->next;
        ++i;
    }
    return i;
}

//从这边开始四个函数都是用来登录退出修改链表的
int search_value(struct Node*head,char *id)
{
    int length = len(head);
    int i;
    int count = 0;
    head = head->next;
    for(i=0; i<length; i++)
    {
        if(strcmp(head->id,id)==0)
        {
            return head->cfd;
        }
        head =  head->next;
    }
    return 0;
}
int search_null(struct Node*head,int c_fd,char *id)
{
    int length = len(head);
    int i;
    int count = 0;
    head = head->next;
    for(i=0; i<length; i++)
    {
        if(strcmp(head->id,"null") == 0)
        {
            if(head->cfd == c_fd)
            {
                return 1;
            }
        }
        head = head->next;
    }
    return 0;
}

int change_value(struct Node*head,int c_fd)
{
    int length = len(head);
    int i;
    head = head->next;
    for(i=0; i<length; i++)
    {
        if(head->cfd == c_fd)
        {
            strcpy(head->id,"null");
            return 0;
        }
        head = head->next;
        
    }
    return 0;
}

int change_null(struct Node*head,int c_fd,char *id)
{
    int length = len(head);
    int i;
    head = head->next;
    for(i=0; i<length; i++)
    {
        if(head->cfd == c_fd)
        {
            strcpy(head->id,id);
            return 0;
        }
        head = head->next;
        
    }
    return 0;
}

int delete_value(struct Node *head, int value)
{
    int length = len(head);
    int i;
    for (i = 0; i < length ; i++)
    {
        if (head->next->cfd == value)
        {
            struct Node *ptr = head->next;
            head->next = ptr->next;
            free(ptr);
        }
        else
        {
            head = head->next;
        }
        
    }
    return 0;
}

//心跳检测有关链表函数
int input_time(struct Node *head,int fd,time_t lastsecond)
{
    int length = len(head);
    int i;
    int count = 0;
    head = head->next;
    for(i=0; i<length; i++)
    {
        if(head->cfd == fd)
        {
            head->last_second = lastsecond;//插入最后一条操作的时间
            return head->cfd;
        }
        head = head->next;
    }
    return 0;
}

//3:群聊判断是否被禁言
int flag_jinyan(struct Node*head,int c_fd)
{
    head = head->next;
    while( head!=NULL)
    {
        if(head->cfd == c_fd)
        {          
            if(head->flag == 1)
            {
                return 1;
            }
            else
                return 0;
        }        
        head=head->next;
    }
}
//3:群聊
void qunliao(struct Node*head,struct send_msg msg,int fd)
{
    msg.cmd = CMD_USER_CHATALL;
    //strcpy(msg.from,msg.name);
    head = head->next;  
    while( head!=NULL)
    {
        if(head->cfd != fd)
        {
            write(head->cfd,&msg,sizeof(msg));           
        }        
        head=head->next;
    }   
}
//1:查看在线用户
void showonlinefriend(struct Node *head,int c_fd)       //write(c_fd,...)
{
    struct send_msg msg;
    bzero(&msg,sizeof(msg));
    msg.cmd = CMD_SHOW_FRIEND;
    int i;
    int length = len(head);
    head = head->next;
    for(i =0; i< length;i++)
    {
        if(head->cfd != c_fd)
        {
            strcpy(msg.friend[i].id,head->id);
        }
        head = head->next;
    }
    write(c_fd,&msg,sizeof(msg)); 

}

//禁言
int change_flag(struct Node*head,char *id)
{
    int length = len(head);
    int i;
    int count = 0;
    head = head->next;
    for(i=0; i<length; i++)
    {
        if(strcmp(head->id,id)==0)
        {
            head->flag = 1;//设置禁言
            return 0;
        }
        head =  head->next;
    }
    return -1;
}

//解禁
int change_flag_jiejin(struct Node*head,char *id)
{
    int length = len(head);
    int i;
    int count = 0;
    head = head->next;
    for(i=0; i<length; i++)
    {
        if(strcmp(head->id,id)==0)
        {
            head->flag = 0;//解禁
            return 0;
        }
        head =  head->next;
    }
    return -1;
}

/*每一个线程函数代表一个客户端*/
void* fun1_read(void*arg)
{
    FILE *fp;
    int read_len = 0;
    int fwrite_len = 0;
    int index = 0;
    //在线链表的head指针是全局变量，要用锁限制多个线程访问
    int ret = -10;
    int ret2 = -10;
    time_t last_time = 0;
    int c_fd = *(int*)arg;
    struct send_msg s_msg;
            printf("客户端:%d连入\n",c_fd);
    while (1)
    {
        //测试printf
        //printf("客户端:%d开始读了\n",c_fd);

        bzero(&s_msg, sizeof(s_msg) );
            
        if( read(c_fd,&s_msg,sizeof(s_msg)) != 0)
        {
            if(s_msg.cmd != CMD_LOGCHOICE_LOG && s_msg.cmd != CMD_LOGCHOICE_REGISTER)
            {
                pthread_mutex_lock(&mutex);
                time(&last_time);//获取当前时间(秒);
                input_time(head,c_fd,last_time);//插入操作时刻
                
                //printf("%ld时刻读到信息\n",last_time);
                pthread_mutex_unlock(&mutex);
            }

            if(s_msg.cmd == CMD_SHOW_FRIEND) //1显示在线用户
            {
                pthread_mutex_lock(&mutex);
                showonlinefriend(head,c_fd);
                pthread_mutex_unlock(&mutex);
                
            }
            else if(s_msg.cmd == CMD_LOGCHOICE_REGISTER)//注册功能
            {
                //形参1代表选定了注册功能
                if(shujuku(&s_msg,1) == 11)//数据库函数返回11成功，返回0失败
                {
                    printf("id:%s\n",s_msg.id);
                    printf("name:%s\n",s_msg.name);
                    printf("pwd:%s\n",s_msg.pwd);//用于测试
                    s_msg.cmd = CMD_LOGCHOICE_REGISTER_SUCCESS;
                    write(c_fd,&s_msg,sizeof(s_msg));
                }
                else
                {
                    s_msg.cmd = CMD_LOGCHOICE_REGISTER_FAIL;
                    write(c_fd,&s_msg,sizeof(s_msg));
                }
            }
            else if(s_msg.cmd == CMD_LOGCHOICE_LOG)//登录功能
            {
                if(strcmp(s_msg.id,"root") == 0)//登录功能
                {
                    
                    if(strcmp(s_msg.pwd,"123456") == 0)
                    {
                        pthread_mutex_lock(&mutex);                                    
                        ret = search_value(head,s_msg.id);
                        pthread_mutex_unlock(&mutex);
                        
                                    //printf("ret:%d\n",ret);
                        if( ret == 0)//在线链表没有，可以登录
                        {
                            pthread_mutex_lock(&mutex);
                            ret2 = search_null(head,c_fd,s_msg.id);  
                            pthread_mutex_unlock(&mutex);
                            if(ret2 == 0)
                            {
                                pthread_mutex_lock(&mutex);
                                insert_tail(head,c_fd,s_msg.id);    
                                pthread_mutex_unlock(&mutex);
                            }
                            else
                            {
                                pthread_mutex_lock(&mutex);
                                change_null(head,c_fd,s_msg.id);
                                pthread_mutex_unlock(&mutex);
                            }
                            pthread_mutex_lock(&mutex);
                            print(head);
                            pthread_mutex_unlock(&mutex);
                        s_msg.cmd = CMD_LOGCHOICE_LOG_ROOT_SUCCESS;
                        strcpy(s_msg.name,"root");
                        write(c_fd,&s_msg,sizeof(s_msg));
                        }
                    }
                    else
                    {
                        s_msg.cmd = CMD_LOGCHOICE_LOG_ROOT_FAIL;
                        write(c_fd,&s_msg,sizeof(s_msg));
                    }
                    
                }
                else if(shujuku(&s_msg,2) == 21)//这里导出name
                {
                        /*登陆成功，加入在线列表*/
                        /*要使用到head锁上*/
                    pthread_mutex_lock(&mutex);                                    
                    ret = search_value(head,s_msg.id);
                    pthread_mutex_unlock(&mutex);
                    
                                //printf("ret:%d\n",ret);
                    if( ret == 0)//在线链表没有，可以登录
                    {
                        pthread_mutex_lock(&mutex);
                        ret2 = search_null(head,c_fd,s_msg.id);  
                        pthread_mutex_unlock(&mutex);
                        if(ret2 == 0)
                        {
                            pthread_mutex_lock(&mutex);
                            insert_tail(head,c_fd,s_msg.id);    
                            pthread_mutex_unlock(&mutex);
                        }
                        else
                        {
                            pthread_mutex_lock(&mutex);
                            change_null(head,c_fd,s_msg.id);
                            pthread_mutex_unlock(&mutex);
                        }
                        pthread_mutex_lock(&mutex);
                        print(head);
                        pthread_mutex_unlock(&mutex);
                        
                        s_msg.cmd = CMD_LOGCHOICE_LOG_SUCCESS;
                                                                                    //这里待优化，要有name信息
                                                            //printf("卡在这里了哦2\n");
                        write(c_fd,&s_msg,sizeof(s_msg));//登录功能
                                                            //printf("卡在这里了哦3\n");
                    }
                    else
                    {
                        s_msg.cmd = CMD_LOGCHOICE_LOG_FAIL_RELOG;
                        write(c_fd,&s_msg,sizeof(s_msg)); 
                                printf("消息发出\n");
                    }
                }
                else if(shujuku(&s_msg,2) == 200)
                {
                    s_msg.cmd = CMD_LOGCHOICE_LOG_FAIL_ACCOUNTNOTEXIST;
                    write(c_fd,&s_msg,sizeof(s_msg));
                }
                else if(shujuku(&s_msg,2) == 201)
                {
                    s_msg.cmd = CMD_LOGCHOICE_LOG_FAIL_PWDERR;
                    write(c_fd,&s_msg,sizeof(s_msg));
                }                
                else
                {
                    printf("登录出现未知错误\n");
                }

            }            
            else if(s_msg.cmd == CMD_USER_CHAT)//私聊
            {
                                                                pthread_mutex_lock(&mutex);
                if(shujuku(&s_msg,4) == 40)//4是插入聊天记录
                {
                    printf("insert siliaojilu err\n");
                    return NULL;
                }
                                //printf("to:%s\n",s_msg.to);
                                //printf("message:%s\n",s_msg.massage);
                                                                
                int thatfd = search_value(head,s_msg.to);
                                                                pthread_mutex_unlock(&mutex);
                                //printf("thatfd:%d\n",thatfd);
                
                if( thatfd == 0)//对方用户不在线
                {
                    bzero(&s_msg, sizeof(s_msg) );
                    s_msg.cmd = CMD_USER_CHAT_FAIL;
                    write(c_fd,&s_msg,sizeof(s_msg));
                }
                else//对方在线,此时服务器收到一个私聊包,只需要负责转发
                {
                    s_msg.cmd = CMD_USER_CHAT_SUCCESS;
                                printf("执行了转发功能\n");
                    write(thatfd,&s_msg,sizeof(s_msg));
                }
            }
            else if(s_msg.cmd == CMD_USER_CHATALL)//群聊
            {
                if(strcmp(s_msg.massage,"woc") == 0)
                {
                    s_msg.cmd = CMD_USER_CHATALLERRMINGAN;
                    strcpy(s_msg.massage , "敏感词汇,不转发");
                    write(c_fd,&s_msg,sizeof(s_msg));
                    
                }
                else
                {
                    if(flag_jinyan(head,c_fd) == 0)
                    {
                        pthread_mutex_lock(&mutex);
                        if(shujuku(&s_msg,4) == 40)//4是插入聊天记录
                        {
                            printf("insert siliaojilu err\n");
                            return NULL;
                        }
                        
                                        qunliao(head,s_msg,c_fd);
                        pthread_mutex_unlock(&mutex);
                    }
                    else
                    {
                        s_msg.cmd = CMD_USER_CHATALLERR;
                        strcpy(s_msg.massage,"您已被禁言");
                        write(c_fd,&s_msg,sizeof(s_msg));
                    }
                }
                
            }
            else if(s_msg.cmd == CMD_LOOK_RECORD_ALL)//群聊记录
            {
                s_msg.cmd =  c_fd;
                
                if( shujuku(&s_msg,5) != 51)//5是查找群聊聊天记录
                {
                    printf("send record err\n");
                    return NULL;
                }
            }
            else if(s_msg.cmd == CMD_LOOK_RECORD_ONE)//私聊记录
            {
                s_msg.cmd =  c_fd;
                
                if( shujuku(&s_msg,6) != 61)//6是查找私聊聊天记录
                {
                    printf("send record err\n");
                    return NULL;
                }
            }
            else if(s_msg.cmd == CMD_JINYAN)//禁言
            {
                //明天从这里继续
                if(change_flag(head,s_msg.id) == 0)//修改成功
                {
                    write(c_fd,&s_msg,sizeof(s_msg));
                }
                else if(change_flag(head,s_msg.id) == -1)
                {
                    s_msg.cmd = CMD_JINYANERR;
                    write(c_fd,&s_msg,sizeof(s_msg));
                }
            }
            else if(s_msg.cmd == CMD_JIEJIN)//解禁
            {
                if(change_flag_jiejin(head,s_msg.id) == 0)//修改成功
                {
                    write(c_fd,&s_msg,sizeof(s_msg));
                }
                else if(change_flag_jiejin(head,s_msg.id) == -1)
                {
                    s_msg.cmd = CMD_JIEJINERR;
                    write(c_fd,&s_msg,sizeof(s_msg));
                }
            }
            else if(s_msg.cmd == CMD_SEND_FILE)     //转发文件
            {
                
                //今天要把这个写完
                if(s_msg.flag == -1)
                {
                    index = 0;
                    char acceptfilename[20];
                    strcpy(acceptfilename,s_msg.massage);
                    printf("文件名:%s\n",acceptfilename);
                    fp = fopen(acceptfilename,"w");
                    if(fp == NULL)
                    {
                        printf("open err\n");
                        exit(-1);
                    }
                }
                //fwrite_len = fwrite(s_msg.massage,sizeof(char),sizeof(s_msg.massage),fp);
                if(s_msg.flag == -2)//正常1024传数据
                {
                    //printf("内容:%s\n",s_msg.massage);
                    fwrite_len = fwrite(s_msg.massage,sizeof(char),sizeof(s_msg.massage),fp);
                    printf("recv %d 写入:%d\n",++index,fwrite_len); 
                }
                if(s_msg.flag >= 0)//0到1024随机数
                {
                    fwrite_len = fwrite(s_msg.massage,sizeof(char),s_msg.flag,fp);//根据flag决定写入多少
                    printf("recv %d 写入:%d\n",++index,fwrite_len); 
                    fclose(fp);
                    printf("文件传输完成!!!\n");
                }
                
            }
            else if(s_msg.cmd == 10008)     //6:转发文件
            {
                printf("id:%d\n",s_msg.flag);
            }
            else if(s_msg.cmd == CMD_DELETE_ACCOUNT)//注销
            {
                    pthread_mutex_lock(&mutex);
                change_value(head,c_fd);
                print(head);
                    pthread_mutex_unlock(&mutex);
                /*写下去数据库中删除用户*/
                if(shujuku(&s_msg,3) == 30)
                {
                    printf("shujuku delete err\n");
                    return NULL;
                }
                s_msg.cmd == CMD_DELETE_ACCOUNT;
                write(c_fd,&s_msg,sizeof(s_msg));
            }
            else if(s_msg.cmd == CMD_OUTLINE)//下线
            {
                    pthread_mutex_lock(&mutex);
                change_value(head,c_fd);            
                print(head);
                    pthread_mutex_unlock(&mutex);
                s_msg.cmd == CMD_OUTLINE;
                write(c_fd,&s_msg,sizeof(s_msg));
            }
            
            else if(s_msg.cmd == CMD_HEART_CHECK)//心跳检测
            {
                pthread_mutex_lock(&mutex);
                time(&last_time);//获取当前时间(秒);
                input_time(head,c_fd,last_time);//插入操作时刻
                
                printf("%ld时刻读到心跳回传信息\n",last_time);
                pthread_mutex_unlock(&mutex);
            }
            else
            {
                ;//得到的CMD错误,啥都不做
            }
        }
        else//客户端正常退出或者异常,跳出循环就结束该客户端与服务器的连接了
        {
            
            pthread_mutex_lock(&mutex);
                //delete_fd(head,c_fd);
                delete_value(head,c_fd);
                //usleep(1);
                print(head);
            pthread_mutex_unlock(&mutex);
            close(c_fd);
            printf("套接字%d关闭成功\n",c_fd);
            return NULL;
        }
        
    }

}


//开辟一条线程专门心跳检测
void* heart_check(void*arg)
{
    struct send_msg msg;//消息结构体
    struct Node*tmp1_head = head;
    struct Node*tmp2_head = head;
    while(1)
    {
        
        sleep(10);//每过一百秒检测所有在线链表的最后一次操作时间是否异常
        now_time = time(NULL);

        
        tmp1_head = tmp1_head->next;
        while(tmp1_head != NULL)
        {
            //printf("cfd:%d多长时间没动,%ld - %ld = %ld\n",tmp1_head->cfd, now_time,tmp1_head->last_second, now_time - tmp1_head->last_second);
            if(now_time - tmp1_head->last_second >= 60/*300*/ && now_time - tmp1_head->last_second < 600 )//5到10分钟没反应就判定你断线,防止垃圾值出现
            {
                //1:尝试发信息
                bzero(&msg,sizeof(msg));
                msg.cmd = CMD_HEART_CHECK;
                write(tmp1_head->cfd,&msg,sizeof(msg));
                
                //2:确实断开连接踢了你
                // printf("执行下线功能\n");
                // delete_value(tmp2_head,tmp1_head->cfd);
                // close(tmp2_head->cfd);
                //     print(tmp2_head);
                
            }
            tmp1_head = tmp1_head->next;
        }
        tmp1_head = tmp2_head;
        
    }
    
}

int main()
{
    /*定义参数*/
    
    /*先定义sockaddr_in，到时候强制类型转换为sockaddr结构体*/
    struct sockaddr_in s_addr= {0};
    socklen_t addrlen = 0;
    int s_fd = 0;
    /*第一步打开socket，失败-1，成功s_fd*/

    s_fd = socket(AF_INET,SOCK_STREAM,0);
    if (s_fd == -1)
    {
        perror("socket");
        exit(-1);
    }

        /*设置端口复用*/
    int opt = 1;
    setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /*第二步bind绑定0成功-1失败*/
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    //s_addr.sin_addr.s_addr = inet_addr(MYADDR);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    addrlen = sizeof(struct sockaddr_in);
    int ret = bind(s_fd, (struct sockaddr*)&s_addr, addrlen);
    if(ret == -1)
    {
        perror("bind");
        close(s_fd);
        exit(-1);
    }
    //printf("s_fd=%d,ret=%d\n",s_fd,ret);
    
    /*第三步listen监听，0成功，-1失败*/
    ret = listen(s_fd,MAXLISTEN);
    if(ret == -1)
    {
        perror("bind");
        close(s_fd);
        exit(-1);
    }
    printf("\033clisten success!\n");
   
    //accept进行连接,接收连接了谁
    struct sockaddr_in c_addr;
    int c_addrlen = sizeof(struct sockaddr_in);
    memset(&c_addr,0,c_addrlen);
    //成功
    pthread_mutex_init(&mutex,NULL);//初始化锁
    
    /*方案二成功*/ 
    int c_fd;

    init(&head);//初始化head指针
    pthread_t tid;
    pthread_t heart_tid;
    pthread_create(&heart_tid,NULL,(void*)heart_check,NULL);
    while (1)
    {
        
        c_fd = accept(s_fd,(struct sockaddr *)&c_addr,&c_addrlen);
        if (c_fd == -1)
        {
            perror("accept:");
            exit(-2);
        }
        printf("accept c_fd success!%d\n",c_fd);
        /*看看哪个客户端连了我,(用于测试)*/
        printf("客户端ip:%s,客户端port:%d\n",inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
        /********************一旦有客户端连接就创建新read线程收东西*/
        pthread_create(&tid,NULL,(void*)fun1_read,(void*)&c_fd);
        pthread_detach(tid);
            //printf("我是哪个线程:%ld\n",tid);//test
        //这里等会传建一个东西保存c_fd，第二个客户端连进来会冲掉第一个的c_fd
    }
    pthread_join(tid,NULL);
    pthread_join(heart_tid,NULL);
    pthread_mutex_destroy(&mutex);
    freeall(head);
    
    
    return 0;
}