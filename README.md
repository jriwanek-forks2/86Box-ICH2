<h1>86Box-ICH2</h1>


<p align="center"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/a/ab/Intel_ICH2.jpg/240px-Intel_ICH2.jpg"></p>

<p align="center">This is a fork of 86Box that tries to emulate the Intel ICH, more specifically the Intel ICH2</p>

<h2>Whats ICH2</h2>
The ICH2 is a southbridge released by Intel in the early 2000's. It came incorporated with many peripherals like USB, Audio & IDE. The ICH series in general also got rid of the ISA bus replacing it with the not so different LPC bus.

<h2>Emulation details</h2>
The general chipset is still in general terms stable but lacks a few crucial components and some functionality remains unknown. Yet more to come. Status info below don't represent the work situation accuretly, I learn more as I walkthrough it. Things might be ok but also not ok.

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

Motherboards: 3 Motherboards are emulated. 1 ICH2, 2 486. Board shows only the ICH2 ones

|Motherboards     |BIOS                          |Status|Notes                                                             |
|-----------------|------------------------------|------|------------------------------------------------------------------|
|Tyan Tomcat i815T|AMIBIOS 7 (AMI Home BIOS fork)|✅   |Keyboard may stop working at setup if you spam the DEL key on POST.|

Southbridge we emulate: Intel ICH2 Desktop

|Southbridge Portion|Status|Notes                                                            |
|-------------------|------|-----------------------------------------------------------------|
|ICH2 Hub           |✅   |Uses the standard 86Box PCI Bridge code with slight modifications.|
|ICH2 LPC           |✅   |Trap implementation is obscure.                                   |
|ICH2 IDE           |✅   |86Box IDE Bus Mastering is broken on BSD.                         |
|ICH2 SMBus         |✅   |Works fine overall. Little different from PIIX4.                  |
|ICH2 USB           |❌   |86Box doesn't emulate USB. Recommended to disable it to save IRQ's|
|ICH2 AC'97         |❌   |AC'97 WILL NOT be emulated anytime                                |
|ICH2 DMA           |✅   |None                                                              |
|ICH2 Peripherals   |✅   |FWH functionality isn't utilized by any of our boards.            |

<h2>Issues within development</h2>
Intel i8xx series in general is way beyond 86Box's scope. 86Box was never meant to go beyond the i486 environment and it can be proven from the crippling performance someone can meet on mid-late i586 or general i686 setups. There were problems encountered within development.

* SMBus craze
    - Caused by the dynarec potentially. Setting the CPU beyond 66Mhz on some boards, it can cause the BIOS to send random commands through the SMBus and generally halt. The current board doesn't have that.

* This works, this bugs, this doesn't
    - Tons of variety can be found within late machines. Different configurations, Super I/O's, I2C chips and anything that could come to anyones imagination. One discovery was the lack of FWH programming some BIOS do, including the introduced one. FWH pins are undocumented where they talk to so although the functionality can be implemented, the position to where the functionality is triggered upon remains unknown. Also other machines may fail to boot for a number of reasons, not one man can solve them all.

* Crippling recompiler
    - The 86Box recompiler is a mess. It can't meet sometimes, the intended Pentium speeds and can't do i686 great at all. i8xx being at i686, gets on the same boiler of crippling out. Forget about the C3, the workaround for the lates can't be used, at least on the configurations we got.

Generally 86Box-ICH2 was developed as a hobby fork, not with any huge eyes and plans.

<h2>FAQ</h2>

* What about Pentium III or even Pentium IV if we target the i8xx series?
    - The Pentium III IPC is way higher than the IPC of the Pentium II. Let alone Pentium IV which although slower due to NetBurst, it also comes with features like XAPIC which no way I got the knowledge to implement myself. Already the 86Box dynarec is way too slow even for a Pentium at some cases. 86Box-ICH2 doesn't target Pentium III or IV emulation at all!

* What about a PR on official 86Box
    - Never going to happen at all. Do not banter them at all to get it inside either.

* Other board/chipset implementations?
    - Any board implementations are off the scope. There are one to two boards planned but for now, none was added to the source. Other than the ICH2 board there are two 486 motherboards meant mostly for leisure testing & playing while not using mainstream 86Box.

* Releases, builds, instructions?
    - This is mostly an experimental fork done by me on leisure time. There's no plan for any release build. If you know how to build 86Box, you can build 86Box-ICH2.

* Why AC'97 isn't implemented?
    - Sheer complexity of the Audio device. ICH2 itself is way too complex by it's own. Our board doesn't have the audio capabilites of the ICH2 for a reason.

<h2>More things and details are yet to come!</h2>
<h3>This fork has nothing to do with mainstream 86Box! Do not ask or promote it on their issues panel or their IRC/Discord servers!</h3>
ICH2 Picture courtesy of Wikimedia. "ICH2" is courtesy of Intel.