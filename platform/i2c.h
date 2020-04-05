#ifndef _I2C_H
#define _I2C_H

#define I2C_IDLE              0
#define I2C_BUSY              6
#define I2C_NO_DATA           7
#define I2C_NACK_ON_ADDRESS   8
#define I2C_OK                12
#define I2C_FAILED            13

typedef unsigned int i2c_error_t;
#define I2C_BUS_0   (0)

#if defined(__cplusplus)
extern "C"{
#endif

i2c_error_t axI2CInit( void );
i2c_error_t axI2CWrite(unsigned char bus, unsigned char addr, unsigned char * pTx, unsigned short txLen);
i2c_error_t axI2CRead(unsigned char bus, unsigned char addr, unsigned char * pRx, unsigned short rxLen);

#if defined(__cplusplus)
}
#endif

#endif // _I2C_H
