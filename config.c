#include "common.h"

//服务器端口号
extern short port;
//www目录
extern char www_home[1024];

//读取配置文件path
//保存到全局变量www_home和port
//返回0正常，其他错误
int read_config(const char* path)
{
	//打开配置文件
	int configfd=open("/home/juju/study/qt02/day05/server/server.config",O_RDONLY);
	if(configfd==-1)
	{
		perror("配置文件打开失败");
		close(configfd);
		return 1;
	}
	//读取缓冲区
	char buf[BUFFERSIZE];
	//每次读取到的数据和总读取到的数据
	ssize_t read_bytes=0,total_bytes=0;
	//读取BUFFERSIZE个字节数据
	while(read_bytes=read(configfd,buf+total_bytes,BUFFERSIZE-total_bytes))
	{
		if(read_bytes==0)break;
		else if(read_bytes<0)
		{
			perror("读取配置文件错误");
			close(configfd);
			return 2;
		}
		else
		{
			total_bytes+=read_bytes;
		}
	}
	//关闭文件描述符
	close(configfd);
	//获取www主目录和端口号,写入到全局变量www_home,port
	sscanf(buf,"www_home %s\r\nport %d",www_home,&port);
	printf("www_home %s\nport %d\n",www_home,port);
	return 0;
}