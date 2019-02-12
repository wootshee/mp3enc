//
//  encoder.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef encoder_h
#define encoder_h

#include <cstddef>

namespace mp3enc {

	//
	// Public encoder interfaces
	//

	struct AudioInputStream {
		virtual ~AudioInputStream() {};
		virtual int GetChannels() = 0;
		virtual int GetSampleRate() = 0;
		virtual size_t GetTotalSamples() = 0;
		virtual int ReadSamples(void* buf, size_t numSamples) = 0;
	};

	struct AudioOutputStream {
		virtual ~AudioOutputStream() {};
		virtual int AttachToInputStream(AudioInputStream* input) = 0;
		virtual int Encode() = 0;
	};
	
} // namespace mp3enc


#endif // #ifndef encoder_h