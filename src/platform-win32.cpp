//
//  platform-win32.cpp - Windows platform definitions
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

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

