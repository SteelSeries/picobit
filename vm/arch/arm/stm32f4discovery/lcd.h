#ifndef LCD_H
#define LCD_H

#define LCD_REG  MMIO16(FSMC_BANK1_BASE + 0x0)
#define LCD_SRAM MMIO16(FSMC_BANK1_BASE + 0x20000)

#define LCD_ADDR_XPOS 0x004E
#define LCD_ADDR_YPOS 0x004F

#define LCD_WRITE(addr,data)                    \
    do {                                        \
        LCD_REG  = (addr);                      \
        LCD_SRAM = (data);                      \
    } while (0)

#define LCD_POS_SET(x,y)                        \
    do {                                        \
        LCD_WRITE(LCD_ADDR_XPOS, x);            \
        LCD_WRITE(LCD_ADDR_YPOS, y);            \
    } while (0)

/* RGB565 */
#define SET_COLOR(r,g,b)                                                \
    (uint16_t)((((b) << 8) & 0xF800)|(((g) << 3) & 0x07E0)|((r) & 0x1F))

void lcd_init(void);

#endif
