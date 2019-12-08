#include "common.h"
//服务器套接字
int sfd;
//服务器端口号
short port;
//www目录
char www_home[1024];
//自定义信号处理函数
void sig_handler(int signum)
{
	close(sfd);
}

int main(int argc,char* argv[])
{
	printf("%s\n",argv[0]);
	if(argc==2)//处理参数
	{
		if(!strcmp(argv[1],"-d"))
		{
			pid_t pid=fork();
			if(-1==pid)//创建子进程失败
			{
				perror("fork");
				return 1;
			}
			else if(0<pid)//在父进程中
			{
				printf("启动web服务pid:%d\n",pid);
				return 0;//父进程退出
			}
			else//子进程
			{
				//创建新的会话，获得新会话ID（就是本进程的ID）
				if(setsid()==-1)
				{
					perror("setsid");
					return 1;
				}
				chdir("/");
				umask(0);
				int fd=open("/dev/null",O_RDWR);
				if(fd==-1)
				{
					perror("open");
					return 1;
				}
				dup2(fd,0);
				dup2(fd,1);
				dup2(fd,2);
				
				//忽略SIGPIPE信号
				if(SIG_ERR==signal(SIGPIPE,SIG_IGN))
				{
					perror("signal");
				}
				//自定义SIGINT信号处理函数
				if(SIG_ERR==signal(SIGINT,sig_handler))
				{
					perror("signal");
				}
			}
		}
		else
		{
			printf("webserver服务器用法\n");
			printf("无参数运行为普通进程\n");
			printf("-d 运行为守护进程\n");
			printf("-h 查看帮助\n");
			return 0;
		}
	}
	//读取配置文件
	if(0!=read_config("server.config"))
	{
		perror("read_config");
		return 1;
	}
	//开启tcp服务器，返回sfd套接字
	sfd=start_server();
	if(-1==sfd)
	{
		perror("start_server");
		return 2;
	}
	//运行服务，循环监听不会退出
	run_server(sfd);
	return 0;
}
//安全的写入数据的函数
int write_data(int fd,const char* buf,int size)
{
	int w_once=0,w_total=0;
	while(1)
	{
		w_once=write(fd,buf+w_total,size-w_total);
		if(w_once==-1)
		{
			perror("write");
			return -1;
		}
		w_total=w_total+w_once;
		if(w_total>=size)break;
	}
	return 0;
}
