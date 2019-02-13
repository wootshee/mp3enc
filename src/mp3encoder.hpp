//
//  mp3stream.hpp - LAME based MP3 encoder
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_MP3ENCODER_HPP
#define MP3ENC_MP3ENCODER_HPP

#include "wavfile.hpp"
#include "utils.hpp"

#include <vector>

#include <lame.h>

namespace mp3enc {

	void encode(WavFile& input, const char* outpath, size_t bufferSize = 128 * 1024);
	
} // namespace mp3enc

#endif // #ifndef MP3ENC_MP3ENCODER_HPP