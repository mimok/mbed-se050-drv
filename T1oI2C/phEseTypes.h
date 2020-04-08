/*
 * Copyright 2020 Michael Grand
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

#ifndef PHESETYPES_H
#define PHESETYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stddef.h"

typedef uint8_t             utf8_t;       /* UTF8 Character String */
typedef uint8_t             bool_t;       /* boolean data type */
typedef uint16_t        ESESTATUS;        /* Return values */
#define STATIC static

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

#ifdef __cplusplus
}
#endif

#endif /* PHESETYPES_H */
