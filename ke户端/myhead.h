#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <termios.h>//用于密码不回显

#define PORT 8888
#define SERVERADDR "192.168.10.130"
//#define SERVERADDR "116.62.12.178"
//#define SERVERADDR "172.30.40.24"

/*cmd具体定义*/
#define CMD_LOGCHOICE_REGISTER 1        //注册功能
#define CMD_LOGCHOICE_REGISTER_SUCCESS 10   //注册成功
#define CMD_LOGCHOICE_REGISTER_FAIL 11     //注册失败
#define CMD_LOGCHOICE_LOG 2                 //登录标志
#define CMD_LOGCHOICE_LOG_SUCCESS 20        //登录成功
#define CMD_LOGCHOICE_LOG_ROOT_SUCCESS 200  //root登录成功
#define CMD_LOGCHOICE_LOG_ROOT_FAIL 201  //root登录失败
#define CMD_LOGCHOICE_LOG_FAIL_ACCOUNTNOTEXIST 210           //登录失败账号不存在
#define CMD_LOGCHOICE_LOG_FAIL_PWDERR 211           //登录失败密码错误
#define CMD_LOGCHOICE_LOG_FAIL_RELOG 212         //重复登录
#define CMD_SHOW_FRIEND   2000                      //查看在线好友
#define CMD_USER_CHAT 3                             //私聊功能
#define CMD_USER_CHAT_FAIL 30
#define CMD_USER_CHAT_SUCCESS 31
#define CMD_USER_CHATALL 4                          //群聊功能
#define CMD_USER_CHATALLERR 41                             //群聊失败
#define CMD_USER_CHATALLERRMINGAN 42                             //群聊敏感词汇

#define CMD_DELETE_ACCOUNT 8                        //注销账号
#define CMD_OUTLINE 9                               //下线
#define CMD_HEART_CHECK 90                          //发送心跳包
#define CMD_LOOK_RECORD_ONE 10000                       //查看私聊聊天记录
#define CMD_LOOK_RECORD_ALL 10001                       //查看群聊聊天记录
#define CMD_JINYAN 10002                                //禁言
#define CMD_JINYANERR 10003                              //禁言失败
#define CMD_JIEJIN 10004                              //解禁
#define CMD_JIEJINERR 10005                              //解禁失败
#define CMD_SEND_FILE 10006                                //传送文件
#define CMD_SEND_FILENAME 10007                                //传送文件名字

struct Friend             //在线用户
{
  char name[15];
  char id[15];
};

struct send_msg
{
    char id[15];
    char name[15];
    char pwd[20];
    char massage[1024];
    int cmd;    //命令
    char from[15];//从哪里来,保存id
    char from_name[15];//从哪里来,保存昵称
    char to[15];//到哪里去
    struct Friend friend[10]; //假设一个人只能加20个好友
    char record[1536];//传输聊天记录
    int flag;           //辅助传输文件用的
};

struct send_msg now_msg;//目前登录在本机的用户,保存id,name
//全局变量
int sockfd;       //sockfd用于通信
pthread_mutex_t mutex;//锁住head防止多个线程同时访问
pthread_cond_t cond;
sem_t sem;


/*函数申明*/
int login(void);//登录
int user(void);//登录到用户界面
int manager(void);//登录到管理员界面

/*功能函数申明*/
void my_fgets(char *str);//得到\0结尾的中间有空格的字符串
void showonlinefriend();
void qunliao();
void siliao();
int zhuxiao();
void input_pwd(char *s_msg_pwd);//密码不回显
void look_record();//查找聊天记录
