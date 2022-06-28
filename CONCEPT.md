# HEADER INFO ( 32 Bits or 4 Bytes ) :

+------------------------------------+  
|	1111 1111 1111   1     xx    x   |  
|        sync       I-D   Layer CRC  |  
+------------------------------------+-----------------------------------+  
|	  xxxx     xx      x        x      xx   xx      x        x      xx   |  
|  Bitrate  Freq  Padding  private  Mode ModeExt copyright  copy Emphasis|  
+------------------------------------------------------------------------+  

# SIDE INFO ( VARIABLE SIZE ) :

+---------------------+------------------+-------------------------------------------------------------------+  
| MAIN_DATA_BEGIN (9) | PRIVATE_BITS (5) | SHARE (4)   |  SCFSI (1)  |  PART_2_3_LENGTH (12) | BIG_VALUES(9) |  
+--------------+-----------------------+---------------+------+------+--------+--------------+---------------+  
| GLOBAL_GAIN(8) | SCALEFAC_COMPRESS(4) | WINDOW_SWITCHING(1) |  BLOCK_TYPE(2)  |    MIXED_BLOCK_FLAG(1)     |  
+----------------+----------------------+--------+------------+------+--------+------+-----------------------+  
|TABLE_SELECT(5) | SUBBLOCK_GAIN(3) | PREFLAG(1) | SCALEFAC_SCALE(1) |   COUNT1(1)   |  
+----------------+------------------+------------+-------------------+---------------+  


-----------------------------------------------------------------------------------
## MAIN_DATA_BEGIN :

    It is a pointer that points to the beginning of the main data. The variable has
    nine bits and specifies the location of the main data as a negative offset
    (jumping backwards) in bytes from the first byte of the audio sync word.
    The number of bytes of the header and side information are not taken into
    account while calculating the location of the main data. This is called bit
    reservoir technique and it allows the encoder to use some extra bits while
    encoding a difficult frame. Since it is nine bits long, it can point upto
    2 9 − 1 = 511 bytes in front of the header. If the value of main_data_begin is
    zero, then the main data follows immediately the side information.
-----------------------------------------------------------------------------------
## SCFSI :

    Scale factor selection information.
    Layer III contains two granules and the encoder can specify separately for each group of scale factor bands whether the second granule will reuse the
    scale factor information of the first granule or not. If the value of scfsi_band is one, then sharing
    of scale factors is allowed between the granules.

-----------------------------------------------------------------------------------
## SCFSI BAND :

    Layer III there has one scale factor for each frequency band and the 21
    frequency bands are separated into 4 groups according to Table . If block
    type is 2 then scale factors are transmitted for each granule and channel.

-----------------------------------------------------------------------------------
## PART_2_3_LENGTH :

    This value contains the number of main_data bits used for scale factors and
    Huffman coded data. The main data is divided into two or four parts, for
    each granule and channel, depending on single or dual channel respectively.
    The size of each of these sections is the first item in the side information
    which is 12 bit unsigned integer.

-----------------------------------------------------------------------------------
## BIG_VALUES :

-----------------------------------------------------------------------------------
## GLOBAL_GAIN :
    The quantizer step size information is known through this variable and the
    formula for requantization is given in the requantization block.

-----------------------------------------------------------------------------------
## SCALEFAC_COMPRESS :
    Determines the number of bits used for the transmission of the scalefactors.
    The number of bits that has to be transferred to scale factor bands is defined
    by two variables called ‘slen1’ and ‘slen2’.

-----------------------------------------------------------------------------------
## WINDOW_SWITCHING :

    Indicates that other than normal window is used. If window_swtiching_flag
    is set then variables block_type, mixed_block_flag, subblock_gain are also
    set. If window_swtiching_flag is not set then the value of block_type is
    zero.

----------------------------------------------------------------------------------
## BLOCK_TYPE :

    Indicates which type of window to be used for each granule. The different
    types of windows along with block type are provided in Table
    -------------------------
     block_type   window type
    -------------------------
        0         reserved
        1        start block
        2      3 short windows
        3           end

----------------------------------------------------------------------------------
## MIXED_BLOCK_FLAG :

    Indicates that different frequencies are transformed with different window
    types. If mixed_block_flag is not set then all the frequency lines are
    transformed as specified by block_type. If it is not set, then the two lowest
    polyphase subbands are transformed with normal window and the remaining
    30 subbands as block_type.

----------------------------------------------------------------------------------
## TABLE_SELECT :

    As the name states, different Huffman coded tables are selected depending
    on the maximum quantized value and local statistics of the signal. There are
    32 different Huffman tables given in the ISO standard. The table_select
    specifies the Huffman table to decode only the big_values.

----------------------------------------------------------------------------------
## SUBBLOCK_GAIN :

    This variable is used only when window_switching_flag is set and for short
    windows (i.e, block_type=2). It indicates the gain offset from the global
    gain for one subblock and the values of the subblock have to be divided
    by 4

----------------------------------------------------------------------------------
## PREFLAG :

----------------------------------------------------------------------------------
## SCALEFAC_SCALE :

----------------------------------------------------------------------------------
## COUNT1 :

    This variable selects which of the two possible Huffman tables will be used
    for quadruples of quantized values with magnitude not exceeding 1

----------------------------------------------------------------------------------

## FREQUENCY MASKING :
    -> refer pdf

-----------------------------------------------------------------------------------
## TEMPORAL MASKING :
    -> refer pdf

-----------------------------------------------------------------------------------

### References
(1) https://www.es.ele.tue.nl/dfbench/mp3decoder.php
(2)
