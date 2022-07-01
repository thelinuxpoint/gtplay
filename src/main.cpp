#include <iostream>
#include <fstream>
/**/
#include "./gmp3/gmp3.hpp"
#include "./util/id3.h"
/**/
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
/**/
std::vector<unsigned char> get_file(const char *dir){

	std::ifstream file(dir, std::ios::in | std::ios::binary | std::ios::ate);
	std::vector<unsigned char> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read((char *)buffer.data(), buffer.size());
	file.close();
	return std::move(buffer);
}
/**/
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
		unsigned offset = 0;
		std::vector<id3> tags = get_id3_tags(buffer, offset);
		GTP::GMP3 decoder(&buffer[offset]);
        GTP::stream(decoder,buffer,offset);

	} catch (std::bad_alloc) {
		printf("File does not exist.\n");
		return -1;
	}

    return 0;
}
