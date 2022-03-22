# mp1_audio
Final master thesis ported to x86 used to be deployed on TI C6713 DSP

MPEG 1 Layer I Audio Encoder and Decoder based on my master thesis.

Link to my master thesis:

https://reposit.haw-hamburg.de/bitstream/20.500.12738/7856/1/Masterarbeit_Kim_Radmacher.pdf

# how to build
use simple Makefile

make clean; make

# how to run
./MPEG_Encoder/bin/encoder <in.wav> <out.mp1> <bitrate (default 192)>

./MPEG_Decoder/bin/decoder <out.mp1> <out.wav>

# known issues
-mono audio only

-48 kHz only

-mpeg 1 header: encode and decode works fine but other mpeg conform decoder (ffmpeg) does not recognize my bitstreams. also decoder can not decode other mp1 conform bitstreams.

-memory consumption needs to be optimized

-clean up (coding style)
