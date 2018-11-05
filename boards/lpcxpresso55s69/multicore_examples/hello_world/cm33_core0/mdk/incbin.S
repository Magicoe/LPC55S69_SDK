;/*
; * Copyright 2016 Freescale Semiconductor, Inc.
; * Copyright 2016 - 2017 NXP
; *
; * All rights reserved.
; * 
; * SPDX-License-Identifier: BSD-3-Clause
; */

        AREA M0CODE, DATA, READONLY, PREINIT_ARRAY, ALIGN=3
        EXPORT m0_image_start
        EXPORT m0_image_end
m0_image_start
        INCBIN core1_image.bin
m0_image_end
        END
