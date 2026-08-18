#include<procinfo.h>
#include<lib.h>
#include<file.h>
#include<context.h>

static char* segment_names[MAX_MM_SEGS+1] = {"code", "rodata", "data", "stack", "invalid"}; 
static char* file_types[MAX_FILE_TYPE+1] = {"stdin", "stdout", "stderr", "reg", "pipe", "invalid"}; 

long get_process_info(struct exec_context *ctx, long cmd, char *ubuf, long len)
{
    long retval = -EINVAL;	
    // printk("hello\n");
    /*
     * TODO your code goes in here
     * */
    if(ubuf == NULL) return retval;
    
    if(cmd == GET_PINFO_GEN) {

	struct general_info temp;
	temp.pid = ctx -> pid;
	temp.ppid = ctx -> ppid;
	temp.pcb_addr = (unsigned long)ctx;
	strcpy(temp.pname, ctx -> name);
	// printk("pid: %x, ppid: %x, pcb_addr: %x, name: %s \n", temp.pid, temp.ppid, temp.pcb_addr, temp.pname);

	int req_size = sizeof(temp);	
	if(ubuf == NULL || req_size > len) return retval;
	memcpy(ubuf, &temp, req_size);	
	return 1;	
    } else if(cmd == GET_PINFO_FILE) {
	int pos = 0;
	struct file_info ans[MAX_OPEN_FILES];
	int found = 0;
	while(pos < MAX_OPEN_FILES) {
		if((ctx->files)[pos] == NULL) {
			pos++;
			continue;
		}
		struct file* cfile = (ctx -> files)[pos];
	
		
		ans[found].mode = cfile->mode;
		ans[found].ref_count = cfile -> ref_count;
		ans[found].filepos = cfile -> offp;
		
		strcpy(ans[found].file_type, file_types[cfile->type]);
		pos++;
		found++;
	}
	int req_size = found * sizeof(struct file_info);
	if(ubuf == NULL || req_size > len) return retval;
	memcpy(ubuf, ans, req_size);	
	return found;
    } else if(cmd == GET_PINFO_MSEG) {
	int pos = 0;
	struct mem_segment_info ans[MAX_MM_SEGS];
	while(pos < MAX_MM_SEGS) {
		struct mm_segment curr = (ctx -> mms)[pos];
		ans[pos].start = curr.start;
		ans[pos].end = curr.end;
		ans[pos].next_free = curr.next_free;
		strcpy(ans[pos].segname, segment_names[pos]);	
		u32 access_flags = curr.access_flags;
		if(access_flags & 1) (ans[pos].perm)[0] = 'R';
		else (ans[pos].perm)[0] = '_';

		if(access_flags & 2) (ans[pos].perm)[1] = 'W';
		else (ans[pos].perm)[1] = '_';

		if(access_flags & 4) (ans[pos].perm)[2] = 'X';
		else (ans[pos].perm)[2] = '_';
		
		(ans[pos].perm)[3] = '\0';
		pos++;
	}
	int req_size = pos * sizeof(struct mem_segment_info);
	if(ubuf == NULL || req_size > len) return retval;
	memcpy(ubuf, ans, req_size);	
	return pos;
    } else if (cmd == GET_PINFO_VMA) {
	struct vm_area* curr = ctx -> vm_area;
	curr = curr -> vm_next;
	
	int sz = 0;
	struct vm_area_info ans;

	while(curr != NULL) {
		ans.start = curr -> vm_start;
		ans.end = curr -> vm_end;

		u32 access_flags = curr -> access_flags;
		if(access_flags & 1) (ans.perm)[0] = 'R';
		else (ans.perm)[0] = '_';

		if(access_flags & 2) (ans.perm)[1] = 'W';
		else (ans.perm)[1] = '_';

		if(access_flags & 4) (ans.perm)[2] = 'X';
		else (ans.perm)[2] = '_';
		
		(ans.perm)[3] = '\0';
		if((sz + 1) * sizeof(ans) > len) return retval;
		memcpy(ubuf + sz*sizeof(struct vm_area_info), &ans, sizeof(ans));
		sz++;
		curr = curr -> vm_next;
	}

	return sz;

    }
    return retval;    

} 
