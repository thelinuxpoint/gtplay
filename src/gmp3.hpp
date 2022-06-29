#include <iostream>

/**/
#define PI          3.141592653589793
#define SQRT2       1.414213562373095
#define	SYNCH		0xff
#define	SYNCL		0xf0
/*  Determining MAINBUF_SIZE:
        max mainDataBegin = (2^9 - 1) bytes (since 9-bit offset) = 511
        max nSlots (concatenated with mainDataBegin bytes from before) = 1440 - 9 - 4 + 1 = 1428
        511 + 1428 = 1939, round up to 1940 (4-byte align)
 */
#define MAINBUF_SIZE	1940
#define MAX_NGRAN		2		/* max granules */
#define MAX_NCHAN		2		/* max channels */
#define MAX_NSAMP		576		/* max samples per channel, per granule */


namespace GTP {

    class GMP3 {

        private:
            /* ###########################
                MP3 Header Information
            ########################### */
            float mpeg_version = 0.0f;
    		unsigned layer = 0;
    		bool crc = false;
    		unsigned bit_rate = 0;
    		unsigned sampling_rate = 0;
    		bool padding = false;
    		int channels = 0;
            
            /* ###########################
                SIDE INFO
            ########################### */
            /* MAIN BUFFER */
	        unsigned char MainBuffer[MAINBUF_SIZE];

	        int part23Length[MAX_NGRAN][MAX_NCHAN];

            /* private functions */
            void gmp3_init_header(unsigned char *buffer);

        public:

            enum class MPEG_VERSION {
                MPEG1 =  0,
	            MPEG2 =  1,
	            MPEG25 = 2
            };
            /* constructor which pointer to main buffer */
            GMP3(unsigned char *buffer);

    };
}
