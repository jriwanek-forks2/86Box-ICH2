/*
 * SMC FDC37C665(SMC665)
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
#include <86box/io.h>
#include <86box/timer.h>
#include <86box/device.h>

#include <86box/lpt.h>
#include <86box/serial.h>
#include <86box/fdd.h>
#include <86box/fdc.h>
#include <86box/fdd_common.h>

#include <86box/sio.h>

typedef struct
{
    fdc_t *fdc;
    serial_t *uart[2];

    uint8_t index, regs[16];
    uint32_t com3_addr, com4_addr;
    int config_enable;
} smc665_t;

#ifdef ENABLE_SMC665_LOG
int smc665_do_log = ENABLE_SMC665_LOG;
void
smc665_log(const char *fmt, ...)
{
    va_list ap;

    if (smc665_do_log) {
	va_start(ap, fmt);
	pclog_ex(fmt, ap);
	va_end(ap);
    }
}
#else
#define smc665_log(fmt, ...)
#endif


static void
smc665_unlock(smc665_t *dev)
{
    if(dev->index & 1)
        dev->config_enable = 1;
    else
        dev->config_enable = 0;

    smc665_log("SMC 665 Config: Configuration registers were %s\n", dev->config_enable ? "enabled" : "disabled");
}

static void
smc665_fdc(smc665_t *dev)
{
    uint16_t fdc_base = (dev->regs[0x05] & 1) ? 0x370 : 0x3f0;
    fdc_remove(dev->fdc);

    if(dev->regs[0] & 0x10) {
        fdc_set_base(dev->fdc, fdc_base);
        smc665_log("SMC 665 FDC: Floppy Drive was enabled\n");
    }
}

static void
smc665_com34(smc665_t *dev)
{
    switch((dev->regs[1] >> 5) & 3)
    {
        case 0:
            dev->com3_addr = 0x338;
            dev->com4_addr = 0x238;
        break;

        case 1:
            dev->com3_addr = 0x3e8;
            dev->com4_addr = 0x2e8;
        break;

        case 2:
            dev->com3_addr = 0x2e8;
            dev->com4_addr = 0x2e0;
        break;

        case 3:
            dev->com3_addr = 0x220;
            dev->com4_addr = 0x228;
        break;
    }
}

static void
smc665_lpt(smc665_t *dev)
{
    uint32_t base = 0;
    int irq;

    lpt1_remove();

    switch(dev->regs[1] & 3)
    {
        case 1:
            base = 0x3bc;
            irq = 7;
        break;

        case 2:
            base = 0x378;
            irq = 7;
        break;

        case 3:
            base = 0x278;
            irq = 5;
        break;
    }

    if(base != 0) {
        lpt1_init(base);
        lpt1_irq(irq);
        smc665_log("SMC 665 LPT: LPT initialized at address 0x%x with IRQ %d\n", base, irq);
    }
}

static void
smc665_uart(int uart, smc665_t *dev)
{
    uint8_t uart_config = dev->regs[2] >> (uart * 4);
    uint32_t base = 0;
    int irq = 0;
    serial_remove(dev->uart[uart]);

    if(uart_config & 4) {
        switch(uart_config & 3)
        {
            case 0:
                base = 0x3f8;
                irq = 4;
            break;

            case 1:
                base = 0x2f8;
                irq = 3;
            break;

            case 2:
                base = dev->com3_addr;
                irq = 4;
            break;

            case 3:
                base = dev->com4_addr;
                irq = 3;
            break;
        }

        serial_setup(dev->uart[uart], base, irq);
        smc665_log("SMC 665 UART %c: Serial initialized at address 0x%x\n", 'A' + uart, base);
    }
}

static void
smc665_fdc_feature(smc665_t *dev)
{
    fdc_update_densel_force(dev->fdc, (dev->regs[5] >> 3) & 3);
    fdc_update_boot_drive(dev->fdc, dev->regs[7] & 3);
    fdc_update_densel_polarity(dev->fdc, (dev->regs[7] >> 2) & 3);
    fdc_set_swap(dev->fdc, !!(dev->regs[1] & 0x20));
    fdc_update_enh_mode(dev->fdc, !!(dev->regs[2] & 2));
}

static void
smc665_write(uint16_t addr, uint8_t val, void *priv)
{
    smc665_t *dev = (smc665_t *)priv;

    if((addr == 0x3f1) && dev->config_enable) {
        switch(dev->index)
        {
            case 0x00:
                dev->regs[dev->index] = val;
                smc665_fdc(dev);
                smc665_fdc_feature(dev);
            break;

            case 0x01:
                dev->regs[dev->index] = val;
                smc665_com34(dev);
                smc665_lpt(dev);
            break;

            case 0x02:
                dev->regs[dev->index] = val;
                smc665_uart(0, dev);
                smc665_uart(1, dev);
                smc665_fdc_feature(dev);
            break;

            case 0x03 ... 0x04:
                dev->regs[dev->index] = val;
            break;

            case 0x05:
                dev->regs[dev->index] = val & 0x7f;
                smc665_fdc_feature(dev);
            break;

            case 0x07:
                dev->regs[dev->index] = val;
                smc665_fdc_feature(dev);
            break;

            case 0x06:
            case 0x08 ... 0x0a:
            case 0x0f:
                 dev->regs[dev->index] = val;
            break;
        }
    }
    else {
        dev->index = val;

        if((dev->index == 0x55) || (dev->index == 0xaa))
            smc665_unlock(dev);
    }
}


static uint8_t
smc665_read(uint16_t addr, void *priv)
{
    smc665_t *dev = (smc665_t *)priv;

}


static void
smc665_reset(void *priv)
{
    smc665_t *dev = (smc665_t *)priv;

    memset(dev->regs, 0, sizeof(dev->regs));
    dev->config_enable = 0;
    dev->com3_addr = 0;
    dev->com4_addr = 0;

    dev->regs[0x00] = 0x3b;
    dev->regs[0x01] = 0x9f;
    dev->regs[0x02] = 0xdc;
    dev->regs[0x03] = 0x78;
    dev->regs[0x06] = 0xff;
    dev->regs[0x0d] = 0x65; /* SMC variation. We got 665 so 65h. */

    fdc_reset(dev->fdc);
    smc665_fdc(dev);
    smc665_com34(dev);
    smc665_lpt(dev);
    smc665_uart(0, dev);
    smc665_uart(1, dev);
    smc665_fdc_feature(dev);
}


static void
smc665_close(void *priv)
{
    smc665_t *dev = (smc665_t *)priv;

    free(dev);
}


static void *
smc665_init(const device_t *info)
{
    smc665_t *dev = (smc665_t *)malloc(sizeof(smc665_t));
    memset(dev, 0, sizeof(smc665_t));

    io_sethandler(0x3f0, 2, smc665_read, NULL, NULL, smc665_write, NULL, NULL, dev); /* Ports 3F0h-3F1h: SMC 665 */

    /* FDC */
    dev->fdc = device_add(&fdc_at_smc_device);

    /* Serial */
    dev->uart[0] = device_add_inst(&ns16550_device, 1);
    dev->uart[1] = device_add_inst(&ns16550_device, 2);

    smc665_reset(dev);

    return dev;
}

const device_t smc665_device = {
    .name = "SMC FDC37C665(SMC 665)",
    .internal_name = "smc665",
    .flags = 0,
    .local = 0x2e,
    .init = smc665_init,
    .close = smc665_close,
    .reset = smc665_reset,
    { .available = NULL },
    .speed_changed = NULL,
    .force_redraw = NULL,
    .config = NULL
};
