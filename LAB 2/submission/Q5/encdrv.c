#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>

#define MAX_CHARS_IN_LINE 1000

int main(int argc, char **argv)
{
	char buf[MAX_CHARS_IN_LINE+1];
	char obuf[MAX_CHARS_IN_LINE+1];

// NOTE: Do not modify anything above this line	
/***
 *      Your Code goes here
 */	
	

	int read_fd = open(argv[1], O_RDONLY);
	int write_fd = open(argv[2], O_CREAT |  O_WRONLY, 774);
	if(read_fd < 0 ||  write_fd < 0) {
		printf("Error\n");
	}
	int fd[2];
	pipe(fd);
		
	
	int curr = 0;
	char tbuff[32];
	while(read(read_fd, buf + curr, 1) == 1) { 
		if(buf[curr] == '\n') {
			sprintf(tbuff, "%d\n", curr + 1);
			int hehe = 0;
			while(tbuff[hehe]) hehe++;
			write(fd[1], tbuff, hehe);
			write(fd[1], buf, curr + 1);
			curr = 0;
			continue;
		}
		curr++;
	}
	tbuff[0] = '\0';
	write(fd[1], tbuff, 0);
	
	close(1);
	dup2(write_fd, 1);

// 	printf("here\n");
	close(fd[1]);
// 	close(fd[0]);
	
	pid_t child = fork();
	if(child < 0) {
		printf("Error\n");
	}
	if(child == 0) {
		close(0);
		dup2(fd[0], 0);
		execl("./encrypt", "encrypt", NULL);
	}


		

	return 0;
}

