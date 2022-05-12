#
# 86Box     A hypervisor and IBM PC system emulator that specializes in
#           running old operating systems and software designed for IBM
#           PC systems and compatibles from 1981 through fairly recent
#           system designs based on the PCI bus.
#
#           This file is part of the 86Box distribution.
#
#           CMake build script.
#
# Authors:  David Hrdlička, <hrdlickadavid@outlook.com>
#
#           Copyright 2020,2021 David Hrdlička.
#

add_library(mch OBJECT machine.c machine_table.c at_base.c boards_discrete.c boards_ich2.c boards_sis_471.c
            m_ps2_mca.c)

if(OPEN_AT)
    target_compile_definitions(mch PRIVATE USE_OPEN_AT)
endif()
