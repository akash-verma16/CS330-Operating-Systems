#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<sys/mman.h>

long how_large(void *address)
{
  /*
   *  TODO: Your code
   */ 
	unsigned page_sz = 4096;
	long l = ((unsigned long)address)/page_sz;
	void *f = (void*)(l * page_sz);
	void *f_copy = f;
	void *sad = (void*)(-1);

	long  ans = 0;
	while(1) {
		void* temp =  mmap(f, 4096, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED_NOREPLACE, 0, 0);
		
		if(temp == sad) {
			ans += 4096;
			f += 4096;
		} else {
			munmap(f, 4096);
			break;
		}
	}
	f_copy -= 4096;
	while(1) {
		void* temp =  mmap(f_copy, 4096, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED_NOREPLACE, 0, 0);
		
		if(temp == sad) {
			ans += 4096;
			f_copy -= 4096;
		} else {
			munmap(f_copy, 4096);
			break;
		}
	}

	
	return ans; 
}
