/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Definitions for the SMBus host controllers.
 *
 *
 *
 * Authors: RichardG, <richardg867@gmail.com>
 *
 *          Copyright 2020 RichardG.
 */

#ifndef EMU_SMBUS_PIIX4_H
#define EMU_SMBUS_PIIX4_H
#include <86box/apm.h>
#include <86box/nvr.h>
#include <86box/acpi.h>

#define SMBUS_PIIX4_BLOCK_DATA_SIZE   32
#define SMBUS_PIIX4_BLOCK_DATA_MASK   (SMBUS_PIIX4_BLOCK_DATA_SIZE - 1)

#define SMBUS_ALI7101_BLOCK_DATA_SIZE 32
#define SMBUS_ALI7101_BLOCK_DATA_MASK (SMBUS_ALI7101_BLOCK_DATA_SIZE - 1)

enum {
    SMBUS_PIIX4 = 0,
    SMBUS_INTEL_ICH2,
    SMBUS_VIA
};

typedef struct {
    uint32_t local;
    uint16_t io_base, byte_rw;
    int      clock;
    double   bit_period;
    uint8_t  stat, next_stat, ctl, cmd, addr,
        data0, data1,
        index, data[SMBUS_PIIX4_BLOCK_DATA_SIZE], block_data_byte,
        irq, smi_en;

    acpi_t    *acpi;
    pc_timer_t response_timer;
    void      *i2c;
} smbus_piix4_t;

typedef struct {
    uint32_t local;
    uint16_t io_base;
    uint8_t  stat, next_stat, ctl, cmd, addr,
        data0, data1,
        index, data[SMBUS_ALI7101_BLOCK_DATA_SIZE];
    pc_timer_t response_timer;
    void      *i2c;
} smbus_ali7101_t;

extern void smbus_piix4_get_acpi(smbus_piix4_t *dev, acpi_t *acpi);
extern void smbus_piix4_get_irq(uint8_t irq, smbus_piix4_t *dev);
extern void smbus_piix4_smi_en(uint8_t smi_en, smbus_piix4_t *dev);
extern void smbus_piix4_remap(smbus_piix4_t *dev, uint16_t new_io_base, uint8_t enable);
extern void smbus_piix4_setclock(smbus_piix4_t *dev, int clock);

extern void smbus_ali7101_remap(smbus_ali7101_t *dev, uint16_t new_io_base, uint8_t enable);

#ifdef EMU_DEVICE_H
extern const device_t piix4_smbus_device;
extern const device_t intel_ich2_smbus_device;
extern const device_t via_smbus_device;

extern const device_t ali7101_smbus_device;
#endif

#endif /*EMU_SMBUS_PIIX4_H*/
