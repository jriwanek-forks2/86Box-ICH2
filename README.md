<h1>86Box-ICH2</h1>


<p align="center"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/a/ab/Intel_ICH2.jpg/240px-Intel_ICH2.jpg"></p>

<p align="center">This is a fork of 86Box that tries to emulate the Intel ICH, more specifically the Intel ICH2.</p>

<h2>Whats ICH2</h2>
The ICH2 is a southbridge released by Intel in the early 2000's. It came incorporated with many peripherals like USB, Audio & IDE. The ICH series in general also got rid of the ISA bus replacing it with the not so different LPC bus.

<h2>Emulation details</h2>
The general chipset is still very early in development and not complete. Yet more to come. Statuses don't represent the work situation accuretly. Thing may be ok but also not ok.

Northbridge we emulate: Intel i815EP

|Northbridge Portion|Status|Notes                                                             |
|-------------------|------|------------------------------------------------------------------|
|i815EP MCH         |✅   |None                                                              |
|i815EP AGP Bridge  |✅   |Uses the standard 86Box PCI Bridge code with slight modifications.|

Super I/O we emulate: National Semiconductor NSC366(PC87366)

|Super I/O Portion|Status|Notes                             |
|-----------------|------|----------------------------------|
|NSC366 FDC        |✅   |None                              |
|NSC366 UART Serial|✅   |None                              |
|NSC366 LPT        |✅   |86Box doesn't emulate LPT modes   |
|NSC366 HWM        |⚠️   |Voltages & Temperatures are broken|

Motherboards: 2 Boards handled (❓ Board details will come after most parts are ready).

|Motherboards     |BIOS                          |Status|Notes                       |
|-----------------|------------------------------|------|----------------------------|
|Tyan Tomcat i815T|AMIBIOS 7 (AMI Home BIOS fork)|❓   |None                        |
|Biostar M6TSL    |AwardBIOS 6.00PG              |❓   |No integrated ESS Solo & GPU|

Southbridge we emulate: Intel ICH2 Desktop

|Southbridge Portion|Status|Notes                                                            |
|-------------------|------|-----------------------------------------------------------------|
|ICH2 Hub           |✅   |Uses the standard 86Box PCI Bridge code with slight modifications.|
|ICH2 LPC           |✅   |Trap implementation is obscure.                                   |
|ICH2 IDE           |✅   |None                                                              |
|ICH2 SMBus         |✅   |None                                                              |
|ICH2 USB           |❌   |86Box doesn't emulate USB. Recommended to disable it to save IRQ's|
|ICH2 AC'97         |❌   |AC'97 WILL NOT be emulated anytime                                |
|ICH2 DMA           |✅   |None                                                              |
|ICH2 Peripherals   |✅   |FWH functionality isn't utilized by any of our boards.            |

<h2>FAQ</h2>

* What about Pentium III or even Pentium IV if we target the i8xx series?
    - The Pentium III IPC is way higher than the IPC of the Pentium II. Let alone Pentium IV which although slower due to NetBurst, it also comes with features like XAPIC which no way I got the knowledge to implement myself. Already the 86Box dynarec is way too slow even for a Pentium at some cases. 86Box-ICH2 doesn't target Pentium III or IV emulation at all!

* What about a PR on official 86Box
    - Never going to happen at all. Do not banter them at all to get it inside either.

* Other board/chipset implementations?
    - There is a plan to try getting the i820 up & running but for now, everything related to boards & chipsets are completely off the scope. Originally, 86Box-ICH2 was only meant to run one board even, not two.

* Releases, builds, instructions?
    - This is mostly an experimental fork done by me on leisure time. There's no plan for any release build. If you know how to build 86Box, you can build 86Box-ICH2.

* Why AC'97 isn't implemented?
    - Sheer complexity of the Audio device. ICH2 itself is way too complex by it's own.

<h2>More things and details are yet to come!</h2>
<h3>This fork has nothing to do with mainstream 86Box! Do not ask or promote it on their issues panel or their IRC/Discord servers!</h3>
ICH2 Picture courtesy of Wikimedia