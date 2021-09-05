//Name - Ankur Vineet
//Id - 2018H1030144P

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/signal.h>
#include<sys/wait.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/stat.h>

int semid;
void handlechild(int sig)
{
	int status;
	waitpid(-1,&status,WNOHANG);
}
void cleanup(int sig)
{
    if (semctl (semid, 0, IPC_RMID) == -1){
        perror ("semctl :"); exit (1);
    }
    exit(0);
}
int main(int argc, char *argv[])
{
	int lfd,connfd,pid;
	if(argc < 2)
	{
		printf("Specify MAX number of clients.\n");
		exit(0);
	}
	int n = atoi(argv[1]);
	struct sembuf sop;
	if((semid = semget(IPC_PRIVATE, 1, S_IRUSR|S_IWUSR)) == -1)
		perror("semid: ");
	if(semctl(semid, 0, SETVAL, n) == -1)
		perror("semctl: ");
	struct sockaddr_in serveraddr;
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(8080);
	if(bind(lfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) == -1)
		perror("bind: ");
	listen(lfd,n);
	signal(SIGCHLD,handlechild);
        while(1)
	{
		sop.sem_num = 0;
		sop.sem_op = -1;
		sop.sem_flg = 0;
		if(semop(semid,&sop,1) == -1)
		{
			if(errno == EINTR)
				continue;
		}
		connfd = accept(lfd,NULL,NULL);
		if((pid = fork()) == 0)
		{
			signal(SIGINT,SIG_DFL);
			close(lfd);
			printf("Client Allocated to PID:%d\n",getpid());
			sleep(5);	//Simulating Client Processing
			//For Echo Request Support Uncomment Following
			/*char recv[100];
			while((n = read(connfd,recv,100)) > 0)
			{
				recv[n] = '\0';
				fputs(recv,stdout);
			}*/
			sop.sem_num = 0;
			sop.sem_op = 1;
			sop.sem_flg = 0;
			semop(semid,&sop,1);
			exit(0);
		}
		else{
			signal(SIGINT,cleanup);
		}
		close(connfd);
	}
	return 0;
}
