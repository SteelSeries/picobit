#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <picobit.h>
#include <dispatch.h>
#include <primitives.h>
#include <bignum.h>
#include <gc.h>

#include "led.h"
#include "time.h"
#include "usart.h"

#include <printf.h>

PRIMITIVE(#%getchar, arch_getchar, 1)
{
    char ch;
    int ret;

    a1 = decode_int(arg1);
    if (a1 != 0) sleep(a1);

    arg1 = OBJ_FALSE;

    ret = usart_getchar(&ch);
    if (ret == 0)
        arg1 = encode_int(ch);
}

PRIMITIVE_UNSPEC(#%putchar, arch_putchar, 1)
{
    a1 = decode_int(arg1);
    usart_putchar((char)a1);

    arg1 = OBJ_FALSE;
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

PRIMITIVE(#%clock, arch_clock, 0)
{
    struct tm tp;

    gettime(&tp);

    arg1 = encode_long(mktime(&tp));
}

PRIMITIVE(#%systick, arch_systick, 0)
{
    arg1 = encode_long(get_systick());
}
