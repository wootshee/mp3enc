//
//  glob-posix.hpp - POSIX file globbing
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#include "config.h"

#include "glob.hpp"
#include "exception.hpp"
#include "platform.hpp"

#include <cassert>
#include <cerrno>
#include <stdexcept>

#include <glob.h>
#include <memory.h>

namespace {

    struct PosixGlob {
        glob_t _data;
        int _consumed;

        PosixGlob(const char* pattern)
        : _consumed(0) {
            memset(&_data, 0, sizeof(glob_t));

            // To maintain compatibility Win32 find_first/find_next,
            // which silently ignore files on errors, POSIX glob
            // implementation ignores them as well
            int res = glob(pattern, GLOB_MARK, NULL, &_data);
            switch (res) {
            case 0:
            case GLOB_NOMATCH:
                break;
            case GLOB_NOSPACE:
                throw mp3enc::CRuntimeError(ENOMEM);
            case GLOB_ABORTED:
                throw mp3enc::CRuntimeError(EIO);
            default:
                throw std::runtime_error("Unexpected glob() error");
            }
        }

        ~PosixGlob() {
            globfree(&_data);
        }

        const char* nextFile() {
            while (_consumed < _data.gl_pathc) {
                const size_t len = strlen(_data.gl_pathv[_consumed]);
                // skip empty filenames (if any) and directories
                if (len == 0 || _data.gl_pathv[_consumed][len - 1] == mp3enc::platform::PathSeparator) {
                    ++_consumed;
                    continue;
                }
                break;
            }
            
            if (_consumed < _data.gl_pathc)
                return _data.gl_pathv[_consumed++];
            
            // No more files left
            return NULL;
        }
    }; // class Glob

} // namespace

namespace mp3enc {
namespace platform {

GlobHandle globInit(const char* pattern) {
    return reinterpret_cast<GlobHandle>(new PosixGlob(pattern));
}

const char* globNext(GlobHandle handle) {
    assert(handle);
    PosixGlob* posixGlob = reinterpret_cast<PosixGlob*>(handle);
    return posixGlob->nextFile();
}

void globClose(GlobHandle handle) {
    assert(handle);
    PosixGlob* posixGlob = reinterpret_cast<PosixGlob*>(handle);
    delete posixGlob;
}

} // namspace platform
} // namespace mp3enc