#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#define MAXLINE 200

int main(int argc, char **argv)
{
	if(argc !=2 )
	{
		printf("Enter Ip Address\n");
		exit(0);
	}
	int sockfd,n;
	char recvline[MAXLINE];
	struct sockaddr_in serveraddr;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
		perror("socket: ");
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8080);
	//inet_pton(AF_INET,argv[1],&serveraddr.sin_addr);
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	if( connect(sockfd,(struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		perror("connect: ");
	//Uncomment to Send Data	
	write(sockfd,"Hello",6);
	close(sockfd);
	return 0;
}
