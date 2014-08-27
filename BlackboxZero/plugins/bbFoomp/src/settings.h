#pragma once
#include "foompbutton.h"

struct Settings
{
  int xpos, ypos;
  int width, height, BorderWidth;
  int FooMode, FooWidth;
  FoompButton buttons[e_last_button_item];
  bool FooDockedToSlit;
  bool FooOnTop, FooTrans, FooAlign, FooShadowsEnabled;
  // Determines style:
  int InnerStyleIndex;
  int OuterStyleIndex;
  int FooScrollSpeed;
  int transparencyAlpha;
  // paths
  char rcpath[MAX_PATH];
  char FooPath[MAX_LINE_LENGTH];
  char NoInfoText[MAX_LINE_LENGTH];

  void ReadRCSettings ();
  void WriteRCSettings ();
  void WriteDefaultRCSettings ();
};


Settings & getSettings ();

