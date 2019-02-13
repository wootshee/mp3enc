//
//  encoder-pool.hpp
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#ifndef MP3ENC_ENCODER_POOL_H
#define MP3ENC_ENCODER_POOL_H

#include "glob.hpp"
#include "mutex.hpp"

#include <vector>

#include <pthread.h>

namespace mp3enc {
    // EncoderPool class manages a pool of worker threads
    // that do the actual WAV -> MP3 encoding
    class EncoderPool {
        threading::Mutex _lock;
        threading::Mutex _lockStdio;
        Glob& _queue;
        std::vector<pthread_t> _workers;
        bool _eof;

        EncoderPool(const EncoderPool&);
        EncoderPool& operator=(const EncoderPool&);
    public:
        EncoderPool(Glob& queue);
        ~EncoderPool() {};

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
