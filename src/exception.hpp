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
	class CRuntimeError : public std::exception {
		int _error;
	public:
		CRuntimeError(int error)
		: _error(error) {
		}

		virtual ~CRuntimeError() throw() {
		}

		virtual const char* what() const throw() {
			return strerror(_error);
		}
	}; // class CRuntimeError
} // namespace mp3enc

#endif // #define MP3ENC_EXCEPTION_HPP