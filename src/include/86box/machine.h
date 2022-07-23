/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Handling of the emulated machines.
 *
 *
 *
 * Authors:	Sarah Walker, <http://pcem-emulator.co.uk/>
 *		Miran Grca, <mgrca8@gmail.com>
 *		Fred N. van Kempen, <decwiz@yahoo.com>
 *
 *		Copyright 2008-2020 Sarah Walker.
 *		Copyright 2016-2020 Miran Grca.
 *		Copyright 2017-2020 Fred N. van Kempen.
 */

#ifndef EMU_MACHINE_H
# define EMU_MACHINE_H

/* Machine feature flags. */
#define MACHINE_BUS_NONE      0x00000000    /* sys has no bus */
/* Feature flags for BUS'es. */
#define MACHINE_BUS_ISA       0x00000001    /* sys has ISA bus */
#define MACHINE_BUS_CARTRIDGE 0x00000002    /* sys has two cartridge bays */
#define MACHINE_BUS_ISA16     0x00000004    /* sys has ISA16 bus - PC/AT architecture */
#define MACHINE_BUS_CBUS      0x00000008    /* sys has C-BUS bus */
#define MACHINE_BUS_PS2       0x00000010    /* system has PS/2 keyboard and mouse ports */
#define MACHINE_BUS_EISA      0x00000020    /* sys has EISA bus */
#define MACHINE_BUS_VLB       0x00000040    /* sys has VL bus */
#define MACHINE_BUS_MCA       0x00000080    /* sys has MCA bus */
#define MACHINE_BUS_PCI       0x00000100    /* sys has PCI bus */
#define MACHINE_BUS_PCMCIA    0x00000200    /* sys has PCMCIA bus */
#define MACHINE_BUS_AGP       0x00000400    /* sys has AGP bus */
#define MACHINE_BUS_AC97      0x00000800    /* sys has AC97 bus (ACR/AMR/CNR slot) */
/* Aliases. */
#define MACHINE_CARTRIDGE     (MACHINE_BUS_CARTRIDGE)    /* sys has two cartridge bays */
/* Combined flags. */
#define MACHINE_PC            (MACHINE_BUS_ISA)                     /* sys is PC/XT-compatible (ISA) */
#define MACHINE_AT            (MACHINE_BUS_ISA | MACHINE_BUS_ISA16) /* sys is AT-compatible (ISA + ISA16) */
#define MACHINE_PC98          (MACHINE_BUS_CBUS)                    /* sys is NEC PC-98x1 series */
#define MACHINE_EISA          (MACHINE_BUS_EISA | MACHINE_AT)       /* sys is AT-compatible with EISA */
#define MACHINE_VLB           (MACHINE_BUS_VLB | MACHINE_AT)        /* sys is AT-compatible with VLB */
#define MACHINE_VLB98         (MACHINE_BUS_VLB | MACHINE_PC98)      /* sys is NEC PC-98x1 series with VLB (did that even exist?) */
#define MACHINE_VLBE          (MACHINE_BUS_VLB | MACHINE_EISA)      /* sys is AT-compatible with EISA and VLB */
#define MACHINE_MCA           (MACHINE_BUS_MCA)                     /* sys is MCA */
#define MACHINE_PCI           (MACHINE_BUS_PCI | MACHINE_AT)        /* sys is AT-compatible with PCI */
#define MACHINE_PCI98         (MACHINE_BUS_PCI | MACHINE_PC98)      /* sys is NEC PC-98x1 series with PCI */
#define MACHINE_PCIE          (MACHINE_BUS_PCI | MACHINE_EISA)      /* sys is AT-compatible with PCI, and EISA */
#define MACHINE_PCIV          (MACHINE_BUS_PCI | MACHINE_VLB)       /* sys is AT-compatible with PCI and VLB */
#define MACHINE_PCIVE         (MACHINE_BUS_PCI | MACHINE_VLBE)      /* sys is AT-compatible with PCI, VLB, and EISA */
#define MACHINE_PCMCIA        (MACHINE_BUS_PCMCIA | MACHINE_AT)     /* sys is AT-compatible laptop with PCMCIA */
#define MACHINE_AGP           (MACHINE_BUS_AGP | MACHINE_PCI)       /* sys is AT-compatible with AGP  */
#define MACHINE_AGP98         (MACHINE_BUS_AGP | MACHINE_PCI98)     /* sys is NEC PC-98x1 series with AGP (did that even exist?) */

#define MACHINE_PCJR          (MACHINE_PC | MACHINE_CARTRIDGE)      /* sys is PCjr */
#define MACHINE_PS2           (MACHINE_AT | MACHINE_BUS_PS2)        /* sys is PS/2 */
#define MACHINE_PS2_MCA       (MACHINE_MCA | MACHINE_BUS_PS2)       /* sys is MCA PS/2 */
#define MACHINE_PS2_VLB       (MACHINE_VLB | MACHINE_BUS_PS2)       /* sys is VLB PS/2 */
#define MACHINE_PS2_PCI       (MACHINE_PCI | MACHINE_BUS_PS2)       /* sys is PCI PS/2 */
#define MACHINE_PS2_PCIV      (MACHINE_PCIV | MACHINE_BUS_PS2)      /* sys is VLB/PCI PS/2 */
#define MACHINE_PS2_AGP       (MACHINE_AGP | MACHINE_BUS_PS2)       /* sys is AGP PS/2 */
#define MACHINE_PS2_A97       (MACHINE_PS2_AGP | MACHINE_BUS_AC97)  /* sys is AGP/AC97 PS/2 */
#define MACHINE_PS2_NOISA     (MACHINE_PS2_AGP & ~MACHINE_AT)       /* sys is AGP PS/2 without ISA */
#define MACHINE_PS2_NOI97     (MACHINE_PS2_A97 & ~MACHINE_AT)       /* sys is AGP/AC97 PS/2 without ISA */
/* Feature flags for miscellaneous internal devices. */
#define MACHINE_FLAGS_NONE    0x00000000    /* sys has no int devices */
#define MACHINE_VIDEO         0x00000001    /* sys has int video */
#define MACHINE_VIDEO_ONLY    0x00000002    /* sys has fixed video */
#define MACHINE_MOUSE         0x00000004    /* sys has int mouse */
#define MACHINE_FDC           0x00000008    /* sys has int FDC */
#define MACHINE_LPT_PRI       0x00000010    /* sys has int pri LPT */
#define MACHINE_LPT_SEC       0x00000020    /* sys has int sec LPT */
#define MACHINE_UART_PRI      0x00000040    /* sys has int pri UART */
#define MACHINE_UART_SEC      0x00000080    /* sys has int sec UART */
#define MACHINE_UART_TER      0x00000100    /* sys has int ter UART */
#define MACHINE_UART_QUA      0x00000200    /* sys has int qua UART */
#define MACHINE_GAMEPORT      0x00000400    /* sys has int game port */
#define MACHINE_SOUND         0x00000800    /* sys has int sound */
#define MACHINE_NIC           0x00001000    /* sys has int NIC */
#define MACHINE_MODEM         0x00002000    /* sys has int modem */
/* Feature flags for advanced devices. */
#define MACHINE_APM           0x00004000    /* sys has APM */
#define MACHINE_ACPI          0x00008000    /* sys has ACPI */
#define MACHINE_HWM           0x00010000    /* sys has hw monitor */
/* Combined flags. */
#define MACHINE_VIDEO_FIXED   (MACHINE_VIDEO | MACHINE_VIDEO_ONLY)  /* sys has fixed int video */
#define MACHINE_SUPER_IO      (MACHINE_FDC | MACHINE_LPT_PRI | MACHINE_UART_PRI | MACHINE_UART_SEC)
#define MACHINE_SUPER_IO_GAME (MACHINE_SUPER_IO | MACHINE_GAMEPORT)
#define MACHINE_SUPER_IO_DUAL (MACHINE_SUPER_IO | MACHINE_LPT_SEC | MACHINE_UART_TER | MACHINE_UART_QUA)
#define MACHINE_AV            (MACHINE_VIDEO | MACHINE_SOUND)       /* sys has video and sound */
#define MACHINE_AG            (MACHINE_SOUND | MACHINE_GAMEPORT)    /* sys has sound and game port */
/* Feature flags for internal storage controllers. */
#define MACHINE_HDC           0x03FE0000    /* sys has int HDC */
#define MACHINE_MFM           0x00020000    /* sys has int MFM/RLL */
#define MACHINE_XTA           0x00040000    /* sys has int XTA */
#define MACHINE_ESDI          0x00080000    /* sys has int ESDI */
#define MACHINE_IDE_PRI       0x00100000    /* sys has int pri IDE/ATAPI */
#define MACHINE_IDE_SEC       0x00200000    /* sys has int sec IDE/ATAPI */
#define MACHINE_IDE_TER       0x00400000    /* sys has int ter IDE/ATAPI */
#define MACHINE_IDE_QUA       0x00800000    /* sys has int qua IDE/ATAPI */
#define MACHINE_SCSI_PRI      0x01000000    /* sys has int pri SCSI */
#define MACHINE_SCSI_SEC      0x02000000    /* sys has int sec SCSI */
#define MACHINE_USB_PRI       0x04000000    /* sys has int pri USB */
#define MACHINE_USB_SEC       0x08000000    /* sys has int sec USB */
/* Combined flags. */
#define MACHINE_IDE           (MACHINE_IDE_PRI)                       /* sys has int single IDE/ATAPI - mark as pri IDE/ATAPI */
#define MACHINE_IDE_DUAL      (MACHINE_IDE_PRI | MACHINE_IDE_SEC)     /* sys has int dual IDE/ATAPI - mark as both pri and sec IDE/ATAPI */
#define MACHINE_IDE_DUALTQ    (MACHINE_IDE_TER | MACHINE_IDE_QUA)
#define MACHINE_IDE_QUAD      (MACHINE_IDE_DUAL | MACHINE_IDE_DUALTQ) /* sys has int quad IDE/ATAPI - mark as dual + both ter and and qua IDE/ATAPI */
#define MACHINE_SCSI          (MACHINE_SCSI_PRI)                      /* sys has int single SCSI - mark as pri SCSI */
#define MACHINE_SCSI_DUAL     (MACHINE_SCSI_PRI | MACHINE_SCSI_SEC)   /* sys has int dual SCSI - mark as both pri and sec SCSI */
#define MACHINE_USB           (MACHINE_USB_PRI)
#define MACHINE_USB_DUAL      (MACHINE_USB_PRI | MACHINE_USB_SEC)
/* Special combined flags. */
#define MACHINE_PIIX		(MACHINE_IDE_DUAL)
#define MACHINE_PIIX3		(MACHINE_PIIX | MACHINE_USB)
/* TODO: ACPI flag. */
#define MACHINE_PIIX4		(MACHINE_PIIX3 | MACHINE_ACPI)

#define IS_ARCH(m, a)		((machines[m].bus_flags & (a)) ? 1 : 0)
#define IS_AT(m)		(((machines[m].bus_flags & (MACHINE_BUS_ISA16 | MACHINE_BUS_EISA | MACHINE_BUS_VLB | MACHINE_BUS_MCA | MACHINE_BUS_PCI | MACHINE_BUS_PCMCIA | MACHINE_BUS_AGP | MACHINE_BUS_AC97)) && !(machines[m].bus_flags & MACHINE_PC98)) ? 1 : 0)

#define CPU_BLOCK(...)		(const uint8_t[]) {__VA_ARGS__, 0}
#define MACHINE_MULTIPLIER_FIXED -1

#define CPU_BLOCK_NONE       0

/* Make sure it's always an invalid value to avoid misdetections. */
#if (defined __amd64__ || defined _M_X64 || defined __aarch64__ || defined _M_ARM64)
#define MACHINE_AVAILABLE	0xffffffffffffffffULL
#else
#define MACHINE_AVAILABLE	0xffffffff
#endif

enum {
    MACHINE_TYPE_NONE = 0,
    MACHINE_TYPE_SARC_2016A,
    MACHINE_TYPE_SYMPHONY_HAYDN,
    MACHINE_TYPE_SIS_471,
    MACHINE_TYPE_INTEL_PIIX,
    MACHINE_TYPE_ICH2,
    MACHINE_TYPE_MAX
};

enum {
    MACHINE_CHIPSET_NONE = 0,
    MACHINE_CHIPSET_SARC_2016A,
    MACHINE_CHIPSET_SYMPHONY_HAYDN,
    MACHINE_CHIPSET_SIS_471,
    MACHINE_CHIPSET_INTEL_430FX,
    MACHINE_CHIPSET_INTEL_I815EP,
    MACHINE_CHIPSET_MAX
};

typedef struct _machine_filter_ {
    const char	*name;
    const char  id;
} machine_filter_t;

typedef struct _machine_cpu_ {
    uint32_t        package;
    const uint8_t  *block;
    uint32_t        min_bus;
    uint32_t        max_bus;
    uint16_t        min_voltage;
    uint16_t        max_voltage;
    float           min_multi;
    float           max_multi;
} machine_cpu_t;

typedef struct _machine_memory_ {
    uint32_t        min, max;
    int             step;
} machine_memory_t;

typedef struct _machine_ {
    const char     *name;
    const char     *internal_name;
    uint32_t        type;
    uint32_t        chipset;
    int             (*init)(const struct _machine_ *);
    uintptr_t       pad, pad0, pad1, pad2;
    const machine_cpu_t cpu;
    uintptr_t       bus_flags;
    uintptr_t       flags;
    const machine_memory_t ram;
    int             ram_granularity;
    int             nvrmask;
    uint16_t        kbc;
    /* Bits:
	7-0	Set bits are forced set on P1 (no forced set = 0x00);
	15-8	Clear bits are forced clear on P1 (no foced clear = 0xff). */
    uint16_t        kbc_p1;
    uint32_t        gpio;
    uint32_t        gpio_acpi;
#ifdef EMU_DEVICE_H
    const device_t *device;
    const device_t *vid_device;
    const device_t *snd_device;
    const device_t *net_device;
#else
    void           *device;
    void           *vid_device;
    void           *snd_device;
    void           *net_device;
#endif
} machine_t;

/* Global variables. */
extern const machine_filter_t machine_types[],
                              machine_chipsets[];
extern const machine_t        machines[];
extern int                    bios_only;
extern int                    machine;

/* Core functions. */
extern int	machine_count(void);
extern int	machine_available(int m);
extern char	*machine_getname(void);
extern char	*machine_getname_ex(int m);
extern char	*machine_get_internal_name(void);
extern int	machine_get_machine_from_internal_name(char *s);
extern void	machine_init(void);
#ifdef EMU_DEVICE_H
extern const device_t	*machine_getdevice(int m);
#endif
extern char	*machine_get_internal_name_ex(int m);
extern int   machine_get_nvrmask(int m);
extern int   machine_has_flags(int m, int flags);
extern int   machine_has_bus(int m, int bus_flags);
extern int   machine_has_cartridge(int m);
extern int   machine_get_min_ram(int m);
extern int   machine_get_max_ram(int m);
extern int   machine_get_ram_granularity(int m);
extern int   machine_get_type(int m);
extern void  machine_close(void);

extern uint8_t  machine_get_p1(void);
extern void     machine_load_p1(int m);
extern uint32_t machine_get_gpi(void);
extern void	    machine_load_gpi(int m);
extern void     machine_set_gpi(uint32_t gpi);

/* Initialization functions for boards and systems. */
extern void	machine_common_init(const machine_t *);

/* m_at.c */
extern void	machine_at_common_init_ex(const machine_t *);

/* boards_sarc_2016a.c */
extern int  machine_at_m396f_init(const machine_t *);
extern int  machine_at_sxd_init(const machine_t *);

/* boards_symphony_haydn.c */
extern int  machine_at_t486dx_init(const machine_t *);
extern int  machine_at_386sc_stock_init(const machine_t *);
extern int  machine_at_386sc_mr_init(const machine_t *);
extern int  machine_at_pkm0031y_init(const machine_t *);

/* boards_sis_471.c */
extern int  machine_at_vi15g_init(const machine_t *);
extern int  machine_at_486sv2g_init(const machine_t *);

/* boards_piix.c */
extern int  machine_at_p54tp4_init(const machine_t *);
extern int  machine_at_ap5c_init(const machine_t *);
extern int  machine_at_acerv30_init(const machine_t *);
extern int  machine_at_m54hi_init(const machine_t *);

/* boards_ich2.c */
extern int	machine_at_s2080_init(const machine_t *);

#endif	/*EMU_MACHINE_H*/
