/*
 * Intel PIIX based Motherboards
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
#include <86box/timer.h>
#include <86box/rom.h>
#include <86box/pci.h>
#include <86box/device.h>
#include <86box/chipset.h>
#include <86box/flash.h>
#include <86box/keyboard.h>
#include <86box/nvr.h>
#include <86box/sio.h>
#include "cpu.h"
#include <86box/machine.h>

/*
 * ASUS PCI/I-P54TP4
 * 
 * North Bridge: Intel 430FX
 * Super I/O: SMC 665
 * BIOS: Award 4.51PG (Also has an unofficial MR BIOS update)
 * Notes: None
*/
int
machine_at_p54tp4_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/piix/p54tp4/T15I0302.AWD",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_bus_slot(0, 0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_bus_slot(0, 0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_bus_slot(0, 0x0c, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_bus_slot(0, 0x0b, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_bus_slot(0, 0x0a, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_bus_slot(0, 0x09, PCI_CARD_NORMAL,      4, 1, 2, 3);

    device_add(&intel_430fx_device); /* Intel 430FX PCIset */
    device_add(&intel_piix_device); /* Intel PIIX */

    device_add(&smc665_device); /* SMC665 */
    device_add(&at_nvr_device); /* Standard AT NVR */
    device_add(&keyboard_ps2_ami_pci_device); /* Standard PS/2 AMI KBC */
    device_add(&intel_flash_bxt_device); /* Intel compatible flash */

    return ret;
}

/*
 * AOpen AP5C
 * 
 * North Bridge: Intel 430FX
 * Super I/O: SMC 665
 * BIOS: AMI WinBios
 * Notes: None
*/
int
machine_at_ap5c_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/piix/ap5c/ap5c22cs.rom",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_bus_slot(0, 0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_bus_slot(0, 0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_bus_slot(0, 0x12, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_bus_slot(0, 0x11, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_bus_slot(0, 0x14, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_bus_slot(0, 0x13, PCI_CARD_NORMAL,      4, 1, 2, 3);

    device_add(&intel_430fx_device); /* Intel 430FX PCIset */
    device_add(&intel_piix_device); /* Intel PIIX */

    device_add(&smc665_device); /* SMC665 */
    device_add(&ami_1994_nvr_device); /* Standard AT NVR with dirty cache hacks */
    device_add(&keyboard_ps2_ami_pci_device); /* Standard PS/2 AMI KBC */
    device_add(&intel_flash_bxt_device); /* Intel compatible flash */

    return ret;
}

/*
 * Acer V30
 * 
 * North Bridge: Intel 430FX
 * Super I/O: SMC 665
 * BIOS: Acer Proprietary
 * Notes: None
*/
int
machine_at_acerv30_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/piix/acerv30/V30R01N9.BIN",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_bus_slot(0, 0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_bus_slot(0, 0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_bus_slot(0, 0x12, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_bus_slot(0, 0x11, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_bus_slot(0, 0x14, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_bus_slot(0, 0x13, PCI_CARD_NORMAL,      4, 1, 2, 3);

    device_add(&intel_430fx_device); /* Intel 430FX PCIset */
    device_add(&intel_piix_device); /* Intel PIIX */

    device_add(&smc665_device); /* SMC665 */
    device_add(&at_nvr_device); /* Standard AT NVR */
    device_add(&keyboard_ps2_acer_pci_device); /* Standard PS/2 Acer KBC */
    device_add(&intel_flash_bxt_device); /* Intel compatible flash */

    return ret;
}

/*
 * Micronics M54Hi
 * 
 * North Bridge: Intel 430FX
 * Super I/O: SMC 665
 * BIOS: Phoenix 4.05
 * Notes: Comes with an On-board Vibra 16C. You need to enter on CMOS Setup and configure the board else it won't boot further.
*/
int
machine_at_m54hi_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/piix/m54hi/M54HI_06.ROM",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_bus_slot(0, 0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_bus_slot(0, 0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_bus_slot(0, 0x14, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_bus_slot(0, 0x13, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_bus_slot(0, 0x12, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_bus_slot(0, 0x11, PCI_CARD_NORMAL,      4, 1, 2, 3);

    device_add(&intel_430fx_device); /* Intel 430FX PCIset */
    device_add(&intel_piix_device); /* Intel PIIX */

    device_add(&smc665_device); /* SMC665 */
    device_add(&at_nvr_device); /* Standard AT NVR */
    device_add(&keyboard_ps2_ami_pci_device); /* Standard PS/2 AMI KBC */
    device_add(&intel_flash_bxt_device); /* Intel compatible flash */

    return ret;
}
