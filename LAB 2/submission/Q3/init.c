#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int found(char *word, char *line, int wordsz, int linesz) {
	int fin = 0;
	for(int start = 0; start <= linesz - wordsz; start++) {
		int same = 1;
		for(int j = 0; j < wordsz; j++) {
			if(word[j] != line[start + j]) same =0;
		}
		if(same) return 1;
	}
	return 0;
}

void strcatmy (char **tfile, char curr, int currsz) {
	char *file = *tfile;
	if(file == NULL) {
		file = (char *)malloc(1);
		file[0] = curr;
		return;
	}
	char * newfile = (char*)(malloc(currsz + 1));
	for(int i = 0; i < currsz; i++) newfile[i] = file[i];
	newfile[currsz] = curr;
	free(file);
	file = newfile;
}

int main (int argc, char **argv) {
	 if(argc != 3) {
                        printf("Error\n");
                        exit(-1);
       	 }

    
	char * word = argv[1];
	int wordsz = 0;
	while(word[wordsz]) wordsz++;
	
	int fb = open(argv[2], O_RDONLY);
	if(fb < 0) {
		printf("Error\n");
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
	

	
	int f = found(word, file, wordsz, curr_size);
	
	if(f) printf("FOUND\n");
	else printf("NOT FOUND\n");

	free(file);
	close(fb);	


    return 0;
}
