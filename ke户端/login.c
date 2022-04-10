#include "myhead.h" 

//密码不回显
void input_pwd(char *s_msg_pwd)
{
    struct termios old,new;
    tcgetattr(0,&old);
    new = old;
    new.c_lflag &= ~(ECHO | ICANON);

    char ch;
    int i = 0;
    int tmp;
    while(1)
    {
        tcsetattr(0,TCSANOW,&new);//设置密码不回显
        scanf("%c",&ch);
        tcsetattr(0,TCSANOW,&old);
        if(ch == 127)//'\b'显示是127
        {
            if(i>0)
            {
                s_msg_pwd[i] = '\0';
                i--;
                printf("\r");
                for(tmp = 0;tmp<20;tmp++)
                {
                    printf(" ");
                }
                printf("\r");
                for(tmp = 0;tmp<i;tmp++)
                {
                    printf("*");
                }    
            }
            continue;
        }
        if(ch == '\n')//结束判断
        {
            s_msg_pwd[i] = '\0';
            break;
        }
        if(i==20)//结束判断
        {
            s_msg_pwd[i] = '\0';
            continue;
        }
        s_msg_pwd[i] = ch;
        i++;
        printf("\r");
        
        for(tmp = 0;tmp<i;tmp++)
        {
            printf("*");
        }
    }
    printf("\n");
}

/*1:登录*/
int my_log(void)
{
    
    int choice = 0;//登录失败返回到哪里
    struct send_msg s_msg;
    while(1)
    {
        bzero(&s_msg,sizeof(s_msg));//清空结构体
        printf("\033c");
        s_msg.cmd = CMD_LOGCHOICE_LOG;

        getchar();  //吃掉scanf choice时候输入的回车
        printf("请输入登录id:\n");
        scanf("%s",s_msg.id);

        printf("请输入登录密码:\n");

        getchar();//密码不回显就这两句
        input_pwd(s_msg.pwd);
        //scanf("%s",s_msg.pwd);

            write(sockfd,&s_msg,sizeof(s_msg));//把登录信息发给服务器
            //usleep(1);
        bzero(&s_msg,sizeof(s_msg));//清空结构体
                            //printf("消息发出\n");
                            //pthread_mutex_lock(&mutex);
                        
        while(1)
        {                    
            read(sockfd,&s_msg,sizeof(s_msg));//读取服务器回传的信息
            if( s_msg.cmd == 20 || s_msg.cmd == 200
                || s_msg.cmd == 201 || s_msg.cmd == 210
                || s_msg.cmd == 211 || s_msg.cmd == 212 )//下线后没有线程处理心跳包，回传的心跳信号会一直在接收窗口中，
            {
                break;//确保我读到的cmd为下面这些情况，要不然就一直卡着直到读到下列情况
            }
        }
        if(s_msg.cmd == CMD_LOGCHOICE_LOG_SUCCESS)//服务器同意登录
        {
            strcpy(now_msg.id , s_msg.id);                              //now_msg全局变量，保存本次成功登录的用户的id,和name
            strcpy(now_msg.name , s_msg.name);
                    //printf("id:%s\n",now_msg.id);
                    //printf("name:%s\n",now_msg.name);
                    //sleep(5);

            return 1;
        }
        else if(s_msg.cmd == CMD_LOGCHOICE_LOG_ROOT_SUCCESS)//服务器同意root登录
        {
            strcpy(now_msg.id , s_msg.id);                              //now_msg全局变量，保存本次成功登录的用户的id,和name
            strcpy(now_msg.name , s_msg.name);
                    //printf("id:%s\n",now_msg.id);
                    //printf("name:%s\n",now_msg.name);
                    //sleep(5);

            return 2;
        }
        else if(s_msg.cmd == CMD_LOGCHOICE_LOG_ROOT_FAIL)//服务器不同意root登录
        {
            printf("密码错误!\n");
            printf("重输(1)返回注册(2):");
                //printf("%d\n",s_msg.cmd);
            scanf("%d",&choice);
            if(choice == 2)
            {
                return 0;
            }
        }
        else if(s_msg.cmd == CMD_LOGCHOICE_LOG_FAIL_ACCOUNTNOTEXIST )//服务器不同意登录
        {
            printf("账号不存在!\n");
            printf("重输(1)返回注册(2):");
                //printf("%d\n",s_msg.cmd);
            scanf("%d",&choice);
            if(choice == 2)
            {
                return 0;
            }
        }
        else if(s_msg.cmd == CMD_LOGCHOICE_LOG_FAIL_PWDERR)//服务器不同意登录
        {
            printf("密码错误!\n");
            printf("重输(1)返回注册(2):");
                //printf("%d\n",s_msg.cmd);
            scanf("%d",&choice);
            if(choice == 2)
            {
                return 0;
            }
        }
        else if(s_msg.cmd == CMD_LOGCHOICE_LOG_FAIL_RELOG)//服务器不同意登录
        {
            printf("其他地方已登录!\n");
            printf("重输(1)返回注册(2):");
                //printf("%d\n",s_msg.cmd);
            scanf("%d",&choice);
            if(choice == 2)
            {
                return 0;
            }
        }
        else
        {

        }
    }
    /*
    printf("\033c");
    if(1)
    printf("注册成功!\n");
    printf("id:%s\n",s_msg.id);
    printf("name:%s\n",s_msg.name);
    printf("pwd:%s\n",s_msg.pwd);
   
    printf("\033c");
    */
}

/*2:注册*/
void my_reg(void)
{
    struct send_msg s_msg;

    printf("\033c");
    bzero(&s_msg, sizeof(s_msg) );
    s_msg.cmd = CMD_LOGCHOICE_REGISTER;//注册功能
    getchar();  //吃掉scanf choice时候输入的回车
    printf("请输入要注册的id:\n");
    scanf("%s",s_msg.id);

    printf("请输入网名:\n");    
    scanf("%s",s_msg.name);

    printf("请输入登录密码:\n");
    scanf("%s",s_msg.pwd);

    printf("\033c");

    /*与服务器通信匹配是否id已被注册*/
    //先清结构体再发送(登录，注册，退出)
        
    write(sockfd,&s_msg,sizeof(s_msg));
    bzero(&s_msg,sizeof(s_msg));//每次用完清零
    //sleep(1);
        //printf("注册时收到服务器信息显示此行\n");

    while(1)
    {
        read(sockfd,&s_msg,sizeof(s_msg));
    
        if(s_msg.cmd == 10 || s_msg.cmd == 11)//保证我收到的信息为下面这两种情况
        {
            break;
        }
    }
    if(s_msg.cmd == CMD_LOGCHOICE_REGISTER_SUCCESS)     //服务器那边确定注册成功，给我们返回一个flag标志
    {
        /*反馈给用户注册账号成功*/
        printf("注册成功!\n");
        printf("id:%s\n",s_msg.id);
        printf("name:%s\n",s_msg.name);
        printf("pwd:%s\n",s_msg.pwd);
        printf("按任意键继续...");
        getchar();      //上一个scanf遗留下的\n
        getchar();      //阻塞进程
        //return 1;    
        printf("\033c");
    }
    else if(s_msg.cmd == CMD_LOGCHOICE_REGISTER_FAIL)
    {
        printf("id已被注册，请重新输入有效id(15位以内数字),3秒后返回.\n");
        sleep(3);
    }
    bzero(&s_msg,sizeof(s_msg));//每次用完清零
    
}

/*入口*/
int login(void)
{
    /*局部变量*/
    char choice;//scanf接收int型，如果输入字符会出错
    int log;
    while (1)
    {
        printf("\t\tQQ\t\t\n");
        printf("\t\t\t\t\n");
        printf("\t\t1:登录\t\t\n");
        printf("\t\t2:注册\t\t\n");
        printf("\t\t3:退出\t\t\n");
        printf("请选择:");
        scanf("%c",&choice);
    
        switch(choice)
        {
            case '1':      /*登录账号*/
                                            printf("test语句");/**/
                log = 0;
                log = my_log();
                break;
            case '2':      /*注册账号*/
                                            printf("test语句");/**/
                my_reg();
                break;
            case '3':      /*退出账号*/
                                            printf("test语句");/**/
                close(sockfd);
                printf("\033c");
                break;
            default :
                printf("\033c");
                break;
        }
            
        if(log==1)//普通用户登录成功
        {
            return 1;
        }
        else if(log==2)//root用户登录成功
        {
            return 2;
        }
        
        if(choice == '3')//跳出login函数退出
        break;
    }
    return 3;
}