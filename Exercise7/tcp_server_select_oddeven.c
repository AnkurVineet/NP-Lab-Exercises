//Name - Ankur Vineet
//Id - 2018H1030144P

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define BUF_LEN 256

int main(int argc, char **argv)
{
	int lfd,acceptfd,maxfd,maxind = -1,port;
	int client[FD_SETSIZE];
	char buf[BUF_LEN];
	fd_set readfd, masterfd;
	struct sockaddr_in serveraddr;
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serveraddr, sizeof(serveraddr));
	if( argc < 2)
	{
		printf("Specify Port Number\n");
		exit(1);
	}
	port = atoi(argv[1]);
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serveraddr.sin_port = htons(port);
        if(bind(lfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) == -1)
                perror("bind: ");
        listen(lfd,10);
	FD_ZERO(&masterfd);
	FD_SET(lfd,&masterfd);
	maxfd = lfd;
	for( int i = 0 ; i < FD_SETSIZE; i++)
		client[i] = -1;
	while(1)
	{
		readfd = masterfd;
		if(select(maxfd+1, &readfd, NULL, NULL, NULL) == -1)
                        perror("select: ");
		if(FD_ISSET(lfd, &readfd))
                {
                	if((acceptfd = accept(lfd,NULL,NULL)) == -1)
                        	perror("accept: ");
                	else
                	{
                        	FD_SET(acceptfd, &masterfd);
                                if(acceptfd > maxfd)
                                	maxfd = acceptfd;
				int j;
				for( j = 0; j < FD_SETSIZE; j++)
				{	
					if(client[j] == -1)
					{
						client[j] = acceptfd;
						printf("Client %d With Fd %d\n",j,acceptfd);
						if(j > maxind)
							maxind = j;
						break;
					}
				}
                        }
                }
		else
		{
			//printf("maxind: %d\n",maxind);
			for(int i = 0 ; i <= maxind; i++)
			{
				//printf("id %d client[i] %d\n",i,client[i]);
				if(client[i] < 0)
					continue;
				if(FD_ISSET(client[i], &readfd))
				{
					int nb;
					bzero(buf,BUF_LEN);
                                        if((nb = recv(client[i], buf, BUF_LEN, 0)) == 0)
                                        {
                                                close(client[i]);
						FD_CLR(client[i], &masterfd);
						client[i] = -1;
                                        }
                                        else if(nb > 0)
                                        {
                                                buf[nb] = '\0';
                                                printf("Message Routed: %s",buf);
						for(int j = 0 ; j <= maxind; j++)
						{
							if(client[i]%2 == 0 && client[j]%2 != 0) 
								send(client[j], buf, strlen(buf), 0);
							else if(client[i]%2 != 0 && client[j]%2 == 0) 
								send(client[j], buf, strlen(buf), 0);
						}
                                        }
				}
			}	
		}
	}
	return 0;
}
