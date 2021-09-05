//Name - Ankur Vineet
//Id - 2018H1030144P

#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINES 1000
#define MAX_BUF 200
int n;
int line = 0;
int *pidlist;
int *freelist;
char *filename;
int linenum;
int readfd;
int childfd;
char cread[MAX_BUF];
int nlines = 0;
int *lineoffset;
//Function to Notify That Child Process Started
void childfree(int sig, siginfo_t *siginfo, void *context)
{
	int pid = siginfo->si_pid;
	for(int i = 0 ; i < n ; i++)
	{
		if(pid == pidlist[i])
		{
			freelist[i] = 1;
			printf("Child %d Free Now.\n",i);
		}
	}	
}
//Function to Notify That Child Process Has Read A Line
void childread(int sig, siginfo_t *siginfo, void *context)
{
	int pid = siginfo->si_pid;
	for(int i = 0 ; i < n ; i++)
	{
		if(pid == pidlist[i])
		{
			freelist[i] = 1;
			line++;
			printf("Child %d Free After Reading line: %d\n",i,line);
		}
	}	
}
//Child Process Start Reading
void startread(int sig)
{
	char procfile[20];
	char procread[4];
	snprintf(procfile, 20, "%d.txt",getpid());		
	int procfd = open(procfile,O_RDONLY);
	read(procfd,&procread,4);
	linenum = atoi(procread);
	if(linenum == -1)return;
	memset(cread,0,MAX_BUF);
	//if(linenum != 0)
	lseek(childfd,lineoffset[linenum],SEEK_SET);
	if(linenum == nlines - 1)
		read(childfd,&cread,MAX_BUF);	
	else{	
		read(childfd,&cread,lineoffset[linenum + 1] - lineoffset[linenum] - 1);
		cread[lineoffset[linenum + 1] - lineoffset[linenum]-1] = '\0';
	}
	printf("Line Number: %d Bytes: %d N: %d -> Pid ",linenum+1,lineoffset[linenum + 1] - lineoffset[linenum] - 1,nlines);
	alarm(1);	
	//line++;	
}
//Child Process Print After Reading
void printline(int sig)
{
	printf("%d : %s\n",getpid(),cread);
	//printf("%d\n",lineoffset[1]);
	kill(getppid(),SIGUSR2);
	char procfile[20];
	char procwrite[4];
	snprintf(procfile, 20, "%d.txt",getpid());
	int freefd = open(procfile,O_WRONLY|O_TRUNC);
	sprintf(procwrite,"%d",-1);
	//printf("Edited File: %s\n",procwrite);
	write(freefd,&procwrite,4);
	close(freefd);
	//printf("line: %d\n",linenum);
	if(linenum == nlines - 2){
		//printf("Kill on line: %d %s\n",linenum,cread);
		//kill(getppid(),SIGINT);
	}
}
//Close All the Child Process and Exit Gracefully
void cleanup(int sig)
{
	int status;
	for(int i = 0; i < n ; i++)
  	{
		char procfile[20];
		snprintf(procfile, 20, "%d.txt",pidlist[i]);	
		kill(pidlist[i],SIGTERM);
       		wait(&status);
		unlink(procfile);
       		printf("Child %d Stopped.\n",i);
  	}
	exit(0);
}
int main(int argc, char **argv)
{	
  filename = argv[1];
  char *num = argv[2];
  if(argc < 3)
  {
	printf("Give File Name and N Child Processes\n");
 	exit(0);
  }
  n = atoi(num);
  if(n <= 0 ){
	printf("At Least One Child Process Required\n");
	exit(0);
  }
  int pid = -1;
  pidlist = (int *)malloc(n*sizeof(int));
  freelist = (int *)calloc(sizeof(int),n);
  readfd = open(argv[1],O_RDONLY);
  if(readfd == -1)
  {
	printf("No Such File Exists\n");
	exit(0);
  }
  char ch;
  lineoffset = (int *)calloc(sizeof(int),MAX_LINES);
  int chcount = 0;
  struct sigaction actionfree,actionread;
  memset(&actionfree, '\0', sizeof(actionread));
  actionfree.sa_sigaction = &childfree;
  actionfree.sa_flags = SA_SIGINFO;
  actionread.sa_sigaction = &childread;
  actionread.sa_flags = SA_SIGINFO;
  printf("I'am Parent With pid: %d\n",getpid());
  sigaction(SIGUSR1, &actionfree, NULL);
  sigaction(SIGUSR2, &actionread, NULL);
  lineoffset[0] = 0;
  nlines++;	
  while(read(readfd,&ch,1))
	{
	    chcount++; 
	    if(ch == '\n'){
		lineoffset[nlines] = chcount;
	    	nlines++;
            }	
	}
  printf("Total Lines to be read: %d\n",nlines-1);
  for(int i = 0 ; i < n; i++)
  {
      pid = fork();
      if(pid < 0)
       	 printf("Fork Error\n");
      else if(pid == 0)
      {
	 //sleep(1);
         printf("Child %d Created With pid: %d\n",i,getpid());
	 childfd = open(filename,O_RDONLY);
	 signal(SIGUSR2,startread);
	 signal(SIGALRM,printline);
	 kill(getppid(),SIGUSR1);
	 int occur = 0;
	 while(1)
         {
	    if(linenum == nlines - 2){
		if(occur != 0)printf("Press Ctrl+C for Cleanup!!\n");
		occur++;
		pause();
	    }	
	    else pause();
	    //printf("Signal Received\n");
	 }
	
	 //exit(0);
      }
      else
      {
         pidlist[i] = pid;
         freelist[i] = 0;
      }	
  }
  
  if(pid != 0)
  {
	
	//for(int i = 0 ; i < nlines ; i++)
	//	printf("Line: %d, offset: %d\n",i,lineoffset[i]);
	
	int sentline = 0;
	int procfd;
	while(sentline < nlines-1)
	{
		for(int i = 0; i < n ; i++)
		{
		   if(freelist[i] && sentline < nlines-1)
		   {
			//for(int i = 0 ;i < n; i++)
			//printf("Line: %d Free %d",sentline,freelist[i]);
			//printf("\n");
			freelist[i] = 0;
			char procfile[20];
			char procwrite[4];
			snprintf(procfile, 20, "%d.txt",pidlist[i]);		
			procfd = open(procfile,O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR | S_IWUSR);
			sprintf(procwrite,"%d",sentline);
			write(procfd,&procwrite,4);
			sentline++;
			kill(pidlist[i],SIGUSR2);
			close(procfd);
			//printf("Line: %d\n",line);		
		   }
		   else
		   {
			char procfile[20];
			char procread[4];
			snprintf(procfile, 20, "%d.txt",pidlist[i]);		
			int procfd = open(procfile,O_RDONLY);
			read(procfd,&procread,4);
			close(procfd);
			int stat = atoi(procread);
			if(stat == -1)
			{	
			//******Comment Out the Following Three Lines for Only Signal Based Synchronization*****//
				freelist[i] = 1;
				line++;
				printf("Child %d Synced After Reading line %d\n",i,line);			
			}			
			
		   }
		}
	}
	int prevline = -1;
	signal(SIGINT,cleanup);
	sigset_t blockSet,prevMask;
	sigemptyset(&blockSet);
	sigaddset(&blockSet,SIGUSR2);
	sigprocmask(SIG_BLOCK,&blockSet,&prevMask);
	//while(line < nlines - 1);
	//pause();
	while(line<nlines);
	
  }
 
  close(readfd);
  return 0;
}
