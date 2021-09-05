//Name - Ankur Vineet
//Id - 2018H1030144P
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#define P_MSG 20
int *pidlist;
int n,msgqid,msgqidreply;
void handle(int sig)
{
	int status;
	for(int i = 0 ; i < n ; i++)
	{
		kill(pidlist[i],SIGTERM);
		wait(&status);
	}
	msgctl(msgqid,IPC_RMID,NULL);
	msgctl(msgqidreply,IPC_RMID,NULL);
	exit(0);
}
struct parentmbuf{
	long mtype;
	char mtext[P_MSG];
};
struct vote{
	long mtype;
	char mtext[P_MSG];
};
int main(int argc, char **argv)
{
	int status,pid;
	int *vote;	
	struct parentmbuf pmsg;
	struct vote vmsg;
	if(argc < 2)
	{
		printf("Specify Number of Child Processes\n");
		exit(0);
	}
	char *num = argv[1];
	n = atoi(num);
	if(n <= 0)
	{
		printf("At Least One Child Process Required\n");
		exit(0);
	}
	pidlist = (int *)malloc(n*sizeof(int));
	vote = (int *)malloc(n*sizeof(int));
	msgqid = msgget(IPC_PRIVATE,IPC_CREAT|S_IRUSR|S_IWUSR);
	msgqidreply = msgget(IPC_PRIVATE,IPC_CREAT|S_IRUSR|S_IWUSR);
	if(msgqid == -1)
		perror("msgget: ");
	for(int i = 0; i < n ; i++)
	{
		pid = fork();
		if(pid < 0)
			printf("Fork Error\n");
		else if(pid == 0)
		{
			printf("Child %d created\n",i);
			while(1)
			{
				msgrcv(msgqid,&pmsg,P_MSG,getpid(),0);
				printf("Child with Pid: %d Received: %s\n",getpid(),pmsg.mtext);
				vmsg.mtype = getpid();
				srand(time(0)+getpid());
				int rnum = rand()%2;
				if(rnum == 1){
					printf("Child With Pid: %d Voted: %d\n",getpid(),1);
					memcpy(vmsg.mtext,"1",P_MSG);
				}
				else{	
					printf("Child With Pid: %d Voted: %d\n",getpid(),0);
					memcpy(vmsg.mtext,"0",P_MSG);
				}
				if(msgsnd(msgqidreply,&vmsg,P_MSG,0) == -1)
					perror("Child msgsend: ");
			}
			exit(0);
		}
		else
		{
			pidlist[i] = pid;
		}
	}
	signal(SIGINT,handle);
	while(1)
	{
		for(int i = 0; i < n ; i++)
		{
			pmsg.mtype = pidlist[i];
			memcpy(pmsg.mtext,"Start Voting",P_MSG);
			if(msgsnd(msgqid,&pmsg,P_MSG,0) == -1){
				perror("Parent msgsend: ");
			}
		}
		int onecount = 0;
		for(int i = 0 ; i < n ; i++)
		{
			msgrcv(msgqidreply,&vmsg,P_MSG,pidlist[i],0);
			printf("Parent Received Vote: %s from Child with PID: %d\n",vmsg.mtext,pidlist[i]);
			vote[i] = atoi(vmsg.mtext);
			if(vote[i] == 1)
				onecount++;
	
		}
		if(onecount > n/2)
			printf("Majority of 1s -> Accept\n\n");
		else if(onecount == n/2 && n%2 == 0)
			printf("Both Equal -> Reject\n\n");
		else
			printf("Majority of 0s -> Reject\n\n");
		/*Uncomment Following Line To Comfortably View the Output*/
		//sleep(1);
	}
	for(int i = 0; i < n ; i++)
	{
		wait(&status);
		printf("Child %d Stopped\n",i);
	}
	msgctl(msgqid,IPC_RMID,NULL);
	msgctl(msgqidreply,IPC_RMID,NULL);
	return 0;
}
