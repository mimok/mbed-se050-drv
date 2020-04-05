/*
 * Copyright 2010-2014,2018-2019 NXP
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * DAL i2c port implementation for linux
 *
 * Project: Trusted ESE Linux
 *
 */

#include <phNxpEsePal_i2c.h>
#include <phEseStatus.h>
#include "log.h"
#include <time.h>
#include "../platform/i2c.h"

#define MAX_RETRY_CNT   10

/*******************************************************************************
**
** Function         phPalEse_i2c_close
**
** Description      Closes PN547 device
**
** param[in]        pDevHandle - device handle
**
** Returns          None
**
*******************************************************************************/
void phPalEse_i2c_close(void *pDevHandle)
{
    if (NULL != pDevHandle)
    {
        #ifdef Android
            close((intptr_t)pDevHandle);
        #else
            pDevHandle = NULL;
        #endif
    }

    return;
}

/*******************************************************************************
**
** Function         phPalEse_i2c_open_and_configure
**
** Description      Open and configure pn547 device
**
** param[in]        pConfig     - hardware information
**
** Returns          ESE status:
**                  ESESTATUS_SUCCESS            - open_and_configure operation success
**                  ESESTATUS_INVALID_DEVICE     - device open operation failure
**
*******************************************************************************/
ESESTATUS phPalEse_i2c_open_and_configure(pphPalEse_Config_t pConfig)
{
    int nHandle;
    int retryCnt = 0;

    LOG_D("%s Opening port",__FUNCTION__);
    /* open port */
    /*Disable as interface reset happens on every session open*/
    //se05x_ic_reset();
retry:
    nHandle=axI2CInit();
    if (nHandle != I2C_OK){
        LOG_E("%s Failed retry ",__FUNCTION__);
        if (nHandle == I2C_BUSY ) {
            retryCnt++;
            LOG_E("Retry open eSE driver, retry cnt : %d ", retryCnt);
            if (retryCnt < MAX_RETRY_CNT) {
                wait_ms(ESE_POLL_DELAY_MS);
                goto retry;
            }
        }
        LOG_E("I2C init Failed: retval %x ",nHandle);
        pConfig->pDevHandle = NULL;
        return ESESTATUS_INVALID_DEVICE;
    }
    LOG_D("I2C driver Initialized :: fd = [%d] ", nHandle);
    pConfig->pDevHandle = (void*) ((intptr_t) nHandle);
    return ESESTATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         phPalEse_i2c_read
**
** Description      Reads requested number of bytes from pn547 device into given buffer
**
** param[in]       pDevHandle       - valid device handle
** param[in]       pBuffer          - buffer for read data
** param[in]       nNbBytesToRead   - number of bytes requested to be read
**
** Returns          numRead   - number of successfully read bytes
**                  -1        - read operation failure
**
*******************************************************************************/
int phPalEse_i2c_read(void *pDevHandle, uint8_t * pBuffer, int nNbBytesToRead)
{
    int ret = -1 , retryCount = 0;;
    int numRead = 0;
    LOG_D("%s Read Requested %d bytes ", __FUNCTION__, nNbBytesToRead);
    //wait_ms(ESE_POLL_DELAY_MS);
    while(numRead != nNbBytesToRead)
    {
        ret=axI2CRead(I2C_BUS_0, SMCOM_I2C_ADDRESS, pBuffer, nNbBytesToRead );
        if(ret != I2C_OK)
        {
            LOG_D("_i2c_read() error : %d ",ret);
            if ((ret == I2C_NACK_ON_ADDRESS) && (retryCount < MAX_RETRY_COUNT))
            {
                retryCount++;
                /* 1ms delay to give ESE polling delay */
                /*i2c driver back off delay is providing 1ms wait time so ignoring waiting time at this level*/
                //wait_ms(ESE_POLL_DELAY_MS);
                LOG_D("_i2c_read() failed. Going to retry, counter:%d  !", retryCount);
                continue;
            }
            return -1;
        }
        else
        {
            numRead = nNbBytesToRead;
            break;
        }
        LOG_D("Read Returned = %d ", ret);
    }
    return numRead;
}

/*******************************************************************************
**
** Function         phPalEse_i2c_write
**
** Description      Writes requested number of bytes from given buffer into pn547 device
**
** param[in]       pDevHandle       - valid device handle
** param[in]       pBuffer          - buffer for read data
** param[in]       nNbBytesToWrite  - number of bytes requested to be written
**
** Returns          numWrote   - number of successfully written bytes
**                  -1         - write operation failure
**
*******************************************************************************/
int phPalEse_i2c_write(void *pDevHandle, uint8_t * pBuffer, int nNbBytesToWrite)
{
    int ret = I2C_OK, retryCount = 0;
    int numWrote = 0;
    if (NULL == pDevHandle)
    {
        return -1;
    }
    pBuffer[0] = 0x5A; //Recovery if stack forgot to add NAD byte.
    do
    {
        /* 1ms delay to give ESE polling delay */
        wait_ms(ESE_POLL_DELAY_MS);
        ret =axI2CWrite(I2C_BUS_0, SMCOM_I2C_ADDRESS, pBuffer , nNbBytesToWrite );
        if (ret != I2C_OK )
        {
            LOG_D("_i2c_write() error : %d ",ret);
            if ((ret == I2C_NACK_ON_ADDRESS)&& (retryCount < MAX_RETRY_COUNT))
            {
                retryCount++;
                /* 1ms delay to give ESE polling delay */
                /*i2c driver back off delay is providing 1ms wait time so ignoring waiting time at this level*/
                //wait_ms(ESE_POLL_DELAY_MS);
                LOG_D("_i2c_write() failed. Going to retry, counter:%d  !", retryCount);
                continue;
            }
            return -1;
        }
        else
        {
            numWrote= nNbBytesToWrite;
            //wait_ms(ESE_POLL_DELAY_MS);
            break;
        }
    }while (ret != I2C_OK);
    return numWrote;
}
