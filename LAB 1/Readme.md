## Lab 1: Problem Descriptions

### Q1. Count it! (`word_count.c`)
Recreate the Unix `wc` utility to count lines, words, or characters in a file based on command-line flags (`-l`, `-w`, `-c`) passed to the binary. Unlike the standard tool, your implementation must count lines even if they strictly lack a terminating newline character. The output must be a single raw integer without any accompanying text or whitespace. You are required to implement this using standard C library functions for file I/O and string manipulation.

### Q2. Traversing the Meta! (`solution.c`)
Traverse a Ring Buffer containing variable-sized events from the `data_tail` (consumer) to the `data_head` (producer). You must parse the `perf_event_header` of each entry to identify its type and perform specific actions: 
1. Print the hex address for `PERF_RECORD_SAMPLE` events.
2. Sum the lost counts for `PERF_RECORD_LOST` events.
3. Calculate and output the combined size in bytes of all "unknown" events that do not match these two specific types.