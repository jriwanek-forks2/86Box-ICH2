#
# 86Box    A hypervisor and IBM PC system emulator that specializes in
#          running old operating systems and software designed for IBM
#          PC systems and compatibles from 1981 through fairly recent
#          system designs based on the PCI bus.
#
#          This file is part of the 86Box distribution.
#
#          CMake build script.
#
# Authors: David Hrdlička, <hrdlickadavid@outlook.com>
#
#          Copyright 2020,2021 David Hrdlička.
#

add_library(dev OBJECT bugger.c cassette.c cartridge.c hasp.c hwm.c hwm_nsc366.c ibm_5161.c
    intel_ich2_acpi.c intel_ich2_gpio.c intel_ich2_trap.c
    isamem.c isartc.c ../lpt.c pci_bridge.c
    postcard.c serial.c clock_ics9xxx.c isapnp.c i2c.c i2c_gpio.c
    smbus_piix4.c keyboard.c keyboard_at.c
    mouse.c mouse_bus.c mouse_serial.c mouse_ps2.c
    tco.c)

if(ISAMEM_RAMPAGE)
    target_compile_definitions(dev PRIVATE USE_ISAMEM_RAMPAGE)
endif()

if(ISAMEM_IAB)
    target_compile_definitions(dev PRIVATE USE_ISAMEM_IAB)
endif()

if(ISAMEM_BRAT)
    target_compile_definitions(dev PRIVATE USE_ISAMEM_BRAT)
endif()

if(LASERXT)
    target_compile_definitions(dev PRIVATE USE_LASERXT)
endif()
