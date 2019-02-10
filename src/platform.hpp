//
//  platform.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef platform_hpp
#define platform_hpp

#ifdef _WIN32
#include "platform-win32.hpp"
namespace mp3enc {
    typedef Platform mp3enc::platform::Win32;
}
#else
#include "platform-posix.hpp"
namespace mp3enc {
    typedef mp3enc::platform::Posix Platform;
}
#endif //#ifdef _WIN32

#endif // #ifndef platform_hpp
