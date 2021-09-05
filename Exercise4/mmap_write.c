//Name - Ankur Vineet
//Id - 2018H1030144P

#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<string.h>
#include<math.h>

// It is possible to run the file with different block size by passing it through command line
// E.g. ./mmap_write <BLOCK_SIZE>
// Default value is 512

#define APPROX_FILE_SIZE 1024000000
long int MAX_BUF = 512;

int main(int argc,char *argv[])
{
	int i;
	if(argc > 1)
		MAX_BUF = atoi(argv[1]);
	char buf[MAX_BUF+1];
	for(i = 0; i < MAX_BUF; i++)
		buf[i] = (char)((i % 25) + 65);
	buf[i] = '\0';
	int t = clock();
	int fd = open("wfile.txt",O_CREAT|O_RDWR|O_TRUNC,0666);
	if(fd < 0)
		perror("open: ");
	for(int i = 0; i < APPROX_FILE_SIZE/MAX_BUF; i++)
	{
		write(fd,buf,strlen(buf));
	}
	close(fd);
	t = clock() - t;
	double timeelap = ((double)t)/CLOCKS_PER_SEC;
	printf("Time Taken by Write: %lf sec.\n",timeelap);
	remove("wfile.txt");
	t = clock();
	fd = open("wfile.txt",O_CREAT|O_RDWR|O_TRUNC,0666);
        if(fd < 0)
		perror("open: ");
	int size = ceil(APPROX_FILE_SIZE/MAX_BUF) * strlen(buf);
	ftruncate(fd,size);
	char *map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  	for(int i = 0 ; i < APPROX_FILE_SIZE/MAX_BUF; i++)
		memcpy(map+(strlen(buf)*i), buf, strlen(buf));
  	msync(map, size, MS_SYNC);
  	munmap(map, size);
	close(fd);
	t = clock() - t;
	timeelap = ((double)t)/CLOCKS_PER_SEC;
	printf("Time Taken by MMAP: %lf sec.\n",timeelap);
	remove("wfile.txt");
	return 0;
}
