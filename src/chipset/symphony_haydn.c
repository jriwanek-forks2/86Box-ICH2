/*
 * Symphony Haydn II
 *
 * Authors:	Tiseno100,
 *
 * Copyright 2022 Tiseno100.
 */

/*

    Symphony Haydn II Configuration Registers:

    Note: We emulate the Haydn II which is Haydn with 486 support and with a cache controller chip.
    Note 2: We don't know how exactly the memory is remapped on old AMI BIOS cores. Recommended to disable it.

    Register 01h:
    Bit 0: FASTA20 Trigger

    Register 02h:
    CPU Bus Speed: Note: We don't know the datasheet default Bus Speed (Speculated CLK2/3 from defaults BIOS gives)
    Bits 5 4 3:
         0 0 0  CLK2/3
         0 0 1  CLK2/4
         0 1 0  CLK2/5
         0 1 1  ATCLK2
         1 0 0  CLK2/6
         1 0 1  Undocumented
         1 1 0  CLK2/2.5
         1 1 1  CLK2/2

    Register 2Dh:
    Bit 7: Remap 256KB to Top Memory, Note: We can also remap 384KB but the registers responsible remain unknown
           Old AMI BIOS cores don't seem to utilize it, plus we don't know what ranges it actually remaps
           Speculated ranges are A, B and D

    Register 2Eh:
    Bit 3: CC000-CFFFF Shadow Read
    Bit 2: CC000-CFFFF Shadow Write
    Bit 1: C8000-CBFFF Shadow Read
    Bit 0: C8000-CBFFF Shadow Write
    Bit 3: C4000-C7FFF Shadow Read
    Bit 2: C4000-C7FFF Shadow Write
    Bit 1: C0000-C3FFF Shadow Read
    Bit 0: C0000-C3FFF Shadow Write

    Register 2Fh:
    Bit 3: DC000-DFFFF Shadow Read
    Bit 2: DC000-DFFFF Shadow Write
    Bit 1: D8000-DBFFF Shadow Read
    Bit 0: D8000-DBFFF Shadow Write
    Bit 3: D4000-D7FFF Shadow Read
    Bit 2: D4000-D7FFF Shadow Write
    Bit 1: D0000-D3FFF Shadow Read
    Bit 0: D0000-D3FFF Shadow Write

    Register 30h:
    Bit 7: E0000-EFFFF Shadow Read
    Bit 6: E0000-EFFFF Shadow Write

    Register 31h:
    Bit 7: F0000-FFFFF Shadow Read
    Bit 6: F0000-FFFFF Shadow Write

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
    uint8_t regs[70];
} symphony_haydn_t;

#ifdef ENABLE_SYMPHONY_HAYDN_LOG
int symphony_haydn_do_log = ENABLE_SYMPHONY_HAYDN_LOG;
static void
symphony_haydn_log(const char *fmt, ...)
{
    va_list ap;

    if (symphony_haydn_do_log) {
	va_start(ap, fmt);
	pclog_ex(fmt, ap);
	va_end(ap);
    }
}
#else
#define symphony_haydn_log(fmt, ...)
#endif


static void
symphony_haydn_fasta20(symphony_haydn_t *dev)
{
    int enable = dev->regs[0x01] & 1;

    if(enable)
        symphony_haydn_log("Symphony Haydn FASTA20: A FASTA20 cycle was provoked\n");

    if(enable)
        mem_a20_alt = 1;
    else
        mem_a20_alt = 0;
    
    mem_a20_recalc();
}

static void
symphony_haydn_bus_recalc(symphony_haydn_t *dev)
{
    int clk_select = (dev->regs[0x02] >> 2) & 7;
    int clk_speed = 7153159;

    switch(clk_select)
    {
        case 0:
            clk_speed = cpu_busspeed / 3;
        break;

        case 1:
            clk_speed = cpu_busspeed / 4;
        break;

        case 2:
            clk_speed = cpu_busspeed / 5;
        break;

        case 4:
            clk_speed = cpu_busspeed / 6;
        break;

        case 6:
            clk_speed = cpu_busspeed / 2.5;
        break;
        
        case 7:
            clk_speed = cpu_busspeed / 2;
        break;
    }

    symphony_haydn_log("Symphony Haydn: Bus speed set to %d\n", clk_speed);

    cpu_set_isa_speed(clk_speed);
}

static void
symphony_haydn_mem_relocate(symphony_haydn_t *dev)
{
    int enable_256k = dev->regs[0x2d] & 0x80;

    if(enable_256k) {
        symphony_haydn_log("Symphony Haydn Memory Remap: 256KB of memory were remapped\n");
        mem_remap_top(256);
    }

}

static void
symphony_haydn_shadow_low(int seg, symphony_haydn_t *dev)
{
    uint32_t base = seg ? 0xd0000 : 0xc0000;
    int reg = seg ? 0x2f : 0x2e;
    uint16_t read, write;

    for(int i = 0; i < 8; i += 2) {
        read = (dev->regs[reg] & (1 << (i + 1))) ? MEM_READ_INTERNAL : MEM_READ_EXTANY;
        write = (dev->regs[reg] & (1 << i)) ? MEM_WRITE_INTERNAL : MEM_WRITE_EXTANY;
        symphony_haydn_log("Symphony Haydn Shadow: Segment 0x%x\n", base);
        mem_set_mem_state_both(base, 0x4000, read | write);
        base += 0x4000;
    }

    flushmmucache_nopc();
}

static void
symphony_haydn_shadow_high(int seg, symphony_haydn_t *dev)
{
    uint32_t base = seg ? 0xf0000 : 0xe0000;
    int reg = seg ? 0x31 : 0x30;
    uint16_t read, write;

    read = (dev->regs[reg] & 0x80) ? MEM_READ_INTERNAL : MEM_READ_EXTANY;
    write = (dev->regs[reg] & 0x40) ? MEM_WRITE_INTERNAL : MEM_WRITE_EXTANY;
    symphony_haydn_log("Symphony Haydn Shadow: Segment 0x%x\n", base);
    mem_set_mem_state_both(base, 0x4000, read | write);

    flushmmucache_nopc();
}

static void
symphony_haydn_write(uint16_t addr, uint8_t val, void *priv)
{
    symphony_haydn_t *dev = (symphony_haydn_t *) priv;

    if(!(addr & 1))
        dev->index = val;
    else {
        if((dev->index == 0x01) ||                             /* System Controller Range 00h-08h */
           (dev->index == 0x02) ||
           (dev->index == 0x06) ||
           (dev->index == 0x07) ||
           (dev->index == 0x08) ||
           ((dev->index >= 0x20) && (dev->index <= 0x33)) ||   /* Memory Controller Range 20h-33h */
           ((dev->index >= 0x40) && (dev->index <= 0x45)))     /* ????????????????? Range 40h-45h */
        dev->regs[dev->index] = val;

        switch(dev->index)
        {
            case 0x01:
                symphony_haydn_fasta20(dev);
            break;

            case 0x02:
                symphony_haydn_bus_recalc(dev);
            break;

            case 0x2d:
                symphony_haydn_mem_relocate(dev);
            break;

            case 0x2e:
            case 0x2f:
                symphony_haydn_shadow_low(dev->index & 1, dev);
            break;

            case 0x30:
            case 0x31:
                symphony_haydn_shadow_high(dev->index & 1, dev);
            break;
        }
    }
}


static uint8_t
symphony_haydn_read(uint16_t addr, void *priv)
{
    symphony_haydn_t *dev = (symphony_haydn_t *) priv;
    
    if(!(addr & 1))
        return dev->index;
    else if(addr & 1)
        if((dev->index > 0) && (dev->index <= 0x45))
            return dev->regs[dev->index];
    else
        return 0xff;
}


static void
symphony_haydn_reset(void *priv)
{
    symphony_haydn_t *dev = (symphony_haydn_t *) priv;
    memset(dev->regs, 0, sizeof(dev->regs)); /* Wash out the registers */
    dev->index = 0;

    symphony_haydn_fasta20(dev);
    symphony_haydn_bus_recalc(dev);
    symphony_haydn_shadow_low(0, dev);
    symphony_haydn_shadow_low(1, dev);
    symphony_haydn_shadow_high(0, dev);
    symphony_haydn_shadow_high(1, dev);
}


static void
symphony_haydn_close(void *priv)
{
    symphony_haydn_t *dev = (symphony_haydn_t *) priv;

    free(dev);
}


static void *
symphony_haydn_init(const device_t *info)
{
    symphony_haydn_t *dev = (symphony_haydn_t *) malloc(sizeof(symphony_haydn_t));
    memset(dev, 0, sizeof(symphony_haydn_t));

    /* Device */
    io_sethandler(0x00a8, 2, symphony_haydn_read, NULL, NULL, symphony_haydn_write, NULL, NULL, dev); /* Ports A8h-A9h: Symphony Haydn II */

    symphony_haydn_reset(dev);

    return dev;
}

const device_t symphony_haydn_device = {
    .name = "Symphony Haydn II",
    .internal_name = "symphony_haydn",
    .flags = 0,
    .local = 0,
    .init = symphony_haydn_init,
    .close = symphony_haydn_close,
    .reset = symphony_haydn_reset,
    { .available = NULL },
    .speed_changed = NULL,
    .force_redraw = NULL,
    .config = NULL
};
