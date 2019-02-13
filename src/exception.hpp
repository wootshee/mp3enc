//
//  exception.hpp - Exception definitions
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_EXCEPTION_HPP
#define MP3ENC_EXCEPTION_HPP

#include <exception>

#include <string.h>

namespace mp3enc {
	class c_runtime_error : public std::exception {
		int _error;
	public:
		c_runtime_error(int error)
		: _error(error) {
		}

		virtual ~c_runtime_error() throw() {
		}

		virtual const char* what() const throw() {
			return strerror(_error);
		}
	}; // class c_runtime_error
} // namespace mp3enc

#endif // #define MP3ENC_EXCEPTION_HPP