#include "myhead.h"

//选择功能界面
int user_interface()
{
    int choice = 0;
    printf("----------------------------\n"); 
    printf("----昵称:%s(账号:%s)\n",now_msg.name,now_msg.id);
    printf("|\t1:查看在线用户\n");
    printf("|\t2:私聊\n");
    printf("|\t3:群聊\n");
    printf("|\t4:查看聊天记录\n");
    printf("|\t5:发送文件\n");
    printf("|\t6:test\n");
    printf("|\t7:    \n");
    printf("|\t8:私聊\n");
    printf("|\t9:群聊\n");
    printf("|\t10:注销账户\n");
    // printf("|            3:加好友            |\n");
    // printf("|            4:删好友             |\n");
    // printf("|            5:创建群             |\n");//扩展性功能暂时是所有登录的用户进行群聊
    // printf("|            6:删出群             |\n");
    printf("|\t11:返回登录\n");
    printf("|-------------------------|\n");
    printf("请选择:");
    scanf("%d",&choice);

    return choice;
}

//得到\0结尾的中间有空格的字符串
void my_fgets(char *str)
{
    
    char *ch;
    fgets(str,500,stdin);
    ch = strchr(str,'\n');
    if(ch)//指针指向null输出0
    {
        *ch = '\0';
    }
}

//1:查看在线用户
void showonlinefriend()
{
    struct send_msg s_msg;
    bzero(&s_msg,sizeof(s_msg));//清空结构体
    s_msg.cmd = CMD_SHOW_FRIEND;//填写cmd
    write(sockfd,&s_msg,sizeof(s_msg));//发出去了 
}
//2:私聊
void siliao()//登录成功时本地会生成一个结构体，里面存放id和name
{
    struct send_msg s_msg;
    printf("\033c");
    printf("\"%s(%s)\"欢迎进入聊天界面!(输入bye返回上一级)\n",now_msg.name,now_msg.id);
    
    while(1)
    {
        bzero(&s_msg,sizeof(s_msg));//清空结构体
        printf("请输入聊天信息:");
        getchar();
        my_fgets(s_msg.massage);
        //scanf("%s",s_msg.massage);//填写message
                if(strcmp(s_msg.massage,"bye") == 0)
                    break;
        /*发送消息之前的准备工作*/
        s_msg.cmd = CMD_USER_CHAT;//填写cmd
                printf("请输入对方id:");
                scanf("%s",s_msg.id);//填写message
                strcpy(s_msg.to,s_msg.id);//填写to(暂时用手动)
                strcpy(s_msg.from,now_msg.id);
                strcpy(s_msg.from_name,now_msg.name);
                    //pthread_mutex_lock(&mutex);
        write(sockfd,&s_msg,sizeof(s_msg));//发出去了
                                printf("发出去了\n");
                    //pthread_mutex_unlock(&mutex);
    }
}
//3:群聊
void qunliao()
{
    struct send_msg s_msg;
    printf("\033c");
    printf("\"%s(%s)\"欢迎进入群聊界面!(输入bye返回上一级)\n",now_msg.name,now_msg.id);
    getchar();
    while(1)
    {
        bzero(&s_msg,sizeof(s_msg));//清空结构体
        printf("请输入聊天信息:");

        my_fgets(s_msg.massage);
        //fgets(s_msg.massage,500,stdin);

        //scanf("%s",s_msg.massage);//填写message

        strcpy(s_msg.from,now_msg.id);
        strcpy(s_msg.from_name,now_msg.name);
        strcpy(s_msg.to,"all");
        /*发送消息之前的准备工作*/
        s_msg.cmd = CMD_USER_CHATALL;//填写cmd
                
        if(strcmp(s_msg.massage,"bye") == 0)
            break;
                    //pthread_mutex_lock(&mutex);
        write(sockfd,&s_msg,sizeof(s_msg));//发出去了
                                printf("发出去了\n");
                    //pthread_mutex_unlock(&mutex);
    }
}

//10:注销账户
int zhuxiao()
{
    int choice  = 0;
    int choice2 = 0;
    struct send_msg s_msg;
    bzero(&s_msg,sizeof(s_msg));//清空结构体
    s_msg.cmd = CMD_DELETE_ACCOUNT;//填写cmd
    strcpy(s_msg.id , now_msg.id);//要注销的id
    strcpy(s_msg.name , now_msg.name);//要注销的name,可以不需要
        printf("真的要注销账号吗?(1:是2:否)\n");
        printf("请输入:");
        scanf("%d",&choice);
        switch (choice)
        {
        case 1:
            printf("再考虑一下,此操作不可逆! 1:我确定要注销账号2:我后悔了\n");
            scanf("%d",&choice2);
            if(choice2 ==1)
            {
                write(sockfd,&s_msg,sizeof(s_msg));//发出去了
                /*写下去*/
                printf("正在注销,请稍后...\n");
                sem_wait(&sem);
                return 12;
            }
            else
            {
                return 0;
            }
        case 2:
            return 0;
        default:            
            break;
        }
        printf("非法输入!请重输.\n");
        sleep(1);
        return 0;   
}

//4:查看聊天记录
void look_record()
{
    getchar();
    struct send_msg s_msg;
    char choice1;
    char id[15];
    while(1)
    {
        printf("\033c");
        printf("\t1:查看私聊记录\n");
        printf("\t2:查看群聊记录\n");
        printf("\t3:返回\n");
        printf("请输入:");
        scanf("%c",&choice1);
        getchar();
        switch (choice1)
        {
            case '1':
            bzero(&s_msg,sizeof(s_msg));//清空结构体
            printf("请输入和哪位好友(id)的聊天记录:");
                scanf("%s",id);
                getchar();
                strcpy(s_msg.to,id);
                strcpy(s_msg.from,now_msg.id);
                s_msg.cmd = CMD_LOOK_RECORD_ONE;
                write(sockfd,&s_msg,sizeof(s_msg));//发出去了
                printf("\033c");
                usleep(10000);//让回车显示在记录后面
                printf("按回车键继续...");
                getchar();
                break;
            case '2':
                /* 群聊记录提供cmd和自己的id */
                bzero(&s_msg,sizeof(s_msg));//清空结构体
                strcpy(s_msg.id , now_msg.id);
                s_msg.cmd = CMD_LOOK_RECORD_ALL;//填写cmd
                write(sockfd,&s_msg,sizeof(s_msg));//发出去了
                printf("\033c");
                usleep(10000);
                printf("按回车键继续...");
                getchar();
                break;
            default:
                return;
        }
    }


    // bzero(&s_msg,sizeof(s_msg));//清空结构体
    // s_msg.cmd = CMD_LOOK_RECORD;//填写cmd
    // write(sockfd,&s_msg,sizeof(s_msg));//发出去了 
}
//5:传文件
void send_file()
{
    struct send_msg s_msg;
    char filename[20];//缓存文件名
    //char buffer[1024];//缓存文件

    //1:初始化内存
    bzero(filename,sizeof(filename));
    bzero(&s_msg,sizeof(s_msg));
    //2:获取文件名
    printf("\033c");
    printf("请输入要发送的文件名:");
    scanf("%s",filename);
    //3:发送文件名
    s_msg.cmd = CMD_SEND_FILE;
    s_msg.flag = -1;                 //传文件名
    strcpy(s_msg.massage,filename);
    write(sockfd,&s_msg,sizeof(s_msg));

    bzero(&s_msg,sizeof(s_msg));            //初始化
    s_msg.cmd = CMD_SEND_FILE;
    s_msg.flag = 0;      //初始化为不用的flag
    FILE *fp = fopen(filename,"r");
    if(fp == NULL)
    {
        printf("FILE open err\n");
        return ;
    }
    int index = 1;
    int len = 0;
    while(  (len = fread(s_msg.massage,sizeof(char),sizeof(s_msg.massage),fp))  > 0)        //必须>0
    {
        if(feof(fp))
        {
            s_msg.flag = len;
            send(sockfd,&s_msg,sizeof(s_msg),0);
            printf("send %d len:%d\n",index++,len);
            break;
        }
        printf("send %d len:%d\n",index++,len);
        s_msg.flag = -2;//发送文件
        if(send(sockfd,&s_msg,sizeof(s_msg),0) < 0)
        {
            printf("send err\n");
            break;
        }
        bzero(&s_msg.massage,sizeof(s_msg.massage));
        
    }
    fclose(fp);
    printf("%s传输完成\n",filename);

    sleep(3);



}

int user(void)
{
    
    int choice;
    printf("\033c");
    printf("usering!\n");
    while(1)
    {
        printf("\033c");
        choice = user_interface();
        sleep(1);
        if(choice == 1)//查看在现用户
        {
            showonlinefriend();
        }
        else if(choice == 2)//私聊
        {
            siliao();//私聊
        }
        else if(choice ==3)
        {
            qunliao();//群聊
        }
        else if(choice ==4)
        {
            look_record();//查看聊天记录
        }
        else if(choice ==5)
        {
            send_file();//发送文件
        }
        else if(choice ==6)
        {
            struct send_msg s_msg;//发送文件
            s_msg.cmd = 10008;
            for(int i = 0;i< 10000;i++)
            {
                s_msg.flag = i;
                write(sockfd,&s_msg,sizeof(s_msg));
                usleep(1000);
            }
            printf("传输完毕\n");
            sleep(10);
        }
        else if(choice ==10)
        {
            if(zhuxiao() == 12)//注销账号
            {
                printf("注销成功! 即将返回登录界面\n");
                sleep(3);
                return 12;//返回登录界面
            }
        }
        else if(choice ==11)//返回登录下线
        {
            return 11;
        }
    }
    
}