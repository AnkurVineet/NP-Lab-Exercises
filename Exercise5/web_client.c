//Name - Ankur Vineet
//Id - 2018H1030144P

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<sys/time.h>

#define BUFNAME 100
#define BUFDATA 1024
void parseurl(char *url, char *host, char *object)
{
	int i,j=0;
	for(i = 0; i < strlen(url); i++)
	{
		if(url[i] == '/')
			break;
		host[i] = url[i];
	}
	if(url[i] == '\0')
		object[0] = '/';
	else{
		for(;i < strlen(url); i++)
		{	
			object[j] = url[i];
			j++;
		}
	}
	//printf("Host: %s\n",host);
	//printf("Object: %s\n",object);
}

int connectbyname(char *host)
{
	int fd,n;
	time_t start,end;
	struct addrinfo  *res, *ressave;
	if((n = getaddrinfo(host,"http",NULL, &res)) != 0)
	{
		printf("%s\n",gai_strerror(n));
		exit(0);
	}
	ressave = res;
	do
	{
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(fd < 0)
			continue;
		if( connect(fd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		close(fd);
	}
	while((res = res->ai_next) != NULL);
	freeaddrinfo(ressave);
	if(res == NULL)
	{
		perror("connect: ");
		exit(0);
	}
	return fd;
}
int main(int argc,char *argv[])
{
	char url[BUFNAME];
	char data[BUFDATA];
	char host[BUFNAME],object[BUFNAME];
	char request[BUFDATA];
	//time_t begin,end;
	struct timeval begin,end;
	bzero(url,BUFNAME);
	bzero(host,BUFNAME);
	bzero(object,BUFNAME);
	int n;
	if(argc < 2)
	{
		printf("Enter URL of Website\n");
		printf("Eg. www.google.com\n");
		exit(0);
	}
	strcpy(url,argv[1]);
	parseurl(url,host,object);
	gettimeofday(&begin,NULL);
	int sfd = connectbyname(host);
	strcpy(request,"GET ");
	strcat(request,object);
	strcat(request," HTTP/1.1\r\n");
	strcat(request,"Host: ");
	strcat(request,host);
	strcat(request,"\r\n");
	strcat(request,"Connection: close\r\n\r\n");
	printf("Request: %s",request);
	write(sfd,request,BUFDATA);
	bzero(data,BUFDATA);
	while((n = read(sfd,data,BUFDATA)) != 0)
	{
		data[n] = '\0';
		printf("%s",data);
		bzero(data,BUFDATA);
	}
	gettimeofday(&end,NULL);
	double time_taken;
    	time_taken = (end.tv_sec - begin.tv_sec) * 1e6;
    	time_taken = (time_taken + (end.tv_usec -
                              begin.tv_usec)) * 1e-6;
	printf("\n\nTime Elapsed During Request: %f seconds.\n\n",time_taken);
	return 0;
}
