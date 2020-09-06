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
#include <malloc.h>
#include <memory>
#include <string>
#include <tchar.h>

// Global Variables:
extern HINSTANCE g_hInst;                              // current instance
extern HWND g_hWnd;                                    // Handle of top-level window