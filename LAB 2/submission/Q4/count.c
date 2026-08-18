#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int found(char *word, char *line, int wordsz, int linesz) {
	int fin = 0;
	
	int l = 0, r = 0;

	while(l < linesz) {
		while(line[l] >= '0' && line[l] <= '9') l++;
		l++;
		r = l;
		while(line[r] >= 'a' && line[r] <= 'z') r++;
		if(strncmp(line + l, word, wordsz) == 0) fin++;

		while(line[l] != '\n' && line[l] != '\0') l++;
		l++;
	}

	return fin;

	for(int start = 0; start <= linesz - wordsz; start++) {
		int same = 1; 
		for(int j = 0; j < wordsz; j++) {
			if(word[j] != line[start + j]) same =0;
		}
		if(same) fin++;
	}
	return fin;
}


int main (int argc, char **argv) {
	 if(argc != 2) {
                        printf("Unable to execute\n");
                        exit(-1);
       	 }

    
		
	int fb = open(argv[1], O_RDONLY);
	if(fb < 0) {
		printf("Unable to execute\n");
		return 0;
	}
	void * curr = malloc(1);
	char *file = (char*)(malloc(100000000));
	int curr_size = 0;
	file[0] = '\0';

	while(read(fb, curr, 1) == 1) {
        	file[curr_size] = *((char*)curr);
		file[curr_size + 1] = '\0';
		curr_size++;
	}
	

	int openatc, closec, readc, writec, statc, execc;

	openatc = found("openat(", file, 7, curr_size);	
	closec = found("close(", file, 6, curr_size);
	readc = found("read(", file, 5, curr_size);
	writec = found("write(", file, 6, curr_size);
	statc = found("stat(", file, 5, curr_size);
	execc = found("execve(", file, 7, curr_size);
	
	printf("openat: %d\n", openatc);
	printf("close: %d\n", closec);
	printf("read: %d\n", readc);
	printf("write: %d\n", writec);
	printf("stat: %d\n", statc);
	printf("execve: %d\n", execc);



	free(file);
	close(fb);	


    return 0;
}
