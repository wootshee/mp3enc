//
//  platform-win32.cpp - Windows platform definitions
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#include "platform.hpp"

namespace mp3enc {
namespace platform {

const char PathSeparator = '\\';
const bool CaseSensitiveGlob = false;

// Cannot imagine a better way of detecting
// machine endianness on Windows...
#if REG_WORD == REG_DWORD_LITTLE_ENDIAN
const bool BigEndian = false;
#else
const bool BigEndian = true;
#endif

int CpuCount() {
    return 1;
}

} // namespace platform
} // namespace mp3enc

