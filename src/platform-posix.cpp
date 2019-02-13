//
//  platform-posix.cpp - POSIX platform definitions
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#include "config.h"

#include <unistd.h>

namespace mp3enc {
namespace platform {

const char PathSeparator = '/';
const bool CaseSensitiveGlob = true;

#ifdef WORDS_BIGENDIAN
static const bool BigEndian = true;
#else
static const bool BigEndian = false;
#endif

int CpuCores() {
    return static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
}

} // namespace platform
} // namespace mp3enc

