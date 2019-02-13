//
//  glob.hpp - cross platform file globbing
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#ifndef MP3ENC_GLOB_HPP
#define MP3ENC_GLOB_HPP

#include <string>

namespace mp3enc {
    namespace platform {
        //
        // Platform-specific glob handle
        //
        typedef void* GlobHandle;
    
        //
        // The following functions are implemented in platform-
        // specific files (glob-posix.cpp, glob-win32.cpp, etc)
        //
    
        GlobHandle globInit(const char* pattern);
        std::string globNext(GlobHandle handle);
        void globClose(GlobHandle handle);
    
    } //namespace platform

    class Glob {
        platform::GlobHandle _handle;
        // Objects of this class must not be copied
        Glob(const Glob&);
        Glob& operator=(const Glob&);

    public:
        Glob(const char* pattern)
        : _handle(platform::globInit(pattern)) {
        }
        ~Glob() {
            if (_handle) {
                platform::globClose(_handle);
                _handle = 0;
            }
        }

        std::string nextMatch() {
            return platform::globNext(_handle);
        }
    }; // class Glob
} // namespace mp3enc

#endif // #ifndef MP3ENC_GLOB_HPP