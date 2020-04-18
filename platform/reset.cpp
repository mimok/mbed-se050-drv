/*
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

#include "reset.h"
#include "mbed.h"

static DigitalOut se050_ena(MBED_CONF_TARGET_SE050_ENAPIN, 0);

void se050_powerOn(void)
{
	se050_ena = 1;
	thread_sleep_for(100);
}

void se050_powerOff(void)
{
	se050_ena = 0;
}

void se050_reset(void)
{
	se050_powerOff();
	se050_powerOn();
}


