/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Header of the implementation of the Intel 8253/8254
 *		Programmable Interval Timer.
 *
 *
 *
 * Author:	Miran Grca, <mgrca8@gmail.com>
 *		Copyright 2019,2020 Miran Grca.
 */

#ifndef EMU_PIT_H
#define EMU_PIT_H

typedef struct {
    uint8_t m, ctrl,
        read_status, latch,
        s1_det, l_det,
        bcd, pad;

    uint16_t rl;

    int rm, wm, gate, out,
        newcount, clock, using_timer, latched,
        state, null_count, do_read_status;

    union {
        int count;
        struct {
            int units     : 4;
            int tens      : 4;
            int hundreds  : 4;
            int thousands : 4;
            int myriads   : 4;
        };
    };

    uint32_t l;

    void (*load_func)(uint8_t new_m, int new_count);
    void (*out_func)(int new_out, int old_out);
} ctr_t;

typedef struct PIT {
    int        flags, clock;
    pc_timer_t callback_timer;

    ctr_t counters[3];

    uint8_t ctrl;
} pit_t;

enum {
    PIT_8253 = 0,
    PIT_8254,
    PIT_8253_FAST,
    PIT_8254_FAST
};

typedef struct {
    uint8_t (*read)(uint16_t addr, void *priv);
    void (*write)(uint16_t addr, uint8_t val, void *priv);
    /* Gets a counter's count. */
    uint16_t (*get_count)(void *data, int counter_id);
    /* Sets a counter's GATE input. */
    void (*set_gate)(void *data, int counter_id, int gate);
    /* Sets if a counter's CLOCK input is from the timer or not - used by PCjr. */
    void (*set_using_timer)(void *data, int counter_id, int using_timer);
    /* Sets a counter's OUT output handler. */
    void (*set_out_func)(void *data, int counter_id, void (*func)(int new_out, int old_out));
    /* Sets a counter's load count handler. */
    void (*set_load_func)(void *data, int counter_id, void (*func)(uint8_t new_m, int new_count));
    void (*ctr_clock)(void *data, int counter_id);
    void *data;
} pit_intf_t;

extern pit_intf_t       pit_devs[2];
extern const pit_intf_t pit_classic_intf;

extern double SYSCLK, PCICLK, AGPCLK;

extern uint64_t PITCONST, ISACONST,
    CGACONST,
    MDACONST,
    HERCCONST,
    VGACONST1,
    VGACONST2,
    RTCCONST;

extern int refresh_at_enable;

/* Sets a counter's CLOCK input. */
extern void pit_ctr_set_clock(ctr_t *ctr, int clock);

extern pit_t *pit_common_init(int type, void (*out0)(int new_out, int old_out), void (*out1)(int new_out, int old_out));
extern pit_t *pit_ps2_init(int type);
extern void   pit_reset(pit_t *dev);

extern void pit_irq0_timer_ps2(int new_out, int old_out);

extern void pit_refresh_timer_xt(int new_out, int old_out);
extern void pit_refresh_timer_at(int new_out, int old_out);

extern void pit_speaker_timer(int new_out, int old_out);

extern void pit_nmi_timer_ps2(int new_out, int old_out);

extern void pit_set_clock(int clock);
extern void pit_handler(int set, uint16_t base, int size, void *priv);

#ifdef EMU_DEVICE_H
extern const device_t i8253_device;
extern const device_t i8254_device;
extern const device_t i8254_sec_device;
extern const device_t i8254_ext_io_device;
extern const device_t i8254_ps2_device;
#endif

#endif /*EMU_PIT_H*/
