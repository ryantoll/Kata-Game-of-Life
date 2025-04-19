// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#include "Resource.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <algorithm>
#include <execution>
#include <malloc.h>
#include <map>
#include <memory>
#include <string>
#include <tchar.h>
#include <vector>

// Global Variables:
extern HINSTANCE g_hInst;                              // current instance
extern HWND g_hWnd;                                    // Handle of top-level window