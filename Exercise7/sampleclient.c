//Name - Ankur Vineet
//Id - 2018H1030144P

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/signal.h>
#include<arpa/inet.h>
#include<string.h>

#define MSGLENGTH 256

void handlechild(int sig)
{
	int status;
	wait(&status);
	printf("Server Exited\n");
	exit(0);
}

int main(int argc, char **argv)
{
	int sockfd,port;
        struct sockaddr_in serveraddr;
	if(argc < 3)
	{
		printf("Specify IP Address and Port Number\n");
		exit(1);
	}
	port = atoi(argv[2]);
        if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
                perror("socket: ");
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(port);
        //inet_pton(AF_INET,argv[1],&serveraddr.sin_addr);
        serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
        if( connect(sockfd,(struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
        {
		perror("connect: ");
		exit(1);
	}
	int pid = fork();
	if(pid == 0)
	{
		int nb;
		char msg[MSGLENGTH];
		while((nb = recv(sockfd, msg, MSGLENGTH, 0)) > 0)
		{
                	msg[nb] = '\0';
                	printf("Message Received from Server: %s",msg);
			fflush(stdout);
		}
		close(sockfd);
		exit(0);
	}
	else
	{
		signal(SIGCHLD,handlechild);
		while(1)
		{
			char msg[MSGLENGTH];
			printf("Enter Message: \n");
			fgets(msg,MSGLENGTH,stdin);
			send(sockfd, msg, strlen(msg), 0);
		}

	}
	return 0;
}
