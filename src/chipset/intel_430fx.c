/*
 *          Intel 82430FX PCIset
 *
 * Authors: Tiseno100,
 *
 *          Copyright 2022 Tiseno100.
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
#include "x86.h"
#include <86box/timer.h>
#include <86box/io.h>
#include <86box/device.h>

#include <86box/mem.h>
#include <86box/pci.h>
#include <86box/smram.h>
#include <86box/spd.h>
#include <86box/chipset.h>

#ifdef ENABLE_INTEL_430FX_LOG
int intel_430fx_do_log = ENABLE_INTEL_430FX_LOG;
static void
intel_430fx_log(const char *fmt, ...)
{
    va_list ap;

    if (intel_430fx_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
        va_end(ap);
    }
}
#else
#    define intel_430fx_log(fmt, ...)
#endif

typedef struct intel_430fx_t {
    uint8_t pci_conf[256];

    smram_t *smram;
} intel_430fx_t;

static void
intel_430fx_cache(intel_430fx_t *dev)
{
    int cache_enable;
    dev->pci_conf[0x52] &= 0x3b;
    cpu_cache_ext_enabled = 0;

    cache_enable = (dev->pci_conf[0x52] & 3) == 3;

    if (cache_enable) {
        cpu_cache_ext_enabled = 1;
        dev->pci_conf[0x52] |= 0x40;
    }

    intel_430fx_log("Intel 430FX: Cache was %s\n", cache_enable ? "enabled" : "disabled");

    cpu_update_waitstates();
}

static void
intel_430fx_dram_hole(intel_430fx_t *dev)
{
    int dram_hole_enable = ((dev->pci_conf[0x57] >> 6) == 1);

    intel_430fx_log("Intel 430FX DRAM Hole: DRAM Hole was %s\n", dram_hole_enable ? "enabled" : "disabled");

    if (dram_hole_enable)
        mem_set_mem_state_both(0x80000, 0x20000, MEM_READ_EXTANY | MEM_WRITE_EXTANY);
    else
        mem_set_mem_state_both(0x80000, 0x20000, MEM_READ_INTERNAL | MEM_WRITE_INTERNAL);

    flushmmucache_nopc();
}

static void
intel_pam_recalc(int addr, uint8_t val)
{
    int region = 0xc0000 + ((addr - 0x5a) << 15);

    if (addr == 0x59)
        mem_set_mem_state_both(0xf0000, 0x10000, ((val & 0x10) ? MEM_READ_INTERNAL : MEM_READ_EXTANY) | ((val & 0x20) ? MEM_WRITE_INTERNAL : MEM_WRITE_EXTANY));
    else {
        mem_set_mem_state_both(region, 0x4000, ((val & 0x01) ? MEM_READ_INTERNAL : MEM_READ_EXTANY) | ((val & 0x02) ? MEM_WRITE_INTERNAL : MEM_WRITE_EXTANY));
        mem_set_mem_state_both(region + 0x4000, 0x4000, ((val & 0x10) ? MEM_READ_INTERNAL : MEM_READ_EXTANY) | ((val & 0x20) ? MEM_WRITE_INTERNAL : MEM_WRITE_EXTANY));
    }

    flushmmucache_nopc();
}

static void
intel_430fx_smram(intel_430fx_t *dev)
{
    if (dev->pci_conf[0x72] & 0x10)
        dev->pci_conf[0x72] &= 0x2f;

    int local_access  = !!(dev->pci_conf[0x72] & 0x40);
    int space_closed  = !!(dev->pci_conf[0x72] & 0x20);
    int smram_enabled = !!(dev->pci_conf[0x72] & 8);
    smram_disable(dev->smram);

    intel_430fx_log("Intel 430FX SMRAM: SMRAM was %s\n", smram_enabled ? "enabled" : "disabled");

    if (smram_enabled) {
        if (local_access && !space_closed) /* Mutually XOR'ed */
            smram_enable(dev->smram, 0x000a0000, 0x000a0000, 0x10000, 1, 1);
        else
            smram_enable(dev->smram, 0x000a0000, 0x000a0000, 0x10000, 0, 1);
    }

    flushmmucache();
}

static void
intel_430fx_write(int func, int addr, uint8_t val, void *priv)
{
    intel_430fx_t *dev = (intel_430fx_t *) priv;

    intel_430fx_log("Intel 430FX: dev->regs[%02x] = %02x\n", addr, val);

    if (func)
        return;

    switch (addr) {
        case 0x04:
            dev->pci_conf[addr] = (val & 2) | 4;
            break;

        case 0x07:
            dev->pci_conf[addr] &= val & 3;
            break;

        case 0x0d:
            dev->pci_conf[addr] = val & 0xf8;
            break;

        case 0x0f:
            dev->pci_conf[addr] = val & 0x40;
            break;

        case 0x50:
            dev->pci_conf[addr] = val & 0xef;
            break;

        case 0x52:
            dev->pci_conf[addr] = val & 0xfb;
            intel_430fx_cache(dev);
            break;

        case 0x57:
            dev->pci_conf[addr] = val & 0xcf;
            intel_430fx_dram_hole(dev);
            break;

        case 0x58:
            dev->pci_conf[addr] = val;
            break;

        case 0x59 ... 0x5f:
            dev->pci_conf[addr] = val & 0x77;
            intel_pam_recalc(addr, val);
            break;

        case 0x60 ... 0x64:
            spd_write_drbs(dev->pci_conf, 0x60, 0x64, 4);
            break;

        case 0x72:
            if (dev->pci_conf[0x72] & 0x10)
                dev->pci_conf[addr] = val & 0x2f;
            else
                dev->pci_conf[addr] = val & 0x7f;

            intel_430fx_smram(dev);
            break;
    }
}

static uint8_t
intel_430fx_read(int func, int addr, void *priv)
{
    intel_430fx_t *dev = (intel_430fx_t *) priv;

    intel_430fx_log("Intel 430FX: dev->regs[%02x] (%02x)\n", addr, dev->pci_conf[addr]);

    if (func)
        return 0xff;
    else
        return dev->pci_conf[addr];
}

static void
intel_430fx_reset(void *priv)
{
    intel_430fx_t *dev = (intel_430fx_t *) priv;
    memset(dev->pci_conf, 0x00, sizeof(dev->pci_conf)); /* Wash out the registers */

    dev->pci_conf[0x00] = 0x86; /* Intel */
    dev->pci_conf[0x01] = 0x80;

    dev->pci_conf[0x02] = 0x2D; /* 82430FX */
    dev->pci_conf[0x03] = 0x12;

    dev->pci_conf[0x04] = 0x06;

    dev->pci_conf[0x07] = 0x02;

    dev->pci_conf[0x08] = 0x02;
    dev->pci_conf[0x0b] = 0x06;

    dev->pci_conf[0x52] = 0x10;
    dev->pci_conf[0x57] = 0x01;

    dev->pci_conf[0x60] = 0x02;
    dev->pci_conf[0x61] = 0x02;
    dev->pci_conf[0x62] = 0x02;
    dev->pci_conf[0x63] = 0x02;
    dev->pci_conf[0x64] = 0x02;

    dev->pci_conf[0x72] = 0x02;

    intel_430fx_cache(dev);

    for (int i = 0x59; i <= 0x5f; i++) /* Reset PAM to defaults */
        intel_pam_recalc(i, 0);
}

static void
intel_430fx_close(void *priv)
{
    intel_430fx_t *dev = (intel_430fx_t *) priv;

    smram_del(dev->smram);
    free(dev);
}

static void *
intel_430fx_init(const device_t *info)
{
    intel_430fx_t *dev = (intel_430fx_t *) malloc(sizeof(intel_430fx_t));
    memset(dev, 0, sizeof(intel_430fx_t));

    /* Device */
    pci_add_card(PCI_ADD_NORTHBRIDGE, intel_430fx_read, intel_430fx_write, dev); /* Device 0: Intel 82430FX PCIset */

    /* SMRAM Segments */
    dev->smram = smram_add();

    intel_430fx_reset(dev);
    return dev;
}

const device_t intel_430fx_device = {
    .name          = "Intel 82430FX PCIset",
    .internal_name = "intel_430fx",
    .flags         = DEVICE_PCI,
    .local         = 0,
    .init          = intel_430fx_init,
    .close         = intel_430fx_close,
    .reset         = intel_430fx_reset,
    { .available = NULL },
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = NULL
};
