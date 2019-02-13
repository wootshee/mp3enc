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
#include "mp3stream.hpp"
#include "wavstream.hpp"

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
        threading::Mutex _lockStdio;
        typename Platform::Glob& _queue;
        std::vector<pthread_t> _workers;
        bool _eof;
    public:
        EncoderPool(typename Platform::Glob& queue)
        // the mutex is created locked to block the workers
        // until Run() method is called
        : _lock(true)
        , _queue(queue)
        , _workers(Platform::CpuCores())
        , _eof(false) {
        }
        
        // ~EncoderPool()
        //
        // Default compiler-generated destructor is sufficient for this class,
        // since threads are joined in Run() method.

        int Run() {
        	//
        	// IMPORTANT!
        	//
        	// This method is not thread-safe and it is asusmed that it is called only once
        	// per lifetime of EncoderPool object.
        	//

        	
        	// Create worker pool
            for (int i = 0; i < _workers.size(); ++i) {
                const int res = pthread_create(&_workers[i], NULL, threadProc, this);
                if (res != 0) {
                    // It is highly unlikely that pthread_create() fails on a healthy system.
                    // If it does, the process cannot proceed further, hence abort().
                    utils::abort_on_error(res);
                }
            }

            // Release the mutex and let the workers do their work
            _lock.Unlock();
            
            int status = 0;
            
            // wait until workers finish their tasks
            for (int i = 0; i < _workers.size(); ++i) {
                uintptr_t workerStatus = 0;
                const int res = pthread_join(_workers[i], (void**) &workerStatus);
                assert(res == 0);
                if (workerStatus != 0) {
                    status = static_cast<int>(workerStatus);
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
        
        const char* getFile() {
            threading::ScopedLock lock(_lock);
            if (_eof)
            	return NULL;
            	
            const char* file = NULL;

            try {            
            	file = _queue.Next();
            } catch (...) {
            	// Let only one worker report the error
            	_eof = true;
            	throw;
            }
            return file;
        }
               
        int worker() {
            int status = 0;
            const char* file = NULL;
            try {
	            for (file = getFile(); file; file = getFile()) {
	            	try {
		                // Open input WAV stream
						WavInputStream<Platform> input(file);
		
						// Create output MP3 stream
						std::string mp3name(file);
						mp3name.replace(mp3name.begin() + mp3name.size() - 3, mp3name.end(), "mp3");
						Mp3OutputStream output(mp3name.c_str());
		
						// Encode audio stream
						output.AttachToInputStream(&input);
						output.Encode();

						// Report success
						threading::ScopedLock lock(_lockStdio);
						utils::print_status(stdout, "%s: OK\n", file);
					} catch (std::exception& e) {
	        			threading::ScopedLock lock(_lockStdio);
						utils::print_status(stderr, "%s: %s\n", file, e.what());
						status = -1;
												
					}
	            }

	            // Worker completed successfully
	            status = 0;

			} catch (std::exception& e) {
	        	threading::ScopedLock lock(_lockStdio);
	        	utils::print_status(stderr, "Error: %s\n", e.what());
	        	status = 1;
	        }
            return status;
        }
    }; // class EncoderPool
} // namespace

#endif /* encoder_pool_h */
