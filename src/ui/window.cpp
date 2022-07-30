#include "window.hpp"
#include "../gmp3/gmp3.hpp"
#include "../util/id3.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <unistd.h>

GTP::GWindow::GWindow(){
	set_title("GTPlay v0.1.0");
    set_default_size(600, 400);
    mainbox = Gtk::manage(new Gtk::VBox());
    mainbox2 = Gtk::manage(new Gtk::Box());

    pbox = Gtk::manage(new GTP::GPanel());

    Gdk::RGBA black_b2;
    black_b2.set("#252525");

    mainbox2->override_background_color(black_b2);

	pbox->set_size_request(-1,20);

    mainbox->pack_start(*mainbox2,true,true,0);
    
    mainbox->pack_end(*pbox,Gtk::PACK_SHRINK,0);


	mainbox2->signal_drag_data_received().connect(
		sigc::mem_fun(
			*this,
             &GWindow::on_label_drop_drag_data_received
        ) 
    );

	std::vector<Gtk::TargetEntry> listTargets;
	listTargets.push_back( Gtk::TargetEntry("STRING") );
	listTargets.push_back( Gtk::TargetEntry("text/plain") );
    
  	mainbox2->drag_dest_set(listTargets);
   
    add(*mainbox);

}

GTP::GWindow::~GWindow(){
	
}

void GTP::GWindow::on_open(){

}

void GTP::GWindow::on_button_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&,Gtk::SelectionData& selection_data, guint, guint){

	selection_data.set(
		selection_data.get_target(), 
		8 /* 8 bits format */,
        (const guchar*)"I'm Data!",
        9
    );
}
// #####################################################
void GTP::GWindow::on_label_drop_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int, int,const Gtk::SelectionData& selection_data, guint, guint time){

	const int length = selection_data.get_length();
	if((length >= 0) && (selection_data.get_format() == 8)){
		thr = new std::thread(
			[=,this]{
				this->on_drag_play(selection_data.get_data_as_string());
			}
		);
  		// thr->detach();
  	}
  	context->drag_finish(false, false, time);
}
// #####################################################

void GTP::GWindow::on_drag_play(std::string dir){

	std::vector<unsigned char> buffer = get_file(dir.substr(7),true);

	try {
		std::cout << "\x1b[1m\x1b[28mSize\x1b[0m : " << (float)buffer.size()/1000000 << " MB" << std::endl;
		unsigned offset = 0;
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

		GTP::GMP3 decoder(&buffer[offset]);
		std::cout << "\x1b[1m\x1b[28mMPEG VERSION\x1b[0m : " << decoder.gmp3_get_mpeg_v() <<" \x1b[1m\x1b[28mLAYER\x1b[0m : "<<decoder.gmp3_get_layer() << std::endl;
		std::cout << "\x1b[1m\x1b[28mSampling Rate\x1b[0m : " << decoder.gmp3_get_sampling_rate() << " Hz" << std::endl;
		std::cout << "\x1b[1m\x1b[28mBitRate\x1b[0m : " << decoder.gmp3_get_bitrate()/1000 <<" kbps"<< std::endl;
		
		unsigned sampling_rate = decoder.gmp3_get_sampling_rate();
		unsigned channels = (decoder.gmp3_get_channel_mode() == GTP::GMP3::Mono) ? 1 : 2;
		snd_pcm_t *handle;
		snd_pcm_hw_params_t *hw = NULL;
		snd_pcm_uframes_t frames = decoder.gmp3_get_bitrate() >= 128000 ? 256 : 512 ;
		snd_pcm_sw_params_t *sw;

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
		printf("\x1b[1m\x1b[28mChannel Mode\x1b[0m : %s\n", channels == 1 ? "Mono":"Stereo" );
		
		auto time_in_sec = (int)((buffer.size()*8)/(decoder.gmp3_get_bitrate()*channels));
		


		printf("\x1b[1m\x1b[28mAudio Time\x1b[0m : %d s\n", time_in_sec );
		printf("\x1b[1m\x1b[28mSamples Per Frame\x1b[0m : %d\n",decoder.spf );
		unsigned d_size = buffer.size();
		/* Start decoding. */

		std::thread t([=,this]{
			this->pbox->m_adjustment->set_value(0);

			while(this->timer < time_in_sec){
				sleep(1);
				this->timer++;
	  			this->pbox->m_adjustment->set_value((float)((this->timer*100)/time_in_sec));
			}
		});
		while (decoder.is_valid() && buffer.size() > offset + decoder.gmp3_get_header_size()) {
	     	// std::cout<<offset<<"/"<<d_size<<std::endl;
			decoder.gmp3_init_header(&buffer[offset]);

			if (decoder.is_valid()) {
				decoder.gmp3_init_frame_params(&buffer[offset]);
				offset += decoder.gmp3_get_frame_size();
			}

			int err = snd_pcm_writei(handle, decoder.gmp3_get_samples(), decoder.spf);

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
	     	// printf("offs %d",(int)((offset)/d_size)*100);
		}

		snd_pcm_drain(handle);
		snd_pcm_close(handle);
		t.join();
	} catch (std::bad_alloc) {
		printf("File does not exist.\n");
	}
}

std::vector<unsigned char> GTP::GWindow::get_file(std::string dir, bool drag){
	std::ifstream file;
	if (drag){
		dir.pop_back();dir.pop_back();
		std::string st = Glib::file_get_contents(dir);
		std::vector<unsigned char> buffer(st.begin(),st.end());
		return std::move(buffer);

	}else{
		std::string st = Glib::file_get_contents(dir);
		std::vector<unsigned char> buffer(st.begin(),st.end());
		return std::move(buffer);
	}

}
