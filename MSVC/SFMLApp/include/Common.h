/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

// Loading static libs via visual studio pragma extension for easier use of this lib.
// These libs won't have to be included in code that uses this lib.
#ifdef _DEBUG
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "../../extlib/SFML/lib/sfml-graphics-s-d.lib")
#pragma comment (lib, "../../extlib/SFML/lib/sfml-system-s-d.lib")
#pragma comment (lib, "../../extlib/SFML/lib/sfml-window-s-d.lib")
#else
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "../../extlib/SFML/lib/sfml-graphics-s.lib")
#pragma comment (lib, "../../extlib/SFML/lib/sfml-system-s.lib")
#pragma comment (lib, "../../extlib/SFML/lib/sfml-window-s.lib")
#endif

// common includes
#include <Windows.h>
#include <tchar.h>
#include <cassert>

// undefining max macro from windows.h, it collides with std::max
#undef max

// tools for debug builds
#ifdef _DEBUG
#include <string>

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

inline void Log(TCHAR* msg) {
    tstring str = msg;
    str.append(_T("\n"));
    OutputDebugString(str.c_str());
}
#else
inline void Log(TCHAR* msg) {}
#endif