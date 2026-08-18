#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<sys/time.h>
#include<sys/wait.h>

int main(int argc, char **argv)
{

	if(argc == 2) {
			
			*(argv + 2) = "1";
			*(argv + 3) = NULL;
	}
			int fact = atoi(*(argv + 1));
			int curr_prod = atoi(*(argv + 2));
			
			curr_prod *= fact;
			fact--;
			char *temp1 = (char*)malloc(32), *temp2 = (char*)malloc(32);
			sprintf(temp1, "%d", fact);
			
			sprintf(temp2, "%d", curr_prod);
		       *(argv + 1)= temp1;
	       		*(argv + 2) = temp2;	       
			if(fact == 1){
				printf("%d\n", curr_prod);
			} else {
				execve("./fact", argv, NULL);	
			}

		return 0;
}
