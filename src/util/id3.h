#include <string>
#include <vector>
#include <stdio.h>

using std::string;
using std::vector;

class id3 {
		private:
			unsigned char *buffer;
			bool valid;
			unsigned int start;
			string version;
			unsigned int offset;
			bool id3_flags[4];
			unsigned int extended_header_size;
			vector<string> id3_frames[2];

			void set_version(unsigned char version, unsigned char revision);
			bool set_flags(unsigned char flags);
			void set_offset(int offset);
			void set_extended_header_size(int size);
			void set_fields(unsigned char *buffer);

		public:
			enum Flags {
				FooterPresent = 0,
				ExperimentalIndicator = 1,
				ExtendedHeader = 2,
				Unsynchronisation = 3
			};

			id3(unsigned char *buffer);
			id3(const id3 &orig);

			bool is_valid();
			string get_id3_version();
			const bool *get_id3_flags();
			int get_id3_offset();
			int get_id3_extended_header_size();
			const vector<string> *get_id3_fields();
			unsigned int get_id3_fields_length();

};

unsigned get_bits(unsigned char *buffer, int start_bit, int end_bit);


unsigned get_bits_inc(unsigned char *buffer, int *offset, int count);

int char_to_int(unsigned char *buffer);
