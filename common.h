#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<libgen.h>
#include<signal.h>
//请求行
typedef struct
{
	char method[12];//请求方法，get
	char path[128];//请求文件
	char protocol[20];//请求协议版本
}request_line_t;
//响应头
typedef struct
{
	int status;//状态码,200,404
	char protocol[20];//协议版本
	char file_type[32];//类型信息
}response_head_t;

//读取配置文件path
//保存到全局变量www_home和port
//返回0正常，其他错误
int read_config(const char* path);
//开启tcp套接字并设置参数
//需要port端口
//成功返回服务端套接字id，-1失败
int start_server();
//运行服务，不会退出
//循环监听进入sfd的请求
void run_server(int sfd);
//从socket读取请求数据，可重入
//并放入请求行结构体requset_line_t* line
//成功返回0，否则失败
int get_requset(int cfd,request_line_t* line);
//根据请求行生成响应头,读取了全局变量www_home
//line请求行，response响应头
//成功返回0，否则失败
int generate_response_head(request_line_t *line,response_head_t *response,char* rpath);
//安全写数据
int write_data(int fd,const char* buf,int size);
//发送响应头和响应体给客户端,响应头结构类似于
//HTTP/1.1  200 OK\r\n
//Content-Type: text/html\r\n
//\r\n
void send_response(response_head_t head,const char* rpath,int cfd);
//可重入函数,path文件路径
//根据文件后缀返回MIME文件类型
const char* get_file_type(const char *path);
//线程体函数，处理客户连接
void* deal_with_cfd(void* arg);
//缓冲区大小
#define BUFFERSIZE 1024