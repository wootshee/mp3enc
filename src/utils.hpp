//
//  utils.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <cassert>
#include <string>

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
        print_status(stderr, "%s%s\n", prefix, strerror(err));
    }
    
    void abort_on_error(int err) {
        print_error("Fatal error: ", err);
        abort();
    }

	template <typename Platform>
	uint16_t native_uint16(uint16_t value, bool bigEndian) {
		if (bigEndian != Platform::BigEndian) {
			// swap bytes if target platform's endianess does not match
			// endianess of input value
			value = (value >> 16) | (value << 16);
		}
		return value;
	} 

	template <typename Platform>
	uint32_t native_uint32(uint32_t value, bool bigEndian) {
		if (bigEndian != Platform::BigEndian) {
			// swap bytes if target platform's endianess does not match
			// endianess of input value
			value = (value << 24) | (value >> 24) |
			((uint32_t) native_uint16<Platform>((uint16_t)(value >> 8), bigEndian)) << 8;
		}
		return value;
	} 

    
} // namespace utils
} // namespace mp3enc

#endif /* utils_h */
