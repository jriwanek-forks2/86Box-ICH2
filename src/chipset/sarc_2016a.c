/*
 * SARC 2016A
 *
 * Authors:	Tiseno100,
 *
 * Copyright 2022 Tiseno100.
 */

/*

    SARC 2016A Configuration Registers:

    Note: The SARC 2016A doesn't handle cache. Cache is handled by the Cyrix chips which is non existent on 86Box.

    Register 81h:
    Bit 0: Shadow RAM Write Protection(1: Off / 0: On)

    Register 82h:
    Bit 5: F0000-FFFFF Shadow Enable
    Bit 4: F0000-FFFFF Shadow Read Enable

    Register 83h:
    Bit 7: C0000-C3FFF Shadow Enable
    Bit 6: C4000-C7FFF Shadow Enable
    Bit 5: C8000-CBFFF Shadow Enable
    Bit 4: CC000-CFFFF Shadow Enable
    Bit 3: C0000-C3FFF Shadow Read Enable
    Bit 2: C4000-C7FFF Shadow Read Enable
    Bit 1: C8000-CBFFF Shadow Read Enable
    Bit 0: CC000-CFFFF Shadow Read Enable

    Register 84h:
    Bit 7: D0000-D3FFF Shadow Enable
    Bit 6: D4000-D7FFF Shadow Enable
    Bit 5: D8000-DBFFF Shadow Enable
    Bit 4: DC000-DFFFF Shadow Enable
    Bit 3: D0000-D3FFF Shadow Read Enable
    Bit 2: D4000-D7FFF Shadow Read Enable
    Bit 1: D8000-DBFFF Shadow Read Enable
    Bit 0: DC000-DFFFF Shadow Read Enable

    Register 83h:
    Bit 7: E0000-E3FFF Shadow Enable
    Bit 6: E4000-E7FFF Shadow Enable
    Bit 5: E8000-EBFFF Shadow Enable
    Bit 4: EC000-EFFFF Shadow Enable
    Bit 3: E0000-E3FFF Shadow Read Enable
    Bit 2: E4000-E7FFF Shadow Read Enable
    Bit 1: E8000-EBFFF Shadow Read Enable
    Bit 0: EC000-EFFFF Shadow Read Enable

    Register 86h:

    Bits 7 6
         0 0  Burst 1
         0 1  Burst 2
         1 0  Burst 3
         1 1  Burst 4

    Bits 5 4
         0 0  Rate 1
         0 1  Rate 2
         1 0  Rate 4
         1 1  Rate 8

    Bit 3: DRAM Write Wait State (1: 1WS / 0: 0WS)

    Register 87h:
    Bit 7: I/O Refresh Disable
    Bit 6: AT Bus Stepping Disable


*/

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#define HAVE_STDARG_H
#include <86box/86box.h>
#include "cpu.h"
#include <86box/timer.h>
#include <86box/io.h>
#include <86box/device.h>

#include <86box/mem.h>
#include <86box/port_92.h>
#include <86box/chipset.h>


typedef struct
{
    int index;
    uint8_t regs[17];

} sarc_2016a_t;

#ifdef ENABLE_SARC_2016A_LOG
int sarc_2016a_do_log = ENABLE_SARC_2016A_LOG;
static void
sarc_2016a_log(const char *fmt, ...)
{
    va_list ap;

    if (sarc_2016a_do_log) {
	va_start(ap, fmt);
	pclog_ex(fmt, ap);
	va_end(ap);
    }
}
#else
#define sarc_2016a_log(fmt, ...)
#endif


static void
sarc_2016a_memory_handler(int cur_reg, int reset, sarc_2016a_t *dev)
{
    uint16_t status;
    uint32_t base;

    if(!reset) {
        if(dev->index == 0x02) { /* System segment */
            base = 0xf0000;
            sarc_2016a_log("SARC 2016A Memory: Shadowing system\n");

            if(dev->regs[0x02] & 0x20) {
                if(dev->regs[0x02] & 0x10)
                    status = MEM_READ_INTERNAL;
                else
                    status = MEM_READ_EXTANY;

                if(dev->regs[0x01] & 1)
                    status |= MEM_WRITE_INTERNAL;
                else
                    status |= MEM_WRITE_EXTANY;
            }
            else
                status = MEM_READ_EXTANY | MEM_WRITE_EXTANY;

            mem_set_mem_state_both(base, 0x10000, status);
            }
        else if(dev->index > 0x02) { /* Rest of the segments */
            for(int i = 0; i < 4; i++) {
                base = 0xc0000 + ((dev->index - 3) << 16) + (i << 14);
                sarc_2016a_log("SARC 2016A Memory: Shadowing 0x%x segment\n", base);

            if(dev->regs[dev->index] & (0x80 >> i)) {
                if(dev->regs[dev->index] & (8 >> i))
                    status = MEM_READ_INTERNAL;
                else
                    status = MEM_READ_EXTANY;

                if(dev->regs[0x01] & 1)
                    status |= MEM_WRITE_INTERNAL;
                else
                    status |= MEM_WRITE_EXTANY;
            }
            else
                status = MEM_READ_EXTANY | MEM_WRITE_EXTANY;

                mem_set_mem_state_both(base, 0x4000, status);
            }
        }
    }
    else {
        sarc_2016a_log("SARC 2016A Memory: Resetting\n");
        mem_set_mem_state_both(0xc0000, 0x40000, MEM_READ_EXTANY | MEM_WRITE_EXTANY);
    }

    flushmmucache_nopc();
}

static void
sarc_2016a_write(uint16_t addr, uint8_t val, void *priv)
{
    sarc_2016a_t *dev = (sarc_2016a_t *) priv;
    
    if(!(addr & 1))
        dev->index = val;
    else {
        dev->index -= 0x80; /* Subtract the index register (index - 0x80). Register 0x80 doesn't actually exist but we ignore it with sanity checks */

        switch(dev->index)
        {
            case 0x01 ... 0x05:
                dev->regs[dev->index] = val;
                sarc_2016a_memory_handler(dev->index, 0, dev);
            break;

            default:
                if((dev->index > 0) && (dev->index < 16))
                    dev->regs[dev->index] = val;
            break;                
        }

        dev->index += 0x80; /* Restore the index register to what value it was */
    }
}


static uint8_t
sarc_2016a_read(uint16_t addr, void *priv)
{
    sarc_2016a_t *dev = (sarc_2016a_t *) priv;
    uint8_t ret = 0xff;

    if(!(addr & 1))
        return dev->index;
    else {
        dev->index -= 0x80;

        if((dev->index > 0) && (dev->index < 16))
            ret = dev->regs[dev->index];

        dev->index += 0x80;

        return ret;
    }
}


static void
sarc_2016a_reset(void *priv)
{
    sarc_2016a_t *dev = (sarc_2016a_t *) priv;
    memset(dev->regs, 0, sizeof(dev->regs)); /* Wash out the registers */
    dev->index = 0;

    sarc_2016a_memory_handler(0, 1, dev);
}


static void
sarc_2016a_close(void *priv)
{
    sarc_2016a_t *dev = (sarc_2016a_t *) priv;

    free(dev);
}


static void *
sarc_2016a_init(const device_t *info)
{
    sarc_2016a_t *dev = (sarc_2016a_t *) malloc(sizeof(sarc_2016a_t));
    memset(dev, 0, sizeof(sarc_2016a_t));

    /* Device */
    io_sethandler(0x0022, 2, sarc_2016a_read, NULL, NULL, sarc_2016a_write, NULL, NULL, dev); /* Ports 22h-23h: SARC 2016A */

    /* Port 92h */
    device_add(&port_92_device);

    sarc_2016a_reset(dev);

    return dev;
}

const device_t sarc_2016a_device = {
    .name = "SARC 2016A",
    .internal_name = "sarc_2016a",
    .flags = 0,
    .local = 0,
    .init = sarc_2016a_init,
    .close = sarc_2016a_close,
    .reset = sarc_2016a_reset,
    { .available = NULL },
    .speed_changed = NULL,
    .force_redraw = NULL,
    .config = NULL
};
