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
        // Mutex that serializes access to a queue
        threading::Mutex _lockQueue;
        // Mutex that serializes worker's access to standard
        // output streams to prevent garbled output
        threading::Mutex _lockStdio;
        // Input queue stores filenames that match the WAV extension
        // pattern
        Glob& _queue;
        // The horde of hard working threads
        std::vector<pthread_t> _workers;
        // Flag that signals that queue has been fully consumed
        bool _eof;

        EncoderPool(const EncoderPool&);
        EncoderPool& operator=(const EncoderPool&);

    public:

        // The class is designed for usage only within main() function
        EncoderPool(Glob& queue);
        ~EncoderPool() {};

        // Signals worker threads to start working on tasks. It is assumed
        // that this method is called only once per lifetime of EncoderPool
        // object and it's return value becomes the return status of the
        // program.
        int Run();

    private:

        static void* threadProc(void* arg);
        std::string getFile();
        int processQueue();
    }; // class EncoderPool
} // namespace mp3enc

#endif // MP3ENC_ENCODER_POOL_H
