//
//  mp3encoder.hpp - LAME based MP3 encoder
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#ifndef MP3ENC_MP3ENCODER_HPP
#define MP3ENC_MP3ENCODER_HPP

#include "wavfile.hpp"

#include <vector>

namespace mp3enc {

    // "Sometimes, the elegant implementation is just a function.
    //  Not a method.  Not a class.  Not a framework.  Just a function."
    //  © John Carmack
 
    // encode() function encodes WAV input file to MP3 taking care of
    // input/outbut buffers. The input/outbut buffers can be re-used
    // between encode() calls.
    void encode(
        WavFile& input,
        std::vector<unsigned char>& inBuf,
        std::vector<unsigned char>& outBuf,
        const char* outpath);
    
} // namespace mp3enc

#endif // #ifndef MP3ENC_MP3ENCODER_HPP