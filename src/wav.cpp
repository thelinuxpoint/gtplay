#include "wav.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

gtp::Wav::Wav(){

	snd_pcm_open(&handle, "default",SND_PCM_STREAM_PLAYBACK, 0);

 	snd_pcm_hw_params_alloca(&params);
 	snd_pcm_hw_params_any(handle, params);

	snd_pcm_hw_params_set_access(handle, params,SND_PCM_ACCESS_RW_INTERLEAVED);
	
	snd_pcm_hw_params_set_format(handle, params,SND_PCM_FORMAT_S16_LE);
	
	std::cout<<"Setting channels as ... "<<2<<std::endl;
	snd_pcm_hw_params_set_channels(handle, params, 2);
	
	std::cout<<"Setting rate as ... "<<this->rate<<std::endl;
	snd_pcm_hw_params_set_rate_near(handle, params,&this->rate, &dir);
	
	std::cout<<"Setting buffer size as ... "<<temporary<<std::endl;
	snd_pcm_hw_params_set_buffer_size_near(handle, params, &temporary);


	std::cout<<"Setting period size as ... "<<period<<std::endl;
	snd_pcm_hw_params_set_period_size_near(handle,params, &period, &dir);
	

	snd_pcm_hw_params(handle, params);

	/*GETTER*/
	snd_pcm_hw_params_get_period_size(params, &period,&dir);
	std::cout<<"Getting period size as ... "<<period<<std::endl;

	snd_pcm_hw_params_get_period_time(params, &tmp, &dir);
	std::cout<<"Getting period time as ... "<<tmp<<std::endl;

	snd_pcm_hw_params_get_buffer_size(params,&temporary);
	std::cout<<"Getting buffer size as ... "<<temporary<<std::endl;


	printf("PCM name: '%s'\n", snd_pcm_name(handle));
	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(handle)));
}

void gtp::Wav::play(const char* file){

	this->fd = open(file,O_RDONLY);
	int count=0;

	while (read(this->fd,this->buf,this->size)){
		count++;
			if (count>12){
				err = snd_pcm_writei(handle, this->buf, frames);
	    		if (err == -EPIPE) {
	       		/* EPIPE means underrun */
    				fprintf(stderr, "underrun occurred\n");
       				snd_pcm_prepare(handle);
     			}
     			else if (err < 0) {
       				fprintf(stderr,"error from writei: %s\n",snd_strerror(err));
       				snd_pcm_prepare(handle);
     			}
     	}
   	}
   	snd_pcm_drain(handle);
   	snd_pcm_close(handle);
}



