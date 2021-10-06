#include <stdio.h>
#include <stdint.h>

int bit_count(uint8_t x)
{
    int count = 0;
    while (x) {
        count++;
        x &= (x-1);
    }
    return count;
}

#define OTHER_BITS (0x0b)
void init(uint8_t channel_mask)
{
    uint8_t n_channels;
    uint16_t cfg_bits;
    uint8_t config[2] = {0, 0};

    n_channels = bit_count(channel_mask);
    cfg_bits = (channel_mask << 4) | OTHER_BITS;

    config[0]  = (cfg_bits >> 8) & 0xff;
    config[1] |= (cfg_bits & 0xff);

    printf("init -- ENTER\n");
    printf("mask: 0x%02x => n_channels: %d\n", channel_mask, n_channels);
    printf("cfg_bits: %04x\n", cfg_bits);
    printf("bytes: %02x : %02x\n", config[0], config[1]);
}

int main()
{
    uint8_t channel_mask = 0xff;
    uint8_t n_channels;
    uint8_t config[2] = {0x0f, 0xf0};
    uint8_t config2[2] = {0, 0};
    uint16_t cfg_bits = ((config[0]<<8) | config[1]) | OTHER_BITS;
    uint16_t cfg_bits2;

    config2[0]  = (cfg_bits >> 8) & 0xff;
    config2[1] |= (cfg_bits & 0xff);

    n_channels = bit_count(channel_mask);

    printf("%04x => %02x : %02x\n", cfg_bits, config[0], config[1]);
    printf("     => %02x : %02x\n", config2[0], config2[1]);

    cfg_bits2 = (channel_mask << 4) | OTHER_BITS;
    printf("mask: 0x%02x => n_channels: %d\n", channel_mask, n_channels);
    printf("cfg_bits: %04x\n", cfg_bits2);


    init(channel_mask);
}

