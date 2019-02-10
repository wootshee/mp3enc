//
//  mp3enc.cpp - tiny mp3encoder
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

//
// Design Note
//
// Since the task insists on using PTHREADs for threading,
// I assume that the target C++ compiler is older than Cx11
// (otherwise classes from <thread> header would do the job).
//
// Hence, I won't use the features from Cx11 and newer standards
// (thread support, smart pointers, lambdas and other fancy stuff)
// and focus on simplicity of the code. If extendability to other
// input/output formats would be desired, I would design the code
// differently :)
//

#include "config.h"

#include <iostream>
#include <memory>

#include "platform.hpp"

#include "encoder-pool.hpp"
#include "wavqueue.hpp"

void usage() {
    std::cout << "Usage: mp3enc <directory>" << std::endl;
}

int main(int argc, const char * argv[]) {
    
    if (argc > 2) {
        usage();
        return 1;
    }
    
    // Prepare queue of input WAV files. Use current directory if
    // no directory is explicitly given to a command.
    mp3enc::WavQueue<mp3enc::Platform> queue(argc == 2 ? argv[1] : "");
    
    // Initialize and run encoder worker pool using all available CPU cores
    mp3enc::EncoderPool<mp3enc::Platform> pool(queue);
    return pool.Run();
}
