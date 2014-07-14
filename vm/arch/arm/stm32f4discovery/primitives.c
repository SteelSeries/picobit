#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <picobit.h>
#include <dispatch.h>
#include <primitives.h>
#include <bignum.h>

#include "led.h"
#include "time.h"
#include "usart.h"

PRIMITIVE_UNSPEC(#%putchar, arch_putchar, 1)
{
    a1 = decode_int(arg1);
    usart_putchar((char)a1);
}

PRIMITIVE_UNSPEC(#%sleep, arch_sleep, 1)
{
    uint16_t msec;

    msec = decode_int(arg1);
    sleep(msec);
    arg1 = OBJ_FALSE;
}

PRIMITIVE_UNSPEC(#%set-led!, arch_set_led, 2)
{
    set_led(decode_int(arg1), decode_int(arg2));

    arg1 = OBJ_FALSE;
    arg2 = OBJ_FALSE;
}

PRIMITIVE_UNSPEC(#%clock, arch_clock, 0)
{
    struct tm tp;

    gettime(&tp);

    arg1 = encode_int(mktime(&tp));
}
