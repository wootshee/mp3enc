//
//  tiny mp3encoder
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
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
// and focus on simplicity of the code.
//

#include <config.h>

#include "glob.hpp"
#include "platform.hpp"

#include "encoder-pool.hpp"
#include "utils.hpp"

#include <stdio.h>

using namespace mp3enc;

void usage() {
    puts("Usage: mp3enc <directory>)");
}

int main(int argc, const char* argv[]) {
    
    if (argc != 2) {
        usage();
        return 1;
    }

    int status = 0;

    try {
        std::string pattern(
            utils::NormalizeDirectory(argv[1]) +
            (platform::CaseSensitiveGlob ? "*.[wW][aA][vV]" : "*.wav"));
    
        Glob wavFiles(pattern.c_str());
        
        // Initialize and run encoder worker pool on given directory
        // using all available CPU cores
        EncoderPool pool(wavFiles);
        status = pool.Run();
    } catch(std::exception& e) {
        utils::error("Error: %s\n", e.what());
    } catch(...) {
        utils::error("Internal error\n");
    }

    return status;
}
