#ifndef __LOG_H__
#define __LOG_H__

#include "platform/mbed_debug.h"
#include "platform/mbed_error.h"

#define LOG_E(...) printf("Fatal Error: "); error(__VA_ARGS__); printf("\n")
#define LOG_D(...) debug(__VA_ARGS__); printf("\n")
#define LOG_W(...) debug(__VA_ARGS__); printf("\n")

#define ENSURE_OR_GO_EXIT(test) if(!(test)) goto exit;

#endif //__LOG_H__
