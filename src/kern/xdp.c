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

#define __linux__
#include <stddef.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <bpf_helpers.h>
#include <bpf.h>

#include "xdp_maps.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

static __always_inline
__u32 xdp_stats_record_action(struct xdp_md *ctx, __u32 action)
{
    if (action >= XDP_ACTION_MAX)
        return XDP_ABORTED;

    /* Lookup in kernel BPF-side return pointer to actual data record */
    struct datarec *rec = bpf_map_lookup_elem(&stats_map, &action);
    if (!rec)
        return XDP_ABORTED;

    /*
     * BPF_MAP_TYPE_PERCPU_ARRAY returns a data record specific to current
     * CPU and XDP hooks runs under Softirq, which makes it safe to update
     * without atomic operations.
     */
    rec->rx_packets++;
    rec->rx_bytes += (ctx->data_end - ctx->data);

    return action;
}

SEC("xdp")
int xdp_skeleton(struct xdp_md *ctx)
{
    __u32 action = XDP_PASS;

    return xdp_stats_record_action(ctx, action);
}

char _license[] SEC("license") = "GPL";
