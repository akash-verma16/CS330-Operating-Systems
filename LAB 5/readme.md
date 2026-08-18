## Lab 5: gemOS & Process Control Block (PCB)

### Q1. General Information (`procinfo.c`)
Implement a handler for the `SYSCALL_PROC_INFO` system call to retrieve general process information. When the command `GET_PINFO_GEN` is received, you must populate the user-provided buffer with data from the Process Control Block (PCB), ensuring strict bounds checking on the buffer length.

### Q2. File information (`procinfo.c`)
Extend the system call handler to retrieve information about open file descriptors (`GET_PINFO_FILE`). You must iterate through the `files` array in the PCB, identify the type of each open file (e.g., STDIN, STDOUT), and populate an array of `struct file_info` in the user's buffer.

### Q3. Memory segment information (`procinfo.c`)
Implement the `GET_PINFO_MSEG` command to extract details about linear memory segments (CODE, DATA, STACK, etc.). You need to access the `mm_segment` array in the PCB, decode the access flags into a string format (e.g., "R-X"), and copy the segment metadata to the user space.

### Q4. VM (`procinfo.c`)
Handle the `GET_PINFO_VMA` command to export information about virtual memory areas (VMAs) created via `mmap`. You must traverse the linked list of `vm_area` structures in the PCB (skipping the dummy head), decode permissions, and serialize the VMA details into the user-provided buffer.