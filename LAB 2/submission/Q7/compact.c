#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void compact(void *start, void *end)
{
  /* 
   * TODO
   * Your code goes here
   */
	char *invalid = (char*)start;
	char * valid = (char*)invalid;

	while(invalid != end && valid != end) {
		// fix invalid and valid
		while(invalid !=  end && *invalid != '\0') invalid++;
		valid = invalid;
		while(valid != end && *valid == '\0') valid++;
		if (invalid == end || valid == end) break;
		*invalid = *valid;
		*valid = '\0';
	}

	
	int count = 0;
	char * temp = (char *)start;
	while(*temp != '\0') temp++;

	while(temp != end) {
		temp++;
		count++;
	}

	sbrk(-1 * count);

	return;    
}
