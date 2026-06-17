#include <stdio.h>

void i2c_send(unsigned char address, unsigned char data)
{
    printf("[I2C TX] Addr: 0x%02X | Data: 0x%02X\n", address, data);
}
