/*
 * Per-machine one-shot boot hooks, hosted in BANK2 so board-specific
 * tables and code do not consume the common bank.
 */
#include <stdint.h>
#include "machine.h"
#include "rtl837x_pins.h"
#include "rtl837x_leds.h"
#include "rtl837x_sfr.h"
#include "rtl837x_regs.h"
#include "rtl837x_common.h"

#pragma codeseg BANK2
#pragma constseg BANK2

void machine_custom_init(void) __banked
{
    reg_bit_set(RTL837X_REG_LED_GLB_IO_EN, 6);
    reg_bit_set(RTL837X_REG_LED_MODE, 17);
    reg_bit_clear(RTL837X_REG_LED_MODE, 9);
    reg_bit_clear(RTL837X_REG_LED_MODE, 7);
}
