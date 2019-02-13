//
//  glob-win32.hpp - Windows file globbing
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#include "glob.hpp"
#include "exception.hpp"

#include <windows.h>
#include <io.h>

#include <cassert>
#include <cerrno>
#include <memory.h>

namespace {
    class FindData {
        intptr_t _handle;
        _finddata_t _data;
        int _error;

    public:
        FindData(const char* pattern)
        : _handle(_findfirst(pattern, &_data))
        , _error(0) {
            if (!_handle)
                if (errno == ENOENT) {
                // No files matched the pattern
                _error = ENOENT;
            } else {
				throw mp3enc::CRuntimeError(errno);
            }
        }
        ~FindData() {
            _findclose(_handle);
            _handle = 0;
        }

        const char* nextFile() {
            if (_error == ENOENT) {
                // no more matches
                return NULL;
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
                        return NULL;
                    }
                    throw mp3enc::CRuntimeError(_error);
                }
            }

            const char* matchedFile = _data.name;
            // Fetch next file. If error occurs in nextFile(),
            // it will be reported on next globNext() call.
            nextFile();
            return matchedFile;
        }

    private:

        bool findNext() {
            const int error = _findnext(_handle, &_data);
            if (error) {
                _error = errno;
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

const char* globNext(GlobHandle handle) {
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