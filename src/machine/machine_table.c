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

const machine_filter_t machine_types[] = {
    { "None",                      MACHINE_TYPE_NONE       },
    { "SiS 471",				   MACHINE_TYPE_SIS_471	   },
    { "Intel ICH2",				   MACHINE_TYPE_ICH2	   }
};

const machine_filter_t machine_chipsets[] = {
    { "None",                       MACHINE_CHIPSET_NONE                },
    { "SiS 471",                    MACHINE_CHIPSET_SIS_471             },
    { "Intel i815EP",               MACHINE_CHIPSET_INTEL_I815EP        }
};


const machine_t machines[] = {

    { "[SiS 471] AOpen Vi15G",                         "vi15g",         MACHINE_TYPE_SIS_471,       MACHINE_CHIPSET_SIS_471,             machine_at_vi15g_init,            0, 0, MACHINE_AVAILABLE, 0 , CPU_PKG_SOCKET3,                   CPU_BLOCK_NONE,        			            20000000, 50000000,  0, 0, 0, 0,                            MACHINE_AT,      MACHINE_FLAGS_NONE,  8192, 131072,  8192, 255, NULL, NULL },
    { "[SiS 471] ASUS VL/I-486SV2G",                   "486sv2g",       MACHINE_TYPE_SIS_471,       MACHINE_CHIPSET_SIS_471,             machine_at_486sv2g_init,          0, 0, MACHINE_AVAILABLE, 0 , CPU_PKG_SOCKET3,                   CPU_BLOCK_NONE,        			            20000000, 50000000,  0, 0, 0, 0,                            MACHINE_PS2_VLB, MACHINE_FLAGS_NONE,  8192,  65536,  8192, 255, NULL, NULL },

    { "[Intel i815E] Biostar M6TSL",                   "m6tsl",            MACHINE_TYPE_ICH2,       MACHINE_CHIPSET_INTEL_I815EP,        machine_at_m6tsl_init,            0, 0, MACHINE_AVAILABLE, 0 , CPU_PKG_SOCKET370,                 CPU_BLOCK(CPU_CYRIX3S),        			    66666667, 133333333, 1300, 3500, 1.5, 8.0, MACHINE_PS2_NOISA,   MACHINE_IDE_DUAL, 32768, 524288, 32768, 255, NULL, NULL },
    { "[Intel i815EP] Tyan Tomcat i815T",              "s2080",            MACHINE_TYPE_ICH2,       MACHINE_CHIPSET_INTEL_I815EP,        machine_at_s2080_init,            0, 0, MACHINE_AVAILABLE, 0 , CPU_PKG_SOCKET370,                 CPU_BLOCK(CPU_CYRIX3S),        			    66666667, 133333333, 1300, 3500, 1.5, 8.0, MACHINE_PS2_NOISA,   MACHINE_IDE_DUAL, 32768, 524288, 32768, 255, NULL, NULL },
    { NULL,                                            NULL,               MACHINE_TYPE_NONE,       MACHINE_CHIPSET_NONE,                NULL,                             0, 0, MACHINE_AVAILABLE, 0 , 0,                                 CPU_BLOCK_NONE,                              0, 0, 0, 0, 0, 0, MACHINE_BUS_NONE,  MACHINE_FLAGS_NONE, 0, 0, 0, 0, NULL, NULL }

};

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
    if (machines[m].get_device)
        return(machines[m].get_device());

    return(NULL);
}

const device_t *
machine_getviddevice(int m)
{
    if (machines[m].get_vid_device)
        return(machines[m].get_vid_device());

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
    return(machines[m].min_ram);
}

int
machine_get_max_ram(int m)
{
#if (!(defined __amd64__ || defined _M_X64 || defined __aarch64__ || defined _M_ARM64))
    return MIN(((int) machines[m].max_ram), 2097152);
#else
    return MIN(((int) machines[m].max_ram), 3145728);
#endif
}

int
machine_get_ram_granularity(int m)
{
    return(machines[m].ram_granularity);
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
