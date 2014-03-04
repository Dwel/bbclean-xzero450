#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "UpdateCursors.h"

/*
    Main function -- sets cursors and reloads them
*/
int main(int argc, char* argv[])
{
    std::string filename;
    if (argc == 2)
    {
        filename = (char*)argv[1];
    }
    else
    {
        filename = "default.ct";
    }

    if (readFileInfo(filename))
    {
        applyCursors();
    }
}
