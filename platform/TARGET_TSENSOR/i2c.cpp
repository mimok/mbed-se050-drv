#include "../i2c.h"
#include "mbed.h"

static I2C *i2cMbed;

i2c_error_t axI2CInit(void)
{
    i2cMbed = new I2C(PB_9,PB_8);

    i2cMbed->frequency(400000);

    return I2C_OK;
}

i2c_error_t axI2CWrite(unsigned char bus_unused_param, 
                       unsigned char addr, 
                       unsigned char *pTx, 
                       unsigned short txLen)
{
    int ret = 0;
    ret = i2cMbed->write(addr, (char*)pTx, txLen);
    if(ret != 0)
    {
        return I2C_FAILED;
    }
    return I2C_OK;
}

i2c_error_t axI2CRead(unsigned char bus, 
                      unsigned char addr, 
                      unsigned char *pRx, 
                      unsigned short rxLen)
{
    int ret = 0;

    ret = i2cMbed->read(addr, (char*)pRx, rxLen);
    if(ret != 0)
    {
        return I2C_FAILED;
    }
    return I2C_OK;
}
