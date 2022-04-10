/*
 *数据库函数传进去客户端发来的id判断有没有
 *通过返回值体现,11就是允许注册，10就是不能注册
 *21可以登录；20不能登录
 *31注销成功,30注销失败
 */

#include"myhead.h"

int shujuku(struct send_msg* s_msg,int num)
{
    if(num == 1)//注册
    {                        
        char sqlite3_buf[20];//存数据库导出的id
        int i;
        /*定义变量*/
        int row;
        int column;
        char **result;
        char sql[200] = " create table if not exists 'stu' (id text primary key, name text,pwd text); "
                        //" insert into stu values('1494238405','wuqifu','wuqifu123.');"
                        "select id from stu;";

        


        char sql_input[200];                                
        char *err = NULL;
        sqlite3 *db = NULL;
        
        /*开始调用函数*/
        int ret = sqlite3_open("info.db", &db);
        if (ret != SQLITE_OK)
        {
            printf("open error");
            return -1;
        }

        sqlite3_get_table(db,sql,&result,&row,&column,&err);
        if (err)
        {
            printf("err:%s\n",err);
            sqlite3_close(db);
            return -1;
        }
        
        for (i = 1; i <= row * column; i++)
        {
            sprintf(sqlite3_buf,"%s",result[i]);
            if(strcmp(sqlite3_buf,s_msg->id) != 0)
            {
                ;
            }
            else//id存在
            {
                sqlite3_free_table(result);
                sqlite3_close(db);
                return 10;
            }
            memset(sqlite3_buf,0,sizeof(sqlite3_buf));
        }

        ////id不存在,把数据导入数据库
        sprintf(sql_input,"insert into stu values(\"%s\",\"%s\",\"%s\");",s_msg->id,s_msg->name,s_msg->pwd);
        sqlite3_exec(db,sql_input,NULL,NULL,&err);
        memset(sql_input,0,sizeof(sql_input));

        sqlite3_free_table(result);
        sqlite3_close(db);
        return 11;
    }

    else if(num == 2)//登录
    {
        char sqlite3_buf[20];//存数据库导出的id或pwd
        int i;
        /*定义变量*/
        int row;
        int column;
        char **result;//指针数组
        char sql[200] = "select id,name, pwd from stu;";
                                                      
        char *err = NULL;
        sqlite3 *db = NULL;
        
        /*开始调用函数*/
        int ret = sqlite3_open("info.db", &db);
        if (ret != SQLITE_OK)
        {
            printf("open error");
            return -1;
        }

        sqlite3_get_table(db,sql,&result,&row,&column,&err);
        if (err)
        {
            printf("err:%s\n",err);
            sqlite3_close(db);
            return -1;
        }
        
        for (i = 3; i <= row * column; i+=3)
        {
            sprintf(sqlite3_buf,"%s",result[i]);
            if(strcmp(sqlite3_buf,s_msg->id) != 0)
            {
                ;
            }
            else
            {
                //中途找到了
                sprintf(sqlite3_buf,"%s",result[i+2]);
                if(strcmp(sqlite3_buf,s_msg->pwd) == 0)//pwd也匹配上了
                {
                        sprintf(sqlite3_buf,"%s",result[i+1]);
                        strcpy(s_msg->name,sqlite3_buf);
                    sqlite3_free_table(result);
                    sqlite3_close(db);
                    return 21;
                }
                else
                {
                    sqlite3_free_table(result);
                    sqlite3_close(db);
                    return 201;
                }
                
                
            }
            memset(sqlite3_buf,0,sizeof(sqlite3_buf));
        }
        //没找到

        sqlite3_free_table(result);
        sqlite3_close(db);
        return 200;
    }

    else if(num == 3)//注销账号
    {
        sqlite3 *db = NULL;
        int ret = sqlite3_open("info.db",&db);
        if(ret != SQLITE_OK)
        {
            printf("open error");
            return -1;
        }
        char *err = NULL;
        char sql[100];
        sprintf(sql,"delete from stu where id = \"%s\" ",s_msg->id);
        sqlite3_exec(db,sql,NULL,NULL,&err);
        if(err)
        {
            sqlite3_close(db);
            return 30;
        }
        sqlite3_close(db);
        return 31;
    }

    else if(num == 4)//插入私聊群聊记录
    {
        sqlite3 *db = NULL;
        int ret = sqlite3_open("info.db",&db);
        if(ret != SQLITE_OK)
        {
            printf("open error");
            return -1;
        }
        char *err = NULL;
        char sql[200] = "create table if not exists 'siliao'(time text,from_id text,from_name text,to_id text,msg text);";
        char sql2[1536];
        time_t time_second;
        time(&time_second);
        char ti[50];
        strcpy(ti,ctime(&time_second));//默认以\n结尾
        char *p = strchr(ti,'\n');
        *p = '\0';
        sprintf(sql2," insert into siliao values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\" );",ti,s_msg->from,s_msg->from_name,s_msg->to,s_msg->massage);
        //printf("len:%ld|%s|\n",strlen(ti),ti);
                printf("%s\n",sql2);
        sqlite3_exec(db,sql,NULL,NULL,&err);
        if(err)
        {
            sqlite3_close(db);
            return 40;
        }
        sqlite3_exec(db,sql2,NULL,NULL,&err);
        if(err)
        {
            sqlite3_close(db);
            return 40;
        }
        sqlite3_close(db);
        return 41;
    }

    else if(num == 5)//查找群聊记录
    {
        int sockfd = s_msg->cmd;//借用cmd传入套接字
        s_msg->cmd = CMD_LOOK_RECORD_ALL;
        sqlite3 *db = NULL;
        int ret = sqlite3_open("info.db",&db);
        if(ret != SQLITE_OK)
        {
            printf("open error");
            return 50;
        }
        char *err = NULL;
        char sql[200];
        sprintf(sql,"select * from siliao where from_id = \"%s\" and to_id = \"all\";",s_msg->id);

        int i,j;
        int len;
        int row;
        int column;
        char **result;
        char record[1536];//临时存放聊天记录
        
        sqlite3_get_table(db,sql,&result,&row,&column,&err);
        if(err)
        {
            printf("%s\n",err);
        }

        if(row == 0)
        {
            strcpy(s_msg->record,"未查询到!");
            write(sockfd,s_msg,sizeof(*s_msg));//注意sizeof是*s_msg
            return 51;
        }

        for (i = 0; i <= row; i++)
        {
            for(j = 0;j<column;j++)//实现123 | 123 | 123 | 类似格式输出
            {
                //sprintf(record,"%s",result[i*column+j]);
                strcat(record,result[i*column+j]);
                char *p=record;
                while(*p != '\0')
                {
                    p++;
                }
                *p++ = ' ';
                *p++ = '|';
                *p++ = ' ';
                *p = '\0';
                p = record;
                
            }
            strcpy(s_msg->record,record);
            //printf("%s\n",s_msg->record);
            //开始发送
            write(sockfd,s_msg,sizeof(*s_msg));//注意sizeof是*s_msg
            memset(record,0,sizeof(record));
            bzero(s_msg->record,sizeof(s_msg->record));
        }

        sqlite3_free_table(result);       
        sqlite3_close(db);
        return 51;
    }

    else if(num == 6)//查找私聊记录
    {
        int sockfd = s_msg->cmd;//借用cmd传入套接字
        s_msg->cmd = CMD_LOOK_RECORD_ONE;
        sqlite3 *db = NULL;
        int ret = sqlite3_open("info.db",&db);
        if(ret != SQLITE_OK)
        {
            printf("open error");
            return 60;
        }
        char *err = NULL;
        char sql[200];
        sprintf(sql,"select * from siliao where (from_id = \"%s\" and to_id = \"%s\") or (from_id = \"%s\" and to_id = \"%s\") order by time;",s_msg->from,s_msg->to,s_msg->to,s_msg->from);
        //printf("%s\n",sql);
        int i,j;
        int len;
        int row;
        int column;
        char **result;
        char record[1536];//临时存放聊天记录
        
        sqlite3_get_table(db,sql,&result,&row,&column,&err);
        if(err)
        {
            printf("%s\n",err);
        }

        if(row == 0)
        {
            strcpy(s_msg->record,"未查询到!");
            write(sockfd,s_msg,sizeof(*s_msg));//注意sizeof是*s_msg
            return 61;
        }

        for (i = 0; i <= row; i++)
        {
            for(j = 0;j<column;j++)//实现123 | 123 | 123 | 类似格式输出
            {
                //sprintf(record,"%s",result[i*column+j]);
                strcat(record,result[i*column+j]);
                char *p=record;
                while(*p != '\0')
                {
                    p++;
                }
                *p++ = ' ';
                *p++ = '|';
                *p++ = ' ';
                *p = '\0';
                p = record;
                
            }
            strcpy(s_msg->record,record);
            printf("%s\n",s_msg->record);
            //开始发送
            write(sockfd,s_msg,sizeof(*s_msg));//注意sizeof是*s_msg
            memset(record,0,sizeof(record));
            bzero(s_msg->record,sizeof(s_msg->record));
        }

        sqlite3_free_table(result);       
        sqlite3_close(db);
        return 61;
    }

}