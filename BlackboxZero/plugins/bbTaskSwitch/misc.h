#ifndef _BBTaskSwitch_MISC_H
#define _BBTaskSwitch_MISC_H




#define T(s) TEXT(s)

#define NUMBER_OF(a) (sizeof(a) / sizeof((a)[0]))

#define MIN(a,b) ((a) > (b) ? (b) : (a))

#ifdef __GNUC__
#define PRINTF_LIKE(string_index, first_to_check) \
        __attribute__((format(printf, string_index, first_to_check)))
#else
#define PRINTF_LIKE(string_index, first_to_check)
#endif




typedef struct {
	LPCTSTR string;
	int length;
} Token;


typedef struct {
	int top;
	int right;
	int bottom;
	int left;
} Padding;




#define DECLARE(TYPE, FUNC, ARGS) \
          __declspec(dllexport) TYPE FUNC ARGS; \
          typedef TYPE FUNC##_f ARGS;

BOOL misc_begin(void);
void misc_end(void);

BOOL is_2k_or_later(void);

DECLARE(void, resize, (HWND, int, int))
DECLARE(void, move, (HWND, int, int))

void update_screen_rect(void);
DECLARE(void, get_screen_rect, (HWND, RECT*))

void my_FreeLibrary(HMODULE);

BOOL get_module_dir(LPTSTR, int, HMODULE);
BOOL get_file_path(LPTSTR, int, LPCTSTR, HMODULE);

DECLARE(LPTSTR, my_strncpy, (LPTSTR, LPCTSTR, int))
DECLARE(LPTSTR, my_strndup, (LPCTSTR, int))
DECLARE(int, my_strncmp, (LPCTSTR, LPCTSTR, int))
DECLARE(void, tokenize, (LPCTSTR, int, Token [], int*, LPCTSTR*))
void descape(LPTSTR);
DECLARE(int, my_atoi, (LPCTSTR, int))

DECLARE(Padding, Padding_FromTokens, (const Token [], int))
DECLARE(int, StyleNumber_FromToken, (Token))
DECLARE(BOOL, BOOL_FromToken, (Token))

DECLARE(PRINTF_LIKE(2, 3) void, notice, (LPCTSTR, LPCTSTR, ...))
DECLARE(void, notice_with_last_error, (LPCTSTR, LPCTSTR))
DECLARE(PRINTF_LIKE(3, 4) void, broam_error, (LPCTSTR, LPCTSTR, LPCTSTR, ...))
DECLARE(BOOL, tokenize_check, (LPCTSTR, int, int, BOOL, int, LPCTSTR, LPCTSTR))

#undef DECLARE




#endif
