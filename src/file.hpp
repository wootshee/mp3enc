//
//  file.hpp - I/O helper classes
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_FILE_HPP
#define MP3ENC_FILE_HPP

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
				throw CRuntimeError(errno);
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
    
} // namespace mp3enc

#endif // #ifndef MP3ENC_FILE_HPP
