typedef struct
{
	void (*add_string)(char *ptr, int len);
	char* (*get_ptr)();
	void (*set_string)(char *ptr, int len);
	void (*truncate)(int len);
	int (*get_length)();
	char* (*lock_buffer)(int n);
	void (*unlock_buffer)();
} STRING_BASE, *PSTRING_BASE;

typedef struct
{
	PSTRING_BASE base;
	char *data;
	int max[2];
	int used;
} STRING, *PSTRING;

//////////////////////////////////////////////////////////////////////////////////////////////
// Predefinition
char* get_ptr();
char* lock_buffer(int n);
void makespace(int size);
void truncate(int len);
void add_string(char *ptr, int len);
void set_string(char *ptr, int len);
void unlock_buffer();
int get_length();
