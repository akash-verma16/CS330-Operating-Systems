## Lab 2: System Calls (Process, File, Pipes)

### Q1. Parallel Primes (`pprime.c`)
Calculate the number of prime numbers up to a given integer $M$ using parallel processing. You must fork two child processes: one checking the range $0$ to $M/2$ and the other checking $M/2+1$ to $M$. The parent must wait for both children and return the total count modulo 256.

### Q2. Recursion without Recursive Functions (`fact.c`)
Compute the factorial of a number without using standard internal C recursion. Instead, the program must invoke itself using the `execl` system call, passing updated arguments for the next step. The implementation is restricted to performing only one multiplication operation per execution instance.

### Q3. Be a Seeker (`init.c`)
Implement a search utility that checks for the existence of a specific string within a file. Using low-level file I/O system calls (`open`, `read`, `lseek`), the program must traverse the file contents and print "FOUND" or "NOT FOUND" based on the result.

### Q4. Count the syscalls (`count.c`)
Parse a system trace file to count the occurrences of specific system calls: `openat`, `close`, `read`, `write`, `stat`, and `execve`. You must tally these calls regardless of whether they succeeded or failed (returned an error). The output must match a strict format listing the count for each syscall type.

### Q5. Piped Encryption (`encdrv.c`)
Create a driver program that encrypts a file by utilizing an external `encrypt` binary via a child process. You must set up pipes to send specific input formats (length + newline + text) to the child's standard input and capture the encrypted result from its standard output into a destination file.

### Q6. Parallel Primes. Again! (`cprime.c`)
Extend the prime counting logic to support dynamic parallelism. The program accepts an argument `num_cp` defining the number of child processes to launch. You must divide the range $M$ among these children, use pipes for Inter-Process Communication (IPC) to collect results, and return the exact total count of primes.

### Q7. Compact It! (`compact.c`)
Implement a memory compaction routine that moves valid data bytes to the beginning of a specified memory range, overwriting "holes" (null bytes). After compacting the data, you must adjust the end of the data segment using the `sbrk` system call to reflect the new, smaller size of the utilized memory.