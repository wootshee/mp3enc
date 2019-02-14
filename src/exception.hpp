//
//  exception.hpp - Exception definitions
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#ifndef MP3ENC_EXCEPTION_HPP
#define MP3ENC_EXCEPTION_HPP

#include <exception>

#include <string.h>

namespace mp3enc {

    // Class that wraps up C runtime errors in std::exception
    // interface
    class CRuntimeError : public std::exception {
        int _error;
    public:
        CRuntimeError(int error)
        : _error(error) {
        }

        virtual ~CRuntimeError() throw() {
        }

        virtual const char* what() const throw() {
            const char* msg = strerror(_error);
            // strerror() on POSIX.1-2001 systems may return NULL.
            // Handle it gracefully
            return msg ? msg : "Unexpected error";
        }
    }; // class CRuntimeError
} // namespace mp3enc

#endif // #define MP3ENC_EXCEPTION_HPP