#include "hal.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    uint8_t b[4] = {0x78, 0x56, 0x34, 0x12};
    assert(read_u16_le(b) == 0x5678);
    assert(read_u32_le(b) == 0x12345678u);
    assert(SWAP16(0x1234) == 0x3412);
    assert(SWAP32(0x12345678u) == 0x78563412u);
    assert(sizeof(RbPlayer) % 2 == 0);
    assert(sizeof(RbEnemy) % 2 == 0);
    printf("endian/packing OK\n");
    return 0;
}
