//
//  utils.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <string>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

namespace mp3enc {
namespace utils {
    // Appends path separator after directory name if needed
    template <typename Platform>
    std::string NormalizeDirectory(const std::string& dir) {
        if (dir.empty() || *dir.rbegin() == Platform::PathSeparator) {
            return dir;
        }
        return dir + Platform::PathSeparator;
    }
    
    void print_status(FILE* to, const char* format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(to, format, args);
        va_end(args);
    }
    
    void print_error(const char* prefix, int err) {
        char msg[1024];
        strerror_r(err, msg, sizeof(msg) / sizeof(*msg));
        print_status(stderr, "%s%s\n", prefix, msg);
    }
    
    void abort_on_error(int err) {
        print_error("Fatal error: ", err);
        abort();
    }
} // namespace utils
} // namespace mp3enc

#endif /* utils_h */
