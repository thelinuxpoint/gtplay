/* #############################################################

	AUTHOR : Prakash Choudhary
	EMAIL  : choudharyprakash0660@gmail.com
	Reference : https://github.com/FlorisCreyf/mp3-decoder

#################################################################*/
#include <iostream>

#include <cmath>
#include <cstring>
#include <alsa/asoundlib.h>
#include <vector>
// project imports
#include "gtables.hpp"
#include "../util/gutil.hpp"

namespace GTP {

	class GMP3 {

		private:
			/* MP3 HEADER Information */
			float mpeg_version;
			unsigned layer;
			bool crc;
			unsigned bit_rate;
			unsigned sampling_rate;
			bool padding;
			int channels;

			unsigned mode_extension[2];

			bool info[3];

			/* Pointer to the buffer */
			unsigned char *buffer;
			/* */
			bool valid = true;
			/*###############################################*/
			int num_prev_frames = 9;
			int prev_frame_size[9];
			int frame_size;

			int main_data_begin;
			bool scfsi[2][4];

			/* Allocate space for two granules and two channels. */
			int part2_3_length[2][2];

			int part2_length[2][2];
			int big_value[2][2];
			int global_gain[2][2];

			// Determines the number of bits used for the transmission of the scalefactors.
			int scalefac_compress[2][2];

			int slen1[2][2];
			int slen2[2][2];
			bool window_switching[2][2];
			int block_type[2][2];
			bool mixed_block_flag[2][2];
			int switch_point_l[2][2];
			int switch_point_s[2][2];
			int table_select[2][2][3];
			int subblock_gain[2][2][3];
			int region0_count[2][2];
			int region1_count[2][2];
			int preflag[2][2];
			int scalefac_scale[2][2];
			int count1table_select[2][2];

			int scalefac_l[2][2][22];
			int scalefac_s[2][2][3][13];

			float prev_samples[2][32][18];
			float fifo[2][1024];

			std::vector<unsigned char> main_data;

			float samples[2][2][576];
			// float pcm[576 * 4];
			float *pcm = NULL ;

			struct {
				const unsigned *long_win;
				const unsigned *short_win;
			} band_index;

			struct {
				const unsigned *long_win;
				const unsigned *short_win;
			} band_width;

			/*###############################################*/
			// PRIVATE FUNCTIONS STARTS HERE
			/**/

			/**/
			void gmp3_set_layer(unsigned char byte);
			/**/
			void gmp3_set_mpeg_v();

			void gmp3_set_crc();

			void gmp3_set_sampling_rate();

			void gmp3_set_bit_rate(unsigned char *buffer);

			void gmp3_set_padding();

			void gmp3_set_channel_mode(unsigned char *buffer);

			void gmp3_set_emphasis(unsigned char *buffer);

			void gmp3_set_frame_size();

			void gmp3_set_main_data(unsigned char *buffer);

			void gmp3_set_info();

			void gmp3_set_tables();

			void gmp3_interleave();

			void gmp3_set_side_info(unsigned char *buffer);

			void gmp3_synth_filterbank(int gr, int ch);

			void gmp3_frequency_inversion(int gr, int ch);

			void gmp3_set_mode_extension(unsigned char *buffer);

			void gmp3_imdct(int gr, int ch);

			void gmp3_alias_reduction(int gr, int ch);

			void gmp3_ms_stereo(int gr);

			void gmp3_reorder(int gr, int ch);

			void gmp3_requantize(int gr, int ch);

			void gmp3_unpack_samples(unsigned char *main_data, int gr, int ch, int bit, int max_bit);

			void gmp3_unpack_scalefac(unsigned char *main_data, int gr, int ch, int &bit);
		public:
			int spf = 0;

			GMP3(unsigned char *buffer);

			bool is_valid();

			enum ChannelMode {
				Stereo = 0,
				JointStereo = 1,
				DualChannel = 2,
				Mono = 3
			};

			enum Emphasis {
				None = 0,
				MS5015 = 1,
				Reserved = 2,
				CCITJ17 = 3
			};

			Emphasis emphasis;

			ChannelMode channel_mode;

			void gmp3_init_header(unsigned char *buffer);
			/**/
			void gmp3_init_frame_params(unsigned char *buffer);


			/* GET Functions */

			unsigned gmp3_get_layer();

			float gmp3_get_mpeg_v();

			bool gmp3_get_crc();

			unsigned gmp3_get_bitrate();

			unsigned gmp3_get_sampling_rate();

			bool gmp3_get_padding();

			ChannelMode gmp3_get_channel_mode();

			unsigned *gmp3_get_mode_extension();

			Emphasis gmp3_get_emphasis();

			unsigned gmp3_get_frame_size();

			unsigned gmp3_get_header_size();

			float *gmp3_get_samples();

			bool *gmp3_get_info();

		/* ############# EOC ############# */
	};

}
