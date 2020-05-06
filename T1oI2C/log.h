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

#ifndef __LOG_H__
#define __LOG_H__

#include "platform/mbed_debug.h"
#include "platform/mbed_error.h"

#if MBED_CONF_SE050_LOGEN
#define LOG_E(...) printf("Fatal Error: "); error(__VA_ARGS__); printf("\n")
#define LOG_D(...) debug(__VA_ARGS__); printf("\n")
#define LOG_W(...) debug(__VA_ARGS__); printf("\n")
#else
#define LOG_E(...)
#define LOG_D(...)
#define LOG_W(...)
#endif

#define ENSURE_OR_GO_EXIT(test) if(!(test)) goto exit;

#endif //__LOG_H__
