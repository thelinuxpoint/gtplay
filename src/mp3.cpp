#include <vector>

#include "mp3.h"

// ###############################################
/* MPEG HEADER
  +---------------------------------------------+
  |	1111 1111 1111   1     xx    x   { 16-bit } |
  |      sync       I-D   Layer CRC             |
  +---------------------------------------------+--------------------------+
  |	  xxxx     xx      x        x      xx   xx      x          x    xx     |
  |  Bitrate  Freq  Padding  private  Mode ModeExt copyright  copy Emphasis|
  +------------------------------------------------------------------------+
*/

void GTP::GMP3::gmp3_init_header(unsigned char *buffer){

	if (buffer[0] == 0xFF && buffer[1] >= 0xE0) {
		this->buffer = buffer;

		gmp3_set_mpeg_v();
		gmp3_set_layer(buffer[1]);
		gmp3_set_crc();
		gmp3_set_info();
		gmp3_set_emphasis();
		gmp3_set_sampling_rate();
		// table
		gmp3_set_channel_mode(buffer);
		gmp3_set_mode_extension();
		gmp3_set_padding();
		gmp3_set_bit_rate(buffer);
		gmp3_set_frame_size();

	}else {
		valid = false;
	}

}
/* ###############################################
	‘1’ - then MPEG version 1
	‘0’ - then MPEG version 2
*/
void GTP::GMP3::gmp3_set_mpeg_v(){
	/* check bit 4 and bit 3 */
	if ((buffer[1] & 0x10) == 0x10 && (buffer[1] & 0x08) == 0x08){
		mpeg_version = 1;
	}else if ((buffer[1] & 0x10) == 0x10 && (buffer[1] & 0x08) != 0x08){
		mpeg_version = 2;
	}else if ((buffer[1] & 0x10) != 0x10 && (buffer[1] & 0x08) == 0x08){
		mpeg_version = 0;
	}else if ((buffer[1] & 0x10) != 0x10 && (buffer[1] & 0x08) != 0x08){
		mpeg_version = 2.5;
	}
}
/* ###############################################
	‘01’ - Layer III
	‘10’ - Layer II
	‘11’ - Layer I
	‘00’ - Reserved
*/
void GTP::GMP3::gmp3_set_layer(unsigned char byte){
	byte = byte << 5;
	byte = byte >> 6;
	layer = 4 - byte;
}
/* ###############################################
    [#] The Last Bit of Second Byte
		‘0’- Protected
		‘1’- Unprotected
*/
void GTP::GMP3::gmp3_set_crc(){
	crc = buffer[1] & 0x01;
}

/* ###############################################
* MPEG 1
	Index  Layer I    Layer II    LayerIII
	0000     Free
	0001      32
	0010      64
	0011      96
	0100     128
	0101     160
	0110     192
	0111     224
	1000     256
	1001     288
	1010     320
	1011     352
	1100     384
	1101     416
	1110     448
	1111  Reserved

*/
void GTP::GMP3::gmp3_set_bit_rate(unsigned char *buffer){

	if (mpeg_version == 1) {
		if (layer == 1) {
			bit_rate = buffer[2] * 32;
		} else if (layer == 2) {
			const int rates[14] {32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384};
			bit_rate = rates[(buffer[2] >> 4) - 1] * 1000;
		} else if (layer == 3) {
			const int rates[14] {32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};
			bit_rate = rates[(buffer[2] >> 4) - 1] * 1000;
		} else
			valid = false;
	} else {
		if (layer == 1) {
			const int rates[14] {32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256};
			bit_rate = rates[(buffer[2] >> 4) - 1] * 1000;
		} else if (layer < 4) {
			const int rates[14] {8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160};
			bit_rate = rates[(buffer[2] >> 4) - 1] * 1000;
		} else
			valid = false;
	}
}
/* ###############################################
Sampling Rate   MPEG 1
   Index
  00           44100 Hz
  01           48000 Hz
  10           32000 Hz
  11           Reserved

*/
void GTP::GMP3::gmp3_set_sampling_rate(){
	int rates[3][3] {44100, 48000, 32000, 22050, 24000, 16000, 11025, 12000, 8000};

	for (int version = 1; version <= 3; version++)
		if (mpeg_version == version) {
			if ((buffer[2] & 0x08) != 0x08 && (buffer[2] & 0x04) != 0x04) {
				sampling_rate = rates[version - 1][0];
				break;
			} else if ((buffer[2] & 0x08) != 0x08 && (buffer[2] & 0x04) == 0x04) {
				sampling_rate = rates[version - 1][1];
				break;
			} else if ((buffer[2] & 0x08) == 0x08 && (buffer[2] & 0x04) != 0x04) {
				sampling_rate = rates[version - 1][2];
				break;
			}
		}
}
/* ###############################################

*/
void GTP::GMP3::gmp3_set_tables(){
	
	switch (sampling_rate) {
		case 32000:
			band_index.short_win = band_index_table.short_32;
			band_width.short_win = band_width_table.short_32;
			band_index.long_win = band_index_table.long_32;
			band_width.long_win = band_width_table.long_32;
			break;
		case 44100:
			band_index.short_win = band_index_table.short_44;
			band_width.short_win = band_width_table.short_44;
			band_index.long_win = band_index_table.long_44;
			band_width.long_win = band_width_table.long_44;
			break;
		case 48000:
			band_index.short_win = band_index_table.short_48;
			band_width.short_win = band_width_table.short_48;
			band_index.long_win = band_index_table.long_48;
			band_width.long_win = band_width_table.long_48;
			break;
	}
}
/* ###############################################

*/
void GTP::GMP3::gmp3_set_padding(){
	padding = buffer[2] & 0x02;
}
/* ###############################################
Bit value     Type of mode
----------------------------
  00             Stereo
  11          Joint Stereo
  10          Dual Channel
  01         Single Channel
*/
void GTP::GMP3::gmp3_set_channel_mode(unsigned char *buffer){
	unsigned value = buffer[3] >> 6;
	channel_mode = static_cast<ChannelMode>(value);
	channels = channel_mode == Mono ? 1 : 2;
}
/* ###############################################
	Applies only to joint stereo.
*/
void GTP::GMP3::gmp3_set_mode_extension(unsigned char *buffer)
{
	if (layer == 3) {
		mode_extension[0] = buffer[3] & 0x20;
		mode_extension[1] = buffer[3] & 0x10;
	}
}
/* ###############################################

*/
void GTP::GMP3::gmp3_set_emphasis(unsigned char *buffer){
	unsigned value = (buffer[3] << 6) >> 6;
	emphasis = static_cast<Emphasis>(value);
}
/* ###############################################

*/
void GTP::GMP3::gmp3_set_info(){
	info[0] = buffer[2] & 0x01;
	info[1] = buffer[3] & 0x08;
	info[2] = buffer[3] & 0x04;
}
/* ###############################################
	FL = samples_per_frame  x   bitrate   +  padding
		------------------    -----------
                8             sample_rate
*/
void GTP::GMP3::gmp3_set_frame_size(){

	unsigned int samples_per_frame;
	switch (layer) {
		case 3:
			if (mpeg_version == 1)
				samples_per_frame = 1152;
			else
				samples_per_frame = 576;
			break;
		case 2:
			samples_per_frame = 1152;
			break;
		case 1:
			samples_per_frame = 384;
			break;
	}

	/* Minimum frame size = 1152 / 8 * 32000 / 48000 = 96
	 * Minimum main_data size = 96 - 36 - 2 = 58
	 * Maximum main_data_begin = 2^9 = 512
	 * Therefore remember ceil(512 / 58) = 9 previous frames.
	 */
	for (int i = num_prev_frames-1; i > 0; --i){
		prev_frame_size[i] = prev_frame_size[i-1];
	}
	prev_frame_size[0] = frame_size;
	frame_size = ( (samples_per_frame / 8) * (bit_rate / sampling_rate));
	if (padding == 1){
		frame_size += 1;
	}
}
/* ###############################################

*/
void GTP::GMP3::gmp3_set_side_info(unsigned char *buffer){
	int count = 0;

	/* Number of bytes the main data ends before the next frame header. */
	main_data_begin = (int)get_bits_inc(buffer, &count, 9);

	/* Skip private bits. Not necessary. */
	count += channel_mode == Mono ? 5 : 3;

	for (int ch = 0; ch < channels; ch++)
		for (int scfsi_band = 0; scfsi_band < 4; scfsi_band++)
			/* - Scale factor selection information.
			 * - If scfsi[scfsi_band] == 1, then scale factors for the first
			 *   granule are reused in the second granule.
			 * - If scfsi[scfsi_band] == 0, then each granule has its own scaling factors.
			 * - scfsi_band indicates what group of scaling factors are reused. */
			scfsi[ch][scfsi_band] = get_bits_inc(buffer, &count, 1) != 0;

	for (int gr = 0; gr < 2; gr++)
		for (int ch = 0; ch < channels; ch++) {
			/* Length of the scaling factors and main data in bits. */
			part2_3_length[gr][ch] = (int)get_bits_inc(buffer, &count, 12);
			/* Number of values in each big_region. */
			big_value[gr][ch] = (int)get_bits_inc(buffer, &count, 9);
			/* Quantizer step size. */
			global_gain[gr][ch] = (int)get_bits_inc(buffer, &count, 8);
			/* Used to determine the values of slen1 and slen2. */
			scalefac_compress[gr][ch] = (int)get_bits_inc(buffer, &count, 4);

			/* Number of bits given to a range of scale factors.
			 * - Normal blocks: slen1 0 - 10, slen2 11 - 20
			 * - Short blocks && mixed_block_flag == 1: slen1 0 - 5, slen2 6-11
			 * - Short blocks && mixed_block_flag == 0: */
			slen1[gr][ch] = slen[scalefac_compress[gr][ch]][0];
			slen2[gr][ch] = slen[scalefac_compress[gr][ch]][1];
			/* If set, a not normal window is used. */
			window_switching[gr][ch] = get_bits_inc(buffer, &count, 1) == 1;

			if (window_switching[gr][ch]) {
				/* The window type for the granule.
				 * 0: reserved
				 * 1: start block
				 * 2: 3 short windows
				 * 3: end block
				 */
				block_type[gr][ch] = (int)get_bits_inc(buffer, &count, 2);
				/* Number of scale factor bands before window switching. */
				mixed_block_flag[gr][ch] = get_bits_inc(buffer, &count, 1) == 1;
				if (mixed_block_flag[gr][ch]) {
					switch_point_l[gr][ch] = 8;
					switch_point_s[gr][ch] = 3;
				} else {
					switch_point_l[gr][ch] = 0;
					switch_point_s[gr][ch] = 0;
				}

				/* These are set by default if window_switching. */
				region0_count[gr][ch] = block_type[gr][ch] == 2 ? 8 : 7;
				/* No third region. */
				region1_count[gr][ch] = 20 - region0_count[gr][ch];

				for (int region = 0; region < 2; region++)
					/* Huffman table number for a big region. */
					table_select[gr][ch][region] = (int)get_bits_inc(buffer, &count, 5);
				for (int window = 0; window < 3; window++)
					subblock_gain[gr][ch][window] = (int)get_bits_inc(buffer, &count, 3);
			} else {
				/* Set by default if !window_switching. */
				block_type[gr][ch] = 0;
				mixed_block_flag[gr][ch] = false;

				for (int region = 0; region < 3; region++)
					table_select[gr][ch][region] = (int)get_bits_inc(buffer, &count, 5);

				/* Number of scale factor bands in the first big value region. */
				region0_count[gr][ch] = (int)get_bits_inc(buffer, &count, 4);
				/* Number of scale factor bands in the third big value region. */
				region1_count[gr][ch] = (int)get_bits_inc(buffer, &count, 3);
				/* # scale factor bands is 12*3 = 36 */
			}

			/*
				If set, add values from a table to the scaling factors.
			*/
			preflag[gr][ch] = (int)get_bits_inc(buffer, &count, 1);
			/* Determines the step size. */
			scalefac_scale[gr][ch] = (int)get_bits_inc(buffer, &count, 1);
			/* Table that determines which count1 table is used. */
			count1table_select[gr][ch] = (int)get_bits_inc(buffer, &count, 1);
		}
}




/* ###############################################

*/
void GTP::GMP3::gmp3_set_main_data(unsigned char *buffer){
	/* header + side_information */
	int constant = channel_mode == Mono ? 21 : 36;
	if (crc == 0)
		constant += 2;

	/* Let's put the main data in a separate buffer so that side info and header
	 * don't interfere. The main_data_begin may be larger than the previous frame
	 * and doesn't include the size of side info and headers. */
	if (main_data_begin == 0) {
		main_data.resize(frame_size - constant);
		memcpy(&main_data[0], buffer + constant, frame_size - constant);
	} else {
		int bound = 0;
		for (int frame = 0; frame < num_prev_frames; frame++) {
			bound += prev_frame_size[frame] - constant;
			if (main_data_begin < bound) {
				int ptr_offset = main_data_begin + frame * constant;
				int buffer_offset = 0;

				int part[num_prev_frames];
				part[frame] = main_data_begin;
				for (int i = 0; i <= frame-1; i++) {
					part[i] = prev_frame_size[i] - constant;
					part[frame] -= part[i];
				}

				main_data.resize(frame_size - constant + main_data_begin);
				memcpy(main_data.data(), buffer - ptr_offset, part[frame]);
				ptr_offset -= (part[frame] + constant);
				buffer_offset += part[frame];
				for (int i = frame-1; i >= 0; i--) {
					memcpy(&main_data[buffer_offset], buffer - ptr_offset, part[i]);
					ptr_offset -= (part[i] + constant);
					buffer_offset += part[i];
				}
				memcpy(&main_data[main_data_begin], buffer + constant, frame_size - constant);
				break;
			}
		}
	}

	int bit = 0;
	for (int gr = 0; gr < 2; gr++)
		for (int ch = 0; ch < channels; ch++) {
			int max_bit = bit + part2_3_length[gr][ch];
			gmp3_unpack_scalefac(main_data.data(), gr, ch, bit);
			gmp3_unpack_samples(main_data.data(), gr, ch, bit, max_bit);
			bit = max_bit;
		}
}
/* ###############################################

 There are a total of 21 scaling
 factors for long windows and 12 for each short window.
*/

void mp3::unpack_scalefac(unsigned char *main_data, int gr, int ch, int &bit)
{
	int sfb = 0;
	int window = 0;
	int scalefactor_length[2] {
		slen[scalefac_compress[gr][ch]][0],
		slen[scalefac_compress[gr][ch]][1]
	};

	/* No scale factor transmission for short blocks. */
	if (block_type[gr][ch] == 2 && window_switching[gr][ch]) {
		if (mixed_block_flag[gr][ch] == 1) { /* Mixed blocks. */
			for (sfb = 0; sfb < 8; sfb++)
				scalefac_l[gr][ch][sfb] = (int)get_bits_inc(main_data, &bit, scalefactor_length[0]);

			for (sfb = 3; sfb < 6; sfb++)
				for (window = 0; window < 3; window++)
					scalefac_s[gr][ch][window][sfb] = (int)get_bits_inc(main_data, &bit, scalefactor_length[0]);
		} else /* Short blocks. */
			for (sfb = 0; sfb < 6; sfb++)
				for (window = 0; window < 3; window++)
					scalefac_s[gr][ch][window][sfb] = (int)get_bits_inc(main_data, &bit, scalefactor_length[0]);

		for (sfb = 6; sfb < 12; sfb++)
			for (window = 0; window < 3; window++)
				scalefac_s[gr][ch][window][sfb] = (int)get_bits_inc(main_data, &bit, scalefactor_length[1]);

		for (window = 0; window < 3; window++)
			scalefac_s[gr][ch][window][12] = 0;
	}

	/* Scale factors for long blocks. */
	else {
		if (gr == 0) {
			for (sfb = 0; sfb < 11; sfb++)
				scalefac_l[gr][ch][sfb] = (int)get_bits_inc(main_data, &bit, scalefactor_length[0]);
			for (; sfb < 21; sfb++)
				scalefac_l[gr][ch][sfb] = (int)get_bits_inc(main_data, &bit, scalefactor_length[1]);
		} else {
			/* Scale factors might be reused in the second granule. */
			const int sb[5] = {6, 11, 16, 21};
			for (int i = 0; i < 2; i++)
				for (; sfb < sb[i]; sfb++) {
					if (scfsi[ch][i])
						scalefac_l[gr][ch][sfb] = scalefac_l[0][ch][sfb];
					else
						scalefac_l[gr][ch][sfb] = (int)get_bits_inc(main_data, &bit, scalefactor_length[0]);

				}
			for (int i = 2; i < 4; i++)
				for (; sfb < sb[i]; sfb++) {
					if (scfsi[ch][i])
						scalefac_l[gr][ch][sfb] = scalefac_l[0][ch][sfb];
					else
						scalefac_l[gr][ch][sfb] = (int)get_bits_inc(main_data, &bit, scalefactor_length[1]);
				}
		}
		scalefac_l[gr][ch][21] = 0;
	}
}

/* ###############################################

*/


void mp3::unpack_samples(unsigned char *main_data, int gr, int ch, int bit, int max_bit)
{
	int sample = 0;
	int table_num;
	const unsigned *table;

	for (int i = 0; i < 576; i++)
		samples[gr][ch][i] = 0;

	/* Get the big value region boundaries. */
	int region0;
	int region1;
	if (window_switching[gr][ch] && block_type[gr][ch] == 2) {
		region0 = 36;
		region1 = 576;
	} else {
		region0 = band_index.long_win[region0_count[gr][ch] + 1];
		region1 = band_index.long_win[region0_count[gr][ch] + 1 + region1_count[gr][ch] + 1];
	}

	/* Get the samples in the big value region. Each entry in the Huffman tables
	 * yields two samples. */
	for (; sample < big_value[gr][ch] * 2; sample += 2) {
		if (sample < region0) {
			table_num = table_select[gr][ch][0];
			table = big_value_table[table_num];
		} else if (sample < region1) {
			table_num = table_select[gr][ch][1];
			table = big_value_table[table_num];
		} else {
			table_num = table_select[gr][ch][2];
			table = big_value_table[table_num];
		}

		if (table_num == 0) {
			samples[gr][ch][sample] = 0;
			continue;
		}

		bool repeat = true;
		unsigned bit_sample = get_bits(main_data, bit, bit + 32);

		/* Cycle through the Huffman table and find a matching bit pattern. */
		for (int row = 0; row < big_value_max[table_num] && repeat; row++)
			for (int col = 0; col < big_value_max[table_num]; col++) {
				int i = 2 * big_value_max[table_num] * row + 2 * col;
				unsigned value = table[i];
				unsigned size = table[i + 1];
				if (value >> (32 - size) == bit_sample >> (32 - size)) {
					bit += size;

					int values[2] = {row, col};
					for (int i = 0; i < 2; i++) {

						/* linbits extends the sample's size if needed. */
						int linbit = 0;
						if (big_value_linbit[table_num] != 0 && values[i] == big_value_max[table_num] - 1)
							linbit = (int)get_bits_inc(main_data, &bit, big_value_linbit[table_num]);

						/* If the sample is negative or positive. */
						int sign = 1;
						if (values[i] > 0)
							sign = get_bits_inc(main_data, &bit, 1) ? -1 : 1;

						samples[gr][ch][sample + i] = (float)(sign * (values[i] + linbit));
					}

					repeat = false;
					break;
				}
			}
	}

	/* Quadruples region. */
	for (; bit < max_bit && sample + 4 < 576; sample += 4) {
		int values[4];

		/* Flip bits. */
		if (count1table_select[gr][ch] == 1) {
			unsigned bit_sample = get_bits_inc(main_data, &bit, 4);
			values[0] = (bit_sample & 0x08) > 0 ? 0 : 1;
			values[1] = (bit_sample & 0x04) > 0 ? 0 : 1;
			values[2] = (bit_sample & 0x02) > 0 ? 0 : 1;
			values[3] = (bit_sample & 0x01) > 0 ? 0 : 1;
		} else {
			unsigned bit_sample = get_bits(main_data, bit, bit + 32);
			for (int entry = 0; entry < 16; entry++) {
				unsigned value = quad_table_1.hcod[entry];
				unsigned size = quad_table_1.hlen[entry];

				if (value >> (32 - size) == bit_sample >> (32 - size)) {
					bit += size;
					for (int i = 0; i < 4; i++)
						values[i] = (int)quad_table_1.value[entry][i];
					break;
				}
			}
		}

		/* Get the sign bit. */
		for (int i = 0; i < 4; i++)
			if (values[i] > 0 && get_bits_inc(main_data, &bit, 1) == 1)
				values[i] = -values[i];

		for (int i = 0; i < 4; i++)
			samples[gr][ch][sample + i] = values[i];
	}

	/* Fill remaining samples with zero. */
	for (; sample < 576; sample++)
		samples[gr][ch][sample] = 0;
}






/* ###############################################
	[#] ALSA in ACTION:
		-> put data into buffer of pcm device.
*/
bool GTP::GMP3::stream(){
	/*
		Badme Likhega me
		pehle configuration kr leta hu
	*/

}


// ###############################################

unsigned GTP::GMP3::gmp3_get_frame_size(){
	return frame_size;
}
// ###############################################

bool *GTP::GMP3::gmp3_get_info(){
	return info;
}
// ###############################################

GTP::GMP3::Emphasis GTP::GMP3::gmp3_get_emphasis(){
	return emphasis;
}
// ###############################################

unsigned *GTP::GMP3::gmp3_get_mode_extension(){
	return mode_extension;
}
// ###############################################

GTP::GMP3::ChannelMode GTP::GMP3::gmp3_get_channel_mode(){
	return channel_mode;
}
// ###############################################

bool GTP::GMP3::gmp3_get_padding(){
	return padding;
}
// ###############################################

float GTP::GMP3::gmp3_get_mpeg_v(){
	return mpeg_version;
}
// ###############################################

unsigned GTP::GMP3::gmp3_get_layer(){
	return layer;
}
// ###############################################

bool GTP::GMP3::gmp3_get_crc(){
	return crc;
}
// ###############################################

unsigned GTP::GMP3::gmp3_get_bitrate(){
	return bit_rate;
}
// ###############################################

unsigned GTP::GMP3::gmp3_get_sampling_rate(){
	return sampling_rate;
}
// ##############################  THE END  ##################################
