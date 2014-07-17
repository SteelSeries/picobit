#include <stdlib.h>
#include <stdio.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/fsmc.h>

#include <printf.h>

#include "time.h"
#include "lcd.h"

#define LCD_X_SIZE 240
#define LCD_Y_SIZE 320

/*
 * LCD pinout:
 * D0 - PD14
 * D1 - PD15
 * D2 - PD0
 * D3 - PD1
 * D4 - PE7
 * D5 - PE8
 * D6 - PE9
 * D7 - PE10
 * D8 - PE11
 * D9 - PE12
 * D10 - PE13
 * D11 - PE14
 * D12 - PE15
 * D13 - PD8
 * D14 - PD9
 * D15 - PD10
 *
 * NOE - PD4
 * NWE - PD5
 * NE1 - PD7
 * A16 - PD11
 */

static void __lcd_gpio_init(void)
{
    uint16_t gpio_set;
    struct sdram_timing tim;

    /* D0, D1, D2, D3, NOE, NWE, D13, D14, D15, A16 */
    gpio_set = (GPIO0|GPIO1|GPIO4|GPIO5|GPIO7|GPIO8|GPIO9|GPIO10|
                GPIO11|GPIO14|GPIO15);
    gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, gpio_set);
    gpio_set_af(GPIOD, GPIO_AF12, gpio_set);
    gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_set);

    /* D4, D5, D6, D7, D8, D9, D10, D11, D12 */
    gpio_set = (GPIO7|GPIO8|GPIO9|GPIO10|GPIO11|GPIO12|GPIO13|GPIO14|GPIO15);
    gpio_mode_setup(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, gpio_set);
    gpio_set_af(GPIOE, GPIO_AF12, gpio_set);
    gpio_set_output_options(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_set);

    /* Give LCD time to init, 100 msec */
    sleep(100);

    tim.trcd = 0;
    tim.trp  = 0;
    tim.twr  = 0;
    tim.trc  = 0; /* ? */
    tim.tras = 5;
    tim.txsr = 0; /* ? */
    tim.tmrd = 5;

    (void)tim;

    /* Timing setup */
    sdram_timing(&tim);
    /* Final setup for FSMC/FMC */
    sdram_command(SDRAM_BANK1, SDRAM_NORMAL, 0, 0);
}

static void __lcd_setup(uint16_t id)
{
    LCD_WRITE(0x0007,0x0021);
    LCD_WRITE(0x0000,0x0001);
    LCD_WRITE(0x0007,0x0023);
    LCD_WRITE(0x0010,0x0000);
    LCD_WRITE(0x0007,0x0033);
    LCD_WRITE(0x0011,0x6830);
    LCD_WRITE(0x0002,0x0600);
    LCD_WRITE(0x0012,0x6CEB);
    LCD_WRITE(0x0003,0xA8A4);
    LCD_WRITE(0x000C,0x0000);
    LCD_WRITE(0x000D,0x080C);
    LCD_WRITE(0x000E,0x2B00);
    LCD_WRITE(0x001E,0x00B0);
    LCD_WRITE(0x0001,0x2b3F);
    LCD_WRITE(0x0005,0x0000);
    LCD_WRITE(0x0006,0x0000);
    LCD_WRITE(0x0016,0xEF1C);
    LCD_WRITE(0x0017,0x0103);
    LCD_WRITE(0x000B,0x0000);
    LCD_WRITE(0x000F,0x0000);
    LCD_WRITE(0x0041,0x0000);
    LCD_WRITE(0x0042,0x0000);
    LCD_WRITE(0x0048,0x0000);
    LCD_WRITE(0x0049,0x013F);
    LCD_WRITE(0x004A,0x0000);
    LCD_WRITE(0x004B,0x0000);
    LCD_WRITE(0x0044,0xEF00);
    LCD_WRITE(0x0045,0x0000);
    LCD_WRITE(0x0046,0x013F);
    LCD_WRITE(0x0030,0x0707);
    LCD_WRITE(0x0031,0x0204);
    LCD_WRITE(0x0032,0x0204);
    LCD_WRITE(0x0033,0x0502);
    LCD_WRITE(0x0034,0x0507);
    LCD_WRITE(0x0035,0x0204);
    LCD_WRITE(0x0036,0x0204);
    LCD_WRITE(0x0037,0x0502);
    LCD_WRITE(0x003A,0x0302);
    LCD_WRITE(0x002F,0x12BE);
    LCD_WRITE(0x003B,0x0302);
    LCD_WRITE(0x0023,0x0000);
    LCD_WRITE(0x0024,0x0000);
    LCD_WRITE(0x0025,0x8000);
    LCD_WRITE(0x004f,0x0000);
    LCD_WRITE(0x004e,0x0000);
}

void lcd_clear(uint16_t color)
{
    int i = 0;

    LCD_POS_SET(0,0);

    LCD_REG = 0x0022;
    for(i = 0; i < LCD_X_SIZE * LCD_Y_SIZE; i++) {
        LCD_SRAM = color;
    }
}

inline void lcd_pixel(uint16_t x, uint16_t y, uint16_t data)
{
    LCD_POS_SET(x, y);
    LCD_WRITE(0x22, data);
}

static void __lcd_test(void)
{
    uint32_t count = 0, mask;
    int i, k;

    for (i = 0; i < LCD_X_SIZE; i++) {
        count++; mask = 0xFFFF;
        for (k = 0; k < LCD_Y_SIZE; k++) {
            lcd_pixel(i, k, count & mask--);
        }
    }
}

void lcd_init(void)
{
    uint16_t dev_code = 0;

    __lcd_gpio_init();

    LCD_REG = 0;
    dev_code = LCD_SRAM;

    printf("LCD -> %X\r\n", (unsigned int)dev_code);

    __lcd_setup(dev_code);
    lcd_clear(SET_COLOR(0xFF,0xFF,0xFF));

    __lcd_test();
}
