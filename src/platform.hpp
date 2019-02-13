//
//  platform.hpp - platform specific stuff
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_PLATFORM_HPP
#define MP3ENC_PLATFORM_HPP

namespace mp3enc {
namespace platform {

    // Path separator character
    extern const char PathSeparator;
    // Is target platform big endian?
    extern const bool BigEndian;
    // Does target platform support case sensitive glob masks?
    extern const bool CaseSensitiveGlob;
    // Determine number of CPUs
    int CpuCount();

} // namespace platform
} // namespace mp3enc

#endif // #ifndef MP3ENC_PLATFORM_HPP
