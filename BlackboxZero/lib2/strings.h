#pragma once
#include "bbapi.h"

inline TCHAR * extract_string (TCHAR * dst, TCHAR const * src, int n)
{
    _tcsncpy_s(dst, n, src, _TRUNCATE);
    return dst;
}

inline TCHAR * strcpy_max (TCHAR * dst, TCHAR const * src, int maxlen)
{
    return extract_string(dst, src, maxlen);
}

