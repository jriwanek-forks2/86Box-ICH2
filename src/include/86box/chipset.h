/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Handling of the emulated chipsets.
 *
 *
 *
 * Authors: Miran Grca, <mgrca8@gmail.com>
 *
 *          Copyright 2019,2020 Miran Grca.
 */

#ifndef EMU_CHIPSET_H
# define EMU_CHIPSET_H

/* SARC 2016A */
extern const device_t   sarc_2016a_device;

/* Symphony Haydn II */
extern const device_t   symphony_haydn_device;

/* SiS 471 */
extern const device_t   sis_471_device;

/* Intel PIIX */
extern const device_t   intel_430fx_device;
extern const device_t   intel_piix_device;

/* Intel ICH2 */
extern const device_t	intel_815ep_device;
extern const device_t   intel_ich2_device;

#endif	/*EMU_CHIPSET_H*/
