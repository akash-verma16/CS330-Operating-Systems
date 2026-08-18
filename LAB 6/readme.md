## Lab 6: Exception Handling & System Hacks

### Q1. Changing Register Operands (`hacks.c`)
Modify the OS division-by-zero exception handler (`do_div_by_zero`) to fix faults by altering CPU registers on the fly. When the system call `sys_config_hs` sets the mode to `DIV_ZERO_OPER_CHANGE`, you must identify the faulting instruction's operand registers (using manual disassembly analysis) and modify their values in the captured user context (`regs`) to ensure the division result becomes zero, preventing the crash.

### Q2. Skipping the instruction (`hacks.c`)
Implement the `DIV_ZERO_SKIP` logic to bypass the faulting instruction entirely. You must manually determine the size of the division instruction for the given test cases. In the exception handler, advance the instruction pointer (`rip`) by this size so that execution resumes at the *next* instruction instead of retrying the faulty one.

### Q3. An exiting user space handler (`hacks.c`)
Handle the `DIV_ZERO_USH_EXIT` mode, where control is transferred to a registered user-space function instead of terminating immediately. You must modify the user context's `rip` to point to the user-provided handler address (`uhaddr`) and prepare the register state (specifically `rdi`) to pass the address of the faulting instruction as an argument to this handler.

### Q4. Skipping the culprit function (`hacks.c`)
Implement stack unwinding to skip the remainder of the current function and return immediately to its caller (`DIV_ZERO_SKIP_FUNC`). You must manipulate the user stack pointer (`rsp`) and base pointer (`rbp`) to discard the current stack frame and restore the saved return address into the instruction pointer (`rip`), ensuring the function "returns" the value 1.