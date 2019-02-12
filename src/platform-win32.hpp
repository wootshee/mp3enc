//
//  platform-win32.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef platform_win32_h
#define platform_win32_h

#include <string>

#include <windows.h>
#include <io.h>

namespace mp3enc {
namespace platform {
    struct Win32 {
        static const char PathSeparator = '\\';
        static const bool CaseSensitiveGlob = false;

        // Cannot imagine a better way of detecting
        // machine endianness on Windows...
		#if REG_WORD == REG_DWORD_LITTLE_ENDIAN
		static const bool BigEndian = false;
		#else
		static const bool BigEndian = true;
		#endif


        // I chose to use _findfirst/_findnext API since it
        // closely resembles POSIX calls and sets errno
        // properly
        class Glob {
            intptr_t _handle;
            _finddata_t _data;
            int _error;
            
            int findnext() {
                const int res = _findnext(_handle, &_data);
                if (res != 0) {
                    if (errno != ENOENT) {
                        _error = errno;
                    }
                }
                return res;
            }
        public:

            Glob(const std::string& pattern)
            : _handle(_findfirst(pattern.c_str(), &_data))
            , _error(0) {
                if (_handle == -1) {
                    _error = errno;
                }
            }
            
            ~Glob() {
                if (_handle != -1)
                    _findclose(_handle);
            }
            
            std::string Next() {
                if (_error != 0) {
                    return NULL;
                }
                
                // skip subdirectories
                while (_data.attrib & _A_SUBDIR) {
                    if (0 != findnext()) {
                        return NULL;
                    }
                }
                std::string res(_data.name);
                findnext();
                return res;
            }
            
            int Error() {
                return _error;
            }
        }; // class Glob
        
    }; // struct Win32
    
} // namespace platform
} // namespace mp3enc

#endif /* platform_win32_h */
