/*
 * 86Box     A hypervisor and IBM PC system emulator that specializes in
 *           running old operating systems and software designed for IBM
 *           PC systems and compatibles from 1981 through fairly recent
 *           system designs based on the PCI bus.
 *
 *           This file is part of the 86Box distribution.
 *
 *           Handling of the emulated machines.
 *
 * Authors:  Sarah Walker, <http://pcem-emulator.co.uk/>
 *           Miran Grca, <mgrca8@gmail.com>
 *           Fred N. van Kempen, <decwiz@yahoo.com>
 *
 *           Copyright 2008-2020 Sarah Walker.
 *           Copyright 2016-2020 Miran Grca.
 *           Copyright 2017-2020 Fred N. van Kempen.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <86box/86box.h>
#include "cpu.h"
#include <86box/mem.h>
#include <86box/rom.h>
#include <86box/device.h>
#include <86box/machine.h>
#include <86box/keyboard.h>
#include <86box/sound.h>
#include <86box/video.h>

const machine_filter_t machine_types[] = {
    { "None",                                          MACHINE_TYPE_NONE               },
    { "SARC 2016A",                                    MACHINE_TYPE_SARC_2016A         },
    { "Symphony Haydn II",                             MACHINE_TYPE_SYMPHONY_HAYDN     },
    { "SiS 471",				                       MACHINE_TYPE_SIS_471	           },
    { "Intel PIIX",				                       MACHINE_TYPE_INTEL_PIIX  	   },
    { "Intel ICH2",				                       MACHINE_TYPE_ICH2	           }
};

const machine_filter_t machine_chipsets[] = {
    { "None",                                          MACHINE_CHIPSET_NONE            },
    { "SARC 2016A",                                    MACHINE_CHIPSET_SARC_2016A      },
    { "Symphony Haydn II",                             MACHINE_CHIPSET_SYMPHONY_HAYDN  },
    { "SiS 471",                                       MACHINE_CHIPSET_SIS_471         },
    { "Intel 430FX",                                   MACHINE_CHIPSET_INTEL_430FX     },
    { "Intel i815EP",                                  MACHINE_CHIPSET_INTEL_I815EP    }
};

const machine_t machines[] = {
    /* SARC 2016A Motherboards */
    {
        .name = "CX Technology SXD",
        .internal_name = "sxd",
        .type = MACHINE_TYPE_SARC_2016A,
        .chipset = MACHINE_CHIPSET_SARC_2016A,
        .init = machine_at_sxd_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_386SX,
            .block = CPU_BLOCK_NONE,
            .min_bus = 16666666,
            .max_bus = 40000000,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_AT,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 1024,
            .max = 16384,
            .step = 1024
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    {
        .name = "PCChips M396F",
        .internal_name = "m396f",
        .type = MACHINE_TYPE_SARC_2016A,
        .chipset = MACHINE_CHIPSET_SARC_2016A,
        .init = machine_at_m396f_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_386SX,
            .block = CPU_BLOCK_NONE,
            .min_bus = 16666666,
            .max_bus = 40000000,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_AT,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 1024,
            .max = 16384,
            .step = 1024
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    /* Symphony Haydn II Motherboards */
    {
        .name = "Commodore T486DX Tower",
        .internal_name = "t486dx",
        .type = MACHINE_TYPE_SYMPHONY_HAYDN,
        .chipset = MACHINE_CHIPSET_SYMPHONY_HAYDN,
        .init = machine_at_t486dx_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET1,
            .block = CPU_BLOCK_NONE,
            .min_bus = 20000000,
            .max_bus = 50000000,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_AT,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 1024,
            .max = 32768,
            .step = 1024
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    {
        .name = "FIC 386SC",
        .internal_name = "386sc_stock",
        .type = MACHINE_TYPE_SYMPHONY_HAYDN,
        .chipset = MACHINE_CHIPSET_SYMPHONY_HAYDN,
        .init = machine_at_386sc_stock_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET1,
            .block = CPU_BLOCK_NONE,
            .min_bus = 25555555,
            .max_bus = 50000000,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_AT,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 1024,
            .max = 32768,
            .step = 1024
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    {
        .name = "FIC 386SC with MR BIOS",
        .internal_name = "386sc_mr",
        .type = MACHINE_TYPE_SYMPHONY_HAYDN,
        .chipset = MACHINE_CHIPSET_SYMPHONY_HAYDN,
        .init = machine_at_386sc_mr_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET1,
            .block = CPU_BLOCK_NONE,
            .min_bus = 25555555,
            .max_bus = 50000000,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_AT,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 1024,
            .max = 32768,
            .step = 1024
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    {
        .name = "DTK PKM-0031Y",
        .internal_name = "pkm0031y",
        .type = MACHINE_TYPE_SYMPHONY_HAYDN,
        .chipset = MACHINE_CHIPSET_SYMPHONY_HAYDN,
        .init = machine_at_pkm0031y_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET1,
            .block = CPU_BLOCK_NONE,
            .min_bus = 16666666,
            .max_bus = 40000000,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_AT,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 1024,
            .max = 32768,
            .step = 1024
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    /* SiS 471 Motherboards */
    {
        .name = "AOpen Vi15G",
        .internal_name = "vi15g",
        .type = MACHINE_TYPE_SIS_471,
        .chipset = MACHINE_CHIPSET_SIS_471,
        .init = machine_at_vi15g_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET3,
            .block = CPU_BLOCK_NONE,
            .min_bus = 20000000,
            .max_bus = 50000000,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_PS2_VLB,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 1024,
            .max = 32768,
            .step = 1024
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    {
        .name = "ASUS VL/I-486SV2G",
        .internal_name = "486sv2g",
        .type = MACHINE_TYPE_SIS_471,
        .chipset = MACHINE_CHIPSET_SIS_471,
        .init = machine_at_486sv2g_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET3,
            .block = CPU_BLOCK_NONE,
            .min_bus = 20000000,
            .max_bus = 50000000,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_PS2_VLB,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 1024,
            .max = 32768,
            .step = 1024
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    /* Intel 430FX Motherboards */
    {
        .name = "ASUS PCI/I-P54TP4",
        .internal_name = "p54tp4",
        .type = MACHINE_TYPE_INTEL_PIIX,
        .chipset = MACHINE_CHIPSET_INTEL_430FX,
        .init = machine_at_p54tp4_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET5_7,
            .block = CPU_BLOCK_NONE,
            .min_bus = 50000000,
            .max_bus = 66666666,
            .min_voltage = 3300,
            .max_voltage = 3600,
            .min_multi = 1.5,
            .max_multi = 2.0
        },
        .bus_flags = MACHINE_PS2_PCI,
        .flags = MACHINE_IDE_DUAL,
        .ram = {
            .min = 4096,
            .max = 131072,
            .step = 4096
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    {
        .name = "AOpen AP5C",
        .internal_name = "ap5c",
        .type = MACHINE_TYPE_INTEL_PIIX,
        .chipset = MACHINE_CHIPSET_INTEL_430FX,
        .init = machine_at_ap5c_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET5_7,
            .block = CPU_BLOCK_NONE,
            .min_bus = 50000000,
            .max_bus = 66666666,
            .min_voltage = 3300,
            .max_voltage = 5000,
            .min_multi = 1.5,
            .max_multi = 2.0
        },
        .bus_flags = MACHINE_PS2_PCI,
        .flags = MACHINE_IDE_DUAL,
        .ram = {
            .min = 4096,
            .max = 131072,
            .step = 4096
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    {
        .name = "Acer V30",
        .internal_name = "acerv30",
        .type = MACHINE_TYPE_INTEL_PIIX,
        .chipset = MACHINE_CHIPSET_INTEL_430FX,
        .init = machine_at_acerv30_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET5_7,
            .block = CPU_BLOCK_NONE,
            .min_bus = 50000000,
            .max_bus = 66666666,
            .min_voltage = 3300,
            .max_voltage = 5000,
            .min_multi = 1.5,
            .max_multi = 2.5
        },
        .bus_flags = MACHINE_PS2_PCI,
        .flags = MACHINE_IDE_DUAL,
        .ram = {
            .min = 8096,
            .max = 131072,
            .step = 8096
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    {
        .name = "Micronics M54Hi",
        .internal_name = "m54hi",
        .type = MACHINE_TYPE_INTEL_PIIX,
        .chipset = MACHINE_CHIPSET_INTEL_430FX,
        .init = machine_at_m54hi_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET5_7,
            .block = CPU_BLOCK_NONE,
            .min_bus = 50000000,
            .max_bus = 66666666,
            .min_voltage = 3300,
            .max_voltage = 3600,
            .min_multi = 1.5,
            .max_multi = 2.0
        },
        .bus_flags = MACHINE_PS2_PCI,
        .flags = MACHINE_IDE_DUAL,
        .ram = {
            .min = 8096,
            .max = 131072,
            .step = 8096
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    /* Intel 815EP Motherboards */
    {
        .name = "Tyan Tomcat i815T",
        .internal_name = "s2080",
        .type = MACHINE_TYPE_ICH2,
        .chipset = MACHINE_CHIPSET_INTEL_I815EP,
        .init = machine_at_s2080_init,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = CPU_PKG_SOCKET370,
            .block = CPU_BLOCK(CPU_CYRIX3S),
            .min_bus = 66666667,
            .max_bus = 133333333,
            .min_voltage = 1300,
            .max_voltage = 3500,
            .min_multi = 1.5,
            .max_multi = 8.0
        },
        .bus_flags = MACHINE_PS2_NOISA,
        .flags = MACHINE_IDE_DUAL,
        .ram = {
            .min = 32768,
            .max = 524288,
            .step = 32768
        },
        .nvrmask = 255,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    },

    /* Reference */
    {
        .name = NULL,
        .internal_name = NULL,
        .type = MACHINE_TYPE_NONE,
        .chipset = MACHINE_CHIPSET_NONE,
        .init = NULL,
        .pad = 0,
        .pad0 = 0,
        .pad1 = MACHINE_AVAILABLE,
        .pad2 = 0,
        .cpu = {
            .package = 0,
            .block = CPU_BLOCK_NONE,
            .min_bus = 0,
            .max_bus = 0,
            .min_voltage = 0,
            .max_voltage = 0,
            .min_multi = 0,
            .max_multi = 0
        },
        .bus_flags = MACHINE_BUS_NONE,
        .flags = MACHINE_FLAGS_NONE,
        .ram = {
            .min = 0,
            .max = 0,
            .step = 0
        },
        .nvrmask = 0,
        .kbc = KBC_UNKNOWN,
        .kbc_p1 = 0,
        .gpio = 0,
        .device = NULL,
        .vid_device = NULL
    }
};

/* Saved copies - jumpers get applied to these.
   We use also machine_gpio to store IBM PC/XT jumpers as they need more than one byte. */
static uint16_t        machine_p1;
static uint32_t        machine_gpio;

uint8_t
machine_get_p1(void)
{
    return machine_p1;
}

void
machine_load_p1(int m)
{
    machine_p1 = machines[machine].kbc_p1;
}

uint32_t
machine_get_gpio(void)
{
    return machine_gpio;
}

void
machine_load_gpio(int m)
{
    machine_gpio = machines[machine].gpio;
}

void
machine_set_gpio(uint32_t gpio)
{
    machine_gpio = gpio;
}

int
machine_count(void)
{
    return((sizeof(machines) / sizeof(machine_t)) - 1);
}

char *
machine_getname(void)
{
    return((char *)machines[machine].name);
}

char *
machine_getname_ex(int m)
{
    return((char *)machines[m].name);
}

const device_t *
machine_getdevice(int m)
{
    if (machines[m].device)
        return(machines[m].device);

    return(NULL);
}

const device_t *
machine_getviddevice(int m)
{
    if (machines[m].vid_device)
        return(machines[m].vid_device);

    return(NULL);
}

char *
machine_get_internal_name(void)
{
    return((char *)machines[machine].internal_name);
}

char *
machine_get_internal_name_ex(int m)
{
    return((char *)machines[m].internal_name);
}

int
machine_get_nvrmask(int m)
{
    return(machines[m].nvrmask);
}

int
machine_has_flags(int m, int flags)
{
    return(machines[m].flags & flags);
}

int
machine_has_bus(int m, int bus_flags)
{
    return(machines[m].bus_flags & bus_flags);
}

int
machine_has_cartridge(int m)
{
    return(machine_has_bus(m, MACHINE_CARTRIDGE) ? 1 : 0);
}

int
machine_get_min_ram(int m)
{
    return(machines[m].ram.min);
}

int
machine_get_max_ram(int m)
{
#if (!(defined __amd64__ || defined _M_X64 || defined __aarch64__ || defined _M_ARM64))
    return MIN(((int) machines[m].ram.max), 2097152);
#else
    return MIN(((int) machines[m].ram.max), 3145728);
#endif
}

int
machine_get_ram_granularity(int m)
{
    return(machines[m].ram.step);
}

int
machine_get_type(int m)
{
    return(machines[m].type);
}

int
machine_get_machine_from_internal_name(char *s)
{
    int c = 0;

    while (machines[c].init != NULL) {
        if (!strcmp(machines[c].internal_name, (const char *)s))
            return(c);
        c++;
    }

    return(0);
}
