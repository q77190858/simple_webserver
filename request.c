#include "common.h"

//从socket读取请求数据，可重入
//并放入请求行结构体requset_line_t* line
//成功返回0，否则失败
int get_requset(int cfd,request_line_t* line)
{
	//定义读取缓冲区
	char buf[BUFFERSIZE];
	//读取到的数据
	ssize_t read_bytes=0;
	//读取BUFFERSIZE个字节数据，不能读到文件结束（那是socket关闭）
	read_bytes=read(cfd,buf,BUFFERSIZE);
	if(read_bytes<0)
	{
		perror("读取请求数据错误");
		return 1;
	}
	//读取请求行数据
	sscanf(buf,"%s%s%s\r\n",line->method,line->path,line->protocol);
	printf("请求方法：%s\n请求文件：%s\n请求协议：%s\n",line->method,line->path,line->protocol);
	return 0;
}