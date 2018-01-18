/*
 * Common.c
 *
 *  Created on: Dec 25, 2017
 *      Author: Josip
 */

#include "../System/System.h"

#include <stdarg.h>

void System_Print(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    fflush(stdout);

    va_end(args);
}
