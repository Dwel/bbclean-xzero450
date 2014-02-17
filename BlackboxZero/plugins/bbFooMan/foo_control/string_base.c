#include "string_base.h"

STRING_BASE s_base=
{
	add_string,
	get_ptr,
	set_string,
	truncate,
	get_length,
	lock_buffer,
	unlock_buffer
};
	
STRING str={&s_base, 0, 0, 0, 0};

// Not used
void set_string(char *ptr, int len) {}
char* lock_buffer(int n) {return 0;}
void unlock_buffer() {}

char* get_ptr() {return str.data;}
int get_length() {return str.used;}

void makespace(int size)
{
	char *new_mem;
	int new_size;
	if(str.max[0]<size) new_size=size+16; else if(str.max[0]>size+32) new_size=size; else return;
	new_mem = calloc(new_size, 1);
	if(new_mem)
	{
		if(str.data)
		{
			memcpy(new_mem, str.data, str.used<size?str.used:size);
			free(str.data);
		}
		str.data = new_mem;
		str.max[0]=str.max[1]=new_size;
	}
}

void truncate(int len)
{
	if(str.used>len) {str.used=len, str.data[len]=0, makespace(len+1);}
}

int strlen_max(char *ptr, unsigned int max)
{
	int n=0;
	if(!ptr) return 0;
	while(n<max && ptr[n]!=0) n++;
	return n;
}

void add_string(char *ptr, int len)
{
	int l = strlen_max(ptr, len);
	makespace(str.used+l+1);
	memcpy(str.data+str.used, ptr, l);
	str.used+=l;
	str.data[str.used]=0;
}

void string_free()
{
	if(str.data) free(str.data);
	str.data=0, str.max[0]=str.max[1]=str.used=0;
}
