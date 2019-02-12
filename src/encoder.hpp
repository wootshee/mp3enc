//
//  encoder.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef encoder_h
#define encoder_h

#include <cassert>

#include "lame.h"

#include <stdio.h>

namespace mp3enc {

	template <class AudioFile>
	class Encoder {

		class Lame {
			lame_global_flags* _lame;

			// Not interested in copying and assignment
			// for the sake of simplicity
			Lame(const Lame&);
			Lame& operator=(const Lame&);			
			
		public:
			Lame()
			: _lame(lame_init()) {
				assert(_lame);
				// Silence lame and let EncoderPool
				// report errors in controlled manner
				lame_set_errorf(_lame, NULL);
				lame_set_debugf(_lame, NULL);
				lame_set_msgf(_lame, NULL);
			}
			~Lame() {
				if (_lame) {
					lame_close(_lame);
					_lame = NULL;
				}
			}

		} _lame;

		AudioFile& _source;

	public:

		Encoder(AudioFile& source)
		: _source(source) {
			
		}


	}; // class Encoder
	
} // namespace mp3enc


#endif // #ifndef encoder_h