<h1>86Box-ICH2</h1>


<p align="center"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/a/ab/Intel_ICH2.jpg/240px-Intel_ICH2.jpg"></p>

<p align="center">This is a fork of 86Box that tries to emulate the Intel ICH, more specifically the Intel ICH2.</p>

<h2>Whats ICH2</h2>
The ICH2 is a southbridge released by Intel in the early 2000's. It came incorporated with many peripherals like USB, Audio & IDE. The ICH series in general also got rid of the ISA bus replacing it with the not so different LPC bus.

<h2>Emulation details</h2>
The general chipset is still very early in development and not complete. Yet more to come.

Northbridge we emulate: Intel i815EP

|Northbridge Portion|Status|Notes                                                            |
|-----------------|------|-----------------------------------------------------------------|
|i815EP MCH       |✅   |None                                                              |
|i815EP AGP Bridge|✅   |Uses the standard 86Box PCI Bridge code with slight modifications.|

Super I/O we emulate: National Semiconductor NSC366(PC87366)

|Super I/O Portion|Status|Notes                             |
|-----------------|------|----------------------------------|
|NSC366 FDC        |✅   |None                              |
|NSC366 UART Serial|✅   |None                              |
|NSC366 LPT        |✅   |86Box doesn't emulate LPT modes   |
|NSC366 HWM        |⚠️   |Voltages & Temperatures are broken|

Motherboards: 2 Boards handled

|Motherboards     |BIOS                          |Status|Notes                                                               |
|-----------------|------------------------------|------|--------------------------------------------------------------------|
|Tyan Tomcat i815T|AMIBIOS 7 (AMI Home BIOS fork)|✅   |May hang on setup if you press the DEL key a lot.                    |
|Biostar M6TSS    |AwardBIOS 6.00PG              |⚠️   |CD boot bugs. 86Box related? Cable Detection probably handled by GPIO|

Southbridge we emulate: Intel ICH2 Desktop

Emulation details
|Chipset Portion|Status|Notes                                                            |
|---------------|------|-----------------------------------------------------------------|
|ICH2 Hub       |✅   |Uses the standard 86Box PCI Bridge code with slight modifications.|
|ICH2 LPC       |⚠️   |GPIO is unimplemented. ACPI needs more work. 86Box lacks APIC.    |
|ICH2 IDE       |✅   |None                                                              |
|ICH2 SMBus     |✅   |Minimal things require to be implemented.                         |
|ICH2 USB       |⚠️   |86Box doesn't emulate USB. Recommended to disable it to save IRQ's|
|ICH2 AC'97     |❌   |AC'97 WILL NOT be emulated anytime                                |
|ICH2 DMA       |✅   |None                                                              |
|ICH2 Peripherals|✅  |FWH functionality isn't utilized by any of our boards.            |

<h2>FAQ</h2>

* What about Pentium III if we target the i8xx series?
    - The Pentium III IPC is way higher than the IPC of the Pentium II. Already the 86Box dynarec is way too slow even for a Pentium at some cases. 86Box-ICH2 doesn't target Pentium III emulation at all.

* What about a PR on official 86Box
    - Not gonna happen.

* Other board implementations?
    - The ASUS CUSL2-C is planned but for now, other boards are off the scope.

* Releases, builds, instructions?
    - This is mostly an experimental fork done by me on leisure time. There's no plan for any release build. If you know how to build 86Box, you can build 86Box-ICH2.

<h2>More things and details are yet to come!</h2>
<h3>This fork has nothing to do with mainstream 86Box! Do not ask or promote it on their issues panel or elsewhere!</h3>