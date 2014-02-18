#ifndef _BBTaskSwitch_CPP_ADAPTOR_H
#define _BBTaskSwitch_CPP_ADAPTOR_H

#ifdef __cplusplus
extern "C" {
#endif




#define DECLARE(TYPE, FUNC, ARGS) \
          __declspec(dllexport) TYPE FUNC ARGS; \
          typedef TYPE FUNC##_f ARGS;

DECLARE(void, Cpp_MakeStyleGradient, (HDC, RECT*, void*, BOOL))
DECLARE(COLORREF, Cpp_StyleItem_TextColor, (void*))
DECLARE(HFONT, Cpp_CreateStyleFont, (void*))

#undef DECLARE




#ifdef __cplusplus
}
#endif

#endif
