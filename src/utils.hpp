//
//  utils.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include "exception.hpp"
#include "platform.hpp"

#include <cassert>
#include <cerrno>
#include <string>

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace mp3enc {
namespace utils {
    // Appends path separator after directory name if needed
    inline
	std::string NormalizeDirectory(const std::string& dir) {
        if (dir.empty() || *dir.rbegin() == platform::PathSeparator) {
            return dir;
        }
        return dir + platform::PathSeparator;
    }

	inline
	void error(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);		
	}
    
	inline
    void print_error(const char* prefix, int err) {
        error("%s%s\n", prefix, strerror(err));
    }
    
	inline
    void abort_on_error(int err) {
        print_error("Fatal error: ", err);
        abort();
    }

	inline
	uint16_t native_uint16(uint16_t value, bool bigEndian) {
		if (bigEndian != platform::BigEndian) {
			// swap bytes if target platform's endianess does not match
			// endianess of input value
			value = (value >> 16) | (value << 16);
		}
		return value;
	} 

	inline
	uint32_t native_uint32(uint32_t value, bool bigEndian) {
		if (bigEndian != platform::BigEndian) {
			// swap bytes if target platform's endianess does not match
			// endianess of input value
			value = (value << 24) | (value >> 24) |
			((uint32_t) native_uint16((uint16_t)(value >> 8), bigEndian)) << 8;
		}
		return value;
	}
    
} // namespace utils
} // namespace mp3enc

#endif /* utils_h */
