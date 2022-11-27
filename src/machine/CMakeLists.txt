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

add_library(mch OBJECT machine.c machine_table.c at_base.c boards_ich2.c boards_sis_471.c boards_sarc_2016a.c boards_symphony_haydn.c boards_piix.c
m_at_t3100e.c m_at_t3100e_vid.c)

if(AN430TX)
    target_compile_definitions(mch PRIVATE USE_AN430TX)
endif()

if(DESKPRO386)
    target_compile_definitions(mch PRIVATE USE_DESKPRO386)
endif()

if(LASERXT)
    target_sources(mch PRIVATE m_xt_laserxt.c)
    target_compile_definitions(mch PRIVATE USE_LASERXT)
endif()

if(OPEN_AT)
    target_compile_definitions(mch PRIVATE USE_OPEN_AT)
endif()
