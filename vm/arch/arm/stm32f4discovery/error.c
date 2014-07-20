#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <picobit.h>
#include <debug.h>

#include "led.h"

void exit(int status)
{
    set_led(BOARD_LED_BLUE, 1);
    printf("Exit, halt!\n");
    while (1);
}

void halt_with_error (void)
{
    set_led(BOARD_LED_BLUE, 1);
    printf("Error, halt!\n");
    while(1);
}

void error (char *prim, char *msg)
{
	fprintf (stderr, "ERROR: %s: %s\n", prim, msg);
	exit (1);
}

void type_error (char *prim, char *type)
{
	fprintf (stderr, "ERROR: %s: An argument of type %s was expected\n",
		 prim, type);
	exit (1);
}
