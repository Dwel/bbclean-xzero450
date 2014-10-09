#pragma once
#include "bblibapi.h"

#ifdef UNICODE
# define tmemcpy wmemcpy
# define tmemmove wmemmove
# define tmemset wmemset
# define tmemcmp wmemcmp
#else
# define tmemcpy (char*)memcpy
# define tmemmove memmove
# define tmemset memset
# define tmemcmp memcmp
#endif

inline TCHAR * extract_string (TCHAR * dst, TCHAR const * src, int n)
{
    _tcsncpy_s(dst, n, src, _TRUNCATE);
    return dst;
}

inline TCHAR * strcpy_max (TCHAR * dst, TCHAR const * src, int maxlen)
{
    return extract_string(dst, src, maxlen);
}

