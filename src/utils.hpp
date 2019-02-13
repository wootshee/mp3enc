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

	void error(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);		
	}
    
    void print_error(const char* prefix, int err) {
        error("%s%s\n", prefix, strerror(err));
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

	// Here I use simple C-stream based file class for the sake of unified (errno based)
	// error reporting.
	class File {
		// No copying, please
		File(const File&);
		File& operator=(const File&);
	public:
		File(FILE* file) : _file(file) {
			if (!file) {
				// It is assummed that File() constructor is called from either InputFile
				// or OutputFile constructors with return value of fopen as it's argument.
				// Hence, we can assume that errno is still relevant at this point.
				throw c_runtime_error(errno);
			}
		}

		// It is expected that objects of this class are always allocated on the stack
		// and never used through pointers. Therefore it is safe to use non-virtual
		// destructor here.
		~File() {
			if (_file) {
				fclose(_file);
				_file = NULL;
			}
		}

		bool Error() const {
			return !_file || ferror(_file) != 0;
		}

		bool Eof() const {
			return _file && feof(_file) != 0;
		}
		
	protected:
		FILE* _file;
	}; // FilePtr

	class InputFile : public File {
		InputFile(const InputFile&);
		InputFile& operator=(const InputFile&);
	public:
		InputFile(const char* path) 
		: File(fopen(path, "rb")) {
		}

		// Compiler-generated destructor is sufficient here

		// Helper method for reading structures (like RIFF chunks)
		template <class T>
		bool ReadStruct(T& val) {
			return Read(reinterpret_cast<void*>(&val), sizeof(val)) == sizeof(val);
		}
		// Read buffer
		size_t Read(void* buf, size_t size) {
			return fread(buf, 1, size, _file);
		}
	}; // class InputFile

	class OutputFile : public File {
		OutputFile(const OutputFile&);
		OutputFile& operator=(const OutputFile&);
	public:
		OutputFile(const char* path) 
		: File(fopen(path, "wb")) {
		}

		// Compiler-generated destructor is sufficient here

		// Read buffer
		size_t Write(const void* buf, size_t size) {
			return fwrite(buf, 1, size, _file);
		}
	}; // class InputFile
    
} // namespace utils
} // namespace mp3enc

#endif /* utils_h */
