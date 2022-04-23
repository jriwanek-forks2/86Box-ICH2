<h1>86Box-ICH2</h1>


<p align="center"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/a/ab/Intel_ICH2.jpg/240px-Intel_ICH2.jpg"></p>

<p align="center">This is a fork of 86Box that tries to emulate the Intel ICH, more specifically the Intel ICH2.</p>

<h2>Whats ICH2</h2>
The ICH2 is a southbridge released by Intel in the early 2000's. It came incorporated with many peripherals like USB, Audio & IDE. The ICH series in general also got rid of the ISA bus replacing it with the not so different LPC bus.

<h2>Emulation details</h2>
The general chipset is still very early in development and not complete. Yet more to come.

Northbridge we emulate: Intel i815EP

Super I/O we emulate: National Semiconductor NSC366(PC87366)

Motherboards: Tyan S2080, Biostar M6TSS

BIOS: AMIBIOS 7 Home BIOS Fork(Tyan S2080), Award 6.00PG(Biostar M6TSS)

Emulation details
|Chipset Portion|Status|Notes                                                            |
|---------------|------|-----------------------------------------------------------------|
|ICH2 Hub       |✅   |Uses the standard 86Box PCI Bridge code with slight modifications.|
|ICH2 LPC       |⚠️   |GPIO is unimplemented. ACPI needs more work. 86Box lacks APIC.    |
|ICH2 IDE       |✅   |None                                                              |
|ICH2 SMBus     |✅   |Minimal things require to be implemented.                         |
|ICH2 USB       |⚠️   |86Box doesn't emulate USB. Recommended to disable it to save IRQ's|
|ICH2 AC'97     |❌   |AC'97 WILL NOT be emulated anytime                                |
|ICH2 DMA       |⚠️   |Needs more research                                               |
|ICH2 Peripherals|✅  |FWH functionality isn't utilized by any of our boards.            |

|Super I/O Portion|Status|Notes                             |
|-----------------|------|----------------------------------|
|NSC366 FDC       |✅    |None                              |
|NSC366 UART Serial|✅   |None                              |
|NSC366 LPT        |✅   |86Box doesn't emulate LPT modes   |
|NSC366 HWM        |⚠️   |Voltages & Temperatures are broken|

<h2>FAQ</h2>

* What about Pentium III if we target the i8xx series?
    - The Pentium III IPC is way higher than the IPC of the Pentium II. Already the 86Box dynarec is way too slow even for a Pentium at some cases. 86Box-ICH2 doesn't target Pentium III emulation at all.

* What about a PR on official 86Box
    - Not gonna happen.

* Other board implementations?
    - The ASUS CUSL2-C is planned but for now, other boards are off the scope.

<h2>More things and details are yet to come!</h2>