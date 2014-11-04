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

class SearchItem : public StringItem
{
  HWND m_hText;
  WNDPROC m_wpEditProc;
  RECT m_textrect;
  // history ?
  std::vector<std::string> m_results;

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
