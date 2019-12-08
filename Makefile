serv:main.c config.c server.c request.c response.c
	gcc main.c config.c server.c request.c response.c -o serv -lpthread
	
clean:
	rm *.o serv