//
//  mutex.hpp
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#ifndef MP3ENC_MUTEX_HPP
#define MP3ENC_MUTEX_HPP

#include "utils.hpp"

#include <cassert>

#include <pthread.h>

namespace mp3enc {
namespace threading {
    
    // Simple mutex type - implements default PTHREAD mutex
    class Mutex {
        pthread_mutex_t _mutex;
        
    public:
        Mutex(bool createLocked = false) {
            // Default mutex settings are sufficient for the task
            const int res = pthread_mutex_init(&_mutex, NULL);
            if (res != 0) {
                // IMO, the failure here might only be caused by out of memory
                // condition. Perhaps, the best thing that can be done is just
                // aborting the process. Throwing an exception does not seem
                // to be of any help.
                utils::abort_on_error(res);
            }
            if (createLocked)
                Lock();
        }
        
        ~Mutex() {
            const int res = pthread_mutex_destroy(&_mutex);
            assert(res == 0);
        }
        
        void Lock() {
            const int res = pthread_mutex_lock(&_mutex);
            assert(res == 0);
        }
        
        void Unlock() {
            const int res = pthread_mutex_unlock(&_mutex);
            assert(res == 0);
        }
    }; // class Mutex
    
    // Scoped lock for convenience
    class ScopedLock {
        Mutex& _mutex;
    public:
        ScopedLock(Mutex& mutex)
        : _mutex(mutex) {
            _mutex.Lock();
        }
        
        ~ScopedLock() {
            _mutex.Unlock();
        }
    }; // class ScopedLock
    
} // namespace threading
} // namespace mp3enc

#endif /* MP3ENC_MUTEX_HPP */
