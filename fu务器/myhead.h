#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<sqlite3.h>
#include<time.h>//获取系统当前时间

#define PORT 8888
#define MYADDR "192.168.10.130"
//#define MYADDR "172.30.40.24"
#define MAXLISTEN 10

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

#define CMD_USER_CHATALL 4                             //群聊
#define CMD_USER_CHATALLERR 41                             //群聊失败
#define CMD_USER_CHATALLERRMINGAN 42                             //群聊敏感词汇

#define CMD_DELETE_ACCOUNT 8                        //注销账号
#define CMD_OUTLINE 9                               //下线
#define CMD_HEART_CHECK 90                          //发送心跳包
#define CMD_LOOK_RECORD_ONE 10000                       //查看私聊聊天记录
#define CMD_LOOK_RECORD_ALL 10001                       //查看群聊聊天记录
#define CMD_JINYAN 10002                              //禁言
#define CMD_JINYANERR 10003                              //禁言失败
#define CMD_JIEJIN 10004                              //解禁
#define CMD_JIEJINERR 10005                              //解禁失败
#define CMD_SEND_FILE 10006                                //传送文件
#define CMD_SEND_FILENAME 10007                                //传送文件名字
#define CMD_TEST 10008                                //用于测试

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
  char from[15];//从哪里来from和to都存id不是存name
  char from_name[15];//从哪里来,保存昵称
  char to[15];//到哪里去
  struct Friend friend[10]; //假设一个人只能加20个好友
  char record[1536];//传输聊天记录
  int flag;           //辅助传输文件用的
};

time_t now_time;//获取当前时刻

struct Node *head;//在线链表头结点


pthread_mutex_t mutex;//锁住head防止多个线程同时访问



/*函数申明*/
int shujuku(struct send_msg *s_msg,int);
                    /*形参num的1代表注册功能，2代表登录验证功能
                      返回值:11可以注册,10不能注册
                            21可以登录,200不能登录账号不存在,201不能登录密码错误
                    */