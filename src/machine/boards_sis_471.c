/*
 * SiS 471 based Motherboards
 *
 * Authors:	Tiseno100,
 *
 * Copyright 2022 Tiseno100.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <86box/86box.h>
#include <86box/mem.h>
#include <86box/io.h>
#include <86box/rom.h>
#include <86box/device.h>
#include <86box/timer.h>
#include <86box/chipset.h>
#include "cpu.h"

#include <86box/fdd.h>
#include <86box/fdc.h>
#include <86box/fdc_ext.h>
#include <86box/keyboard.h>
#include <86box/nvr.h>
#include <86box/machine.h>

/*
 * AOpen Vi15G
 * 
 * Chipset: SiS 471
 * Super I/O: None
 * BIOS: AMI WinBIOS
 * Notes: None
*/
int
machine_at_vi15g_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sis_471/vi15g/VI15G_R23.ROM",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    device_add(&sis_471_device); /* SiS 471 */
    device_add(&keyboard_at_ami_device); /* Standard AT AMI KBC */
    device_add(&at_nvr_device); /* Standard AT NVR */

    if(fdc_type == FDC_INTERNAL) /* Add a Generic FDC */
        device_add(&fdc_at_device);

    return ret;
}

/*
 * ASUS VL/I-486SV2G
 * 
 * Chipset: SiS 471
 * Super I/O: None
 * BIOS: Award 4.50PG
 * Notes: None
*/
int
machine_at_486sv2g_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sis_471/486sv2g/SV2G0402.AWD",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    device_add(&sis_471_device); /* SiS 471 */
    device_add(&keyboard_ps2_ami_device); /* PS/2 Compatible AMI KBC */
    device_add(&at_nvr_device); /* Standard AT NVR */

    if(fdc_type == FDC_INTERNAL) /* Add a Generic FDC */
        device_add(&fdc_at_device);

    return ret;
}
