#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define CACHE_BITS  32

static void putBits(uint8_t* pBitBuf, uint32_t value, const uint32_t numberOfBits);
static uint8_t writeBits(uint8_t *pBitBuf, uint32_t value, const uint32_t numberOfBits);
static int32_t readBits(uint8_t* pBitstream, const uint32_t numberOfBits, int32_t *retBits);

const uint32_t bitMask[32 + 1] = 
{
    0x0,        0x1,        0x3,       0x7,       0xf,       0x1f,
    0x3f,       0x7f,       0xff,      0x1ff,     0x3ff,     0x7ff,
    0xfff,      0x1fff,     0x3fff,    0x7fff,    0xffff,    0x1ffff,
    0x3ffff,    0x7ffff,    0xfffff,   0x1fffff,  0x3fffff,  0x7fffff,
    0xffffff,   0x1ffffff,  0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff,
    0x3fffffff, 0x7fffffff, 0xffffffff
};

static uint32_t bitsInCache = 0;
static uint32_t cacheWord = 0;
static uint32_t bitNdx = 0;
static uint32_t bufSize = 64*sizeof(uint8_t);//2*4;    // write always 4 byte 
//const static uint32_t bitStreamSize = 64*sizeof(uint8_t);
static uint32_t validBitsWr = 0, validBitsRd = 0;

int main(void)
{
    uint32_t writeData[2] = {0x76543210, 0xfedcba98};
    int32_t readData = 0;
    uint8_t pFRAME1[64];
    memset(pFRAME1, 0, 64*sizeof(uint8_t));
    uint8_t *pFRAME1Rd = pFRAME1;
    uint8_t *pFRAME1Wr = pFRAME1;

    for(int i; i < 2; i++)
    {
        printf("want to write 0x%x\n", writeData[i]);
    }

#if 0
    writeBits(pFRAME1Wr, writeData[0], 32);
    printf("wrote 32 bits 0x%x\n", writeData[0]);

    writeBits(pFRAME1Wr, (writeData[1]>>(32-32)) & 0xF, 4);
    printf("wrote 4 bits 0x%x\n", (writeData[1]>>(32-32)) & 0xF);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-28)) & 0xF, 4);
    printf("wrote 4 bits 0x%x\n", (writeData[1]>>(32-28)) & 0xF);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-24)) & 0x3F, 6);
    printf("wrote 6 bits 0x%x\n", (writeData[1]>>(32-24)) & 0x3F);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-14)) & 0x3FF, 10);
    printf("wrote 10 bits 0x%x\n", (writeData[1]>>(32-14)) & 0x3FF);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-8)) & 0xF, 4);
    printf("wrote 4 bits 0x%x\n", (writeData[1]>>(32-8)) & 0xF);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-4)) & 0xF, 4);
    printf("wrote 4 bits 0x%x\n", (writeData[1]>>(32-4)) & 0xF);
#else
    // this is right, because we want to start with msb to shift msb left most
    writeBits(pFRAME1Wr, writeData[0], 32);
    printf("wrote 32 bits 0x%x\n", *((uint32_t*)&pFRAME1Wr[0]));

    writeBits(pFRAME1Wr, (writeData[1]>>(32-4)) & 0xF, 4);
    printf("wrote 4 bits 0x%x\n", (writeData[1]>>(32-4)) & 0xF);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-8)) & 0xF, 4);
    printf("wrote 4 bits 0x%x\n", (writeData[1]>>(32-8)) & 0xF);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-14)) & 0x3F, 6);
    printf("wrote 6 bits 0x%x\n", (writeData[1]>>(32-14)) & 0x3F);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-24)) & 0x3FF, 10);
    printf("wrote 10 bits 0x%x\n", (writeData[1]>>(32-24)) & 0x3FF);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-28)) & 0xF, 4);
    printf("wrote 4 bits 0x%x\n", (writeData[1]>>(32-28)) & 0xF);
    writeBits(pFRAME1Wr, (writeData[1]>>(32-32)) & 0xF, 4);
    printf("wrote 4 bits 0x%x\n", (writeData[1]>>(32-32)) & 0xF);
#endif
    for(int i; i < 4*2; i++)
    {
        printf("pFRAME1Wr[%d] = 0x%x\n", i, pFRAME1Wr[i]);
    }
    // 32
    if(readBits(pFRAME1Rd, 32, &readData))
    {
        printf("read failed or done\n");
        return -1;
    }

    printf("read 32 bits 0x%x\n", readData);
    
    // 1. 4
    if(readBits(pFRAME1Rd, 4, &readData))
    {
        printf("read failed or done\n");
        return -1;
    }
    
    readData = readData & 0xF;
    printf("read 4 bits 0x%x\n", readData);
    // 2. 4
    if(readBits(pFRAME1Rd, 4, &readData))
    {
        printf("read failed or done\n");
        return -1;
    }
    
    readData = readData & 0xF;
    printf("read 4 bits 0x%x\n", readData);  
    // 6
    if(readBits(pFRAME1Rd, 6, &readData))
    {
        printf("read failed or done\n");
        return -1;
    }
    
    readData = readData & 0x3F;
    printf("read 6 bits 0x%x\n", readData);
    // 10
    if(readBits(pFRAME1Rd, 10, &readData))
    {
        printf("read failed or done\n");
        return -1;
    }
    
    readData = readData & 0x3FF;
    printf("read 10 bits 0x%x\n", readData);  
    // 6
    if(readBits(pFRAME1Rd, 4, &readData))
    {
        printf("read failed or done\n");
        return -1;
    }
    
    readData = readData & 0xF;
    printf("read 4 bits 0x%x\n", readData);
    // 4
    if(readBits(pFRAME1Rd, 4, &readData))
    {
        printf("read failed or done\n");
        return -1;
    }
    
    readData = readData & 0xF;
    printf("read 4 bits 0x%x\n", readData);  

    printf("final read:\n");
    for(int i=0; i < 4*2; i++)
    {
        printf("pFRAME1[%d] = 0x%x\n", i, pFRAME1Rd[i]);
    }  
    
    return 0;
}

uint8_t writeBits(uint8_t *pBitBuf, uint32_t value, const uint32_t numberOfBits)
{
    const uint32_t validMask = bitMask[numberOfBits];

    // Put always 32 bits into buffer
    if ((bitsInCache + numberOfBits) < CACHE_BITS)
    {
        bitsInCache += numberOfBits;
        cacheWord = (cacheWord << numberOfBits) | (value & validMask);
    }
    else
    {
        int missing_bits = CACHE_BITS - bitsInCache;
        int remaining_bits = numberOfBits - missing_bits;
        value = value & validMask;
        // Avoid shift left by 32 positions
        uint32_t cW = (missing_bits == 32) ? 0 : (cacheWord << missing_bits);
        cW |= (value >> (remaining_bits));
printf("cW = 0x%x\n", cW);
        putBits(pBitBuf, cW, 32);

        //pBitBuf += 4; // move to next 32 bits

        cacheWord = value;
        bitsInCache = remaining_bits;
    }

    return validBitsWr;
}

void putBits(uint8_t* pBitBuf, uint32_t value, const uint32_t numberOfBits)
{
    if (numberOfBits != 0)
    {
        uint32_t byteOffset0 = bitNdx >> 3;
        uint32_t bitOffset = bitNdx & 0x7;

        bitNdx = (bitNdx + numberOfBits) & (bufSize - 1);
        validBitsWr += numberOfBits;

        uint32_t byteMask = bufSize - 1;

        uint32_t byteOffset1 = (byteOffset0 + 1) & byteMask;
        uint32_t byteOffset2 = (byteOffset0 + 2) & byteMask;
        uint32_t byteOffset3 = (byteOffset0 + 3) & byteMask;

        // Create tmp containing free bits at the left border followed by bits to
        // write, LSB's are cleared, if available Create mask to apply upon all
        // buffer bytes
        uint32_t tmp = (value << (32 - numberOfBits)) >> bitOffset;
        uint32_t mask = ~((bitMask[numberOfBits] << (32 - numberOfBits)) >> bitOffset);

        // read all 4 bytes from buffer and create a 32-bit cache
#if 1
        uint32_t cache = (((uint32_t)pBitBuf[byteOffset0]) << 24) |
                    (((uint32_t)pBitBuf[byteOffset1]) << 16) |
                    (((uint32_t)pBitBuf[byteOffset2]) << 8) |
                    (((uint32_t)pBitBuf[byteOffset3]) << 0);
#else
        uint32_t cache = (((uint32_t)pBitBuf[byteOffset0]) << 0) |
                    (((uint32_t)pBitBuf[byteOffset1]) << 8) |
                    (((uint32_t)pBitBuf[byteOffset2]) << 16) |
                    (((uint32_t)pBitBuf[byteOffset3]) << 24);
#endif

        cache = (cache & mask) | tmp;
        printf("cache = 0x%x\n", cache );
#if 1
        pBitBuf[byteOffset0] = (uint8_t)(cache >> 24);
        pBitBuf[byteOffset1] = (uint8_t)(cache >> 16);
        pBitBuf[byteOffset2] = (uint8_t)(cache >> 8);
        pBitBuf[byteOffset3] = (uint8_t)(cache >> 0);
#else
        pBitBuf[byteOffset0] = (uint8_t)(cache >> 0);
        pBitBuf[byteOffset1] = (uint8_t)(cache >> 8);
        pBitBuf[byteOffset2] = (uint8_t)(cache >> 16);
        pBitBuf[byteOffset3] = (uint8_t)(cache >> 24);
#endif

        printf("wrote 0x%x, byteOffset0 = %d\n", cache, byteOffset0);
        
        if ((bitOffset + numberOfBits) > 32)
        {
            uint32_t byteOffset4 = (byteOffset0 + 4) & byteMask;
            // remaining bits: in range 1..7
            // replace MSBits of next byte in buffer by LSBits of "value"
            int bits = (bitOffset + numberOfBits) & 7;
            cache = (uint32_t)pBitBuf[byteOffset4] & (~(bitMask[bits] << (8 - bits)));
            cache |= value << (8 - bits);
            pBitBuf[byteOffset4] = (uint8_t)cache;
        }
    }
}

int32_t readBits(uint8_t* pBitstream, const uint32_t numberOfBits, int32_t* retBits)
{
    uint32_t byteOffset = bitNdx >> 3;
    uint32_t bitOffset = bitNdx & 0x07;
//printf("got %d valid bits\n", validBits);
    bitNdx = (bitNdx + numberOfBits) & (bufSize - 1);

    uint32_t byteMask = bufSize - 1;

    if(validBitsRd < numberOfBits)
    {
        //int readBytes = fread(pBitstream, sizeof(uint8_t), bufSize, in_file);
        if((validBitsRd+bufSize*8) <= bufSize*8)
        {
            validBitsRd += bufSize*8;//readBytes*8;
        }
        
        if(validBitsRd <= 0 /*0*/)
        {
            return 1;
        }
    }

//printf(" pBitstream[%d] = 0x%x\n", byteOffset, pBitstream[byteOffset]);
//printf(" pBitstream[%d] = 0x%x\n", byteOffset+1, pBitstream[byteOffset+1]);
//printf(" pBitstream[%d] = 0x%x\n", byteOffset+2, pBitstream[byteOffset+2]);
//printf(" pBitstream[%d] = 0x%x\n", byteOffset+3, pBitstream[byteOffset+3]);
#if 1
    *retBits = (pBitstream[byteOffset & byteMask] << 24) |
                       (pBitstream[(byteOffset + 1) & byteMask] << 16) |
                       (pBitstream[(byteOffset + 2) & byteMask] << 8) |
                        pBitstream[(byteOffset + 3) & byteMask];
#else
    *retBits = (pBitstream[byteOffset & byteMask]) |
               (pBitstream[(byteOffset + 1) & byteMask] << 8) |
               (pBitstream[(byteOffset + 2) & byteMask] << 16) |
                pBitstream[(byteOffset + 3) & byteMask] << 24;
#endif
//printf(" *retBits = 0x%x\n", *retBits);
    if (bitOffset)
    {
        *retBits <<= bitOffset;
        *retBits |= pBitstream[(byteOffset + 4) & byteMask] >> (8 - bitOffset);
    }

    validBitsRd -= numberOfBits;

    *retBits = (*retBits >> (32 - numberOfBits));
//    printf("read uint32_t 0x%x\n", *retBits);

    return 0;
}
