//
//  encoder-pool.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef encoder_pool_h
#define encoder_pool_h

#include "mutex.hpp"
#include "wavqueue.hpp"

#include <cassert>
#include <vector>

#include <pthread.h>
#include <stdint.h>

namespace mp3enc {
    // EncoderPool class manages a pool of worker threads
    // that do the actual WAV -> MP3 encoding
    template <class Platform>
    class EncoderPool {
        threading::Mutex _lock;
        WavQueue<Platform>& _queue;
        std::vector<pthread_t> _workers;
        bool _eof;
    public:
        EncoderPool(WavQueue<Platform>& queue)
        // the mutex is created locked to block the workers
        // until Run() method is called
        : _lock(true)
        , _queue(queue)
        , _workers(Platform::CpuCores())
        , _eof(false) {
            for (size_t i = 0; i < _workers.size(); ++i) {
                const int res = pthread_create(&_workers[i], NULL, threadProc, this);
                if (res != 0) {
                    // It is highly unlikely that pthread_create() fails on a healthy system.
                    // If it does, the process cannot proceed further, hence abort().
                    utils::abort_on_error(res);
                }
            }
        }
        
        // ~EncoderPool()
        //
        // Default compiler-generated destructor is sufficient for this class,
        // since threads are joined in Run() method.
        
        int Run() {
            // Release the mutex and let the workers do their work
            _lock.Unlock();
            
            int status = 0;
            
            // wait until workers finish their tasks
            for (int i = 0; i < _workers.size(); ++i) {
                uintptr_t workerStatus = 0;
                const int res = pthread_join(_workers[i], (void**) &workerStatus);
                assert(res == 0);
                if (workerStatus != 0) {
                    status = (int) workerStatus;
                }
            }
            
            return status;
        }
    private:
        // PTHREAD's thread proc
        static void* threadProc(void* arg) {
            EncoderPool<Platform>* thisPtr =
                reinterpret_cast<EncoderPool<Platform>*>(arg);
            return reinterpret_cast<void*>(thisPtr->worker());
        }
        
        std::string getFile() {
            if (_eof) {
                // no more files to process
                return std::string();
            }
            
            std::string file(_queue.Next());
            switch (_queue.Error()) {
                case ENOENT:
                    _eof = true;
                case 0:
                    break;
                default:
                    // report error
                    utils::print_error("Error: ", _queue.Error());
                    // set EOF marker to report the error only by one worker,
                    // others will simply exit due to EOF
                    _eof = true;
            }
            return file;
        }
        
        std::string getFirstFile() {
            threading::ScopedLock lock(_lock);
            return getFile();
        }
        
        std::string getNextFile(const std::string processedFile, int status) {
            threading::ScopedLock lock(_lock);
            
            // Report processed file status
            if (status != 0) {
                utils::print_error((processedFile + ": ").c_str(), status);
            } else {
                utils::print_status(stdout, "%s: OK\n", processedFile.c_str());
            }
            
            // Get next file from the queue
            return getFile();
        }
        
        int worker() {
            int status = 0;
            for (std::string file = getFirstFile(); !file.empty(); file = getNextFile(file, status)) {
                // do actual encoding here
                
            }
            return status;
        }
    }; // class EncoderPool
}

#endif /* encoder_pool_h */
