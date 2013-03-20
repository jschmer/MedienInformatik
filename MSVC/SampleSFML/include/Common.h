#pragma once

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