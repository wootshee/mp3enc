//
//  audiostream.hpp - Audio stream interfaces
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_AUDIOSTREAM_HPP
#define MP3ENC_AUDIOSTREAM_HPP

#include <cstddef>

namespace mp3enc {

	//
	// Public encoder interfaces
	//

	struct AudioInputStream {
		virtual ~AudioInputStream() {};
		virtual int GetChannels() const = 0;
		virtual int GetSampleRate() const = 0;
		virtual int GetBitsPerSample() const = 0;
		virtual size_t GetTotalSamples() const = 0;
		virtual int ReadSamples(void* buf, int numSamples) = 0;
	};

	struct AudioOutputStream {
		virtual ~AudioOutputStream() {};
		virtual void AttachToInputStream(AudioInputStream* input) = 0;
		virtual void Encode() = 0;
	};
	
} // namespace mp3enc


#endif // #ifndef MP3ENC_AUDIOSTREAM_HPP