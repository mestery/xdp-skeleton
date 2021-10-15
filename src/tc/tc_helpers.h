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

#ifndef __TC_HELPERS_H
#define __TC_HELPERS_H

#include <stddef.h>

/*
 * This file contains definitions which typically live here:
 *
 * https://github.com/torvalds/linux/blob/master/include/uapi/linux/bpf.h
 *
 * Since we're using goebpf as our eBPF library, and some of these don't exist
 * in the goebpf bpf_helpers.h file found here:
 *
 * https://github.com/dropbox/goebpf/blob/master/bpf_helpers.h
 *
 * We can add the outliers here until we merge them back upstream into
 * goebpf.
 */

struct bpf_tunnel_key {
    __u32 tunnel_id;
    union {
        __u32 remote_ipv4;
        __u32 remote_ipv6[4];
    };
    __u8 tunnel_tos;
    __u8 tunnel_ttl;
    __u16 tunnel_ext;       /* Padding, future use. */
    __u32 tunnel_label;
};

#endif /* __TC_HELPERS_H */
