//
//  platform-win32.cpp - Windows platform definitions
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#include <config.h>

#include "platform.hpp"

#include <Windows.h>

namespace mp3enc {
namespace platform {

const char PathSeparator = '\\';
const bool CaseSensitiveGlob = false;

// Cannot imagine a better way of detecting
// machine endianness on Windows...
#if REG_DWORD == REG_DWORD_LITTLE_ENDIAN
const bool BigEndian = false;
#else
const bool BigEndian = true;
#endif

int CpuCount() {
    SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

} // namespace platform
} // namespace mp3enc
