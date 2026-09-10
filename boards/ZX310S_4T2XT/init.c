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
	// For this device, the reset value of RTL837X_PIN_MUX_0 is 0x30000000,
	// which would disables all LEDS, enable them manually:
	REG_SET(RTL837X_PIN_MUX_0, 0x30db68bf);
}
