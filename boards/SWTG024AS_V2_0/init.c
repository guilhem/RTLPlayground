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
    uint16_t pval;

    reg_bit_set(RTL837X_REG_LED_GLB_IO_EN, 6);
    reg_bit_set(RTL837X_REG_LED_MODE, 17);
    reg_bit_clear(RTL837X_REG_LED_MODE, 9);
    reg_bit_clear(RTL837X_REG_LED_MODE, 7);

    // OEM firmware sets these companion SDS0 polarity bits for the RTL8221B.
    sds_read(0, 0, 0);
    pval = SFR_DATA_U16;
    sds_write_v(0, 0, 0, pval | 0x100);

    sds_read(0, 6, 2);
    pval = SFR_DATA_U16;
    sds_write_v(0, 6, 2, pval | 0x4000);
}
