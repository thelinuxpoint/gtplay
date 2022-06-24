#include<iostream>

#include "wav.h"

namespace GTP {

	class GMP3 {

		private:

			float mpeg_version;
			unsigned layer;
			bool crc;
			unsigned bit_rate;
			unsigned sampling_rate;
			/**/
			unsigned char *buffer;
			/**/
			bool valid;
			/**/
			void gmp3_init_header(unsigned char *buffer);
			/**/
			void gmp3_set_layer(unsigned char byte);
			/**/
			void gmp3_set_mpeg_v();


		public:

			GMP3(unsigned char *buffer);

			bool stream();

			bool is_valid();

			enum class ChannelMode {
				Stereo = 0,
				JointStereo = 1,
				DualChannel = 2,
				Mono = 3
			};
			
			enum class Emphasis {
				None = 0,
				MS5015 = 1,
				Reserved = 2,
				CCITJ17 = 3
			};
		/* ############# EOC ############# */
	};

}
