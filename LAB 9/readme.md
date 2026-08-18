## Lab 9: Scheduling & Page Tables

### Q1: Scheduling Policy (`rr_schedule.c`)
Replace the default scheduler with a Round-Robin (RR) policy. You must manage a global linked list (`rr_list_head`) of runnable processes (`exec_context`). You are required to implement `rr_add_context` to append new processes, `rr_remove_context` to handle exits/sleeps, and `rr_pick_next_context` to select the next process, defaulting to the swapper (`pid 0`) if the list is empty.

### Q2. Examine the Page Table (`rr_schedule.c`)
Implement a page table walker (`do_walk_pt`) for the `walk_pt` system call. Given a virtual address, you must traverse the x86-64 4-level page table (PGD $\to$ PUD $\to$ PMD $\to$ PTE) starting from the current process's `pgd`. For each level, print the entry's virtual address, content, next-level physical address, and flags. If a mapping is missing at any level, report it and stop.