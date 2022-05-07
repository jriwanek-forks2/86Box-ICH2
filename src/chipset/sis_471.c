/*
 * SiS 471 ISA/VESA Chipset
 *
 * Authors:	Tiseno100,
 *
 * Copyright 2022 Tiseno100.
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
#include <86box/pic.h>
#include <86box/port_92.h>
#include <86box/smram.h>
#include <86box/chipset.h>

typedef struct
{
    int index;
    uint8_t regs[39];

    port_92_t *port_92;
    smram_t *smram;
    int clear_smi;
} sis_471_t;

#ifdef ENABLE_SIS_471_LOG
int sis_471_do_log = ENABLE_SIS_471_LOG;
static void
sis_471_log(const char *fmt, ...)
{
    va_list ap;

    if (sis_471_do_log) {
	va_start(ap, fmt);
	pclog_ex(fmt, ap);
	va_end(ap);
    }
}
#else
#define sis_471_log(fmt, ...)
#endif

static void
sis_471_cache(sis_471_t *dev)
{
    if(dev->regs[0x01] & 0x80) {
        sis_471_log("SiS 471 Cache: Cache was Enabled\n");
        cpu_cache_int_enabled = 1;
        cpu_cache_ext_enabled = 1;
        dev->regs[0x01] &= 0x70; /* Setup Hacked Size */
        dev->regs[0x01] |= 0x40;
    }
    else {
        sis_471_log("SiS 471 Cache: Cache was Disabled\n");
        cpu_cache_int_enabled = 0;
        cpu_cache_ext_enabled = 0;
    }

    cpu_update_waitstates();
}

static void
sis_471_shadow(sis_471_t *dev)
{
    uint16_t shadow_profile;

    shadow_profile = (dev->regs[0x02] & 0x80) ? MEM_READ_INTERNAL : MEM_READ_EXTANY;
    shadow_profile |= (dev->regs[0x02] & 0x40) ? MEM_WRITE_EXTANY : MEM_WRITE_INTERNAL;

    sis_471_log("SiS 471 Shadow: Shadow RAM was %s\n", (dev->regs[0x02] & 0x80) ? "Enabled" : "Disabled");

    mem_set_mem_state_both(0xf0000, 0x10000, shadow_profile);

    for(int i = 0; i < 6; i++)
        mem_set_mem_state_both(0xc0000 + (i * 0x8000), 0x8000, ((dev->regs[2] >> i) & 1) ? shadow_profile : (MEM_READ_EXTANY | MEM_WRITE_EXTANY));
     
}

static void
sis_471_port_92_feature(sis_471_t *dev)
{
    int period = (dev->regs[0x07] & 8) ? 6 : 2;

    sis_471_log("SiS 471 Port 92: Features GATEA20 %s and FASTRESET %s with Period %dus\n", !!(dev->regs[0x07] & 0x10) ? "Enabled" : "Disabled", !!(dev->regs[0x07] & 2) ? "Enabled" : "Disabled", period);
    port_92_set_features(dev->port_92, !!(dev->regs[0x07] & 0x10), !!(dev->regs[0x07] & 2));
    port_92_set_period(dev->port_92, period * TIMER_USEC);
}

static void
sis_471_dram_populate(sis_471_t *dev)
{
    uint32_t size = mem_size >> 10;
    sis_471_log("SiS 471 DRAM: %d Megabytes were detected. Populating appropriately\n", size);

    switch(size)
    {
        default:
            pclog("SiS 471 DRAM: Illegal size! Change memory size. Not populating\n");
        break;

        case 8:
            dev->regs[0x09] |= 0x18;
        break;

        case 16:
            dev->regs[0x09] |= 0x1c;
        break;

        case 24:
            dev->regs[0x09] |= 0x1a;
        break;

        case 32:
            dev->regs[0x09] |= 0x3c;
        break;

        case 40:
            dev->regs[0x09] |= 0x3b;
        break;

        case 48:
            dev->regs[0x09] |= 0x3e;
        break;

        case 64:
            dev->regs[0x09] |= 0x33;
        break;

        case 72:
            dev->regs[0x09] |= 0x2d;
        break;

        case 80:
            dev->regs[0x09] |= 0x3f;
        break;

        case 96:
            dev->regs[0x09] |= 0x34;
        break;

        case 128:
            dev->regs[0x09] |= 0x35;
        break;
    }
}

static void
sis_471_relocation(sis_471_t *dev)
{
    int mem_size_kb = mem_size >> 10;

    if((mem_size_kb >= 1) && (mem_size_kb <= 8) && !!(dev->regs[0x0b] & 2) && (((dev->regs[0x02] >> 2) & 0x0f) == 0)) { /* It also can relocate D0000-EFFFF(Additional 128KB) but it's not implemented due to 86Box limitations */
        sis_471_log("SiS 471 Relocate: 256KB from the A0000-BFFFF section were relocated\n")
        mem_remap_top(128);
    }
}

static void
sis_471_bus_speed(sis_471_t *dev)
{
    int bus_speed;

    switch(dev->regs[0x10] >> 5)
    {
        default:
            bus_speed = 7159153;
        break;
        case 1:
            bus_speed = cpu_busspeed / 10;
        break;
        case 2:
            bus_speed = cpu_busspeed / 8;
        break;
        case 3:
            bus_speed = cpu_busspeed / 6;
        break;
        case 4:
            bus_speed = cpu_busspeed / 5;
        break;
        case 5:
            bus_speed = cpu_busspeed / 4;
        break;
        case 6:
            bus_speed = cpu_busspeed / 3;
        break;
        case 7:
            bus_speed = cpu_busspeed / 2;
        break;
    }

    sis_471_log("SiS 471 Bus: The Bus speed was updated at clock %d\n", bus_speed);
}

static void
sis_471_smram(sis_471_t *dev)
{
    smram_disable(dev->smram);

    uint32_t proper, remapped, size;
    int local_access = !!(dev->regs[0x13] & 0x10);

    switch(dev->regs[0x13] >> 5)
    {
        case 0:
        default:
            proper = 0xe0000;
            remapped = 0xa0000;
            size = 0x10000;
        break;

        case 1:
            proper = 0xe0000;
            remapped = 0xa0000;
            size = 0x10000;
        break;

        case 2:
            proper = 0xe0000;
            remapped = 0xe0000;
            size = 0x10000;
        break;

        case 4:
            proper = 0x60000;
            remapped = 0xa0000;
            size = 0x10000;
        break;

        case 5:
            proper = 0x60000;
            remapped = 0xa0000;
            size = 0x10000;
        break;
    }

    sis_471_log("SiS 471 SMRAM: Configured at Proper: 0x%x Remap: 0x%x\n", proper, remapped);
    smram_enable(dev->smram, proper, remapped, size, local_access, 1);
}

static void
sis_471_sw_smi_trigger(uint16_t addr, uint8_t val, void *priv)
{
    sis_471_t *dev = (sis_471_t *) priv;
    int pin = (dev->regs[0x0b] & 8) ? 15 : 12;

    sis_471_log("SiS 471 SMI Trap: Trap was triggered\n");

    if(dev->regs[0x0b] & 0x80) {
        if(!!(dev->regs[0x18] & 2) && !dev->clear_smi) { /* Go by SMI */
            if(dev->regs[0x0b] & 0x10) {
                smi_line = 1;
                sis_471_log("SiS 471 SMI Trap: Provoking an SMI\n");
            }
            else if(!dev->clear_smi) { /* Go by IRQ */
                picint(1 << pin);
                sis_471_log("SiS 471 SMI Trap: Provoking an IRQ at line %d\n", pin);
            }
        }
    }

    dev->regs[0x19] |= 1; /* Write to the SMI/IRQ Status Register */
    dev->clear_smi = 1; /* Disallow any new SMI or IRQ triggers unless the SMI Clear register is written */
}

static void
sis_471_sw_smi_set_base(sis_471_t *dev)
{
    uint16_t base = (dev->regs[0x15] << 8) | dev->regs[0x14];
    
    if(base != 0)
        sis_471_log("SiS 471 SMI Base: A New Base was given 0x%x\n", base);

    io_handler(base != 0, base, 1, NULL, NULL, NULL, sis_471_sw_smi_trigger, NULL, NULL, dev);
}

static void
sis_471_port_92_handler(sis_471_t *dev)
{
    sis_471_log("SiS 471 Port 92: The Port 92h is %s\n", (dev->regs[0x22] & 1) ? "Enabled" : "Disabled");

    if(dev->regs[0x22] & 1)
        port_92_add(dev->port_92);
    else
        port_92_remove(dev->port_92);
}

static void
sis_471_write(uint16_t addr, uint8_t val, void *priv)
{
    sis_471_t *dev = (sis_471_t *) priv;

    if(addr == 0x22)
        dev->index = val;
    else {
        dev->index -= 0x50; /* Register minus 0x50 */

        /* Note: You can write normally on the reserved registers so we don't really bother masking them. We only do sanity control for RO & RWC Registers. */

        if(dev->index == 0x19) /* Status Registers */
            dev->regs[dev->index] &= val;
        else if(dev->index == 0x26)
            dev->regs[dev->index] = val | 1; /* There's a read only register here that defines if the Keyboard Interface is enabled. We always say one there. */
        else if(dev->index < 39) /* Rest of Writable Registers with sanity check */
            dev->regs[dev->index] = val;

        switch(dev->index)
        {
            case 0x01:  /* Cache Handler */
                sis_471_cache(dev);
            break;

            case 0x02: /* Shadow RAM Handler */
                sis_471_shadow(dev);
            break;

            case 0x07: /* GATEA20/FAST Handler */
                sis_471_port_92_feature(dev);
            break;

            case 0x09: /* DRAM Population Handler */
                dev->regs[dev->index] &= 0x3f;
                sis_471_dram_populate(dev);
            break;

            case 0x0b: /* 256KB Relocation Handler */
                sis_471_relocation(dev);
            break;

            case 0x10: /* Bus Speed Handler */
                sis_471_bus_speed(dev);
            break;

            case 0x13: /* SMRAM Handler */
                sis_471_smram(dev);
            break;

            case 0x14 ... 0x15: /* SMI/IRQ Trap Base Handler */
                sis_471_sw_smi_set_base(dev);
            break;

            case 0x1b: /* SMI Clear Handler */
                dev->clear_smi = 0; /* Clears the flag so another SMI/IRQ request can be provoked */
            break;

            case 0x22: /* Port 92h Enable/Disable Handler */
                sis_471_port_92_handler(dev);
            break;
        }

        dev->index = 0; /* Reset the Index Register */
    }
}


static uint8_t
sis_471_read(uint16_t addr, void *priv)
{
    sis_471_t *dev = (sis_471_t *) priv;

    if(addr == 0x22)
        return dev->index;
    else {
        dev->index -= 0x50;

        if(dev->index < 39) {
            if(dev->index == 0x1c) /* Reads to this register return the NMI Mask Index port value at 70h */
                return inb(0x70);
            else    
                return dev->regs[dev->index];
        }
        else
            return 0xff;

        dev->index = 0; /* Reset the Index Register */
    }
}


static void
sis_471_reset(void *priv)
{
    sis_471_t *dev = (sis_471_t *) priv;

    dev->clear_smi = 0;
    dev->regs[0x11] = 0x09;
    dev->regs[0x26] = 0x01;

    /* Execute & configure all handlers */
    sis_471_cache(dev);
    sis_471_shadow(dev);
    sis_471_port_92_feature(dev);
    sis_471_dram_populate(dev);
    sis_471_relocation(dev);
    sis_471_bus_speed(dev);
    sis_471_smram(dev);
    sis_471_sw_smi_set_base(dev);
    sis_471_port_92_handler(dev);
}


static void
sis_471_close(void *priv)
{
    sis_471_t *dev = (sis_471_t *) priv;

    smram_del(dev->smram);
    free(dev);
}


static void *
sis_471_init(const device_t *info)
{
    sis_471_t *dev = (sis_471_t *) malloc(sizeof(sis_471_t));
    memset(dev, 0, sizeof(sis_471_t));

    io_sethandler(0x0022, 2, sis_471_read, NULL, NULL, sis_471_write, NULL, NULL, dev);

    dev->port_92 = device_add(&port_92_device);

    dev->smram = smram_add();

    sis_471_reset(dev);

    return dev;
}

const device_t sis_471_device = {
    .name = "SiS 471 ISA/VESA Chipset",
    .internal_name = "sis_471",
    .flags = 0,
    .local = 0,
    .init = sis_471_init,
    .close = sis_471_close,
    .reset = sis_471_reset,
    { .available = NULL },
    .speed_changed = NULL,
    .force_redraw = NULL,
    .config = NULL
};
