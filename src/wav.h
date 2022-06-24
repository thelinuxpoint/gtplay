/*
	sample is of 16 bit
	frame = no of channel x 16bit
	
*/
#include<alsa/asoundlib.h>
#include <string>

namespace GTP {
	
	class GWAV {

		private:
			int fd;
			int err;
			int dir;
			unsigned int tmp;
			unsigned int rate = 44100;
			int size = 128;


		public:

			snd_pcm_t *handle;
  			snd_pcm_hw_params_t *params;
			snd_pcm_uframes_t frames = 32;
			snd_pcm_uframes_t period = frames*4;
			snd_pcm_uframes_t temporary = period * 8;

			char buf[256];
			//#################################
			Wav();
			//#################################
			void play(const char * ptr);

			void play(std::string file);

	};

}
