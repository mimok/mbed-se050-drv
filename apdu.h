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

#ifndef SE050_DRV_APDU_H_
#define SE050_DRV_APDU_H_

#include <stdint.h>
#include "phNxpEse_Api.h"
#include <stdbool.h>
#include "se050_enums.h"

#define APDU_OK 	0
#define APDU_ERROR 	1

typedef uint8_t apdu_status_t;

typedef struct {
	SE050_I2CM_TAG_t tag;
	phNxpEse_data cmd;
	phNxpEse_data rsp;
	uint8_t sw;
} i2cm_tlv_t;

typedef struct {
	uint8_t *outrandom;
	uint8_t *chipId;
	phNxpEse_data signature;
	uint8_t *timeStamp;
} attestation_t;

typedef struct {
	uint8_t major;
	uint8_t minor;
	uint8_t patch;
	uint16_t appletConfig;
	uint16_t secureBox;
} versionInfo_t;

#define APDU_BUFF_SZ 900
typedef struct apdu_ctx {
	uint8_t atr[64];
	uint8_t atrLen;
	versionInfo_t version;
	uint8_t buff[APDU_BUFF_SZ];
	phNxpEse_data payload;
	phNxpEse_data in;
	phNxpEse_data out;
	uint16_t sw;
} apdu_ctx_t;


uint32_t setTLVU8(SE050_TAG_t tag, uint8_t *buff, uint8_t value, bool extended);
uint32_t setTLVU16(SE050_TAG_t tag, uint8_t *buff, uint16_t value,
		bool extended);
uint32_t setTLVU32(SE050_TAG_t tag, uint8_t *buff, uint32_t value,
		bool extended);
uint32_t setTLVarray(SE050_TAG_t tag, uint8_t *buff, const uint8_t *array,
		uint32_t len, bool extended);
int32_t getTLVarray(SE050_TAG_t tag, uint8_t *buff, uint8_t *array[],
		uint32_t *len, bool extended);


apdu_status_t APDU_case4(const uint8_t *header, apdu_ctx_t *ctx);

void se050_initApduCtx(apdu_ctx_t *ctx);
apdu_status_t se050_connect(apdu_ctx_t *apdu_ctx) ;
apdu_status_t se050_select(apdu_ctx_t *apdu_ctx);
apdu_status_t se050_i2cm_attestedCmds(uint8_t addr, uint8_t freq,
		i2cm_tlv_t *tlv, uint8_t sz_tlv, SE050_AttestationAlgo_t algo,
		uint8_t *random, attestation_t *attestation, apdu_ctx_t *ctx);

#endif /* SE050_DRV_APDU_H_ */
