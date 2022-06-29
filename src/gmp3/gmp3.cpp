#include <vector>

#include "gmp3.hpp"

// ###############################################
/* MPEG HEADER
  +---------------------------------------------+
  |	1111 1111 1111   1     xx    x              |
  |      sync       I-D   Layer CRC             |
  +---------------------------------------------+--------------------------+
  |	  xxxx     xx      x        x      xx   xx      x          x    xx     |
  |  Bitrate  Freq  Padding  private  Mode ModeExt copyright  copy Emphasis|
  +------------------------------------------------------------------------+
*/
#define PI    3.141592653589793

#define SQRT2 1.414213562373095


GTP::GMP3::GMP3(unsigned char *buffer){
	if (buffer[0] == 0xFF && buffer[1] >= 0xE0) {
		valid = true;
		frame_size = 0;
		main_data_begin = 0;
		gmp3_init_header(buffer);
	}
}

/* ###############################################

*/
void GTP::GMP3::gmp3_init_header(unsigned char *buffer){

	if (buffer[0] == 0xFF && buffer[1] >= 0xE0) {
		this->buffer = buffer;

		gmp3_set_mpeg_v();
		gmp3_set_layer(buffer[1]);
		gmp3_set_crc();
		gmp3_set_info();
		gmp3_set_emphasis(buffer);
		gmp3_set_sampling_rate();
		gmp3_set_tables();
		gmp3_set_channel_mode(buffer);
		gmp3_set_mode_extension(buffer);
		gmp3_set_padding();
		gmp3_set_bit_rate(buffer);
		gmp3_set_frame_size();

	}else {
		valid = false;
	}

}
/* ###############################################

*/

void GTP::GMP3::gmp3_init_frame_params(unsigned char *buffer){
	gmp3_set_side_info(&buffer[crc == 0 ? 6 : 4]);
	gmp3_set_main_data(buffer);
	for (int gr = 0; gr < 2; gr++) {
		for (int ch = 0; ch < channels; ch++)
			gmp3_requantize(gr, ch);

		if (channel_mode == JointStereo && mode_extension[0])
			gmp3_ms_stereo(gr);

		for (int ch = 0; ch < channels; ch++) {
			if (block_type[gr][ch] == 2 || mixed_block_flag[gr][ch])
				gmp3_reorder(gr, ch);
			else
				gmp3_alias_reduction(gr, ch);

			gmp3_imdct(gr, ch);
			gmp3_frequency_inversion(gr, ch);
			gmp3_synth_filterbank(gr, ch);
		}
	}
	gmp3_interleave();
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
	int rates[3][3] = {
		{ 44100, 48000, 32000 },		/* MPEG-1 */
		{ 22050, 24000, 16000 },		/* MPEG-2 */
		{ 11025, 12000,  8000 },		/* MPEG-2.5 */
	};
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
void GTP::GMP3::gmp3_set_mode_extension(unsigned char *buffer){
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

void GTP::GMP3::gmp3_unpack_scalefac(unsigned char *main_data, int gr, int ch, int &bit)
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


void GTP::GMP3::gmp3_unpack_samples(unsigned char *main_data, int gr, int ch, int bit, int max_bit)
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
	OK - GIVEN IN PDF
*/
void GTP::GMP3::gmp3_requantize(int gr, int ch){

	float exp1, exp2;
	int window = 0;
	int sfb = 0;
	const float scalefac_mult = scalefac_scale[gr][ch] == 0 ? 0.5 : 1;

	for (int sample = 0, i = 0; sample < 576; sample++, i++) {
		if (block_type[gr][ch] == 2 || (mixed_block_flag[gr][ch] && sfb >= 8)) {
			if (i == band_width.short_win[sfb]) {
				i = 0;
				if (window == 2) {
					window = 0;
					sfb++;
				} else
					window++;
			}

			exp1 = global_gain[gr][ch] - 210.0 - 8.0 * subblock_gain[gr][ch][window];
			exp2 = scalefac_mult * scalefac_s[gr][ch][window][sfb];
		} else {
			if (sample == band_index.long_win[sfb + 1])
				/* Don't increment sfb at the zeroth sample. */
				sfb++;

			exp1 = global_gain[gr][ch] - 210.0;
			exp2 = scalefac_mult * (scalefac_l[gr][ch][sfb] + preflag[gr][ch] * pretab[sfb]);
		}

		float sign = samples[gr][ch][sample] < 0 ? -1.0f : 1.0f;
		float a = std::pow(std::abs(samples[gr][ch][sample]), 4.0 / 3.0);
		float b = std::pow(2.0, exp1 / 4.0);
		float c = std::pow(2.0, -exp2);

		samples[gr][ch][sample] = sign * a * b * c;
	}
}
/* ###############################################

*/
/**
 * Reorder short blocks, mapping from scalefactor subbands (for short windows) to 18 sample blocks.
 * @param gr
 * @param ch
 */
void GTP::GMP3::gmp3_reorder(int gr, int ch)
{
	int total = 0;
	int start = 0;
	int block = 0;
	float samples[576] = {0};

	for (int sb = 0; sb < 12; sb++) {
		const int sb_width = band_width.short_win[sb];

		for (int ss = 0; ss < sb_width; ss++) {
			samples[start + block + 0] = this->samples[gr][ch][total + ss + sb_width * 0];
			samples[start + block + 6] = this->samples[gr][ch][total + ss + sb_width * 1];
			samples[start + block + 12] = this->samples[gr][ch][total + ss + sb_width * 2];

			if (block != 0 && block % 5 == 0) { /* 6 * 3 = 18 */
				start += 18;
				block = 0;
			} else
				block++;
		}

		total += sb_width * 3;
	}

	for (int i = 0; i < 576; i++)
		this->samples[gr][ch][i] = samples[i];
}
/* ###############################################

*/
/**
 * The left and right channels are added together to form the middle channel. The
 * difference between each channel is stored in the side channel.
 * @param gr
 */
void GTP::GMP3::gmp3_ms_stereo(int gr)
{
	for (int sample = 0; sample < 576; sample++) {
		float middle = samples[gr][0][sample];
		float side = samples[gr][1][sample];
		samples[gr][0][sample] = (middle + side) / SQRT2;
		samples[gr][1][sample] = (middle - side) / SQRT2;
	}
}
/* ###############################################

*/
/**
 * @param gr
 * @param ch
 */
void GTP::GMP3::gmp3_alias_reduction(int gr, int ch)
{
	static const float cs[8] {
			.8574929257, .8817419973, .9496286491, .9833145925,
			.9955178161, .9991605582, .9998991952, .9999931551
	};
	static const float ca[8] {
			-.5144957554, -.4717319686, -.3133774542, -.1819131996,
			-.0945741925, -.0409655829, -.0141985686, -.0036999747
	};

	int sb_max = mixed_block_flag[gr][ch] ? 2 : 32;

	for (int sb = 1; sb < sb_max; sb++)
		for (int sample = 0; sample < 8; sample++) {
			int offset1 = 18 * sb - sample - 1;
			int offset2 = 18 * sb + sample;
			float s1 = samples[gr][ch][offset1];
			float s2 = samples[gr][ch][offset2];
			samples[gr][ch][offset1] = s1 * cs[sample] - s2 * ca[sample];
			samples[gr][ch][offset2] = s2 * cs[sample] + s1 * ca[sample];
		}
}
/* ###############################################

*/
/**
 * Inverted modified discrete cosine transformations (IMDCT) are applied to each
 * sample and are afterwards windowed to fit their window shape. As an addition, the
 * samples are overlapped.
 * @param gr
 * @param ch
 */
void GTP::GMP3::gmp3_imdct(int gr, int ch)
{
	static bool init = true;
	static float sine_block[4][36];
	float sample_block[36];

	if (init) {
		int i;
		for (i = 0; i < 36; i++)
			sine_block[0][i] = std::sin(PI / 36.0 * (i + 0.5));
		for (i = 0; i < 18; i++)
			sine_block[1][i] = std::sin(PI / 36.0 * (i + 0.5));
		for (; i < 24; i++)
			sine_block[1][i] = 1.0;
		for (; i < 30; i++)
			sine_block[1][i] = std::sin(PI / 12.0 * (i - 18.0 + 0.5));
		for (; i < 36; i++)
			sine_block[1][i] = 0.0;
		for (i = 0; i < 12; i++)
			sine_block[2][i] = std::sin(PI / 12.0 * (i + 0.5));
		for (i = 0; i < 6; i++)
			sine_block[3][i] = 0.0;
		for (; i < 12; i++)
			sine_block[3][i] = std::sin(PI / 12.0 * (i - 6.0 + 0.5));
		for (; i < 18; i++)
			sine_block[3][i] = 1.0;
		for (; i < 36; i++)
			sine_block[3][i] = std::sin(PI / 36.0 * (i + 0.5));
		init = false;
	}

	const int n = block_type[gr][ch] == 2 ? 12 : 36;
	const int half_n = n / 2;
	int sample = 0;

	for (int block = 0; block < 32; block++) {
		for (int win = 0; win < (block_type[gr][ch] == 2 ? 3 : 1); win++) {
			for (int i = 0; i < n; i++) {
				float xi = 0.0;
				for (int k = 0; k < half_n; k++) {
					float s = samples[gr][ch][18 * block + half_n * win + k];
					xi += s * std::cos(PI / (2 * n) * (2 * i + 1 + half_n) * (2 * k + 1));
				}

				/* Windowing samples. */
				sample_block[win * n + i] = xi * sine_block[block_type[gr][ch]][i];
			}
		}

		if (block_type[gr][ch] == 2) {
			float temp_block[36];
			memcpy(temp_block, sample_block, 36 * 4);

			int i = 0;
			for (; i < 6; i++)
				sample_block[i] = 0;
			for (; i < 12; i++)
				sample_block[i] = temp_block[0 + i - 6];
			for (; i < 18; i++)
				sample_block[i] = temp_block[0 + i - 6] + temp_block[12 + i - 12];
			for (; i < 24; i++)
				sample_block[i] = temp_block[12 + i - 12] + temp_block[24 + i - 18];
			for (; i < 30; i++)
				sample_block[i] = temp_block[24 + i - 18];
			for (; i < 36; i++)
				sample_block[i] = 0;
		}

		/* Overlap. */
		for (int i = 0; i < 18; i++) {
			samples[gr][ch][sample + i] = sample_block[i] + prev_samples[ch][block][i];
			prev_samples[ch][block][i] = sample_block[18 + i];
		}
		sample += 18;
	}
}
/* ###############################################

*/

void GTP::GMP3::gmp3_frequency_inversion(int gr, int ch)
{
	for (int sb = 1; sb < 18; sb += 2)
		for (int i = 1; i < 32; i += 2)
			samples[gr][ch][i * 18 + sb] *= -1;
}
/* ###############################################

*/

void GTP::GMP3::gmp3_synth_filterbank(int gr, int ch)
{
	static float n[64][32];
	static bool init = true;

	if (init) {
		init = false;
		for (int i = 0; i < 64; i++)
			for (int j = 0; j < 32; j++)
				n[i][j] = std::cos((16.0 + i) * (2.0 * j + 1.0) * (PI / 64.0));
	}

	float s[32], u[512], w[512];
	float pcm[576];

	for (int sb = 0; sb < 18; sb++) {
		for (int i = 0; i < 32; i++)
			s[i] = samples[gr][ch][i * 18 + sb];

		for (int i = 1023; i > 63; i--)
			fifo[ch][i] = fifo[ch][i - 64];

		for (int i = 0; i < 64; i++) {
			fifo[ch][i] = 0.0;
			for (int j = 0; j < 32; j++)
				fifo[ch][i] += s[j] * n[i][j];
		}

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 32; j++) {
				u[i * 64 + j] = fifo[ch][i * 128 + j];
				u[i * 64 + j + 32] = fifo[ch][i * 128 + j + 96];
			}

		for (int i = 0; i < 512; i++)
			w[i] = u[i] * synth_window[i];

		for (int i = 0; i < 32; i++) {
			float sum = 0;
			for (int j = 0; j < 16; j++)
				sum += w[j * 32 + i];
			pcm[32 * sb + i] = sum;
		}
	}

	memcpy(samples[gr][ch], pcm, 576 * 4);
}
/* ###############################################

*/
void GTP::GMP3::gmp3_interleave(){
	int i = 0;
	for (int gr = 0; gr < 2; gr++)
		for (int sample = 0; sample < 576; sample++)
			for (int ch = 0; ch < channels; ch++)
				pcm[i++] = samples[gr][ch][sample];

}
/* ###############################################

*/
float *GTP::GMP3::gmp3_get_samples(){
	return pcm;
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
	return true;

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
