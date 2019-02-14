//
//  encoder-pool.cpp
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#include <config.h>

#include "encoder-pool.hpp"
#include "mp3encoder.hpp"
#include "wavfile.hpp"

#include <cassert>
#include <vector>

#include <stdint.h>

namespace mp3enc {
  
EncoderPool::EncoderPool(Glob& queue)
// the mutex is created locked to block the workers
// until Run() method is called
: _lockQueue(true)
, _queue(queue)
, _workers(platform::CpuCount())
, _eof(false) {
    assert(!_workers.empty());
}

//
// IMPORTANT!
//
// This method is not thread-safe and it is asusmed that it is called only once
// per lifetime of EncoderPool object.
//        
int EncoderPool::Run() {
    // Create worker pool
    for (size_t i = 0; i < _workers.size(); ++i) {
        const int res = pthread_create(&_workers[i], NULL, threadProc, this);
        if (res != 0) {
            // It is highly unlikely that pthread_create() fails on a healthy system.
            // If it does, the process cannot proceed further, hence abort().
            utils::abort_on_error(res);
        }
    }

    // Release the queue lock and let the workers do their work
    _lockQueue.Unlock();
    
    int status = 0;
    
    // wait until workers finish their tasks
    for (size_t i = 0; i < _workers.size(); ++i) {
        uintptr_t workerStatus = 0;
        const int res = pthread_join(_workers[i], (void**) &workerStatus);
        assert(res == 0);
        if (workerStatus != 0) {
            status = static_cast<int>(workerStatus);
        }
    }
    
    return status;
}
// PTHREAD's thread proc
void* EncoderPool::threadProc(void* arg) {
    EncoderPool* thisPtr =
        reinterpret_cast<EncoderPool*>(arg);
    return reinterpret_cast<void*>(thisPtr->processQueue());
}

std::string EncoderPool::getFile() {
    threading::ScopedLock lock(_lockQueue);
    if (_eof)
        return std::string();
        
    std::string file;

    try {            
        file = _queue.nextMatch();
    } catch (...) {
        // Let only one worker report the error
        _eof = true;
        throw;
    }
    return file;
}
        
int EncoderPool::processQueue() {
    int status = 0;
    try {
        // I/O buffers are allocated by a first call to encode()
        // function and then re-used for all subsequent files
        std::vector<unsigned char> outBuf;
        std::vector<unsigned char> inBuf;
        for (std::string file(getFile()); !file.empty(); file = getFile()) {
            try {
                // Open input WAV stream
                WavFile input(file.c_str());

                // Encode input file to MP3 using default buffer size
                std::string mp3name(file);
                mp3name.replace(mp3name.begin() + mp3name.size() - 3, mp3name.end(), "mp3");
                encode(input, inBuf, outBuf, mp3name.c_str());

                // Report success
                threading::ScopedLock lock(_lockStdio);
                printf("%s: OK\n", file.c_str());
            } catch (std::exception& e) {
                // Failed to process data
                threading::ScopedLock lock(_lockStdio);
                utils::error("%s: %s\n", file.c_str(), e.what());
                status = -1;
            }
        }

        // Worker completed successfully
        status = 0;

    } catch (std::exception& e) {
        // Input queue thrown critical error
        threading::ScopedLock lock(_lockStdio);
        utils::error("Error: %s\n", e.what());
        status = 1;
    }
    return status;
}

} // namespace mp3enc