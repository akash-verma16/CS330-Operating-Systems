## Lab 7: Trace Buffer & Memory Validation

### Q1. Trace Buffer (`tb.c`)
Implement a custom character device-like abstraction called a "Trace Buffer" which acts as a unidirectional pipe associated with a single file descriptor. You must manage the file object (`struct file`), allocate the internal buffer (4KB), and implement the `read`, `write`, `close`, and `create_trace_buffer` operations. The read/write logic must handle circular wrapping of data (ring buffer semantics) and maintain correct Read (`R`) and Write (`W`) offsets.

### Q2. Checking validity of user buffer (`tb.c`)
Secure the read/write operations by implementing `tb_validate` to verify user-supplied buffer addresses. Before any data transfer, you must ensure the entire buffer range `[buff, buff + count)` resides strictly within valid memory segments (CODE, DATA, STACK) or allocated VMAs (`vm_area`) and possesses the correct permissions (Read vs. Write) as per the process's address space layout.