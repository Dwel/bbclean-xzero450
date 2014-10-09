#pragma once
#include "bbapi.h"

#include "winutils.h"
#include "numbers.h"
#include "memory.h"
#include <tchar.h>

inline bool IS_SPC (TCHAR c)
{
  return _istcntrl(c) || _istspace(c);
}

/*template <class T>
constexpr size_t array_count(T * t)
{
  return sizeof(t) / sizeof(*t);
}*/
#ifndef array_count
# define array_count(s) (sizeof(s) / sizeof(*s))
#endif


#if 0

// #define BBOPT_MEMCHECK

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* Convenience defs */
//#define IS_SPC(c) ((unsigned char)(c) <= 32)
//#define IS_SLASH(c) ((c) == '\\' || (c) == '/')

/*#ifndef offsetof
# define offsetof(s,m) ((size_t)&(((s*)0)->m))
//#endif


#define c_new(t) (t*)c_alloc(sizeof(t))
#define c_del(v) m_free(v)

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

#ifdef BBOPT_MEMCHECK
# define m_alloc(s) _m_alloc(s,__FILE__,__LINE__)
# define c_alloc(s) _c_alloc(s,__FILE__,__LINE__)
# define m_realloc(p,s) _m_realloc(p,s,__FILE__,__LINE__)
# define m_free(p) _m_free(p,__FILE__,__LINE__)
#else
# define m_alloc(n) malloc(n)
# define c_alloc(n) calloc(1,n)
# define m_free(v) free(v)
# define m_realloc(p,s) realloc(p,s)
# define m_alloc_check_leaks(title)
# define m_alloc_check_memory()
# define m_alloc_size() 0
#endif /* BBOPT_MEMCHECK */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* bools.c */

BBLIB_EXPORT int get_false_true(const char *arg);
BBLIB_EXPORT const char *false_true_string(int f);
BBLIB_EXPORT void set_bool(void *v, const char *arg);

/* strings.c */

BBLIB_EXPORT int replace_string(char *out, int bufsize, int offset, int len, const char *in);
BBLIB_EXPORT char *extract_string(char *dest, const char *src, int n);
BBLIB_EXPORT char *strcpy_max(char *dest, const char *src, int maxlen);
BBLIB_EXPORT char* stristr(const char *aa, const char *bb);
BBLIB_EXPORT int get_string_index (const char *key, const char * const * string_array);
BBLIB_EXPORT unsigned calc_hash(char *p, const char *s, int *pLen, int delim);

BBLIB_EXPORT char *new_str_n(const char *s, int n);
BBLIB_EXPORT char *new_str(const char *s);
BBLIB_EXPORT void free_str(char **s);
BBLIB_EXPORT void replace_str(char **s, const char *n);
BBLIB_EXPORT char *concat_str(const char *s1, const char *s2);

BBLIB_EXPORT char *m_formatv(const char *fmt, va_list arg_list);
BBLIB_EXPORT char *m_format(const char *fmt, ...);

/* tokenize.c */

BBLIB_EXPORT int nexttoken(const char **p_out, const char **p_in, const char *delims);
BBLIB_EXPORT char* NextToken(char* buf, const char** string, const char *delims);
BBLIB_EXPORT int get_string_within (char *dest, int size, const char **p_src, const char *delims);
BBLIB_EXPORT const char *get_special_command(const char **p_path, char *buffer, int size);
BBLIB_EXPORT int skip_spc(const char **pp);

/* paths.c */

BBLIB_EXPORT TCHAR* unquote(TCHAR *src);
BBLIB_EXPORT TCHAR *quote_path(TCHAR *path);
BBLIB_EXPORT const TCHAR *file_basename(const TCHAR *path);
BBLIB_EXPORT const TCHAR *file_extension(const TCHAR *path);
BBLIB_EXPORT TCHAR *file_directory(TCHAR *buffer, const TCHAR *path);
BBLIB_EXPORT TCHAR *fix_path(TCHAR *path);
BBLIB_EXPORT int is_absolute_path(const TCHAR *path);
BBLIB_EXPORT TCHAR *join_path(TCHAR *buffer, const TCHAR *dir, const TCHAR *filename);
BBLIB_EXPORT TCHAR *replace_slashes(TCHAR *buffer, const TCHAR *path);

BBLIB_EXPORT void bbshell_set_utf8(int f);
BBLIB_EXPORT void bbshell_set_defaultrc_path(const TCHAR *s);
#endif

#if 0

BBLIB_EXPORT TCHAR* get_exe_path(HINSTANCE h, TCHAR* pszPath, int nMaxLen);
BBLIB_EXPORT TCHAR *set_my_path(HINSTANCE h, TCHAR *dest, const TCHAR *fname);
BBLIB_EXPORT const TCHAR *get_relative_path(HINSTANCE h, const TCHAR *path);

/* tinylist.c */

#ifndef LIST_NODE_DEFINED
typedef struct list_node { struct list_node *next; void *v; } list_node;
#endif
#ifndef STRING_NODE_DEFINED
typedef struct string_node { struct string_node *next; char str[1]; } string_node;
#endif

#define dolist(_e,_l) for (_e=(_l);_e;_e=_e->next)
#define skipUntil(_e, _l, _pred) for(_e=(_l);(_e)&&!(_pred);_e=_e->next)

BBLIB_EXPORT void *member(void *a0, void *e0);
BBLIB_EXPORT void *member_ptr(void *a0, void *e0);
BBLIB_EXPORT void *assoc(void *a0, void *e0);
BBLIB_EXPORT void *assoc_ptr(void *a0, void *e0);
BBLIB_EXPORT int remove_assoc(void *a, void *e);
BBLIB_EXPORT int remove_node (void *a, void *e);
BBLIB_EXPORT int remove_item(void *a, void *e);
BBLIB_EXPORT void reverse_list (void *d);
BBLIB_EXPORT void append_node (void *a0, void *e0);
BBLIB_EXPORT void cons_node (void *a0, void *e0);
BBLIB_EXPORT void *copy_list (void *l0);
BBLIB_EXPORT void *nth_node (void *v0, int n);
BBLIB_EXPORT void *new_node(void *p);
BBLIB_EXPORT int listlen(void *v0);
BBLIB_EXPORT void freeall(void *p);

BBLIB_EXPORT struct string_node *new_string_node(const char *s);
BBLIB_EXPORT void append_string_node(struct string_node **p, const char *s);

/* m_alloc.c */

#ifdef BBOPT_MEMCHECK
BBLIB_EXPORT void * _m_alloc(unsigned n, const char *file, int line);
BBLIB_EXPORT void * _c_alloc (unsigned n, const char *file, int line);
BBLIB_EXPORT void   _m_free(void *v, const char *file, int line);
BBLIB_EXPORT void * _m_realloc (void *v, unsigned s, const char *file, int line);
BBLIB_EXPORT void   _m_setinfo(const char *file, int line);

BBLIB_EXPORT unsigned m_alloc_usable_size(void *v);
BBLIB_EXPORT void m_alloc_dump_memory(void);
BBLIB_EXPORT void m_alloc_check_memory(void);
BBLIB_EXPORT void m_alloc_check_leaks(const char *title);
BBLIB_EXPORT unsigned m_alloc_size(void);
#endif /* BBOPT_MEMCHECK */

/* moreutils.c */

BBLIB_EXPORT int locate_file(HINSTANCE hInstance, TCHAR *path, const TCHAR *fname, const TCHAR *ext);
BBLIB_EXPORT int bbPlugin_LocateFile(HINSTANCE hInstance, LPTSTR lpPluginPath, DWORD nSize, LPTSTR lpString);
BBLIB_EXPORT int check_filetime(const TCHAR *fn, FILETIME *ft0);
BBLIB_EXPORT unsigned int eightScale_down(unsigned int i);
BBLIB_EXPORT unsigned int eightScale_up(unsigned int i);
BBLIB_EXPORT int my_substr_icmp(const TCHAR *a, const TCHAR *b);
BBLIB_EXPORT int n_stricmp(const TCHAR **pp, const TCHAR *s);
BBLIB_EXPORT int trim_address(TCHAR q[MAX_PATH], int is, int js);
BBLIB_EXPORT int get_substring_index(const TCHAR *key, const TCHAR * const * string_list);
BBLIB_EXPORT int substr_icmp(const TCHAR *a, const TCHAR *b);
BBLIB_EXPORT const TCHAR *string_empty_or_null(const TCHAR *s);
BBLIB_EXPORT const TCHAR *get_delim(const TCHAR *path, int d);
//BBLIB_EXPORT TCHAR *add_slash(TCHAR *d, const TCHAR *s);
BBLIB_EXPORT int is_relative_path(const TCHAR *path);
BBLIB_EXPORT TCHAR * make_bb_path(HINSTANCE h, TCHAR *dest, const TCHAR *src);
BBLIB_EXPORT TCHAR * make_full_path(HINSTANCE h, TCHAR *buffer, const TCHAR *filename);
BBLIB_EXPORT TCHAR * get_path(TCHAR *pszPath, int nMaxLen, const TCHAR *file);
BBLIB_EXPORT void draw_line_h(HDC hDC, int x1, int x2, int y, int w, COLORREF C);
BBLIB_EXPORT HICON GetIcon(HWND iWin);
BBLIB_EXPORT COLORREF Settings_CreateShadowColor(COLORREF textColor);
BBLIB_EXPORT int FuzzyMatch(COLORREF focus, COLORREF unfocus);
BBLIB_EXPORT COLORREF split(COLORREF c, int To);

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
#if defined __cplusplus && defined BBOPT_MEMCHECK
#undef new
#undef delete
inline void *operator new (size_t n)
{
    return _m_alloc(n, NULL, -1);
}

inline void operator delete (void *v)
{
     _m_free(v, NULL, -1);
}

inline void * operator new[] (size_t n)
{
    return _m_alloc(n, NULL, -1);
}

inline void operator delete[] (void *v)
{
     _m_free(v, NULL, -1);
}
#define new (_m_setinfo(__FILE__,__LINE__),false)?NULL:new
#define delete _m_setinfo(__FILE__,__LINE__),delete
#endif /* BBOPT_MEMCHECK */
/* ------------------------------------------------------------------------- */
#endif
