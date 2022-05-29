/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Definitions for the Super I/O chips.
 *
 * Author:	Fred N. van Kempen, <decwiz@yahoo.com>
 *		Copyright 2017-2020 Fred N. van Kempen.
 */

#ifndef EMU_SIO_H
# define EMU_SIO_H

/* SMC 665(FDC37C665) */
extern const device_t   smc665_device;

/* National Semiconductor NSC366 (PC87366) */
extern const device_t   nsc366_device;
extern const device_t   nsc366_4f_device;

#endif	/*EMU_SIO_H*/
