# default rule
default: all

.PHONY: all
all: 
	make -C MPEG_Encoder
	make -C MPEG_Decoder

.PHONY: clean
clean:
	make -C MPEG_Encoder clean
	make -C MPEG_Decoder clean
