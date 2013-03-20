#pragma once

#include <Windows.h>
#include <tchar.h>
#include <cassert>
#include <string>

// undefining max macro from windows.h, it collides with std::max
#undef max

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