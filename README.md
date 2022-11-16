<h1>86Box-ICH2</h1>


<p align="center"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/a/ab/Intel_ICH2.jpg/240px-Intel_ICH2.jpg"></p>

<<<<<<< HEAD
<p align="center">This is a fork of 86Box that tries to emulate the Intel ICH, more specifically the Intel ICH2 but also a few legacy minor boards</p>

<h2>Whats ICH2</h2>
The ICH2 is a southbridge released by Intel in the early 2000's. It came incorporated with many peripherals like USB, Audio & IDE. The ICH series in general also got rid of the ISA bus replacing it with the not so different LPC bus.
=======
Minimum system requirements and recommendations
-----------------------------------------------
* Intel Core 2 or AMD Athlon 64 processor
* Windows version: Windows 7 Service Pack 1, Windows 8.1 or Windows 10
* Linux version: Ubuntu 16.04, Debian 9.0 or other distributions from 2016 onwards
* macOS version: macOS High Sierra 10.13
* 4 GB of RAM

Performance may vary depending on both host and guest configuration. Most emulation logic is executed in a single thread; therefore, systems with better IPC (instructions per clock) generally should be able to emulate higher clock speeds.
>>>>>>> upstream/master

<h2>Emulation details</h2>
The fork emulates an Intel ICH2 desktop board with the basic components it needs. There are a few quirks that haven't been implemented or fixed yet. Additionally there's is one 386SX, few 486 and a Intel 430FX chipset for some basic 386SX to Pentium needs although mainstream 86Box covers that.

<<<<<<< HEAD
<h2>Issues within development</h2>
Intel i8xx series in general is way beyond 86Box's scope. It's already stated by themselves to never target it. If they're greedy they will. 86Box was never meant to go beyond the i486 environment and it can be proven from the crippling performance someone can meet on mid-late i586 or general i686 setups. There were problems encountered within development.
=======
It is also possible to use 86Box on its own with the `--vmpath`/`-P` command line option.
>>>>>>> upstream/master

* SMBus craze
    - Caused by the dynarec potentially. Setting the CPU beyond 66Mhz on some boards, it can cause the BIOS to send random commands through the SMBus and generally halt. The current board doesn't have that.

    - By analysis it's caused due to the emulated timer on 86Box being severely broken for late machine tasks.

* This works, this bugs, this doesn't
    - Tons of variety can be found within late machines. Different configurations, Super I/O's, I2C chips and anything that could come to anyones imagination. One discovery was the lack of FWH programming some BIOS do, including the introduced one. FWH pins are undocumented where they talk to so although the functionality can be implemented, the position to where the functionality is triggered upon remains unknown. Also other machines may fail to boot for a number of reasons, not one man can solve them all.

* Crippling recompiler
    - The 86Box recompiler is a mess. It can't meet sometimes the intended Pentium speeds and can't do i686 great at all. i8xx being at i686, gets on the same boiler of crippling out.

* The accuracy "paranoia"
    - 86Box advertises itself to be very accurate but accuracy comes at a heavy cost. The situation of this worsens as the emulator tries to target accuracy more and more. It might delight 8088 to 286 emulation but 386 and later begin to suffer heavily. Good machines which handled Pentium's fine on PCem struggle at 486's. If you want mid to late 90's performance on games, you're well off on PCem.

Generally 86Box-ICH2 was developed as a hobby fork, not with any huge views and plans.

<h2>FAQ</h2>

* What about Pentium III or even Pentium IV if we target the i8xx series?
    - The Pentium III IPC is way higher than the IPC of the Pentium II. Let alone Pentium IV which although slower due to NetBurst, it also comes with features like XAPIC which no way I got the knowledge to implement myself. Already the 86Box dynarec is way too slow even for a Pentium at some cases. 86Box-ICH2 doesn't target Pentium III or IV emulation at all!

* What about a PR on official 86Box
    - Never going to happen at all. Do not banter them at all to get it inside either.

* Other board/chipset implementations?
    - Any board implementations are off the scope. There are one to two boards planned but for now, none was added to the source. Other than the ICH2 board there are a few 486 and Pentium motherboards meant mostly for leisure testing & playing while not using mainstream 86Box.

* Releases, builds, instructions?
    - This is mostly an experimental fork done by me on leisure time. There's no plan for any release build. If you know how to build 86Box, you can build 86Box-ICH2.

* Why AC'97 isn't implemented?
    - Sound handling of 86Box is completely undocumented but easy to understand. Problem is Intel AC'97 utilizes 6 Channels 86Box can't exactly do. 86Box handles two channels maximum although you can configure the Intel AC'97 to run this way. Our emulated board, the Tyan S2080 however, being a workstation board doesn't utilize the AC'97 through off it's completely ignored.

<h2>More things and details are yet to come!</h2>
<h3>This fork has nothing to do with mainstream 86Box! Do not ask or promote it on their issues panel or their IRC/Discord servers!</h3>
ICH2 Picture courtesy of Wikimedia. "ICH2" is courtesy of Intel.