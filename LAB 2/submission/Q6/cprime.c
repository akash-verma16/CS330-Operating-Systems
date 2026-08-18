#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/syscall.h>
extern int isPrime(long x); // Returns zero if composite.
                            // Returns one if prime. 

long find_primes(int num_cp, long M)
{
	int start = 1, end = 0;
	
	int **all_fd = (int **) (malloc(num_cp * sizeof(int*)));
	int *fd;
	for(int process_num = 0; process_num < num_cp; process_num++) {
		start = end + 1;
		if(process_num == num_cp - 1) end = M;
		else end = start + (M/num_cp) - 1;

		fd = (int*) malloc(2 * sizeof(int));
		//pipe(fd);
		syscall(SYS_pipe, fd);
		all_fd[process_num] = fd;

		pid_t new_pid = fork();

		if(new_pid == 0) {
			//child;
			int sum = 0;
			for(int i = start; i <= end; i++) {
				sum += isPrime(i);
			}
			
			char buf[32];
			buf[0] = '\0';

			sprintf(buf, "%d\n",sum);
			int sz = 0;
			while(buf[sz]) sz++;

			write(fd[1], buf, sz);
			exit(0);
		}
	}


	int fin_sum = 0;

	for(int i = 0; i < num_cp; i++) {
		wait(NULL);
	}

	for(int i = 0; i < num_cp; i++) {
		char buf[32];
		int curr = 0;
		while(read(*(all_fd[i]), buf + curr, 1) == 1) {
			if(buf[curr] == '\n') break;
			curr++;
		}
		buf[curr] = '\0';
		fin_sum += atoi(buf);
	}

	return fin_sum;

	//find primes
	return 0;   
}
