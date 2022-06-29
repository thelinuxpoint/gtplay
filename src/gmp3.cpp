#include <vector>
//
#include "gmp3.hpp"
#include "gtables.hpp"
// ###############################################
/* MPEG HEADER
  +---------------------------------------------+
  |	1111 1111 1111   1     xx    x   { 16-bit } |
  |      sync       I-D   Layer CRC             |
  +---------------------------------------------+--------------------------+
  |   xxxx     xx      x        x      xx   xx      x          x    xx     |
  |  Bitrate  Freq  Padding  private  Mode ModeExt copyright  copy Emphasis|
  +------------------------------------------------------------------------+
*/
GTP::GMP3::GMP3(unsigned char *buffer){

}
