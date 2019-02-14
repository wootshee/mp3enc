//
//  glob-win32.cpp - Windows file globbing
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#include <config.h>

#include "glob.hpp"
#include "exception.hpp"
#include "platform.hpp"

#include <windows.h>
#include <io.h>

#include <cassert>
#include <cerrno>
#include <string>
#include <memory.h>

namespace {
    class FindData {
        intptr_t _handle;
        _finddata_t _data;
        std::string _fullPath;
        std::string::size_type _dirEnd;
        int _error;

    public:
        FindData(const char* pattern)
        : _handle(_findfirst(pattern, &_data))
        , _fullPath(pattern)
        , _error(0) {
            if (!_handle)
                if (errno == ENOENT) {
                // No files matched the pattern
                _error = ENOENT;
            } else {
                throw mp3enc::CRuntimeError(errno);
            }

            // find and save directory end position within pattern string
            _dirEnd = _fullPath.find_last_of(mp3enc::platform::PathSeparator);
            if (_dirEnd != _fullPath.npos)
                _dirEnd++;
            else
                _dirEnd = 0;
            if (_error == 0) {
                // Windows stores only file names in _finddata structure.
                // Hence, it is a programmer's task to keep track of
                // full paths for matched files
                _fullPath.replace(_dirEnd, _fullPath.npos, _data.name);
            }
        }
        ~FindData() {
            _findclose(_handle);
            _handle = 0;
        }

        std::string nextFile() {
            if (_error == ENOENT) {
                // no more matches
                return std::string();
            }

            if (_error) {
                // previous nextFile() call failed,
                // report the error
                throw mp3enc::CRuntimeError(_error);
            }
            
            // skip subdirectories
            while (_data.attrib & _A_SUBDIR) {
                if (!findNext()) {
                    if (_error == ENOENT) {
                        // no more matches
                        return std::string();
                    }
                    throw mp3enc::CRuntimeError(_error);
                }
            }

            std::string matchedFile(_fullPath.c_str());
            // Fetch next file. If error occurs in findNext(),
            // it will be reported on next globNext() call.
            findNext();
            return matchedFile;
        }

    private:

        bool findNext() {
            const int error = _findnext(_handle, &_data);
            if (error) {
                _error = errno;
            }
            if (_error == 0) {
                _fullPath.replace(_dirEnd, _fullPath.npos, _data.name);
            }
            return error == 0;
        }  
    };
} // namespace

namespace mp3enc {
namespace platform {

GlobHandle globInit(const char* pattern) {
    return reinterpret_cast<GlobHandle>(new FindData(pattern));
}

std::string globNext(GlobHandle handle) {
    FindData* glob = reinterpret_cast<FindData*>(handle);
    assert(glob);
    return glob->nextFile();
}

void globClose(GlobHandle handle) {
    FindData* glob = reinterpret_cast<FindData*>(handle);
    delete glob;
}

} // namspace platform
} // namespace mp3enc