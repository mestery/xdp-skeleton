/* SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-2-clause) */
/*
 * Copyright (c) 2021, Kyle Mestery
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GENEVE_H
#define __GENEVE_H

#include <stddef.h>

/* From include/net/geneve.h */
struct geneve_opt {
    __be16  opt_class;
    __u8    type;
#ifdef __LITTLE_ENDIAN_BITFIELD
    __u8    length:5;
    __u8    r3:1;
    __u8    r2:1;
    __u8    r1:1;
#else
    __u8    r1:1;
    __u8    r2:1;
    __u8    r3:1;
    __u8    length:5;
#endif
    __u8    opt_data[];
};

#define GENEVE_CRIT_OPT_TYPE (1 << 7)

struct genevehdr {
#ifdef __LITTLE_ENDIAN_BITFIELD
    __u8 opt_len:6;
    __u8 ver:2;
    __u8 rsvd1:6;
    __u8 critical:1;
    __u8 oam:1;
#else
    __u8 ver:2;
    __u8 opt_len:6;
    __u8 oam:1;
    __u8 critical:1;
    __u8 rsvd1:6;
#endif
    __be16 proto_type;
    __u8 vni[3];
    __u8 rsvd2;
    struct geneve_opt options[];
};

#endif /* __GENEVE_H */
