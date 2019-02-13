//
//  mp3encoder.hpp - LAME based MP3 encoder
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#ifndef MP3ENC_MP3ENCODER_HPP
#define MP3ENC_MP3ENCODER_HPP

#include "wavfile.hpp"

namespace mp3enc {

    void encode(WavFile& input, const char* outpath, size_t bufferSize = 128 * 1024);
    
} // namespace mp3enc

#endif // #ifndef MP3ENC_MP3ENCODER_HPP