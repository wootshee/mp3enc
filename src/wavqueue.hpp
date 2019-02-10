//
//  wavqueue.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef wavqueue_h
#define wavqueue_h

#include <string>

#include "utils.hpp"

namespace mp3enc {
    
    template <typename Platform>
    class WavQueue {
        typename Platform::Glob _glob;
    public:
        
        WavQueue(const std::string& dirPath)
        : _glob(utils::NormalizeDirectory<Platform>(dirPath) +
                (Platform::CaseSensitiveGlob ? "*.[wW][aA][vV]" : "*.wav")) {
        }
        
        std::string Next() {
            const char* next = _glob.Next();
            if (!next)
                return std::string();
            return std::string(next);
        }
        
        int Error() {
            return _glob.Error();
        }
    };
} // namespace mp3enc

#endif /* wavqueue_h */
