#include <regex>

#include "id3.h"

// #######################################################
id3::id3(unsigned char *buffer){

	this->buffer = buffer;
	if (buffer[0] == 'I' && buffer[1] == 'D' && buffer[2] == '3') {
		set_version(buffer[3], buffer[4]);
		if(set_flags(buffer[5])) {
			valid = true;
			set_offset(char_to_int(&buffer[6]));
			set_extended_header_size(char_to_int(&buffer[10]));
			set_fields(&buffer[10 + extended_header_size]);
		} else
			valid = false;
	} else
		valid = false;
}
// #######################################################

id3::id3(const id3 &orig){
	this->start = orig.start;
	this->version = orig.version;
	this->offset = orig.offset;
	for (int i = 0; i < 4; i++)
		this->id3_flags[i] = orig.id3_flags[i];
	this->extended_header_size = orig.extended_header_size;
	for (int i = 0; i < 2; i++)
		this->id3_frames[i] = orig.id3_frames[i];
}
// #######################################################

bool id3::is_valid(){
	return valid;
}
// #######################################################

void id3::set_version(unsigned char version, unsigned char revision){
	char v[6];
	sprintf(v, "2.%u.%u", version, revision);
	this->version = (string)v;
}
// #######################################################

string id3::get_id3_version(){
	return this->version;
}
// #######################################################

void id3::set_offset(int offset){
	this->offset = offset;
}
// #######################################################

int id3::get_id3_offset(){
	return this->offset;
}
// #######################################################

bool id3::set_flags(unsigned char flags){

	/*    x              x       x   x         0 0 0 0
	      |              |       |   |         | | | |
		Unsyncronized  Extended Exp Footer |  Must Be Unset
	*/
	for (int bit_num = 0; bit_num < 4; bit_num++)
		if (flags >> bit_num & 1)
			return false;

	for (int bit_num = 4; bit_num < 8; bit_num++)
		if (flags >> bit_num & 1)
			this->id3_flags[bit_num-4] = true;
		else
			this->id3_flags[bit_num-4] = false;

	/*  id3_flags { bit4 , bit5 , bit6 , bit7 }  */

	return true;
}
// #######################################################

const bool *id3::get_id3_flags(){
	return this->id3_flags;
}
// #######################################################

void id3::set_extended_header_size(int size){

	// if bit6 is set
	if (this->id3_flags[ExtendedHeader] == 1)
		this->extended_header_size = size;
	else
		this->extended_header_size = 0;
}
// #######################################################

int id3::get_id3_extended_header_size(){
	return this->extended_header_size;
}
// #######################################################

void id3::set_fields(unsigned char *buffer){

	int footer_size = id3_flags[FooterPresent] * 10;
	int size = offset - extended_header_size - footer_size;
	int i = 0;

	std::regex re("[A-Z0-9]");
	string str(1, (char)buffer[i]);

	while (!std::regex_match(str, re) && i < size) {
		string id = "";
		string content = "";
		int field_size = 0;

		for (int j = i; j < i + 4; j++)
			id += buffer[j];
		this->id3_frames[0].push_back(id);

		i += 4;
		field_size = char_to_int(&buffer[i]);

		i += 6;
		for (int j = i; j < field_size + i; j++)
			content += buffer[j];
		this->id3_frames[1].push_back(content);

		i += field_size;
		str = (char)buffer[i];
	}
}
// #######################################################

const vector<string> *id3::get_id3_fields(){

	return this->id3_frames;
}
// #######################################################

unsigned int id3::get_id3_fields_length(){

	return this->id3_frames[1].size();
}
// #######################################################

unsigned get_bits(unsigned char *buffer, int start_bit, int end_bit){

	int start_byte = 0;
	int end_byte = 0;

	start_byte = start_bit >> 3;
	end_byte = end_bit >> 3;
	start_bit = start_bit % 8;
	end_bit = end_bit % 8;

	/* Get the bits. */
	unsigned result = ((unsigned)buffer[start_byte] << (32 - (8 - start_bit))) >> (32 - (8 - start_bit));

	if (start_byte != end_byte) {
		while (++start_byte != end_byte) {
			result <<= 8;
			result += buffer[start_byte];
		}
		result <<= end_bit;
		result += buffer[end_byte] >> (8 - end_bit);
	} else if (end_bit != 8)
		result >>= (8 - end_bit);

	return result;
}
// #######################################################

unsigned get_bits_inc(unsigned char *buffer, int *offset, int count){
	unsigned result = get_bits(buffer, *offset, *offset + count);
	*offset += count;
	return result;
}
// #######################################################

int char_to_int(unsigned char *buffer){
	unsigned num = 0x00;
	for (int i = 0; i < 4; i++)
		num = (num << 7) + buffer[i];
	return num;
}
