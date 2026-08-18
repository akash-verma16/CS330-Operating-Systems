#include <fork.h>
#include <page.h>
#include <mmap.h>
#include <apic.h>

/* #################################################*/

static inline void invlpg(unsigned long addr) {
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}
/**
 * cfork system call implemenations
 */


void copy_pfn(struct exec_context *ctx, struct exec_context *new_ctx, u64 parent_pfn, u64 kid_pfn, int curr, u64 addr) {
    
    if(curr == 4) {
	for(int i = 0; i < 512; i++) {
//		printk("curr 4, i = %d\n", i);
       		*((u64*)(osmap(kid_pfn) + (i << 3))) = *((u64*)(osmap(parent_pfn) + (i << 3)));
		u64* tt = ((u64*)(osmap(kid_pfn) + (i << 3)));
	        
		if((*tt) & 2) *tt -= 2; 
		if((*tt) & 4) *tt -= 4;	
		if(((*((u64*)(osmap(kid_pfn) + (i << 3)))) & 1) == 0) continue;
		u64 *hehe = ((u64*)(osmap(parent_pfn) + (i << 3)));
//		printk("parent data pfn %x\n", ((hehe) ));
		u64 new_addr = (addr << 9) + i;
		new_addr = (new_addr << 12);
//		if (new_addr == 0x7FFFFF000) continue;	
//		printk("Address is %x\n", new_addr);
		invlpg(new_addr);
		if((*hehe) & 8) {
			*hehe = *hehe - 8;
			*((u64*)(osmap(kid_pfn) + (i << 3))) = *hehe; 
		}
		tt = ((u64*)(osmap(kid_pfn) + (i << 3)));

		if((*tt) & 2) *tt -= 2; 
		if((*tt) & 4) *tt -= 4;	
		u64 pfn = ((*hehe) >> 12);
//		printk("%x\n",((*hehe)&1));
//		printk("%x\n", pfn);
		s8 f = get_pfn(pfn);
//		printk("new refcount: %d\n", f);
	}
//	printk("exited\n");
 //   	printk("Inside kernel 4\n");
    	return;	    
    }
    
    // printk("Inside kernel\n");
    int mi, ma;
    if(curr == 1) {
    	mi = 0;
	ma = 1;
    } else if(curr == 2) {
	mi = 4;
	ma = 32;
    } else {
    	mi = 0;
       	ma = 512;
    }
    for(int i = mi; i < ma; i++) {
       *((u64*)(osmap(kid_pfn) + (i << 3))) = *((u64*)(osmap(parent_pfn) + (i << 3)));
	u64* tt = ((u64*)(osmap(kid_pfn) + (i << 3)));
	if((*tt) & 2) *tt -= 2;
		if((*tt) & 4) *tt -= 4;	
       if( *((u64*)(osmap(kid_pfn) + (i << 3))) & 1) {
	       // invlpg((u64)(osmap(kid_pfn) + (i << 3)));
	       u64 new_page = os_pfn_alloc(OS_PT_REG);
	       u64 curr_parent_pfn = (*((u64*)(osmap(parent_pfn) + (i << 3))) >> 12);
	       u64 temp = *((u64*)(osmap(parent_pfn) + (i << 3)));
	       temp = temp - ((temp >> 12) << 12);
	       temp = (temp + (new_page << 12)); 
	       *((u64*)(osmap(kid_pfn) + (i << 3))) = temp; 
		 tt = ((u64*)(osmap(kid_pfn) + (i << 3)));
	if((*tt) & 2) *tt -= 2;
		if((*tt) & 4) *tt -= 4;	
	       u64 new_addr = (addr << 9) + i;
	       copy_pfn(ctx, new_ctx, curr_parent_pfn, new_page, curr + 1, new_addr);
       }	       
    }
}

long do_cfork(){
    u32 pid;
    struct exec_context *new_ctx = get_new_ctx();
    struct exec_context *ctx = get_current_ctx();
     /* Do not modify above lines
     * 
     * */   
     /*--------------------- Your code [start]---------------*/
    pid = new_ctx -> pid;
    *(new_ctx) = *(ctx);
    new_ctx -> pid = pid;
    new_ctx -> ppid = ctx -> pid;
    (new_ctx -> regs).rax = 0;
    u64 new_first = os_pfn_alloc(OS_PT_REG);
    new_ctx -> pgd = new_first;
    copy_pfn(ctx, new_ctx, ctx -> pgd, new_ctx -> pgd, 1, 0);

     /*--------------------- Your code [end] ----------------*/
    
     /*
     * The remaining part must not be changed
     */
    copy_os_pts(ctx->pgd, new_ctx->pgd);
    do_file_fork(new_ctx);
    setup_child_context(new_ctx);
    reset_timer();

    return pid;
}


/* Cow fault handling, for the entire user address space
 * For address belonging to memory segments (i.e., stack, data) 
 * it is called when there is a CoW violation in these areas. 
 */

long handle_cow_fault(struct exec_context *current, u64 vaddr, int access_flags)
{
	long retval = -1;
	invlpg(vaddr);
	if((access_flags & PROT_WRITE) == 0) return -1;

	u64* phy_addr = (u64*)((u64)current -> pgd);
	
	u64* vir_addr = (u64*) (osmap((u64)phy_addr)); 
	phy_addr = vir_addr + (vaddr >> 39);

	vir_addr = (u64*) (osmap((*phy_addr) >> 12));
	phy_addr = vir_addr + ((vaddr >> 30) - ((vaddr >> 39) << 9));

	vir_addr = (u64*) (osmap((*phy_addr) >> 12));
	phy_addr = vir_addr + ((vaddr >> 21) - ((vaddr >> 30) << 9));

	vir_addr = (u64*) (osmap((*phy_addr) >> 12));
	phy_addr = vir_addr + ((vaddr >> 12) - ((vaddr >> 21) << 9));
	
	u64 fin_pfn = ((*phy_addr) >> 12);

	//printk("%x\n", fin_pfn);
	s8 temp = get_pfn(fin_pfn);
	if(temp == 2) {
		put_pfn(fin_pfn);
		if((*phy_addr & 8) == 0) *phy_addr += 8;
		return 1;
	}
	put_pfn(fin_pfn);

	temp = put_pfn(fin_pfn);

	u64 new_pfn = os_pfn_alloc(USER_REG);
	*phy_addr -= (((*phy_addr) >> 12) << 12);
	*phy_addr +=(new_pfn << 12);
	if(((*phy_addr) & 8) == 0) (*phy_addr) += 8;

	u64 *parent_address = (u64*)(osmap(fin_pfn));
	u64 *kid_address = (u64*)(osmap(new_pfn));

	for(int i = 0; i < 512; i++) {
		*(kid_address + i) = *(parent_address + i);
	}
  
	return 1;
}
