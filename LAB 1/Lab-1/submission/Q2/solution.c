#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
	if(argc != 2){
		perror("usage: ./solution <testcase number>");
		return -1;
	}
	buffer_init(atoi(argv[1]));
	const struct ring_buffer *rb = buffer_get_base();

	/* ------ YOUR CODE ENDS HERE ------*/

	u64 data_size = rb -> data_size;

	u64 tail = rb-> data_tail, head = rb -> data_head;

//	printf("%ld \n", data_size);
//	printf("%ld %ld \n", tail, head);

	void *curr = rb->data_base, *start = rb->data_base;

	
	u64 lost_count = 0, unknown_size = 0;
       //	printf("%ld %ld \n", tail, head);
	
	while(tail < head) {

		int temp = tail % (data_size);

		curr = start + temp;
		struct perf_event_header *curr_event_header = curr;
		//printf("% \n", (void*)(curr_event_header -> size));

		
		if(curr_event_header -> type == PERF_RECORD_SAMPLE) {
			struct sample_event *s_event = curr;
			
			printf("%p\n", (void*)(s_event -> addr));
		} else if(curr_event_header -> type == PERF_RECORD_LOST){
			struct lost_event *s_event = curr;
			lost_count += s_event -> lost;
		} else {
			unknown_size += curr_event_header -> size;
		}
		//curr += curr_event_header->size;
		tail += curr_event_header -> size;
	}


	/* print formats */
	// printf("0x%...."); // to print sample addresses
	printf("number of lost records: %ld\n", lost_count);
	printf("unknown size: %ld\n", unknown_size);

	/* ------ YOUR CODE ENDS HERE ------*/
	buffer_exit();
	return 0;
}

