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

add_library(chipset OBJECT intel_815ep.c intel_ich2.c intel_430fx.c intel_piix.c sarc_2016a.c sis_471.c symphony_haydn.c ../ioapic.c)

if(OLIVETTI)
    target_sources(chipset PRIVATE olivetti_eva.c)
endif()
