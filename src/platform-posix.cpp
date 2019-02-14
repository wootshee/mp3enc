//
//  platform-posix.cpp - POSIX platform definitions
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#include "config.h"

#include "platform.hpp"

#include <unistd.h>

namespace mp3enc {
namespace platform {

const char PathSeparator = '/';
const bool CaseSensitiveGlob = true;

#ifdef WORDS_BIGENDIAN
const bool BigEndian = true;
#else
const bool BigEndian = false;
#endif

int CpuCount() {
    return static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
}

} // namespace platform
} // namespace mp3enc
