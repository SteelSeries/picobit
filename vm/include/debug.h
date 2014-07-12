#ifndef PICOBIT_DEBUG_H
#define PICOBIT_DEBUG_H

#if defined(CONFIG_VM_DEBUG) || defined(CONFIG_GC_DEBUG)
#include <stdio.h>
#endif

#ifdef CONFIG_VM_DEBUG
#ifdef CONFIG_ARM_STM32F4DISCOVERY
  #include <printf.h>

  #define printf tfp_printf
  #define sprintf tfp_sprintf
  #define fflush do {} while (0)
#endif
#define IF_TRACE(x) x
#else
#define IF_TRACE(x)
#define led_blink(x...)
#endif

#ifdef CONFIG_GC_DEBUG
#define IF_GC_TRACE(x) x
#else
#define IF_GC_TRACE(x)
#endif

#ifdef CONFIG_DEBUG_STRINGS
void show_type (obj o);
void show_obj (obj o);
void show_state (rom_addr pc);
#else
#define show_type(o)
#define show_obj(o)
#define show_state(pc)
#endif

#endif
