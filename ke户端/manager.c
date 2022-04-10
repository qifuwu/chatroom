#include "myhead.h"

int manager_interface()
{
    int choice = 0;
    printf("----------------------------\n"); 
    printf("|\t管理员:root\n");
    printf("|\t1:查看在线用户\n");
    printf("|\t2:私聊\n");
    printf("|\t3:群聊\n");
    printf("|\t4:查看聊天记录\n");
    printf("|\t5:\n");
    printf("|\t5:    \n");
    printf("|\t6:禁言    \n");
    printf("|\t7:解禁    \n");
    printf("|\t11:返回登录\n");
    printf("|-------------------------|\n");
    printf("请选择:");
    scanf("%d",&choice);
    return choice;
                
}
//6:禁言
void jinyan()
{
    char id[15];
    printf("\033c");
    showonlinefriend();
    struct send_msg s_msg;
    bzero(&s_msg,sizeof(s_msg));
        s_msg.cmd = CMD_JINYAN;
    usleep(10000);
    printf("请输入禁言对象:");
    
    scanf("%s",id);
    strcpy(s_msg.id,id);
    write(sockfd,&s_msg,sizeof(s_msg));
    usleep(10000);
    printf("按回车继续...");
    getchar();
    getchar();
    

    
}
//7:解禁
void jiejin()
{
    char id[15];
    printf("\033c");
    showonlinefriend();
    struct send_msg s_msg;
    bzero(&s_msg,sizeof(s_msg));
        s_msg.cmd = CMD_JIEJIN;
    usleep(10000);
    printf("请输入解禁对象:");
    
    scanf("%s",id);
    strcpy(s_msg.id,id);
    write(sockfd,&s_msg,sizeof(s_msg));
    usleep(10000);
    printf("按回车继续...");
    getchar();
    getchar();
    

    
}

int manager(void)
{
    int choice;
    
    printf("manager!\n");
    while(1)
    {
        printf("\033c");
        choice = manager_interface();
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
            look_record();//查找聊天记录
        }
        else if(choice ==6)//禁言
        {
            jinyan();//查找聊天记录
        }
        else if(choice ==7)//解禁
        {
            jiejin();//查找聊天记录
        }

        else if(choice ==11)//返回登录下线
        {
            return 11;
        }
    }
}