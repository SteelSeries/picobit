#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <debug.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/f4/rng.h>

#include "random.h"

typedef struct __random
{
    uint32_t     val;
    uint8_t      *buf;
    size_t       len;
    volatile int ready;
    int          first;
} random_t;

static random_t g_rand = {
    .val = 0,
    .buf = NULL,
    .len = 0,
    .ready = 0,
    .first = 0
};

static void __start(void);
static void __stop(void);

void random_init(void)
{
    uint32_t val;

    val = RNG_CR | RNG_CR_IE;
    RNG_CR = val;
}

long random(void)
{
    long rnd;

    g_rand.len     = sizeof(long);
    g_rand.buf     = (uint8_t *)&rnd;

    __start();

    while (!g_rand.ready);

    return rnd;
}

void hash_rng_isr(void)
{
    uint32_t val;

    val = RNG_SR;
    if ((val & (RNG_SR_SEIS | RNG_SR_CEIS)) || (val & RNG_SR_DRDY) == 0)
        return;

    val = RNG_DR;
    if (g_rand.first) {
        g_rand.first = 0;
        g_rand.val = val;
    }

    if (g_rand.val == val)
        return;

    g_rand.val = val;
    if (g_rand.len >= 4) {
        g_rand.len -= 4;
        *(uint32_t *)&g_rand.buf[g_rand.len] = val;
    } else {
        while (g_rand.len > 0) {
            g_rand.buf[--g_rand.len] = val;
        }
    }

    if (g_rand.len == 0) {
        __stop();
        g_rand.ready = 1;
    }
}


static void __start(void)
{
    uint32_t val;

    g_rand.first = 1;
    g_rand.ready = 0;

    val = RNG_CR;
    val |= RNG_CR_RNGEN;
    RNG_CR = val;
}

static void __stop(void)
{
    uint32_t val;

    val = RNG_CR;
    val &= ~RNG_CR_RNGEN;
    RNG_CR = val;
}
