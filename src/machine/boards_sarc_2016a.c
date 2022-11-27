/*
 *          SARC 2016A based Motherboards
 *
 * Authors: Tiseno100,
 *
 *          Copyright 2022 Tiseno100.
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
 * PCChips M396F
 * 
 * Chipset: SARC 2016A
 * Super I/O: None
 * BIOS: AMI Color
 * Notes: None
*/
int
machine_at_m396f_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sarc_2016a/m396f/pcchips-m396f.bin-5f31ab54cf61e537920047.bin",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    device_add(&sarc_2016a_device); /* SARC 2016A */
    device_add(&keyboard_at_ami_device); /* Standard AT AMI KBC */
    device_add(&at_nvr_device); /* Standard AT NVR */

    if(fdc_type == FDC_INTERNAL) /* Add a Generic FDC */
        device_add(&fdc_at_device);

    return ret;
}

/*
 * CX Technology SXD
 * 
 * Chipset: SARC 2016A
 * Super I/O: None
 * BIOS: AMI Color
 * Notes: None
*/
int
machine_at_sxd_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sarc_2016a/sxd/3cyw001-61b008bc17e17833962104.bin",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    device_add(&sarc_2016a_device); /* SARC 2016A */
    device_add(&keyboard_at_ami_device); /* Standard AT AMI KBC */
    device_add(&at_nvr_device); /* Standard AT NVR */

    if(fdc_type == FDC_INTERNAL) /* Add a Generic FDC */
        device_add(&fdc_at_device);

    return ret;
}
