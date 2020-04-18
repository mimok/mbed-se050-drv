/*
 * @copyright Copyright (c) 2020, Michael Grand
 * @license SPDX-License-Identifier: Apache-2.0
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

#ifndef SE050_DRV_APDU_H_
#define SE050_DRV_APDU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "T1oI2C/phNxpEse_Api.h"
#include <stdbool.h>
#include "se050_enums.h"

/**
 * @file apdu.h
 * @author Michael Grand
 */

/**
 * Status returned by function used to pass APDU command to the SE050.
 * To status value are defined : APDU_OK (0) and APDU_ERROR (1).
 */
typedef enum {
	APDU_OK,		///< Proper execution of the fuction
	APDU_ERROR		///< An error has occured
} apdu_status_t;

/**
 * This structure allows to store command/response to/from the I2C
 * slave device connected to the SE050 chip.
 */
typedef struct {
	/// Used to store the type of a command either CONFIG, WRITE or READ
	SE050_I2CM_TAG_t tag;
	/// Structure containing a pointer to the cmd buffer and the length of the command
	phNxpEse_data cmd;
	/// Structure containing a point to the rsp buffer and the length of the obtained response
	phNxpEse_data rsp;
	/// Status word related to the command response
	uint8_t sw;
} i2cm_tlv_t;

/**
 * This structure contains pointers to attestation data fields. Currently, actual data
 * are stored in a single buffer.
 */
typedef struct {
	/// Structure containing a pointer to attested data and its length
	phNxpEse_data data;
	/// 16-byte Random returned by SE050
	uint8_t *outrandom;
	/// 18-byte ChipId returned by the SE050
	uint8_t *chipId;
	/// Structure containing a pointer to the attestation signature and its length
	phNxpEse_data signature;
	/// 12-byte time stamp returned by the SE050
	uint8_t *timeStamp;
} attestation_t;

/**
 * This structure is used to store the version of the SE050 firmware.
 */
typedef struct {
	/// Major version
	uint8_t major;
	/// Minor version
	uint8_t minor;
	/// Patch version
	uint8_t patch;
	/// AppletConfig version
	uint16_t appletConfig;
	/// SecureBox versin
	uint16_t secureBox;
} versionInfo_t;

/**
 * @biref Size of the APDU buffer
 */
#define APDU_BUFF_SZ 900
/**
 * @brief Structure storing the context of the connection.
 */
typedef struct {
	/// ATR value
	uint8_t atr[64];
	/// Length of the ATR
	uint8_t atrLen;
	/// Version of the SE050 firmware
	versionInfo_t version;
	/// APDU buffer which is used by this driver
	uint8_t buff[APDU_BUFF_SZ];
	/// Structure pointing to the APDU buffer and containing its length
	phNxpEse_data payload;
	/// Structure pointing to the APDU buffer and containing the length of the current command
	phNxpEse_data in;
	/// Structure pointing to the APDU buffer and containing the length of the current response
	phNxpEse_data out;
	/// Status word related to the current command response.
	uint16_t sw;
} apdu_ctx_t;

/**
 * This command is used to initialize the APDU context structure.
 * @param ctx Pointer to an APDU context structure
 */
void se050_initApduCtx(apdu_ctx_t *ctx);

/**
 * Allows connection to a SE050 chip (fixed I2C address 0x48).
 * This command trigger a chip reset followed by a select command.
 * ATR buffer and firmware version will be filled by this command.
 * @param ctx Pointer to an initialized APDU context structure
 * @returns status indicating if connect is successful
 */
apdu_status_t se050_connect(apdu_ctx_t *apdu_ctx);

/**
 * Disconnect from SE050 chip.
 * @param ctx Pointer to an initialized APDU context structure
 * @returns status indicating if disconnection is successful
 */
apdu_status_t se050_disconnect(apdu_ctx_t *ctx);

/**
 * Allows select the applet programmed in the SE050 chip.
 * This command fills the firmware version filed of the APDU context.
 * @param ctx Pointer to an initialized APDU context structure
 * @returns status indicating SE050 applet is properly selected
 */
apdu_status_t se050_select(apdu_ctx_t *apdu_ctx);


#define I2CM_100KHz 0 ///< I2C bus frequency between SE050 and I2C sensor set to 100KHz
#define I2CM_400KHz 1 ///< I2C bus frequency between SE050 and I2C sensor set to 400KHz

/**
 * Allows to execute an attested set of commands.
 * @param addr Address of the I2C sensor which has to be accessed
 * @param freq Frequency of the IC2 bus between SE050 and slave I2C sensor (I2CM_100KHz or I2CM_400KHz)
 * @param tlv Pointer to an array of I2C commands
 * @param sz_tlv Size of the tlv array
 * @param algo Algorithm which has to be used for the attestation generation
 * @param random Pointer to an 16-byte buffer containing random data
 * @param attestation Pointer to an attestation structure
 * @param ctx Pointer to an initialized APDU context structure
 * @returns status indicating SE050 applet is properly selected
 *
 * Example:
 * @code
 *  #include "apdu.h"
 *
 * 	apdu_status_t status;
 *	i2cm_tlv_t tlv[5] = {0};
 *	SE050_AttestationAlgo_t algo = SE050_AttestationAlgo_EC_SHA_512;
 *	uint8_t random[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
 *
 *	uint8_t cmd_config_i2cm[2] = {I2C_SENSOR_BUS_ADDRESS, 0x01};
 *	tlv[0].tag = SE050_TAG_I2CM_Config;
 *	tlv[0].cmd.len = 2;
 *	tlv[0].cmd.p_data = &cmd_config_i2cm[0];
 *	uint8_t cmd_config[2] = {0x01, TMP3_ONESHOT | TMP3_RES11};
 *	tlv[1].tag = SE050_TAG_I2CM_Write;
 *	tlv[1].cmd.len = 2;
 *	tlv[1].cmd.p_data = &cmd_config[0];
 *	uint8_t cmd_gettemp[1] = {0x00};
 *	tlv[2].tag = SE050_TAG_I2CM_Write;
 *	tlv[2].cmd.len = 1;
 *	tlv[2].cmd.p_data = &cmd_gettemp[0];
 *	tlv[3].tag = SE050_TAG_I2CM_Read;
 *	tlv[3].cmd.len = 2;
 *
 *	status = se050_i2cm_attestedCmds(
 *			0x48,
 *			0x01,
 *			&tlv[0],
 *			4,
 *			algo,
 *			&random[0],
 *			attestation,
 *			ctx);
 *	*temp = (tlv[3].rsp.p_data[0] << 8)  | tlv[3].rsp.p_data[1];
 *
 *	if(status != APDU_OK || ctx->sw != 0x9000)
 *		return APDU_ERROR;
 *	else
 *		return APDU_OK;
 * @endcode
 */
apdu_status_t se050_i2cm_attestedCmds(uint8_t addr, uint8_t freq,
		i2cm_tlv_t *tlv, uint8_t sz_tlv, SE050_AttestationAlgo_t algo,
		uint8_t *random, attestation_t *attestation, apdu_ctx_t *ctx);

#ifdef __cplusplus
}
#endif
#endif /* SE050_DRV_APDU_H_ */
