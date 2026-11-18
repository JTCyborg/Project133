#include <string.h>
#include <stdio.h>
#include "ibm.h"
#include "io.h"
#include "mem.h"
#include "cpu.h"

#include "ali1429.h"

static int ali1429_index;
static uint8_t ali1429_regs[256];

/*
 * AMI486 firmware diagnostics:
 *   OUT E9,AL - append an ASCII byte to the [E9] line in pcem.log.
 *   OUT 80,AL - append one POST code as [POST80] XX.
 *
 * The handlers are installed only for the AMI486 model. They do not
 * replace or modify the normal PCem device handlers.
 */
static char debugcon_line[512];
static size_t debugcon_length;
static int debugcon_previous_cr;

static void debugcon_flush_line(void) {
        if (debugcon_length) {
                debugcon_line[debugcon_length] = '\0';
                pclog("[E9] %s\n", debugcon_line);
                debugcon_length = 0;
                pclog_flush();
        }
}

static void debugcon_out(uint16_t port, uint8_t val, void *priv) {
        (void)port;
        (void)priv;

        if (val == '\r') {
                debugcon_flush_line();
                debugcon_previous_cr = 1;
                return;
        }
        if (val == '\n') {
                if (!debugcon_previous_cr)
                        debugcon_flush_line();
                debugcon_previous_cr = 0;
                return;
        }
        debugcon_previous_cr = 0;

        if (val == '\t' || (val >= 0x20 && val <= 0x7e)) {
                if (debugcon_length >= sizeof(debugcon_line) - 1)
                        debugcon_flush_line();
                debugcon_line[debugcon_length++] = (char)val;
        } else {
                char escaped[5];
                int count = snprintf(escaped, sizeof(escaped), "\\x%02X", val);
                int i;
                for (i = 0; i < count; i++) {
                        if (debugcon_length >= sizeof(debugcon_line) - 1)
                                debugcon_flush_line();
                        debugcon_line[debugcon_length++] = escaped[i];
                }
        }
}

static void debugpost_out(uint16_t port, uint8_t val, void *priv) {
        (void)port;
        (void)priv;
        debugcon_flush_line();
        pclog("[POST80] %02X\n", val);
        pclog_flush();
}

static void debug_ports_init(void) {
        io_sethandler(0x00e9, 1, NULL, NULL, NULL, debugcon_out, NULL, NULL, NULL);
        io_sethandler(0x0080, 1, NULL, NULL, NULL, debugpost_out, NULL, NULL, NULL);
        pclog("[DEBUG PORTS] AMI486: E9h=DebugCon, 80h=POST log enabled\n");
        pclog_flush();
}

static void ali1429_recalc() {
        int c;

        for (c = 0; c < 8; c++) {
                uint32_t base = 0xc0000 + (c << 15);
                if (ali1429_regs[0x13] & (1 << c)) {
                        switch (ali1429_regs[0x14] & 3) {
                        case 0:
                                mem_set_mem_state(base, 0x8000, MEM_READ_EXTERNAL | MEM_WRITE_EXTERNAL);
                                break;
                        case 1:
                                mem_set_mem_state(base, 0x8000, MEM_READ_INTERNAL | MEM_WRITE_EXTERNAL);
                                break;
                        case 2:
                                mem_set_mem_state(base, 0x8000, MEM_READ_EXTERNAL | MEM_WRITE_INTERNAL);
                                break;
                        case 3:
                                mem_set_mem_state(base, 0x8000, MEM_READ_INTERNAL | MEM_WRITE_INTERNAL);
                                break;
                        }
                } else
                        mem_set_mem_state(base, 0x8000, MEM_READ_EXTERNAL | MEM_WRITE_EXTERNAL);
        }

        flushmmucache();
}

void ali1429_write(uint16_t port, uint8_t val, void *priv) {
        if (!(port & 1))
                ali1429_index = val;
        else {
                ali1429_regs[ali1429_index] = val;
                //                pclog("ALI1429 write %02X %02X %04X:%04X %i\\n",ali1429_index,val,CS,pc,ins);
                switch (ali1429_index) {
                case 0x13:
                        ali1429_recalc();
                        break;
                case 0x14:
                        ali1429_recalc();
                        break;
                }
        }
}

uint8_t ali1429_read(uint16_t port, void *priv) {
        if (!(port & 1))
                return ali1429_index;
        if ((ali1429_index >= 0xc0 || ali1429_index == 0x20) && cpu_iscyrix)
                return 0xff; /*Don't conflict with Cyrix config registers*/
        return ali1429_regs[ali1429_index];
}

void ali1429_reset() { memset(ali1429_regs, 0xff, 256); }

void ali1429_init() {
        io_sethandler(0x0022, 0x0002, ali1429_read, NULL, NULL, ali1429_write, NULL, NULL, NULL);
        if (romset == ROM_AMI486)
                debug_ports_init();
}
