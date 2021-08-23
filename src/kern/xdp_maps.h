/* SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-3-clause) */
/*
 * Copyright (c) 2021, Kyle Mestery
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __XDP_MAPS_H
#define __XDP_MAPS_H

#include "bpf_helpers.h"

/* This is the data record stored in the map */
struct datarec {
    __u64 rx_packets;
    __u64 rx_bytes;
};

#ifndef XDP_ACTION_MAX
#define XDP_ACTION_MAX (XDP_REDIRECT + 1)
#endif

BPF_MAP_DEF(stats_map) = {
    .map_type        = BPF_MAP_TYPE_PERCPU_ARRAY,
    .key_size        = sizeof(__u32),
    .value_size      = sizeof(struct datarec),
    .max_entries     = XDP_ACTION_MAX,
    .persistent_path = "/sys/fs/bpf/stats_map",
};
BPF_MAP_ADD(stats_map);

/* XDP enabled TX ports for redirect map */
BPF_MAP_DEF(if_redirect) = {
    .map_type    = BPF_MAP_TYPE_DEVMAP,
    .key_size    = sizeof(__u32),
    .value_size  = sizeof(__u32),
    .max_entries = 64,
    .persistent_path = "/sys/fs/bpf/if_redirect",
};
BPF_MAP_ADD(if_redirect);

/* The maximum number of eBPF programs we support for tail call map */
#define MAX_SNI_PROGS           16

/* Cache between tail calls */
struct tail_call_cache {
    size_t                      protocol;
} __attribute((packed));

BPF_MAP_DEF(jmp_map) = {
    .map_type           = BPF_MAP_TYPE_PROG_ARRAY,
    .key_size           = sizeof(__u32),
    .value_size         = sizeof(__u32),
    .max_entries        = MAX_SNI_PROGS,
    .persistent_path    = "/sys/fs/bpf/jmp_map",
};
BPF_MAP_ADD(jmp_map);

BPF_MAP_DEF(tail_call) = {
    .map_type           = BPF_MAP_TYPE_PERCPU_ARRAY,
    .key_size           = sizeof(__u32),
    .value_size         = sizeof(struct tail_call_cache),
    .max_entries        = 64,
    .persistent_path    = "/sys/fs/bpf/tail_call",
};
BPF_MAP_ADD(tail_call);

#endif /* __XDP_MAPS_H */
