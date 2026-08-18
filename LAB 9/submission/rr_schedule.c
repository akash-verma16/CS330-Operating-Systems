#include<context.h>
#include<page.h>
#include<memory.h>
#include<lib.h>


//////////////////////  Q1: RR Scheduling   ///////////////////////////////////////
//args:
//      ctx: new exec_context to be added in the linked list
void rr_add_context(struct exec_context *ctx)
{
	struct exec_context *curr = rr_list_head;
	if(curr == NULL) {
		rr_list_head  = ctx;
		rr_list_head -> next = NULL;
		return;
	}
	while(curr) {
		if(curr -> next == NULL) break;
		curr = curr -> next;
	}
	curr -> next = ctx;
	ctx -> next = NULL;
	return;
}

//args:
//      ctx: exec_context to be removed from the linked list
void rr_remove_context(struct exec_context *ctx)
{
	struct exec_context *curr = rr_list_head;
	if(curr == ctx) {
		rr_list_head  = curr -> next;
		return;
	}
	while(curr != NULL && curr -> next != ctx) curr = curr -> next;
	if(curr) {
		curr -> next = curr -> next -> next;
	}
	return;
}

//args:
//      ctx: exec_context corresponding the currently running process

/*
struct exec_context *rr_pick_next_context(struct exec_context *ctx)
{
	if(ctx -> pid == 0) {
		if(rr_list_head == NULL) return ctx;
		
		return rr_list_head;
	}	
	struct exec_context *curr = rr_list_head;
	while(curr != NULL && curr -> next != NULL && curr != ctx) {
		curr = curr -> next;
	}
	if(curr) { 
		if(curr -> next == NULL) return rr_list_head;
		else return curr -> next;
	} else {
		return get_ctx_by_pid(0);
	}
}
*/
struct exec_context *rr_pick_next_context(struct exec_context *ctx)
{
	if(rr_list_head == NULL) return get_ctx_by_pid(0);
	if(ctx == get_ctx_by_pid(0)) return rr_list_head;

	if(ctx -> next) return ctx -> next;
	return rr_list_head;
}

//////////////////////  Q2: Get the PAGE TABLE details for given address   ///////////////////////////////////////


//args:
//      ctx: exec_context corresponding the currently running process
//      addr: address for which the PAGE TABLE details are to be printed

int do_walk_pt(struct exec_context *ctx, unsigned long addr)
{
    u64 *vaddr_base = (u64 *)osmap(ctx->pgd);
    /*TODO*/
    u64  invalid = 0;

    u64 offset = (addr >> 39);
    u64 *next = (u64*)((u64)vaddr_base + offset*8);
    u64 one = 1;
    u64 val = ((*next) & ((one << 44) - 1));
    u64 flags = ((*next) & ((one << 12) - 1));
    invalid = (1 & flags);
    u64 phy_addr = ((val - flags) >> 12);
    if(!invalid) {
    	printk("No L1 entry\n");
    	printk("No L2 entry\n");
    	printk("No L3 entry\n");
    	printk("No L4 entry\n");
	return 0;
    }
    printk("L1-entry addr: %x, L1-entry contents: %x, PFN: %x, Flags: %x\n", 
		    next, val, phy_addr, flags );

    
    vaddr_base = (u64 *) osmap(phy_addr);
    offset = ((addr >> 30) - ((addr >> 39) << 9)) ;
    next = (u64*)((u64)vaddr_base + offset*8);
    one = 1;
    val = ((*next) & ((one << 44) - 1));
    flags = ((*next) & ((one << 12) - 1));
    invalid = (1 & flags);
    phy_addr = ((val - flags) >> 12);
    if(!invalid) {
    	printk("No L2 entry\n");
    	printk("No L3 entry\n");
    	printk("No L4 entry\n");
	return 0;
    }
    printk("L2-entry addr: %x, L2-entry contents: %x, PFN: %x, Flags: %x\n", 
		    next, val, phy_addr, flags );

    vaddr_base = (u64 *) osmap(phy_addr);
    offset = ((addr >> 21) - ((addr >> 30) << 9)) ;
    next = (u64*)((u64)vaddr_base + offset*8);
    one = 1;
    val = ((*next) & ((one << 44) - 1));
    flags = ((*next) & ((one << 12) - 1));
    invalid = (1 & flags);
    phy_addr = ((val - flags) >> 12);
    if(!invalid) {
    	printk("No L3 entry\n");
    	printk("No L4 entry\n");
	return 0;
    }
    printk("L3-entry addr: %x, L3-entry contents: %x, PFN: %x, Flags: %x\n", 
		    next, val, phy_addr, flags );

    vaddr_base = (u64 *) osmap(phy_addr);
    offset = ((addr >> 12) - ((addr >> 21) << 9)) ;
    next = (u64*)((u64)vaddr_base + offset*8);
    one = 1;
    val = ((*next) & ((one << 44) - 1));
    flags = ((*next) & ((one << 12) - 1));
    invalid = (1 & flags);
    phy_addr = ((val - flags) >> 12);
    if(!invalid) {
    	printk("No L4 entry\n");
	return 0;
    }
    printk("L4-entry addr: %x, L4-entry contents: %x, PFN: %x, Flags: %x\n", 
		    next, val, phy_addr, flags );

  

    return 0;
}

