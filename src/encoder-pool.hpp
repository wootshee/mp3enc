//
//  encoder-pool.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_ENCODER_POOL_H
#define MP3ENC_ENCODER_POOL_H

#include "glob.hpp"
#include "mutex.hpp"
#include "mp3stream.hpp"
#include "platform.hpp"
#include "wavstream.hpp"

#include <cassert>
#include <vector>

#include <pthread.h>
#include <stdint.h>

namespace mp3enc {
    // EncoderPool class manages a pool of worker threads
    // that do the actual WAV -> MP3 encoding
    class EncoderPool {
        threading::Mutex _lock;
        threading::Mutex _lockStdio;
        Glob& _queue;
        std::vector<pthread_t> _workers;
        bool _eof;
    public:
        EncoderPool(Glob& queue);
        
        // ~EncoderPool()
        //
        // Default compiler-generated destructor is sufficient for this class,
        // since threads are always joined in Run() method.

        // Signals worker threads to start working on tasks. It is assumed
        // that this method is called only once per lifetime of EncoderPool
        // object.
        int Run();

    private:

        static void* threadProc(void* arg);
        const char* getFile();
        int processQueue();
    }; // class EncoderPool
} // namespace mp3enc

#endif // MP3ENC_ENCODER_POOL_H
