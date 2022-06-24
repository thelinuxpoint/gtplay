// ###############################################
/* MPEG HEADER
  +---------------------------------------------+
  |	1111 1111 1111   1     xx    x   { 16-bit } |
  |		 sync    	I-D   Layer CRC             |
  +---------------------------------------------+--------------------------+
  |	  xxxx     xx      x        x      xx   xx      x          x    xx     |
  |  Bitrate  Freq  Padding  private  Mode ModeExt copyright  copy Emphasis|
  +------------------------------------------------------------------------+
*/
void GTP::GMP3::gmp3_init_header(unsigned char *buffer){
	if (buffer[0] == 0xFF && buffer[1] >= 0xE0) {
		this->buffer = buffer;
		gmp3_set_mpeg_v();
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
// ###############################################






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
