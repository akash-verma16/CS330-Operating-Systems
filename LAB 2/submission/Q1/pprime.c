#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<sys/time.h>
#include<sys/wait.h>


extern int isPrime(long x); // Returns zero if composite.
                            // Returns one if prime. You may use this if you want. 
int find_primes(long M)
{


		int status1, status2;

		pid_t child1, child2;
		child1 = fork();
		if(child1 < 0) {
			perror("child 1 not made");
			exit(-1);
		} else if(child1 == 0) {
			int x = 0;
			for(int i = 2; i <= M/2; i++) {
				if(isPrime(i)) (x)++;
			}
			
			exit(x%256);
		} else {
			
			child2 = fork();
			if(child2 < 0) {
				perror("child 2 not made");
			} else if(child2 == 0) {
				int x = 0;
				for(int i = M/2 + 1; i <= M; i++) {
					if(isPrime(i)) (x)++;
				}
				exit(x%256);
			} else {
				//parent
				wait(&status1);
				wait(&status2);
				

				
				return (WEXITSTATUS(status1) + WEXITSTATUS(status2))%256;
			}
		}


}
