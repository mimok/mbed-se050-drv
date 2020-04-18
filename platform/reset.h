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

#ifndef MBED_SE050_DRV_PLATFORM_RESET_H_
#define MBED_SE050_DRV_PLATFORM_RESET_H_

#if defined(__cplusplus)
extern "C"{
#endif

/**
 * Switch on the SE050 and attached I2C sensor power supply using SE050 ENA pin.
 * Sleep for 100ms after switch on to allow SE050 to boot.
 */
void se050_powerOn(void);

/**
 * Switch off the SE050 and attached I2C sensor power supply using SE050 ENA pin.
 */
void se050_powerOff(void);

/**
 * Switch off then switch on the SE050 and attached I2C sensor power supply using SE050 ENA pin.
 */
void se050_reset(void);

#if defined(__cplusplus)
}
#endif

#endif /* MBED_SE050_DRV_PLATFORM_RESET_H_ */
