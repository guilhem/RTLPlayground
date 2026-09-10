"""Host check of the actual probe: both slots, errors, and stale-buffer handling."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parent
repo = root.parent
source = (repo / 'cmd_parser.c').read_text()
probe = source[source.index('static void sfp_probe('):source.index('void parse_sfp(void)')]
harness = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define __code
#define RTL837X_REG_I2C_CTRL 0x418
#define RTL837X_REG_I2C_OUT 0x424
static uint8_t sfp_buf[16], calls, last_len;
static bool sfp_read_block(uint8_t slot, uint8_t offset, uint8_t len) {
    const uint8_t offsets[] = {11, 12, 11, 20, 20};
    const uint8_t lengths[] = {1, 1, 2, 1, 16};
    assert(calls < 10 && slot == calls / 5);
    assert(offset == offsets[calls % 5] && len == lengths[calls % 5]);
    calls++;
    last_len = len;
    if (len > 1) return false; /* leave previous bytes in the buffer */
    sfp_buf[0] = offset;
    return true;
}
static void print_string(const char *s) { fputs(s, stdout); }
static void itoa(uint8_t v) { printf("%u", v); }
static void print_byte(uint8_t v) { printf("%02x", v); }
static void write_char(char c) { putchar(c); }
static void print_reg(uint16_t reg) {
    assert(reg == 0x418 || reg == 0x424);
    if (reg == 0x418) printf("0x%08x", last_len > 1 ? 2 : 0);
    else print_string("0xdeadbeef");
}
'''
harness += probe + '\nint main(void) { sfp_probe(0); sfp_probe(1); assert(calls == 10); }\n'
with tempfile.TemporaryDirectory(prefix='probe-check-', dir=root) as tmp:
    path = Path(tmp)
    (path / 'probe.c').write_text(harness)
    subprocess.run(['gcc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                    str(path / 'probe.c'), '-o', str(path / 'probe')], check=True)
    output = subprocess.check_output([str(path / 'probe')], text=True)
lines = [line for line in output.splitlines() if line]
assert len(lines) == 10
assert sum(' ERROR ' in line for line in lines) == 4
assert sum(' OK ' in line for line in lines) == 6
for line in lines:
    assert (' bytes=' in line) == (' OK ' in line)
    assert 'raw=0xdeadbeef' in line
    assert ('ctrl=0x00000002' in line) == (' ERROR ' in line)
print('PASS: actual probe C, five reads per slot, immediate status, no stale bytes labelled valid')
