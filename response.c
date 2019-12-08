#include "common.h"

extern char www_home[1024];

//根据请求行生成响应头,读取了全局变量www_home
//line请求行，response响应头
//成功返回0，否则失败
int generate_response_head(request_line_t *line,response_head_t *response,char* rpath)
{
	//如果是目前支持的协议
	if(!strcmp(line->method,"GET")&&!(strcmp(line->protocol,"HTTP/1.1")||!strcmp(line->protocol,"HTTP/1.0")))
	{
		//如果没有带文件，就默认主页
		if(!strcmp(line->path,"/"))
		{
			strcpy(line->path,"/index.html");
		}
		//拼接文件真实路径
		sprintf(rpath,"%s%s",www_home,line->path);
		printf("文件真实路径：%s\n",rpath);
		//状态码
		if(access(rpath,F_OK)!=0)//如果不文件存在
		{
			response->status=404;
			sprintf(rpath,"%s/404.html",www_home);
		}
		else
			response->status=200;
		//协议版本与请求的一致
		strcpy(response->protocol,line->protocol);
		//文件类型
		strcpy(response->file_type,get_file_type(line->path));
		return 0;
	}
	else//协议不支持的情况
	{
		printf("目前不支持的动作：%s或协议：%s\n",line->method,line->protocol);
		response->status=404;
		sprintf(rpath,"%s/404.html",www_home);
		return 1;
	}
}

//发送响应头和响应体给客户端,响应头结构类似于
//HTTP/1.1  200 OK\r\n
//Content-Type: text/html\r\n
//\r\n
void send_response(response_head_t head,const char* rpath,int cfd)
{
	char line[1024];
	if(head.status==200)
	{
		sprintf(line,"%s 200 OK\r\nContent-Type: %s\r\n\r\n",head.protocol,head.file_type);
	}
	else//404
	{
		sprintf(line,"%s 404 Not Found\r\nContent-Type: text/html\r\n\r\n",head.protocol);
	}
	if(write(cfd,line,strlen(line))==-1)
	{
		perror("write");
		return;
	}
	//发送rpath文件
	int fd=open(rpath,O_RDONLY);
	if(fd==-1)
	{
		perror("open");
		return;
	}
	char buf[2048];
	int r,w,w_once;
	while(1)
	{
		r=read(fd,buf,2048);
		if(r==0)break;
		if(write_data(cfd,buf,r)==-1)return;
	}
	//关闭文件
	close(fd);
}

//可重入函数
//根据文件后缀获得MIME文件类型
const char* get_file_type(const char *path)
{
	char filepath[1024];
	//复制一个副本
	strcpy(filepath,path);
	//获得文件名
	char* filename=basename(filepath);
	//获得文件后缀
	char* suffix=strrchr(filename,'.');
	if(suffix==NULL)
	{
		printf("获取文件类型失败\n");
		return "";
	}
	if(!strcasecmp(".html",suffix))
	{
		return "text/html";
	}
	else if(!strcasecmp(".jpg",suffix)||!strcasecmp(".jpeg",suffix))
	{
		return "image/jpeg";
	}
	else if(!strcasecmp(".png",suffix))
	{
		return "image/png";
	}
	else if(!strcasecmp(".gif",suffix))
	{
		return "image/gif";
	}
	else if(!strcasecmp(".css",suffix))
	{
		return "text/css";
	}
	else if(!strcasecmp(".js",suffix))
	{
		return "application/javascript";
	}
	else
	{
		printf("不识别的文件类型\n");
		return "";
	}
	return "";
}