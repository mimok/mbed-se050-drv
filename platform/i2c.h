/**
 * Copyright (c) 2020, Michael Grand
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
i2c_error_t axI2CClose(void);

#if defined(__cplusplus)
}
#endif

#endif // _I2C_H
