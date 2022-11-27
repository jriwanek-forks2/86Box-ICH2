/*
 *          Symphony Haydn II based Motherboards
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
 * Commodore T486DX
 * 
 * Chipset: Symphony Haydn II
 * Super I/O: None
 * BIOS: AMI Color
 * Notes: Also called Schneider T486DX
*/
int
machine_at_t486dx_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/symphony_haydn/t486dx/cbm-t486dx-66-bios-v1.03-391684-02.bin",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    device_add(&symphony_haydn_device); /* Symphony Haydn */
    device_add(&keyboard_at_ami_device); /* Standard AT AMI KBC */
    device_add(&at_nvr_device); /* Standard AT NVR */

    if(fdc_type == FDC_INTERNAL) /* Add a Generic FDC */
        device_add(&fdc_at_device);

    return ret;
}

/*
 * FIC 386SC
 * 
 * Chipset: Symphony Haydn II
 * Super I/O: None
 * BIOS: AMI Color (Has also an MR BIOS flash update)
 * Notes: None
*/
void
machine_at_386sc_common(const machine_t *model)
{
    machine_at_common_init_ex(model);

    device_add(&symphony_haydn_device); /* Symphony Haydn */
    device_add(&keyboard_at_ami_device); /* Standard AT AMI KBC */
    device_add(&at_nvr_device); /* Standard AT NVR */

    if(fdc_type == FDC_INTERNAL) /* Add a Generic FDC */
        device_add(&fdc_at_device);
}

int
machine_at_386sc_stock_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/symphony_haydn/386sc/386-386 SC Syphony.BIN",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_386sc_common(model);

    return ret;
}

int
machine_at_386sc_mr_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/symphony_haydn/386sc/V016B401.rom",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_386sc_common(model);

    return ret;
}

/*
 * DTK PKM-0031Y
 * 
 * Chipset: Symphony Haydn II
 * Super I/O: None
 * BIOS: DTK 4.27
 * Notes: None
*/
int
machine_at_pkm0031y_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/symphony_haydn/pkm0031y/DTK.BIO",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    device_add(&symphony_haydn_device); /* Symphony Haydn */
    device_add(&keyboard_at_ami_device); /* Standard AT AMI KBC */
    device_add(&at_nvr_device); /* Standard AT NVR */

    if(fdc_type == FDC_INTERNAL) /* Add a Generic FDC */
        device_add(&fdc_at_device);

    return ret;
}
