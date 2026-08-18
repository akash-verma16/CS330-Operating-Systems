#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

// Use Library function such as fopen, geline, fread etc
// use manpages as mentioned in the writeup. 
// -----------------------
//
//
// Your solution begins here

int num_characters(int nread, char *line) {
	return nread;
}

int num_words(int nread, char *line) {
	int count = 0;
	for(int i = 0; i < nread; i++) {
		while(i < nread && (line[i] == ' ')) {
			i++;
		}
		if(line[i] == '\n') break;
		while(i < nread && line[i] != ' ') i++;
		count++;
	}
	return count;
}

int num_lines(int nread, char *line) {
	return 1;
}


int main(int argc, char *argv[]) {
	char **argptr = &argv[2], **mode = &argv[1];
	FILE* file = fopen(*argptr, "r");
	if((strcmp(*mode, "-l") != 0 && strcmp(*mode, "-w") != 0 && strcmp(*mode, "-c") != 0)) return 1;	
	char*line = NULL;
	size_t len = 0;
	ssize_t nread;
	int ans = 0;

	while((nread=getline(&line, &len, file)) != -1) {
		// printf("%ld,%s",nread,line);
		if((*mode)[1] == 'l') ans += num_lines(nread, line); 
		if((*mode)[1] == 'c') ans += num_characters(nread, line); 
		if((*mode)[1] == 'w') ans += num_words(nread, line); 
	}
	printf("%d", ans);
	free(line);
	fclose(file);
	
	return 0;
}

