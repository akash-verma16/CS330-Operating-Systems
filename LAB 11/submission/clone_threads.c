#include<clone_threads.h>
#include<entry.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<mmap.h>
#include<fork.h>
#include<page.h>

extern int destroy_user_mappings(struct exec_context *ctx);

static void set_kstack_of_thread(struct exec_context *ctx)
{
   ctx->os_stack_pfn = os_pfn_alloc(OS_PT_REG);
   ctx->os_rsp = (((u64) ctx->os_stack_pfn) << PAGE_SHIFT) + PAGE_SIZE;
   stats->num_processes++;
   ctx->type = EXEC_CTX_USER_TH;	
}

//XXX Do not modify anything above this line

/*
  system call handler for clone, create thread like execution contexts
*/
long do_clone(void *th_func, void *user_stack, void *user_arg) 
{
  int ctr;
  struct exec_context *thread_ctx = get_new_ctx();  //This is to be used for the newly created thread
  struct exec_context *ctx = get_current_ctx();
  u32 pid = thread_ctx->pid;
  struct thread *n_thread;

  if(!ctx->ctx_threads){  // This is the first thread
          ctx->ctx_threads = os_alloc(sizeof(struct ctx_thread_info));
          bzero((char *)ctx->ctx_threads, sizeof(struct ctx_thread_info));
          ctx->ctx_threads->pid = ctx->pid;
  }

 /* XXX Do not modify anything above. Your implementation goes here */

 // TODO your code goes here
/*
  thread_ctx -> ppid = ctx -> pid;
  thread_ctx -> type = EXEC_CTX_USER_TH;
  for(int i = 0; i < MAX_MM_SEGS; i++) {
	(thread_ctx -> mms)[i] = (ctx -> mms)[i];
  }
  thread_ctx -> vm_area = ctx -> vm_area;
  for(int i = 0; i < CNAME_MAX; i++) {
	(thread_ctx -> name)[i] = (ctx -> name)[i];
  }

  thread_ctx -> regs = ctx -> regs;

  thread_ctx -> pending_signal_bitmap = ctx -> pending_signal_bitmap;
  for(int i = 0; i < MAX_SIGNALS; i++) {
	(thread_ctx -> sighandlers)[i] = (ctx -> sighandlers)[i];
  }
  thread_ctx -> ticks_to_sleep = ctx -> ticks_to_sleep;
  thread_ctx -> alarm_config_time = ctx -> alarm_config_time;
  thread_ctx -> ticks_to_alarm = ctx -> ticks_to_alarm;
  
  for(int i = 0; i < MAX_OPEN_FILES; i++) {
	(thread_ctx -> files)[i] = (ctx -> files)[i];
  }

  thread_ctx -> lock = ctx -> lock;
  thread_ctx -> next = ctx -> next;
*/
  *(thread_ctx) = *(ctx);
  thread_ctx -> next = NULL;
  thread_ctx -> state = READY;
  thread_ctx -> ctx_threads = NULL;
  thread_ctx -> pid = pid;
  thread_ctx -> ppid = ctx -> pid;
  thread_ctx -> type = EXEC_CTX_USER_TH;

  (thread_ctx -> regs).rbp = (u64) user_stack;
  (thread_ctx -> regs).entry_rsp = (u64) user_stack;
  (thread_ctx -> regs).entry_rip = (u64) th_func;
  (thread_ctx -> regs).rdi = (u64) user_arg;

  struct thread* curr = find_unused_thread(ctx);
  if(curr == NULL) return -1;
  curr -> pid = thread_ctx -> pid;
  curr -> status = TH_USED;
  curr -> parent_ctx = ctx;

 //End of your logic
  
 //XXX The following two lines should be there. 
  set_kstack_of_thread(thread_ctx);  //Allocate kstack for the thread
  return pid;
}



// XXX Reference implementation for a process exit
// You can refer this to implement your version of do_exit

void do_exit_single(u8 normal) 
{
  int ctr;
  struct exec_context *ctx = get_current_ctx();
  struct exec_context *new_ctx;

 
  do_file_exit(ctx);   // Cleanup the files

  // cleanup of this process
  destroy_user_mappings(ctx); 
  do_vma_exit(ctx);
  if(!put_pfn(ctx->pgd)) 
      os_pfn_free(OS_PT_REG, ctx->pgd);   //XXX Now its fine as it is a single core system
  if(!put_pfn(ctx->os_stack_pfn))
     os_pfn_free(OS_PT_REG, ctx->os_stack_pfn);
  release_context(ctx); 
  new_ctx = pick_next_context(ctx);
  dprintk("Scheduling %s:%d [ptr = %x]\n", new_ctx->name, new_ctx->pid, new_ctx); 
  schedule(new_ctx);  //Calling from exit
}

//handler for exit()
void do_exit(u8 normal)
{
  struct exec_context *ctx = get_current_ctx();
  struct exec_context *new_ctx;
  if(ctx -> type == EXEC_CTX_USER_TH) {
	  handle_thread_exit(ctx, normal);
	  // os_pfn_free(OS_PT_REG, ctx -> os_stack_pfn);
  	  release_context(ctx); 
    	  new_ctx = pick_next_context(ctx);
  	  schedule(new_ctx);  //Calling from exit

  } else {
	cleanup_all_threads(ctx);
	do_exit_single(normal);
  }
  return;
}




////////////////////////////////////////////////////////// Semaphore implementation ////////////////////////////////////////////////////
//
//


// A spin lock implementation using cmpxchg
// XXX you can use it for implementing the semaphore
// Do not modify this code

static void spin_init(struct spinlock *spinlock)
{
	spinlock->value = 0;
	//printk("spinlock initialised\n");
}

static void spin_lock(struct spinlock *spinlock)
{
	unsigned long *addr = &(spinlock->value);

	asm volatile(
		"mov $1,  %%rcx;"
		"mov %0,  %%rdi;"
		"try: xor %%rax, %%rax;"
		"lock cmpxchg %%rcx, (%%rdi);"
		"jnz try;"
		:
		: "r"(addr)
		: "rcx", "rdi", "rax", "memory"
	);
}

static void spin_unlock(struct spinlock *spinlock)
{
	spinlock->value = 0;
}

static int init_sem_metadata_in_context(struct exec_context *ctx)
{
   if(ctx->lock){
	   printk("Already initialized MD. Call only for the first time\n");
	   return -1;
   }
   ctx->lock = (struct lock*) os_alloc(sizeof(struct lock) * MAX_LOCKS);
   if(ctx->lock == NULL){
			printk("[pid: %d]BUG: Out of memory!\n", ctx->pid);
                        return -1;
   }
	
   for(int i=0; i<MAX_LOCKS; i++)
			ctx->lock[i].state = LOCK_UNUSED;
}

// XXX Do not modify anything above this line

/*
  system call handler for semaphore creation
*/
int do_sem_init(struct exec_context *ctx, sem_t *sem_id, int value)
{
	if(ctx->lock == NULL)
		init_sem_metadata_in_context(ctx);
        // TODO Your implementation goes here
	for(int i = 0; i < MAX_LOCKS; i++) {
		if((ctx -> lock)[i].state == LOCK_UNUSED) {
			spin_init(&((ctx -> lock)[i].sem.lock));
			spin_lock(&((ctx -> lock)[i].sem.lock));
			(ctx -> lock)[i].state = LOCK_USED;
			(ctx -> lock)[i].sem.value = value;
		        (ctx -> lock)[i].sem.wait_queue = NULL;
			(ctx -> lock)[i].id = sem_id;
			spin_unlock(&((ctx -> lock)[i].sem.lock));
			return 0;	
		}
	}	

	return -1;
}

/*
  system call handler for semaphore acquire
*/

int do_sem_wait(struct exec_context *ctx, sem_t *sem_id)
{
	for(int i = 0; i < MAX_LOCKS; i++) {
		if((ctx -> lock)[i].state == LOCK_USED && (ctx -> lock)[i].id == 
				(sem_id)) {
			
			if((ctx -> lock)[i].sem.value == 0) {
				struct exec_context * curr = (ctx -> lock)[i].sem.wait_queue;
				if(curr == NULL) {
					spin_lock(&((ctx -> lock)[i].sem.lock));
					(ctx -> lock)[i].sem.wait_queue = ctx;
					ctx -> next = NULL;
					ctx -> state = WAITING;
					spin_unlock(&((ctx -> lock)[i].sem.lock));
					struct exec_context *nxt = pick_next_context(ctx);
					schedule(nxt);
					return 0;
				}
				spin_lock(&((ctx -> lock)[i].sem.lock));
				while(curr -> next != NULL) {
					curr = curr -> next;
				}	
				curr -> next = ctx;
				ctx -> next = NULL;
				ctx -> state = WAITING;
				spin_unlock(&((ctx -> lock)[i].sem.lock));
				struct exec_context *nxt = pick_next_context(ctx);
				schedule(nxt);
			} else {
				spin_lock(&((ctx -> lock)[i].sem.lock));
				(ctx -> lock)[i].sem.value--;
				spin_unlock(&((ctx -> lock)[i].sem.lock));
			}
			return 0;
		}
	}
	return -EAGAIN;
}

/*
  system call handler for semaphore release
*/
int do_sem_post(struct exec_context *ctx, sem_t *sem_id)
{
	// printk("bhai \n"); 
	for(int i = 0; i < MAX_LOCKS; i++) {
		if((ctx -> lock)[i].state == LOCK_USED && (ctx -> lock)[i].id == 
					sem_id) {
			spin_lock(&((ctx -> lock)[i].sem.lock));
			if((ctx -> lock)[i].sem.wait_queue) {
				//(ctx -> lock)[i].sem.value++;
				(ctx -> lock)[i].sem.wait_queue -> state = READY;
				//schedule((ctx -> lock)[i].sem.wait_queue);
				(ctx -> lock)[i].sem.wait_queue = (ctx-> lock)[i].sem.wait_queue -> next;	
			} else {
				(ctx -> lock)[i].sem.value++;
			}
			spin_unlock(&((ctx -> lock)[i].sem.lock));
			return 0;
		}
	}
	return -EAGAIN;
}
