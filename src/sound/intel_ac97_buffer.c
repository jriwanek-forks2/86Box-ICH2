/*
 * Intel AC'97 Buffer
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
#include <86box/intel_ac97.h>

#ifdef ENABLE_INTEL_AC97_LOG
int intel_ac97_buffer_do_log = ENABLE_INTEL_AC97_LOG;
static void
intel_ac97_buffer_log(const char *fmt, ...)
{
    va_list ap;

    if (intel_ac97_buffer_do_log) {
	va_start(ap, fmt);
	pclog_ex(fmt, ap);
	va_end(ap);
    }
}
#else
#define intel_ac97_buffer_log(fmt, ...)
#endif

static void
intel_ac97_buffer_write(uint16_t addr, uint8_t val, void *priv)
{
    intel_ac97_buffer_t *dev = (intel_ac97_buffer_t *) priv;

}


static uint8_t
intel_ac97_buffer_read(uint16_t addr, void *priv)
{
    intel_ac97_buffer_t *dev = (intel_ac97_buffer_t *) priv;
    return 0xff;
}


static void
intel_ac97_buffer_reset(void *priv)
{
    intel_ac97_buffer_t *dev = (intel_ac97_buffer_t *) priv;
    memset(dev->regs, 0, sizeof(dev->regs)); /* Wash out the registers */

}


static void
intel_ac97_buffer_close(void *priv)
{
    intel_ac97_buffer_t *dev = (intel_ac97_buffer_t *) priv;

    free(dev);
}


static void *
intel_ac97_buffer_init(const device_t *info)
{
    intel_ac97_buffer_t *dev = (intel_ac97_buffer_t *) malloc(sizeof(intel_ac97_buffer_t));
    memset(dev, 0, sizeof(intel_ac97_buffer_t));

    return dev;
}

const device_t intel_ac97_buffer_device = {
    .name = "Intel AC'97 Buffer",
    .internal_name = "intel_ac97_buffer",
    .flags = 0,
    .local = 0,
    .init = intel_ac97_buffer_init,
    .close = intel_ac97_buffer_close,
    .reset = intel_ac97_buffer_reset,
    { .available = NULL },
    .speed_changed = NULL,
    .force_redraw = NULL,
    .config = NULL
};
