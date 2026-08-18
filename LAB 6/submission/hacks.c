#include<hacks.h>
#include<sigexit.h>
#include<entry.h>
#include<lib.h>
#include<context.h>
#include<memory.h>

struct hack_config{
	             long cur_hack_config;
		     u64 usr_handler_addr;
};

//Used to store the current hack configuration
static struct hack_config hconfig = {-1, -1};


//system call handler to configure the hack sematics
//user space connection is already created, You need to
//store the hack semantics in the 'hconfig' structure (after validation)
//which will be used when division-by-zero occurs

long sys_config_hs(struct exec_context *ctx, long hack_mode, void *uhaddr)
{

	hconfig.cur_hack_config = -1;
	hconfig.usr_handler_addr = -1;
	
	if(hack_mode != DIV_ZERO_OPER_CHANGE
	&& hack_mode != DIV_ZERO_SKIP 
	&& hack_mode != DIV_ZERO_USH_EXIT
	&& hack_mode != DIV_ZERO_SKIP_FUNC
	&& hack_mode != DIV_ZERO_MAX) {
		return -EINVAL;
	}

	if(hack_mode == DIV_ZERO_USH_EXIT) {
		if((u64)uhaddr < CODE_START || (u64)uhaddr >= RODATA_START) return -EINVAL;
	}
	
	hconfig.cur_hack_config = hack_mode;
	hconfig.usr_handler_addr = (long long unsigned int) uhaddr;

	return 0; 
}


/*This is the handler for division by zero
 * 'regs' is a structure defined in include/context.h which
 * is already filled with the user execution state (by the asm handler)
 * and will be restored back when the function returns 
 *
 */
int do_div_by_zero(struct user_regs *regs)
{	
	long hack_mode = hconfig.cur_hack_config;	
	if(hack_mode != DIV_ZERO_OPER_CHANGE
	&& hack_mode != DIV_ZERO_SKIP 
	&& hack_mode != DIV_ZERO_USH_EXIT
	&& hack_mode != DIV_ZERO_SKIP_FUNC
	&& hack_mode != DIV_ZERO_MAX) {
		printk("Error...exiting\n");
		do_exit(0);
	}

	
	if(hconfig.cur_hack_config == DIV_ZERO_OPER_CHANGE) {
		regs -> rax = 0;
		regs -> rcx = 1;
	}
	else if(hconfig.cur_hack_config == DIV_ZERO_SKIP) {
		regs -> entry_rip += 3;
	} else if (hconfig.cur_hack_config == DIV_ZERO_USH_EXIT) {
		regs -> rdi = regs -> entry_rip;
		regs -> entry_rip = hconfig.usr_handler_addr;
	} else if(hack_mode == DIV_ZERO_SKIP_FUNC) {
	//	regs -> = 1;
		regs -> rax = 1;
		regs -> entry_rsp = (u64)(((void *) regs -> rbp) + 8);
		regs -> rbp = *((u64*)(((void *) regs -> rbp)));
		regs -> entry_rip = *((u64*) regs -> entry_rsp);
		regs -> entry_rsp = (u64)(((void *) regs -> entry_rsp) + 8);

	}



	return 0;   	
}
