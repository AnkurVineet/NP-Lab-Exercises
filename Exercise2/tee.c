//Name-Ankur Vineet
//ID - 2018H1030144P
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
int main(int argc, char **argv)
{
	char ch;
	int fds[argc-1];
	printf("Custom tee Output: \n");
	for(int i = 1 ; i < argc; i++)
	{
		fds[i-1] = open(argv[i],O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
	}
	while(read(STDIN_FILENO,&ch,1))
	{
		putchar(ch);
		for(int i = 1; i < argc; i++)
			write(fds[i-1],&ch,1);
	}
	return 0;
}
