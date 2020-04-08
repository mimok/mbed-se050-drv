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
