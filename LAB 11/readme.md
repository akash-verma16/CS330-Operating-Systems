## Lab 11: Threads & Copy-on-Write

### Q1. Threads of Execution! (`clone_threads.c`)
Implement the `do_clone` system call to create kernel-level threads that share the parent's address space. You must allocate a new execution context, copy relevant fields (file descriptors, signal handlers), but set up a distinct user stack and instruction pointer based on the arguments. You also need to update `do_exit` to correctly handle thread termination without killing the entire process unless it's the last thread.

### Q2. Order, Order! Lock the culprit (`clone_threads.c`)
Implement semaphore synchronization primitives (`sem_init`, `sem_wait`, `sem_post`). You must manage a per-process array of locks, each protected by a spinlock. The `sem_wait` function must decrement the value or add the process to a wait queue if 0, while `sem_post` must increment the value and wake up the head of the wait queue if necessary.

### Q3. Copy-on-Write fork (`cfork.c`)
Implement `cfork`, a variant of `fork` that uses Copy-on-Write (CoW) optimization. Instead of duplicating memory immediately, you must mark all user-space pages as read-only in both parent and child page tables and increment their reference counts. You must also implement `handle_cow_fault` to catch write attempts to these pages, allocate new physical frames, copy the data, and restore write permissions only for the faulting process.