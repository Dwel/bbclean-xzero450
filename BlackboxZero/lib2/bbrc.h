#pragma once
#include "bblibapi.h"

enum { e_RCFILE_HTS = 40 }; // hash table size

struct lin_list
{
    lin_list * m_next;
    lin_list * hnext;
    lin_list * wnext;
    unsigned hash, k, o;
    int i;
    bool is_wild;
    bool dirty;
    //char flags;
    TCHAR str[3];
};

struct fil_list
{
    fil_list * m_next;
    lin_list * lines;
    lin_list * wild;
    lin_list * ht[e_RCFILE_HTS];
    unsigned hash;

    bool dirty;
    bool newfile;
    bool tabify;
    bool write_error;
    bool is_style;
    bool is_070;

    int k;
    TCHAR path[1];
};

struct rcreader_init
{
    fil_list * rc_files;
    void (*write_error) (TCHAR const * filename);
    bool dos_eol;
    bool translate_065;
    bool timer_set;
    int used;
    int found_last_value;
};

BBLIB_EXPORT void init_rcreader (rcreader_init * init);
BBLIB_EXPORT void reset_rcreader ();

BBLIB_EXPORT bool set_translate_065 (int f);
// check whether a style uses 0.70 conventions
BBLIB_EXPORT bool get_070 (TCHAR const * path);
BBLIB_EXPORT void check_070 (fil_list *fl);
BBLIB_EXPORT bool is_stylefile(TCHAR const * path);

BBLIB_EXPORT FILE * create_rcfile (TCHAR const * path);
BBLIB_EXPORT TCHAR * read_file_into_buffer (TCHAR const * path, int max_len);
BBLIB_EXPORT TCHAR scan_line(TCHAR ** pp, TCHAR ** ss, int * ll);
BBLIB_EXPORT int read_next_line (FILE * fp, TCHAR * szBuffer, unsigned dwLength);

BBLIB_EXPORT TCHAR const * read_value (TCHAR const * path, TCHAR const * szKey, long * ptr);
BBLIB_EXPORT int found_last_value ();
BBLIB_EXPORT void write_value (TCHAR const * path, TCHAR const * szKey, TCHAR const * value);
BBLIB_EXPORT int rename_setting (TCHAR const * path, TCHAR const * szKey, TCHAR const * new_keyword);
BBLIB_EXPORT int delete_setting (LPTSTR path, LPTSTR szKey);

/* ------------------------------------------------------------------------- */
/* only used in bbstylemaker */

BBLIB_EXPORT int scan_component (TCHAR const ** p);
BBLIB_EXPORT int xrm_match (TCHAR const * key, TCHAR const * pat);

BBLIB_EXPORT fil_list * read_file (const TCHAR * filename);
BBLIB_EXPORT lin_list * make_line (fil_list * fl, TCHAR const * key, TCHAR const * val);
BBLIB_EXPORT void free_line (fil_list * fl, lin_list * tl);
BBLIB_EXPORT lin_list ** get_simkey (lin_list ** slp, TCHAR const * key);
BBLIB_EXPORT void make_style070 (fil_list * fl);
BBLIB_EXPORT void make_style065 (fil_list * fl);

