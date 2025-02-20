/*
 * Intel 82371FB PCI ISA IDE XCELERATOR
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

#include <86box/apm.h>
#include <86box/dma.h>
#include <86box/hdc.h>
#include <86box/hdc_ide.h>
#include <86box/hdc_ide_sff8038i.h>
#include <86box/mem.h>
#include <86box/pci.h>
#include <86box/port_92.h>
#include <86box/chipset.h>

#ifdef ENABLE_INTEL_PIIX_LOG
int intel_piix_do_log = ENABLE_INTEL_PIIX_LOG;
static void
intel_piix_log(const char *fmt, ...)
{
    va_list ap;

    if (intel_piix_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
        va_end(ap);
    }
}
#else
#    define intel_piix_log(fmt, ...)
#endif

typedef struct intel_piix_t {
    uint8_t pci_conf[2][256];
    int     apm_smi;

    apm_t      *apm;
    sff8038i_t *ide_drive[2];
} intel_piix_t;

/* PCI-to-ISA Bridge Configurations*/
static void
intel_piix_dma_alias(intel_piix_t *dev)
{
    int dma_alias_enable = dev->pci_conf[0][0x4c] & 0x80;

    intel_piix_log("Intel PIIX DMA: DMA Aliases were %s\n", dma_alias_enable ? "enabled" : "disabled");

    if (dma_alias_enable)
        dma_alias_set_piix();
    else
        dma_alias_remove_piix();
}

static int
intel_piix_irq_table(int val)
{
    val &= 0x0f;

    switch (val) {
        default: /* Reserved */
            return PCI_IRQ_DISABLED;

        case 3 ... 7: /* Valid */
        case 9 ... 12:
        case 14 ... 15:
            return val;
    }
}

static void
intel_piix_pirq(int addr, intel_piix_t *dev)
{
    int pirq   = (addr - 0x60) + 1;
    int enable = dev->pci_conf[0][addr] & 0x80;
    int irq    = intel_piix_irq_table(dev->pci_conf[0][addr]);

    if (enable) {
        if (irq != -1)
            intel_piix_log("Intel PIIX PIRQ: PIRQ%c was given IRQ %d\n", '@' + pirq, irq);
        else
            intel_piix_log("Intel PIIX PIRQ: PIRQ%c assigned to a reserved IRQ, Disabling\n", '@' + pirq);

        pci_set_irq_routing(pirq, irq);
    } else
        pci_set_irq_routing(pirq, PCI_IRQ_DISABLED);
}

static void
intel_piix_clock_divisor(intel_piix_t *dev)
{
    int divisor = 3 + (dev->pci_conf[0][0x6a] & 1);

    intel_piix_log("Intel PIIX Divisor: Divided by %d\n", divisor);
    cpu_set_isa_pci_div(divisor);
}

static void
intel_piix_mirq(int addr, intel_piix_t *dev)
{
    int mirq   = (addr - 0x70);
    int enable = dev->pci_conf[0][addr] & 0x80;
    int irq    = intel_piix_irq_table(dev->pci_conf[0][addr]);

    if (enable) {
        if (irq != -1)
            intel_piix_log("Intel PIIX MIRQ: MIRQ%d was given IRQ %d\n", mirq, irq);
        else
            intel_piix_log("Intel PIIX MIRQ: MIRQ%d assigned to a reserved IRQ, Disabling\n", mirq);

        pci_set_mirq_routing(mirq, irq);
    } else
        pci_set_mirq_routing(mirq, PCI_IRQ_DISABLED);
}

static void
intel_piix_apmc_smi_config(intel_piix_t *dev)
{
    int apm_enable = !!(dev->pci_conf[0][0xa2] & 0x80);

    intel_piix_log("Intel PIIX SMI: APMC SMI was %s\n", apm_enable ? "enabled" : "disabled");

    if (apm_enable)
        dev->apm_smi = 1;
    else
        dev->apm_smi = 0;
}

static void
intel_piix_apm_smi(uint16_t addr, uint8_t val, void *priv)
{
    intel_piix_t *dev = (intel_piix_t *) priv;

    if (dev->apm_smi) {
        intel_piix_log("Intel PIIX SMI: An APMC SMI was provoked!\n");
        smi_raise();

        dev->pci_conf[0][0xaa] |= 0x80; // Set the APMC SMI status
    }
}

/* IDE Configurations */
static void
intel_piix_ide_bm(intel_piix_t *dev)
{
    int      enable  = dev->pci_conf[1][4] & 1;
    uint16_t bm_addr = (dev->pci_conf[1][0x21] << 8) | (dev->pci_conf[1][0x20] & 0xf0);

    if (enable)
        intel_piix_log("Intel PIIX IDE BM: BM address updated to 0x%x\n", bm_addr);
    else
        intel_piix_log("Intel PIIX IDE BM: BM Disabled\n");

    sff_bus_master_handler(dev->ide_drive[0], enable, bm_addr);
    sff_bus_master_handler(dev->ide_drive[1], enable, bm_addr + 8);
}

static void
intel_piix_ide(intel_piix_t *dev)
{
    int enable     = dev->pci_conf[1][4] & 1;
    int pri_enable = dev->pci_conf[1][0x41] & 0x80;
    int sec_enable = dev->pci_conf[1][0x43] & 0x80;

    ide_sec_disable();
    ide_pri_disable();

    if (enable) {
        if (pri_enable) {
            ide_pri_enable();
            intel_piix_log("Intel PIIX IDE: Primary channel was enabled\n");
        }

        if (sec_enable) {
            ide_sec_enable();
            intel_piix_log("Intel PIIX IDE: Secondary channel was enabled\n");
        }
    } else
        intel_piix_log("Intel PIIX IDE: IDE was disabled entirely!");
}

static void
intel_piix_write(int func, int addr, uint8_t val, void *priv)
{
    intel_piix_t *dev = (intel_piix_t *) priv;

    if (func == 0) {
        intel_piix_log("Intel PIIX PCI-to-ISA: dev->regs[%02x] = %02x\n", addr, val);

        switch (addr) {
            case 0x04:
                dev->pci_conf[func][addr] = (val & 8) | 7;
                break;

            case 0x07:
                dev->pci_conf[func][addr] &= val & 0x38;
                break;

            case 0x4c:
                dev->pci_conf[func][addr] = val;
                intel_piix_dma_alias(dev);
                break;

            case 0x4e:
                dev->pci_conf[func][addr] = val & 0xf7;
                break;

            case 0x60 ... 0x63:
                dev->pci_conf[func][addr] = val & 0x8f;
                intel_piix_pirq(addr, dev);
                break;

            case 0x69:
                dev->pci_conf[func][addr] = val & 0xfa;
                break;

            case 0x6a:
                dev->pci_conf[func][addr] = val & 3;
                intel_piix_clock_divisor(dev);
                break;

            case 0x70 ... 0x71:
                dev->pci_conf[func][addr] = val & 0xcf;
                intel_piix_mirq(addr, dev);
                break;

            case 0x76 ... 0x77:
                dev->pci_conf[func][addr] = val & 0x8f;
                break;

            case 0x78 ... 0x79:
                dev->pci_conf[func][addr] = val;
                break;

            case 0xa0:
                dev->pci_conf[func][addr] = val & 0x1f;
                break;

            case 0xa2:
                dev->pci_conf[func][addr] = val;
                intel_piix_apmc_smi_config(dev);
                break;

            case 0xa4:
                dev->pci_conf[func][addr] = val & 0xf9;
                break;

            case 0xa5:
                dev->pci_conf[func][addr] = val;
                break;

            case 0xa7:
                dev->pci_conf[func][addr] = val & 0xe0;
                break;

            case 0xa8:
                dev->pci_conf[func][addr] = val;
                break;

            case 0xaa:
                dev->pci_conf[func][addr] |= val;
                break;

            case 0xae:
                dev->pci_conf[func][addr] = val;
                break;
        }

    } else if (func == 1) {
        intel_piix_log("Intel PIIX IDE: dev->regs[%02x] = %02x\n", addr, val);

        switch (addr) {
            case 0x04:
                dev->pci_conf[func][addr] = (val & 5) | 2;
                intel_piix_ide(dev);
                intel_piix_ide_bm(dev);
                break;

            case 0x07:
                dev->pci_conf[func][addr] &= val & 0x38;
                break;

            case 0x0d:
                dev->pci_conf[func][addr] = val & 0xf0;
                break;

            case 0x20 ... 0x21:
                dev->pci_conf[func][addr] = val;
                intel_piix_ide_bm(dev);
                break;

            case 0x40:
                dev->pci_conf[func][addr] = val;
                break;

            case 0x41:
                dev->pci_conf[func][addr] = val & 0xb3;
                intel_piix_ide(dev);
                break;

            case 0x42:
                dev->pci_conf[func][addr] = val;
                break;

            case 0x43:
                dev->pci_conf[func][addr] = val & 0xb3;
                intel_piix_ide(dev);
                break;
        }
    }
}

static uint8_t
intel_piix_read(int func, int addr, void *priv)
{
    intel_piix_t *dev = (intel_piix_t *) priv;

    if (func == 0) {
        intel_piix_log("Intel PIIX PCI-to-ISA: dev->regs[%02x] (%02x)\n", addr, dev->pci_conf[0][addr]);
        return dev->pci_conf[func][addr];
    } else if (func == 1) {
        intel_piix_log("Intel PIIX IDE: dev->regs[%02x] (%02x)\n", addr, dev->pci_conf[1][addr]);
        return dev->pci_conf[func][addr];
    } else
        return 0xff;
}

static void
intel_piix_reset(void *priv)
{
    intel_piix_t *dev = (intel_piix_t *) priv;
    memset(dev->pci_conf, 0x00, sizeof(dev->pci_conf)); /* Wash out the registers */

    /* PCI-to-ISA */
    dev->pci_conf[0][0x00] = 0x86; /* Intel */
    dev->pci_conf[0][0x01] = 0x80;

    dev->pci_conf[0][0x02] = 0x2e; /* PIIX */
    dev->pci_conf[0][0x03] = 0x12;

    dev->pci_conf[0][0x04] = 0x07;

    dev->pci_conf[0][0x07] = 0x02;

    dev->pci_conf[0][0x08] = 0x02;

    dev->pci_conf[0][0x0a] = 0x01;
    dev->pci_conf[0][0x0b] = 0x06;

    dev->pci_conf[0][0x0e] = 0x80;

    dev->pci_conf[0][0x4c] = 0x4d;
    dev->pci_conf[0][0x4e] = 0x03;

    dev->pci_conf[0][0x60] = 0x80;
    dev->pci_conf[0][0x61] = 0x80;
    dev->pci_conf[0][0x62] = 0x80;
    dev->pci_conf[0][0x63] = 0x80;

    dev->pci_conf[0][0x69] = 0x02;

    dev->pci_conf[0][0x70] = 0x80;
    dev->pci_conf[0][0x71] = 0x80;

    dev->pci_conf[0][0x76] = 0x0c;
    dev->pci_conf[0][0x77] = 0x0c;

    dev->pci_conf[0][0xa0] = 0x08;
    dev->pci_conf[0][0xa8] = 0x0f;

    intel_piix_dma_alias(dev);
    intel_piix_pirq(0x60, dev);
    intel_piix_pirq(0x61, dev);
    intel_piix_pirq(0x62, dev);
    intel_piix_pirq(0x63, dev);
    intel_piix_clock_divisor(dev);
    intel_piix_mirq(0x70, dev);
    intel_piix_mirq(0x71, dev);
    intel_piix_apmc_smi_config(dev);

    /* IDE */
    dev->pci_conf[1][0x00] = 0x86; /* Intel */
    dev->pci_conf[1][0x01] = 0x80;

    dev->pci_conf[1][0x02] = 0x30; /* PIIX IDE */
    dev->pci_conf[1][0x03] = 0x12;

    dev->pci_conf[1][0x06] = 0x80;
    dev->pci_conf[1][0x07] = 0x02;

    dev->pci_conf[1][0x08] = 0x02;

    dev->pci_conf[1][0x09] = 0x80;
    dev->pci_conf[1][0x0a] = 0x01;
    dev->pci_conf[1][0x0b] = 0x01;

    dev->pci_conf[1][0x20] = 0x01;

    sff_bus_master_reset(dev->ide_drive[0], 0);
    sff_bus_master_reset(dev->ide_drive[1], 0);
    intel_piix_ide_bm(dev);
    intel_piix_ide(dev);
}

static void
intel_piix_close(void *priv)
{
    intel_piix_t *dev = (intel_piix_t *) priv;

    free(dev);
}

static void *
intel_piix_init(const device_t *info)
{
    intel_piix_t *dev = (intel_piix_t *) malloc(sizeof(intel_piix_t));
    memset(dev, 0, sizeof(intel_piix_t));
    int slot;

    /* Device */
    slot = pci_add_card(PCI_ADD_SOUTHBRIDGE, intel_piix_read, intel_piix_write, dev); /* Device 7(Usually): Intel 82371FB PCI ISA IDE XCELERATOR */

    /* DMA */
    dma_alias_set_piix();

    /* APM */
    dev->apm = device_add(&apm_pci_device);
    io_sethandler(0x00b2, 1, NULL, NULL, NULL, intel_piix_apm_smi, NULL, NULL, dev); /* PIIX specific configuration for APMC SMI */

    /* MIRQ */
    pci_enable_mirq(0);
    pci_enable_mirq(1);

    /* Port 92h */
    device_add(&port_92_pci_device);

    /* SFF Compatible IDE */
    dev->ide_drive[0] = device_add_inst(&sff8038i_device, 1);
    dev->ide_drive[1] = device_add_inst(&sff8038i_device, 2);
    sff_set_slot(dev->ide_drive[0], slot);
    sff_set_slot(dev->ide_drive[1], slot);

    intel_piix_reset(dev);
    return dev;
}

const device_t intel_piix_device = {
    .name          = "Intel 82371FB PCI ISA IDE XCELERATOR",
    .internal_name = "intel_piix",
    .flags         = DEVICE_PCI,
    .local         = 0,
    .init          = intel_piix_init,
    .close         = intel_piix_close,
    .reset         = intel_piix_reset,
    { .available = NULL },
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = NULL
};
