#include "common.h"
extern short port;
//开启tcp套接字并设置参数
//需要port端口
//成功返回服务端套接字id，-1失败
int start_server()
{
	//服务器和客户端地址
	struct sockaddr_in server_addr;
	//打开套接字
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd==-1)
	{
		perror("socket");
		return -1;
	}
	//忽略错误“address already in use”
	int on=1;
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	//设置服务器协议,地址和端口
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	//绑定服务器地址和端口号到socket
	if(bind(sfd,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1)
	{
		close(sfd);
		perror("bind");
		return -1;
	}
	//设置socket为监听模式
	if(listen(sfd,10)==-1)
	{
		perror("listen");
		return -1;
	}
	return sfd;
}
//运行服务，不会退出
//循环监听进入sfd的请求
void run_server(int sfd)
{
	//客户端地址
	struct sockaddr_in client_addr;
	socklen_t c_addr_size;
	//客户端socket
	int cfd;
	//子线程id
	pthread_t tid;
	//循环监听进入的连接
	while(1)
	{
		cfd=accept(sfd,(struct sockaddr*)&client_addr,&c_addr_size);
		if(cfd==-1)
		{
			close(cfd);
			perror("accept");
			continue;
		}
		printf("新连接：%d\n",cfd);
		//开启新线程处理新连接
		if(pthread_create(&tid,NULL,deal_with_cfd,(void*)&cfd)!=0)
		{
			close(cfd);
			perror("pthread_create");
			continue;
		}
		//线程分离,资源不再需要主线程回收，而是由系统回收
		pthread_detach(tid);
	}
}

//线程体
//接收客户端套接字fd，接收请求并返回响应
void* deal_with_cfd(void* arg)
{
	int cfd=*(int*)arg;
	printf("%ud线程处理连接%d\n",pthread_self(),cfd);
	//请求头结构体
	request_line_t line;
	//获得请求头
	if(0!=get_requset(cfd,&line))
	{
		perror("get_requset");
		return NULL;
	}
	printf("%ud线程处理连接%d\n",pthread_self(),cfd);
	//创建响应头结构体
	response_head_t response;
	//需要发送的文件路径
	char rpath[1024];
	//根据请求行生成响应头,并获得需要发送的文件的路径
	if(0!=generate_response_head(&line,&response,rpath))
	{
		perror("generate_response_head");
		return NULL;
	}
	//发送响应头和响应体文件给客户端
	send_response(response,rpath,cfd);
	printf("response:%d %s %s\n",response.status,response.protocol,response.file_type);
	//关闭客户端连接socket
	close(cfd);
	return NULL;
}