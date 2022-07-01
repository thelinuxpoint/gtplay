#include <iostream>
#include <fstream>
#include <functional>
#include <signal.h>
// #include <gtkmm.h>
#include <thread>
/*  Project Imports  */
#include "./gmp3/gmp3.hpp"
#include "./util/id3.h"

static volatile sig_atomic_t keep_running = 1;

//
static void sig_handler(int _){

	printf("\33[2K\r[\x1b[35mCTRL+C\x1b[0m] received ... Playing till last frame\n");
    keep_running = 0;
}
bool stream(GTP::GMP3 &decoder, std::vector<unsigned char> &buffer, unsigned offset) {

	unsigned sampling_rate = decoder.gmp3_get_sampling_rate();
	unsigned channels = 2;// decoder.gmp3_get_channel_mode() == GTP::GMP3::Mono ? 1 : 2;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *hw = NULL;
	snd_pcm_uframes_t frames = decoder.gmp3_get_bitrate() >= 128000 ? 256 : 512 ;
	snd_pcm_sw_params_t *sw;

    signal(SIGINT, sig_handler);
	if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0)
		exit(1);

	snd_pcm_hw_params_alloca(&hw);
	snd_pcm_hw_params_any(handle, hw);

	if (snd_pcm_hw_params_set_access(handle, hw, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		exit(1);
	if (snd_pcm_hw_params_set_format(handle, hw, SND_PCM_FORMAT_FLOAT_LE) < 0)
		exit(1);
	if (snd_pcm_hw_params_set_channels(handle, hw, channels) < 0)
		exit(1);
	if (snd_pcm_hw_params_set_rate_near(handle, hw, &sampling_rate, NULL) < 0)
		exit(1);
	if (snd_pcm_hw_params_set_period_size_near(handle, hw, &frames, NULL) < 0)
		exit(1);
	if (snd_pcm_hw_params(handle, hw) < 0)
		exit(1);
	if (snd_pcm_hw_params_get_period_size(hw, &frames, NULL) < 0)
		exit(1);
	if (snd_pcm_hw_params_get_period_time(hw, &sampling_rate, NULL) < 0)
		exit(1);
	printf("Time : %d seconds\n", (int)(buffer.size()/(decoder.gmp3_get_bitrate()/8))/2 );
	/* Start decoding. */
	while (decoder.is_valid() && buffer.size() > offset + decoder.gmp3_get_header_size() && keep_running) {
		// printf("offset %d\n",offset );
		decoder.gmp3_init_header(&buffer[offset]);
		if (decoder.is_valid()) {
			decoder.gmp3_init_frame_params(&buffer[offset]);
			offset += decoder.gmp3_get_frame_size();
		}
		int err = snd_pcm_writei(handle, decoder.gmp3_get_samples(), 1152);

		if (err == -EPIPE){
			snd_pcm_prepare(handle);
			// printf("underrun occured\n");
		}else if (err == -ESTRPIPE) {
			while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			sleep(1);	/* wait until suspend flag is gone */
			if (err < 0) {
				snd_pcm_prepare(handle);
			}
     	}
	}
	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	return true;
}

/*#######################*/
std::vector<id3> get_id3_tags(std::vector<unsigned char> &buffer, unsigned &offset){
	std::vector<id3> tags;
	int i = 0;
	bool valid = true;

	while (valid) {
		id3 tag(&buffer[offset]);
		if (valid = tag.is_valid()) {
			tags.push_back(tag);
			offset += tags[i++].get_id3_offset() + 10;
		}
	}
	return tags;
}
/*#######################*/
std::vector<unsigned char> get_file(const char *dir){

	std::ifstream file(dir, std::ios::in | std::ios::binary | std::ios::ate);
	std::vector<unsigned char> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read((char *)buffer.data(), buffer.size());
	file.close();
	return std::move(buffer);
}
/*#######################*/

int main(int argc, char const *argv[]){
    std::cout << "******************** GTPlay ********************" << '\n';

    if (argc > 2) {
		printf("Unexpected number of arguments.\n");
		return -1;
	} else if (argc == 1) {
		printf("No directory specified.\n");
		return -1;
	}
    try {

		std::vector<unsigned char> buffer = get_file(argv[1]);

		std::cout << "Size : " << (float)buffer.size()/1000000 << " MB" << std::endl;
		unsigned offset = 0;
		std::vector<id3> tags = get_id3_tags(buffer, offset);
		GTP::GMP3 decoder(&buffer[offset]);
		std::cout << "MPEG VERSION : " << decoder.gmp3_get_mpeg_v() << std::endl;

		std::cout << "Sampling Rate : " << decoder.gmp3_get_sampling_rate() << " Hz" << std::endl;
		std::cout << "BitRate : " << decoder.gmp3_get_bitrate()/1000 <<" kbps"<< std::endl;
		auto stream_th = std::bind(stream,decoder,buffer,offset);
        // GTP::stream(decoder,buffer,offset);
		std::thread t(stream_th);
		t.join();
	} catch (std::bad_alloc) {
		printf("File does not exist.\n");
		return -1;
	}
	std::cout<<"**************** Decoding Finished ****************"<<std::endl;
    return 0;
}
