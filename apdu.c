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

#include "apdu.h"
#include <string.h>

uint32_t setTLVU8(SE050_TAG_t tag, uint8_t *buff, uint8_t value, bool extended) {

	uint32_t i = 0;
	buff[i++] = tag;
	if (extended)
		buff[i++] = 0x00;
	buff[i++] = 0x01;
	buff[i++] = value;

	return i;
}

uint32_t setTLVU16(SE050_TAG_t tag, uint8_t *buff, uint16_t value,
		bool extended) {

	uint32_t i = 0;
	buff[i++] = tag;
	if (extended)
		buff[i++] = 0x00;
	buff[i++] = 0x02;
	buff[i++] = (value & 0xFF00) >> 8;
	buff[i++] = value & 0x00FF;

	return i;
}

uint32_t setTLVU32(SE050_TAG_t tag, uint8_t *buff, uint32_t value,
		bool extended) {

	uint32_t i = 0;
	buff[i++] = tag;
	if (extended)
		buff[i++] = 0x00;
	buff[i++] = 0x04;
	buff[i++] = (value & 0xFF000000) >> 24;
	buff[i++] = (value & 0x00FF0000) >> 16;
	buff[i++] = (value & 0x0000FF00) >> 8;
	buff[i++] = (value & 0x000000FF);

	return i;
}

//TODO: extended array 0x82?
uint32_t setTLVarray(SE050_TAG_t tag, uint8_t *buff, const uint8_t *array,
		uint32_t len, bool extended) {

	uint32_t i = 0;

	//first move data to prevent overwriting
	i += (extended) ? 3 : 2;
	memmove(&buff[i], &array[0], len);

	//then set tag and length
	i = 0;
	buff[i++] = tag;
	if (extended)
		buff[i++] = (len & 0x0000FF00) >> 8;
	buff[i++] = (len & 0x000000FF);

	return i + len;
}

int32_t getTLVarray(SE050_TAG_t tag, uint8_t *buff, uint8_t *array[],
		uint32_t *len, bool extended) {

	if (tag != buff[0])
		return -1;

	if (buff[1] == 0x82) { //extended
		*len = buff[2] << 8 | buff[3];
		*array = &buff[4];
		return *len + 4;
	} else {
		*len = buff[1];
		*array = &buff[2];
		return *len + 2;
	}
}

static uint32_t setI2CMCmds(i2cm_tlv_t *tlv, uint8_t sz_tlv,
		phNxpEse_data *payload) {

	uint32_t i = 0;
	for (int k = 0; k < sz_tlv; k++) {
		switch (tlv[k].tag) {
		case SE050_TAG_I2CM_Config:
			i += setTLVarray(tlv[k].tag, &payload->p_data[i],
					&tlv[k].cmd.p_data[0], 2, true);
			break;
		case SE050_TAG_I2CM_Write:
			i += setTLVarray(tlv[k].tag, &payload->p_data[i],
					&tlv[k].cmd.p_data[0], tlv[k].cmd.len, true);
			break;
		case SE050_TAG_I2CM_Read:
			i += setTLVU16(tlv[k].tag, &payload->p_data[i],
					tlv[k].cmd.len & 0x0000FFFF, true);
			break;
		default:
			return -1;
		}
	}
	return i;
}

static int32_t getI2CMRsps(i2cm_tlv_t *tlv, uint8_t sz_tlv,
		phNxpEse_data *payload) {

	uint32_t i = 0;
	uint8_t tag;
	uint16_t len;
	for (int k = 0; k < sz_tlv; k++) {
		tag = payload->p_data[i++];
		if (tag != tlv[k].tag)
			return APDU_ERROR;
		tlv[k].sw = payload->p_data[i++];
		switch (tag) {
		case SE050_TAG_I2CM_Config:
		case SE050_TAG_I2CM_Write:
			break;
		case SE050_TAG_I2CM_Read:
			len = payload->p_data[i++] << 8;
			len |= payload->p_data[i++];
			tlv[k].rsp.len = len;
			tlv[k].rsp.p_data = &payload->p_data[i];
			i += len;
			break;
		default:
			return APDU_ERROR;
		}
	}
	return APDU_OK;
}

// does not support extend command
apdu_status_t APDU_case4(const uint8_t *header, apdu_ctx_t *ctx) {
	ESESTATUS status = ESESTATUS_OK;

	memmove(&ctx->in.p_data[5], &ctx->in.p_data[0], ctx->in.len);
	memcpy(&ctx->in.p_data[0], &header[0], 4);
	ctx->in.p_data[4] = ctx->in.len & 0xFF;
	ctx->in.len += 5;
	ctx->in.p_data[ctx->in.len++] = ctx->out.len & 0xFF;
	if(ctx->out.len == 0)
		ctx->out.len = APDU_BUFF_SZ;

	status = phNxpEse_Transceive(&ctx->in, &ctx->out);
	if (status == ESESTATUS_OK) {
		ctx->sw = ctx->out.p_data[ctx->out.len - 2] << 8
				| ctx->out.p_data[ctx->out.len - 1];
		ctx->out.len -= 2;
		return APDU_OK;
	} else {
		ctx->out.len = 0;
		ctx->sw = 0;
		return APDU_ERROR;
	}
}

void se050_initApduCtx(apdu_ctx_t *ctx) {
	memset(&ctx->atr[0], 0, sizeof(ctx->atr));
	memset(&ctx->version, 0, sizeof(ctx->version));
	ctx->in.len = APDU_BUFF_SZ;
	ctx->in.p_data = &ctx->buff[0];
	ctx->out.len = 0;
	ctx->out.p_data = &ctx->buff[0];
	ctx->sw = 0x0000;
}

apdu_status_t se050_connect(apdu_ctx_t *ctx) {
	ESESTATUS ret;
	apdu_status_t status;
	phNxpEse_initParams initParams = {.initMode = ESE_MODE_NORMAL};

	ret = phNxpEse_open(initParams);
	if (ret != ESESTATUS_SUCCESS) {
		return APDU_ERROR;
		//MBED_ERROR(MBED_ERROR_CODE_OPEN_FAILED, "Can't connect to eSE");
	}

	ret = phNxpEse_init(initParams, &ctx->out);
	if (ret != ESESTATUS_SUCCESS) {
		ctx->payload.len = 0;
		return APDU_ERROR;
		//MBED_ERROR(MBED_ERROR_CODE_INITIALIZATION_FAILED, "Can't connect to eSE");
	}
	ctx->atrLen = ctx->out.len;
	memcpy(ctx->atr, ctx->out.p_data, ctx->atrLen);

	status = se050_select(ctx);
	if(status != APDU_OK || ctx->sw != 0x9000)
		return APDU_ERROR;
	ctx->version.major = ctx->out.p_data[0];
	ctx->version.minor = ctx->out.p_data[1];
	ctx->version.patch = ctx->out.p_data[2];
	ctx->version.appletConfig = ctx->out.p_data[3] << 8 | ctx->out.p_data[4];
	ctx->version.secureBox = ctx->out.p_data[5] << 8 | ctx->out.p_data[6];
	return APDU_OK;
}

//TODO: replace payload with applet version
apdu_status_t se050_select(apdu_ctx_t *ctx) {

	apdu_status_t status;
	const uint8_t select_header[] = { 0x00, 0xA4, 0x04, 0x00 };
	const uint8_t applet_aid[] = { 0xA0, 0x00, 0x00, 0x03, 0x96, 0x54, 0x53,
			0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00 };

	ctx->in.len = sizeof(applet_aid);
	memcpy(&ctx->in.p_data[0], &applet_aid[0], ctx->in.len);
	ctx->out.len = 0;

	status = APDU_case4(&select_header[0], ctx);

	return status;
}

apdu_status_t se050_i2cm_attestedCmds(uint8_t addr, uint8_t freq,
		i2cm_tlv_t *tlv, uint8_t sz_tlv, SE050_AttestationAlgo_t algo,
		uint8_t *random, attestation_t *attestation, apdu_ctx_t *ctx) {

	apdu_status_t status;

	const uint8_t select_header[] = { 0x80, SE050_INS_CRYPTO
			| SE050_INS_ATTEST, SE050_P1_DEFAULT, SE050_P2_I2CM };

	uint16_t cmdsLen;
	cmdsLen = setI2CMCmds(tlv, sz_tlv, &ctx->in);

	uint32_t lc = 0;
	lc += setTLVarray(SE050_TAG_1, &ctx->in.p_data[lc], &ctx->in.p_data[0],
			cmdsLen, false); //pas possible car on ecrase data avec tag
	lc += setTLVU32(SE050_TAG_2, &ctx->in.p_data[lc], 0xF0000012, false);
	lc += setTLVU8(SE050_TAG_3, &ctx->in.p_data[lc], algo, false);
	lc += setTLVarray(SE050_TAG_7, &ctx->in.p_data[lc], random, 16, false);
	ctx->in.len = lc; //TODO add check on data length
	ctx->out.len = 0;

	status = APDU_case4(&select_header[0], ctx);
	if (ctx->sw != 0x9000 | ctx->out.len <= 0)
		return APDU_ERROR;

	int32_t le = 0;
	uint32_t fieldLen = 0;
	phNxpEse_data rsp_tag1_tlv = { .len = 0, .p_data = NULL };
	le += getTLVarray(SE050_TAG_1, &ctx->out.p_data[le], &rsp_tag1_tlv.p_data,
			&rsp_tag1_tlv.len, false);
	le += getTLVarray(SE050_TAG_3, &ctx->out.p_data[le],
			&attestation->timeStamp, &fieldLen, false);
	le += getTLVarray(SE050_TAG_4, &ctx->out.p_data[le],
			&attestation->outrandom, &fieldLen, false);
	le += getTLVarray(SE050_TAG_5, &ctx->out.p_data[le], &attestation->chipId,
			&fieldLen, false);
	le += getTLVarray(SE050_TAG_6, &ctx->out.p_data[le],
			&attestation->signature.p_data, &fieldLen, false);
	attestation->signature.len = fieldLen;
	//ctx->out.len = le;

	getI2CMRsps(tlv, sz_tlv, &rsp_tag1_tlv);

	return APDU_OK;
}
