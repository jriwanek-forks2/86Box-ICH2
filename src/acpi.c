/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		ACPI emulation.
 *
 *
 *
 * Authors:	Miran Grca, <mgrca8@gmail.com>
 *
 *		Copyright 2020 Miran Grca.
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>
#define HAVE_STDARG_H
#include <86box/86box.h>
#include "cpu.h"
#include <86box/device.h>
#include <86box/mem.h>
#include <86box/io.h>
#include <86box/pci.h>
#include <86box/pic.h>
#include <86box/plat.h>
#include <86box/timer.h>
#include <86box/keyboard.h>
#include <86box/nvr.h>
#include <86box/pit.h>
#include <86box/apm.h>
#include <86box/tco.h>
#include <86box/acpi.h>
#include <86box/machine.h>
#include <86box/i2c.h>
#include <86box/video.h>

int acpi_rtc_status = 0;

static double cpu_to_acpi;
#define ENABLE_ACPI_LOG 1
#ifdef ENABLE_ACPI_LOG
int acpi_do_log = ENABLE_ACPI_LOG;


static void
acpi_log(const char *fmt, ...)
{
    va_list ap;

    if (acpi_do_log) {
	va_start(ap, fmt);
	pclog_ex(fmt, ap);
	va_end(ap);
    }
}
#else
#define acpi_log(fmt, ...)
#endif

static uint64_t acpi_clock_get() {
    return tsc * cpu_to_acpi;
}

static uint32_t acpi_timer_get(acpi_t *dev) {
    uint64_t clock = acpi_clock_get();
    if (dev->regs.timer32)
        return clock & 0xffffffff;
    else
        return clock & 0xffffff;
}

static double acpi_get_overflow_period(acpi_t *dev) {
    uint64_t timer = acpi_clock_get();
    uint64_t overflow_time;

    if (dev->regs.timer32) {
        overflow_time = (timer + 0x80000000LL) & ~0x7fffffffLL;
    } else {
        overflow_time = (timer + 0x800000LL) & ~0x7fffffLL;
    }

    uint64_t time_to_overflow = overflow_time - timer;

    return ((double)time_to_overflow / (double)ACPI_TIMER_FREQ) * 1000000.0;
}


static void
acpi_timer_update(acpi_t *dev, bool enable)
{
    if (enable) {
        timer_on_auto(&dev->timer, acpi_get_overflow_period(dev));
    } else {
        timer_stop(&dev->timer);
    }
}

static void
acpi_timer_overflow(void *priv)
{
    acpi_t *dev = (acpi_t *) priv;
    int sci_en = dev->regs.pmcntrl & 1;

    dev->regs.pmsts |= TMROF_STS;

    acpi_log("Intel ICH2 ACPI: Beating calmly\n");

    if(dev->regs.pmen & 1) /* Timer Overflow Interrupt Enable */
    {
        acpi_log("Intel ICH2 ACPI: Overflow detected. Provoking an %s\n", sci_en ? "SCI" : "SMI");

        if(sci_en) /* Trigger an SCI or SMI depending on the status of the SCI_EN register */
            acpi_update_irq(dev);
        else
            acpi_raise_smi(dev, 1);
    }

    acpi_timer_update(dev, 1); // Prepare for the next beat
}


void
acpi_update_irq(acpi_t *dev)
{
    int sci_level = (dev->regs.pmsts & dev->regs.pmen) & (RTC_EN | PWRBTN_EN | GBL_EN | TMROF_EN);

    acpi_log("Intel ICH2 ACPI: An IRQ was risen on line %d\n", dev->irq_line);

    if (sci_level) {
		pci_set_irq(dev->slot, dev->irq_pin);
    } else {
		pci_clear_irq(dev->slot, dev->irq_pin);
    }
}


void
acpi_raise_smi(void *priv, int do_smi)
{
    acpi_t *dev = (acpi_t *) priv;

    if (do_smi && (dev->regs.smi_en & 1))
        smi_raise();
}


static uint32_t
acpi_reg_read_default_regs(int size, uint16_t addr, void *p)
{
    acpi_t *dev = (acpi_t *) p;
    uint32_t ret = 0x00000000;
    int shift16, shift32;

    addr &= 0x3f;
    shift16 = (addr & 1) << 3;
    shift32 = (addr & 3) << 3;

    switch (addr) {
	case 0x00: case 0x01:
		/* PMSTS - Power Management Status Register (IO) */
		ret = (dev->regs.pmsts >> shift16) & 0xff;
		if (addr == 0x01)
			ret |= (acpi_rtc_status << 2);
		break;
	case 0x02: case 0x03:
		/* PMEN - Power Management Resume Enable Register (IO) */
		ret = (dev->regs.pmen >> shift16) & 0xff;
		break;
	case 0x04: case 0x05:
		/* PMCNTRL - Power Management Control Register (IO) */
		ret = (dev->regs.pmcntrl >> shift16) & 0xff;
		if (addr == 0x05)
			ret = (ret & 0xdf);	/* Bit 5 is write-only. */
		break;
	case 0x08: case 0x09: case 0x0a: case 0x0b:
		/* PMTMR - Power Management Timer Register (IO) */
		ret = (acpi_timer_get(dev) >> shift32) & 0xff;
#ifdef USE_DYNAREC
		if (cpu_use_dynarec)
			update_tsc();
#endif
		break;
    }

#ifdef ENABLE_ACPI_LOG
    if (size != 1)
	acpi_log("(%i) ACPI Read  (%i) %02X: %02X\n", in_smm, size, addr, ret);
#endif
    return ret;
}


static uint32_t
acpi_reg_read_intel_ich2(int size, uint16_t addr, void *p)
{
    acpi_t *dev = (acpi_t *) p;
    uint32_t ret = 0x00000000;
    int shift16, shift32;

    addr &= 0x7f;
    shift16 = (addr & 1) << 3;
    shift32 = (addr & 3) << 3;

    switch (addr) {
    case 0x10: case 0x11: case 0x12: case 0x13:
		/* PROC_CNT—Processor Control Register */
        ret = (dev->regs.pcntrl >> shift32) & 0xff;
        break;
	case 0x28: case 0x29:
		/* GPE0_STS—General Purpose Event 0 Status Register */
		ret = (dev->regs.gpsts >> shift16) & 0xff;
		break;
	case 0x2a: case 0x2b:
		/* GPE0_EN—General Purpose Event 0 Enables Register */
		ret = (dev->regs.gpen >> shift16) & 0xff;
		break;
	case 0x2c: case 0x2d:
		/* GPE1_STS—General Purpose Event 1 Status Register */
		ret = (dev->regs.gpsts1 >> shift16) & 0xff;
		break;
	case 0x2e: case 0x2f:
		/* GPE1_EN—General Purpose Event 1 Enable Register */
		ret = (dev->regs.gpen1 >> shift16) & 0xff;
		break;
    case 0x30: case 0x31: case 0x32: case 0x33:
		/* SMI_EN—SMI Control and Enable Register */
		ret = (dev->regs.smi_en >> shift32) & 0xff;
		break;
    case 0x34: case 0x35: case 0x36: case 0x37:
		/* SMI_STS—SMI Status Register */
		ret = (dev->regs.smi_sts >> shift32) & 0xff;
		break;
	case 0x40: case 0x41:
		/* MON_SMI—Device Monitor SMI Status and Enable Register */
		ret = (dev->regs.mon_smi >> shift16) & 0xff;
		break;
	case 0x44: case 0x45:
		/* DEVACT_STS—Device Activity Status Register */
		ret = (dev->regs.devact_sts >> shift16) & 0xff;
		break;
	case 0x48: case 0x49:
		/* DEVTRAP_EN—Device Trap Enable Register */
		ret = (dev->regs.devtrap_en >> shift16) & 0xff;
		break;
    case 0x4c ... 0x4d:
        /* BUS_ADDR_TRACK—Bus Address Tracker Register */
        ret = (dev->regs.bus_addr_track >> shift16) & 0xff;
        break;
    case 0x4e:
        /* BUS_CYC_TRACK—Bus Cycle Tracker Register */
        ret = dev->regs.bus_cyc_track;
        break;
    case 0x60 ... 0x70:
        /* TCO Registers */
        ret = tco_read(addr, dev->tco);
        break;
	default:
		ret = acpi_reg_read_default_regs(size, addr, p);
		break;
    }

#ifdef ENABLE_ACPI_LOG
    if (size != 1)
	    acpi_log("(%i) ACPI Read  (%i) %02X: %02X\n", in_smm, size, addr, ret);
#endif
    return ret;
}


static void
acpi_reg_write_default_regs(int size, uint16_t addr, uint8_t val, void *p)
{
    acpi_t *dev = (acpi_t *) p;
    int shift16, sus_typ;

    addr &= 0x3f;
#ifdef ENABLE_ACPI_LOG
    if (size != 1)
	acpi_log("(%i) ACPI Write (%i) %02X: %02X\n", in_smm, size, addr, val);
#endif
    shift16 = (addr & 1) << 3;

    switch (addr) {
	case 0x00: case 0x01:
		/* PMSTS - Power Management Status Register (IO) */
		dev->regs.pmsts &= ~((val << shift16) & 0x8d31);
		if ((addr == 0x01) && (val & 0x04))
			acpi_rtc_status = 0;
		break;
	case 0x02: case 0x03:
		/* PMEN - Power Management Resume Enable Register (IO) */
		dev->regs.pmen = ((dev->regs.pmen & ~(0xff << shift16)) | (val << shift16)) & 0x0521;
		break;
	case 0x04: case 0x05:
		/* PMCNTRL - Power Management Control Register (IO) */
        if((addr == 0x05) && !!(val & 0x20) && !!(val & 4) && !!(dev->regs.smi_en & 0x00000010)) {
            dev->regs.smi_sts |= 0x00000010; /* ICH2 Specific. Trigger an SMI if SLP_SMI_EN bit is set instead of transistioning to a Sleep State. */
            acpi_raise_smi(dev, 1);
        }
		else if ((addr == 0x05) && (val & 0x20)) {
			sus_typ = dev->suspend_types[(val >> 2) & 7];

			if (sus_typ & SUS_POWER_OFF) {
				/* Soft power off. */
				plat_power_off();
				return;
			}

			if (sus_typ & SUS_SUSPEND) {
				if (sus_typ & SUS_NVR) {
					/* Suspend to RAM. */
					nvr_reg_write(0x000f, 0xff, dev->nvr);
				}

				if (sus_typ & SUS_RESET_PCI)
					device_reset_all_pci();

				if (sus_typ & SUS_RESET_CPU)
					cpu_alt_reset = 0;

				if (sus_typ & SUS_RESET_PCI) {
					pci_reset();
					keyboard_at_reset();

					mem_a20_alt = 0;
					mem_a20_recalc();
				}

				if (sus_typ & (SUS_RESET_CPU | SUS_RESET_CACHE))
					flushmmucache();

				if (sus_typ & SUS_RESET_CPU)
					resetx86();

				/* Since the UI doesn't have a power button at the moment, pause emulation,
				   then trigger a resume event so that the system resumes after unpausing. */
				plat_pause(1);
				timer_set_delay_u64(&dev->resume_timer, 50 * TIMER_USEC);
			}
		}
		dev->regs.pmcntrl = ((dev->regs.pmcntrl & ~(0xff << shift16)) | (val << shift16)) & 0x3f07 /* 0x3c07 */;
		break;
    }
}


static void
acpi_reg_write_intel_ich2(int size, uint16_t addr, uint8_t val, void *p)
{
    acpi_t *dev = (acpi_t *) p;
    int shift16, shift32;

    addr &= 0x7f;
#ifdef ENABLE_ACPI_LOG
    if (size != 1)
	acpi_log("(%i) ACPI Write (%i) %02X: %02X\n", in_smm, size, addr, val);
#endif
    shift16 = (addr & 1) << 3;
    shift32 = (addr & 3) << 3;

    switch (addr) {
    case 0x10: case 0x11: case 0x12: case 0x13:
		/* PROC_CNT—Processor Control Register */
        dev->regs.pcntrl = ((dev->regs.pcntrl & ~(0xff << shift32)) | (val << shift32)) & 0x000201fe;
        break;
	case 0x28: case 0x29:
		/* GPE0_STS—General Purpose Event 0 Status Register */
		dev->regs.gpsts &= ~((val << shift16) & 0x09fb);
		break;
	case 0x2a: case 0x2b:
		/* GPE0_EN—General Purpose Event 0 Enables Register */
		dev->regs.gpen = ((dev->regs.gpen & ~(0xff << shift16)) | (val << shift16)) & 0x097d;
		break;
	case 0x2c: case 0x2d:
		/* GPE1_STS—General Purpose Event 1 Status Register */
		dev->regs.gpsts1 &= ~((val << shift16) & 0x09fb);
		break;
	case 0x2e: case 0x2f:
		/* GPE1_EN—General Purpose Event 1 Enable Register */
		dev->regs.gpen1 = ((dev->regs.gpen & ~(0xff << shift16)) | (val << shift16)) & 0x097d;
		break;
    case 0x30: case 0x31: case 0x32: case 0x33:
		/* SMI_EN—SMI Control and Enable Register */
		dev->regs.smi_en = ((dev->regs.smi_en & ~(0xff << shift32)) | (val << shift32)) & 0x0000867f;

        if(addr == 0x30) {
            if(val & 0x80) {
                acpi_log("Intel ICH2 ACPI: SCI risen per BIOS request\n");
                dev->regs.pmsts |= 0x0020;

                if(dev->regs.pmen & 0x0020) /* Provoke an SCI when Global Enable is Active*/
                    acpi_update_irq(dev);
            }
        }
		break;
    case 0x34: case 0x35: case 0x36: case 0x37:
		/* SMI_STS—SMI Status Register */
		dev->regs.smi_sts &= ~((val << shift32) & 0x0001ff7c);
		break;
	case 0x40: case 0x41:
		/* MON_SMI—Device Monitor SMI Status and Enable Register */
		dev->regs.mon_smi = ((dev->regs.mon_smi & ~(0xff << shift16)) | (val << shift16)) & 0x097d;
		break;
	case 0x44: case 0x45:
		/* DEVACT_STS—Device Activity Status Register */
		dev->regs.devact_sts &= ~((val << shift16) & 0x3fef);
		break;
	case 0x48: case 0x49:
		/* DEVTRAP_EN—Device Trap Enable Register */
		dev->regs.devtrap_en = ((dev->regs.devtrap_en & ~(0xff << shift16)) | (val << shift16)) & 0x3c2f;
		if (dev->trap_update)
			dev->trap_update(dev->trap_priv);
		break;
    case 0x4c ... 0x4d:
        /* BUS_ADDR_TRACK—Bus Address Tracker Register */
        dev->regs.bus_addr_track = ((dev->regs.bus_addr_track & ~(0xff << shift16)) | (val << shift16)) & 0x097d;
        break;
    case 0x4e:
        /* BUS_CYC_TRACK—Bus Cycle Tracker Register */
        dev->regs.bus_cyc_track = val;
        break;
    case 0x60 ... 0x70:
        /* TCO Registers */
        tco_write(addr, val, dev->tco);
        break;
	default:
		acpi_reg_write_default_regs(size, addr, val, p);
        if((addr == 0x04) && !!(val & 4) && !!(dev->regs.smi_en & 4)) {
            dev->regs.smi_sts = 0x00000004;
            acpi_raise_smi(dev, 1);
        }
        break;
    }
}


static void
acpi_i2c_set(acpi_t *dev)
{
    if (dev->i2c)
	i2c_gpio_set(dev->i2c, !(dev->regs.gpio_dir & 0x02) || (dev->regs.gpio_val & 0x02), !(dev->regs.gpio_dir & 0x04) || (dev->regs.gpio_val & 0x04));
}


static uint32_t
acpi_reg_readl(uint16_t addr, void *p)
{
    uint32_t ret = 0x00000000;

    ret = acpi_reg_read_intel_ich2(4, addr, p);
    ret |= (acpi_reg_read_intel_ich2(4, addr + 1, p) << 8);
    ret |= (acpi_reg_read_intel_ich2(4, addr + 2, p) << 16);
    ret |= (acpi_reg_read_intel_ich2(4, addr + 3, p) << 24);

    acpi_log("Intel ICH2 ACPI: Read L %08X from %04X\n", ret, addr);

    return ret;
}


static uint16_t
acpi_reg_readw(uint16_t addr, void *p)
{
    uint16_t ret = 0x0000;

    ret = acpi_reg_read_intel_ich2(2, addr, p);
    ret |= (acpi_reg_read_intel_ich2(2, addr + 1, p) << 8);

    acpi_log("Intel ICH2 ACPI: Read W %08X from %04X\n", ret, addr);

    return ret;
}


static uint8_t
acpi_reg_read(uint16_t addr, void *p)
{
    uint8_t ret = 0x00;

    ret = acpi_reg_read_intel_ich2(1, addr, p);

    acpi_log("Intel ICH2 ACPI: Read B %02X from %04X\n", ret, addr);

    return ret;
}

static void
acpi_reg_writel(uint16_t addr, uint32_t val, void *p)
{
    acpi_log("Intel ICH2 ACPI: Write L %08X to %04X\n", val, addr);

    acpi_reg_write_intel_ich2(4, addr, val & 0xff, p);
    acpi_reg_write_intel_ich2(4, addr + 1, (val >> 8) & 0xff, p);
    acpi_reg_write_intel_ich2(4, addr + 2, (val >> 16) & 0xff, p);
    acpi_reg_write_intel_ich2(4, addr + 3, (val >> 24) & 0xff, p);
}


static void
acpi_reg_writew(uint16_t addr, uint16_t val, void *p)
{
    acpi_log("Intel ICH2 ACPI: Write W %04X to %04X\n", val, addr);

    acpi_reg_write_intel_ich2(2, addr, val & 0xff, p);
    acpi_reg_write_intel_ich2(2, addr + 1, (val >> 8) & 0xff, p);
}


static void
acpi_reg_write(uint16_t addr, uint8_t val, void *p)
{
    acpi_log("Intel ICH2 ACPI: Write B %02X to %04X\n", val, addr);

    acpi_reg_write_intel_ich2(1, addr, val, p);
}


void
acpi_update_io_mapping(acpi_t *dev, uint32_t base, int chipset_en)
{
    acpi_log("Intel ICH2 ACPI: Update I/O %04X to %04X (%sabled)\n", dev->io_base, base, chipset_en ? "en" : "dis");

    if (dev->io_base != 0x0000) {
	io_removehandler(dev->io_base, 0x0080,
			 acpi_reg_read, acpi_reg_readw, acpi_reg_readl,
			 acpi_reg_write, acpi_reg_writew, acpi_reg_writel, dev);
    }

    dev->io_base = base;

    if (chipset_en && (dev->io_base != 0x0000)) {
	io_sethandler(dev->io_base, 0x0080,
		      acpi_reg_read, acpi_reg_readw, acpi_reg_readl,
		      acpi_reg_write, acpi_reg_writew, acpi_reg_writel, dev);
    }
}


static void
acpi_timer_resume(void *priv)
{
    acpi_t *dev = (acpi_t *) priv;

    dev->regs.pmsts |= 0x8000;

    /* Nasty workaround for ASUS P2B-LS and potentially others, where the PMCNTRL
       SMI trap handler clears the resume bit before returning control to the OS. */
    if (in_smm)
	timer_set_delay_u64(&dev->resume_timer, 50 * TIMER_USEC);
}


void
acpi_set_timer32(acpi_t *dev, uint8_t timer32)
{
    dev->regs.timer32 = timer32;
}


void
acpi_set_slot(acpi_t *dev, int slot)
{
    dev->slot = slot;
}


void
acpi_set_irq_mode(acpi_t *dev, int irq_mode)
{
    dev->irq_mode = irq_mode;
}


void
acpi_set_irq_pin(acpi_t *dev, int irq_pin)
{
    dev->irq_pin = irq_pin;
}


void
acpi_set_irq_line(acpi_t *dev, int irq_line)
{
    dev->irq_line = irq_line;
}


void
acpi_set_mirq_is_level(acpi_t *dev, int mirq_is_level)
{
    dev->mirq_is_level = mirq_is_level;
}


void
acpi_set_gpireg2_default(acpi_t *dev, uint8_t gpireg2_default)
{
    dev->gpireg2_default = gpireg2_default;
    dev->regs.gpireg[2] = dev->gpireg2_default;
}


void
acpi_set_nvr(acpi_t *dev, nvr_t *nvr)
{
    dev->nvr = nvr;
}

void
acpi_set_tco(acpi_t *dev, tco_t *tco)
{
    dev->tco = tco;
}

void
acpi_set_trap_update(acpi_t *dev, void (*update)(void *priv), void *priv)
{
    dev->trap_update = update;
    dev->trap_priv = priv;
}


uint8_t
acpi_ali_soft_smi_status_read(acpi_t *dev)
{
    return dev->regs.ali_soft_smi = 1;
}


void
acpi_ali_soft_smi_status_write(acpi_t *dev, uint8_t soft_smi)
{
    dev->regs.ali_soft_smi = soft_smi;
}


static void
acpi_apm_out(uint16_t port, uint8_t val, void *p)
{
    acpi_t *dev = (acpi_t *) p;

    acpi_log("[%04X:%08X] APM write: %04X = %02X (AX = %04X, BX = %04X, CX = %04X)\n", CS, cpu_state.pc, port, val, AX, BX, CX);

    port &= 0x0001;

	if (port == 0x0000) {
		dev->apm->cmd = val;
    
        if(dev->regs.smi_en & 0x00000020) { /* ICH2 APMC SMI */
            dev->regs.smi_sts |= 0x00000020;
            acpi_raise_smi(dev, 1);
        }

	} else
		dev->apm->stat = val;
}


static uint8_t
acpi_apm_in(uint16_t port, void *p)
{
    acpi_t *dev = (acpi_t *) p;
    uint8_t ret = 0xff;

    port &= 0x0001;

	if (port == 0x0000)
		ret = dev->apm->cmd;
	else
		ret = dev->apm->stat;

    acpi_log("[%04X:%08X] APM read: %04X = %02X\n", CS, cpu_state.pc, port, ret);

    return ret;
}


static void
acpi_reset(void *priv)
{
    acpi_t *dev = (acpi_t *) priv;
    int i;

    memset(&dev->regs, 0x00, sizeof(acpi_regs_t));
    dev->regs.gpireg[0] = 0xff; dev->regs.gpireg[1] = 0xff;
    /* A-Trend ATC7020BXII:
       - Bit 3: 80-conductor cable on secondary IDE channel (active low)
       - Bit 2: 80-conductor cable on primary IDE channel (active low)
       Gigabyte GA-686BX:
       - Bit 1: CMOS battery low (active high) */
    dev->regs.gpireg[2] = dev->gpireg2_default;

    /* Power on always generates a resume event. */
    dev->regs.pmsts |= 0x8000;

    acpi_rtc_status = 0;

    acpi_timer_update(dev, 1);
}


static void
acpi_speed_changed(void *priv)
{
    acpi_t *dev = (acpi_t *) priv;
    cpu_to_acpi = ACPI_TIMER_FREQ / cpuclock;
    bool timer_enabled = timer_is_enabled(&dev->timer);
    timer_stop(&dev->timer);

    if (timer_enabled)
        timer_on_auto(&dev->timer, acpi_get_overflow_period(dev));
}


static void
acpi_close(void *priv)
{
    acpi_t *dev = (acpi_t *) priv;

    if (dev->i2c) {
	if (i2c_smbus == i2c_gpio_get_bus(dev->i2c))
		i2c_smbus = NULL;
	i2c_gpio_close(dev->i2c);
    }

    timer_stop(&dev->timer);

    free(dev);
}


static void *
acpi_init(const device_t *info)
{
    acpi_t *dev;

    dev = (acpi_t *)malloc(sizeof(acpi_t));
    if (dev == NULL) return(NULL);
    memset(dev, 0x00, sizeof(acpi_t));

    cpu_to_acpi = ACPI_TIMER_FREQ / cpuclock;
    dev->vendor = info->local;

    dev->irq_line = 9;

	dev->apm = device_add(&apm_pci_acpi_device);
	io_sethandler(0x00b2, 0x0002, acpi_apm_in, NULL, NULL, acpi_apm_out, NULL, NULL, dev);

    dev->suspend_types[1] = SUS_SUSPEND | SUS_RESET_CPU;
    dev->suspend_types[5] = SUS_SUSPEND | SUS_NVR | SUS_RESET_CPU | SUS_RESET_PCI;
    dev->suspend_types[6] = SUS_POWER_OFF;
    dev->suspend_types[7] = SUS_POWER_OFF;

    timer_add(&dev->timer, acpi_timer_overflow, dev, 0);
    timer_add(&dev->resume_timer, acpi_timer_resume, dev, 0);

    acpi_reset(dev);

    return dev;
}

const device_t intel_ich2_acpi_device = {
    .name = "Intel ICH2 ACPI",
    .internal_name = "acpi_intel_ich2",
    .flags = DEVICE_PCI,
    .local = VEN_INTEL_ICH2,
    .init = acpi_init,
    .close = acpi_close,
    .reset = acpi_reset,
    { .available = NULL },
    .speed_changed = acpi_speed_changed,
    .force_redraw = NULL,
    .config = NULL
};
