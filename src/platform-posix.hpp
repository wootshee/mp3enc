//
//  platform-posix.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef platform_posix_h
#define platform_posix_h

#include <string>

#include <glob.h>
#include <unistd.h>

namespace mp3enc {
namespace platform {
    struct Posix {
        static const char PathSeparator = '/';
        // POSIX supports extended glob wildcards and case sensitive
        // file systems
        static const bool CaseSensitiveGlob = true;
        
        static const int CpuCores() {
            return static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
        }
        
        //
        // POSIX Glob class uses POSIX glob() function for globbing
        //
        class Glob {
            int _pos;
            glob_t _glob;
            int _error;
            
        public:
            Glob(const std::string& pattern)
            : _pos(0)
            , _error(0) {
                memset(&_glob, 0, sizeof(_glob));
                // To maintain compatibility Win32 find_first/find_next,
                // which silently ignore files on errors, POSIX glob
                // implementation ignores them as well
                int res = glob(pattern.c_str(), GLOB_MARK, NULL, &_glob);
                if (res != 0) {
                    _error = errno;
                }
            }
            
            ~Glob() {
                if (_glob.gl_pathv)
                    globfree(&_glob);
            }
            
            const char* Next() {
                if (_error != 0) {
                    return NULL;
                }
                
                while (_pos < _glob.gl_pathc) {
                    const size_t len = strlen(_glob.gl_pathv[_pos]);
                    // skip empty filenames (if any) and directories
                    if (len == 0 || _glob.gl_pathv[_pos][len - 1] == PathSeparator) {
                        ++_pos;
                        continue;
                    }
                    break;
                }
                
                if (_pos < _glob.gl_pathc)
                    return _glob.gl_pathv[_pos++];
                
                // No more files left
                _error = ENOENT;
                return NULL;
            }
            
            int Error() {
                return _error;
            }
        }; // class Glob
        
    }; // struct Win32

} // namespace platform
} // namespace mp3enc

#endif /* platform_posix_h */
