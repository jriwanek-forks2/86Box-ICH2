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

add_library(sio OBJECT sio_ali5123.c sio_nsc366.c sio_smc665.c)

if(SIO_DETECT)
    target_sources(sio PRIVATE sio_detect.c)
endif()
