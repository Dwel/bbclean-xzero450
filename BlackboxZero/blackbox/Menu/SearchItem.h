#pragma once
# ifndef WINVER
#  define WINVER 0x0500
#  define _WIN32_WINNT 0x0501
#  define _WIN32_IE 0x0501
# endif
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <stdlib.h>
# include <stdio.h>
# ifndef BBLIB_STATIC
#  define BBLIB_EXPORT __declspec(dllexport)
# endif
#include <windef.h>
#include <bblib.h>
#include "Menu.h"
#include <vector>
#include <string>

#include <blackbox/Search/lookup.h>

class SearchItem : public StringItem
{
  tstring m_bbPath;
  HWND m_hText;
  WNDPROC m_wpEditProc;
  RECT m_textrect;
  bool m_indexing;
  //std::vector<std::string> m_results;

public:
  SearchItem (const char * pszCommand, const char * init_string);
  ~SearchItem ();

  void Paint (HDC hDC);
  void Measure (HDC hDC, SIZE *size, StyleItem * pSI);
  void Invoke (int button);

  static LRESULT CALLBACK EditProc (HWND hText, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
  void OnInput ();
};

struct ResultItem : CommandItem
{
    tstring m_typed;

    ResultItem (const char* pszCommand, const char* pszTitle, bool bChecked)
        : CommandItem(pszCommand, pszTitle, bChecked)
    { }
    void Mouse (HWND hwnd, UINT uMsg, DWORD wParam, DWORD lParam);
    void Invoke (int button);
};

