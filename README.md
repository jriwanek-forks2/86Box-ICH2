<h1>86Box-ICH2</h1>


<p align="center"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/a/ab/Intel_ICH2.jpg/240px-Intel_ICH2.jpg"></p>

<p align="center">This is a fork of 86Box that tries to emulate the Intel ICH, more specifically the Intel ICH2.</p>

<h2>Whats ICH2</h2>
The ICH2 is a southbridge released by Intel in the early 2000's. It came incorporated with many peripherals like USB, Audio & IDE. The ICH series in general also got rid of the ISA bus replacing it with the not so different LPC bus.

<h2>Emulation details</h2>
86Box-ICH2 only includes the Tyan S2080 motherboard. A basic workstation board lacking difficult to implement features like Audio & VGA. It comes also by par with an Intel i815E(P) MCH Bridge & a NSC 366 Super I/O. The motherboard also supports 512MB maximum & Pentium III or Mendocino Celeron CPU support.

The general chipset is still very early in development and not complete. Yet more to come

<h2>FAQ</h2>

* What about Pentium III if we target the i8xx series?
    - The Pentium III IPC is way higher than the IPC of the Pentium II. Already the 86Box dynarec is way too slow even for a Pentium at some cases. 86Box-ICH2 doesn't target Pentium III emulation at all.

* What about a PR on official 86Box
    - Not gonna happen.

* Other board implementations?
    - The ASUS CUSL2-C is planned but for now, other boards are off the scope.

<h2>More things and details are yet to come!</h2>