#include<tb.h>
#include<lib.h>
#include<file.h>
#include<context.h>
#include<memory.h>

/*
 * *  Trace buffer implementation.
 *
 */


///////////////////////////////////////////////////////////////////////////
////           TODO:     Trace buffer functionality                   /////
///////////////////////////////////////////////////////////////////////////


// Check whether passed buffer is valid memory location for read.
static int tb_validate(unsigned long buff, u32 count, int acflags) 
{
	struct exec_context *ctx = get_current_ctx();
	struct mm_segment * mms = ctx -> mms;	
	if(buff >= mms[MM_SEG_CODE].start && buff + count <= mms[MM_SEG_CODE].next_free - 1) {
		if(acflags & 2) return -1;
		else return 0;
	} else if(buff >= mms[MM_SEG_RODATA].start && buff + count <= mms[MM_SEG_RODATA].next_free - 1) {
		if(acflags & 2) return -1;
		else return 0;
	} else if(buff >= mms[MM_SEG_DATA].start && buff + count <= mms[MM_SEG_DATA].next_free - 1) {
		return 0;
	} else if(buff >= mms[MM_SEG_STACK].start && buff + count <= mms[MM_SEG_STACK].end - 1) {
		return 0;
	} else {
		struct vm_area* curr = ctx -> vm_area;
	   	while(curr != NULL) {
			if(buff >= curr -> vm_start && buff + count <= curr -> vm_end) {
				break;
			}
			curr = curr -> vm_next;
		}
		if(curr != NULL) {
			if((curr -> access_flags & acflags) == acflags) return 0;
			else return -1;
		}
	}

	return -1;
}

static long tb_close(struct file *filep)
{
	os_page_free(USER_REG, filep -> tb -> space);
	os_free(filep -> tb, sizeof(struct tb_info));
	os_free(filep -> fops, sizeof(struct fileops));
	os_free(filep, sizeof(struct file));
}

static int tb_read(struct file *filep, char *buff, u32 count)
{
	if(tb_validate((u64)buff, count, 2) == -1) return -EBADMEM;
	int read = 0;
	struct tb_info * tb = filep -> tb;
	while(read < count && tb -> words) {
		buff[read] = tb -> space[tb -> ro];
		read++;
		tb -> ro ++;
		tb -> ro %= 4096;
		tb -> words --;
	}

	return read;
}

static int tb_write(struct file *filep, char *buff, u32 count)
{
	if(tb_validate((u64)buff, count, 1) == -1) return -EBADMEM;
	int written = 0;
	struct tb_info *tb = filep -> tb;
	while(written < count) {
		if(tb -> ro == tb -> wo && tb -> words > 0) break;
		tb -> space[tb -> wo] = buff[written++];
		tb -> wo++;
		tb -> words++;
		tb -> wo %= 4096;
	}

	return written;
}

int sys_create_tb(struct exec_context *current, int mode)
{
    int ret_fd = -1;
    for(int i = 0; i < MAX_OPEN_FILES; i++) {
	if(current -> files[i] == NULL) {
		ret_fd = i;
		break;
	}
    }
    if(ret_fd == -1) return -EINVAL;

    struct file* cfile = (struct file*)(os_alloc(sizeof(struct file)));
    current -> files[ret_fd] = cfile;

    cfile -> type = TRACE_BUFFER;
    cfile -> mode = mode;
    cfile -> offp = 0;
    cfile -> ref_count = 1;
    cfile -> inode = cfile -> pipe = NULL;
    
    cfile -> fops = (struct fileops*)(os_alloc(sizeof(struct fileops)));
    cfile -> fops -> read = &tb_read;
    cfile -> fops -> write =  &tb_write;
    cfile -> fops -> lseek = NULL;
    cfile -> fops -> close = &tb_close;

    cfile -> tb = (struct tb_info*)(os_alloc(sizeof(struct tb_info)));

    cfile -> tb -> ro = cfile -> tb -> wo = cfile -> tb -> words = 0;

    cfile -> tb -> space = (char *)(os_page_alloc(USER_REG));

    return ret_fd;
}
