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

#define DEBUG
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

/* Used to parse the packet across calls */
struct hdr_cursor {
    struct xdp_md *ctx;
    void *pos;
};

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
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    size_t data_len = ctx->data_end - ctx->data;
    struct ethhdr *eth_header;
    struct iphdr *ip_header;
    struct hdr_cursor nh = {};

    bpf_printk("xdp_skeleton: Entering function\n");

    nh.ctx = ctx;
    nh.pos = data;

    eth_header = data;
    if ((void *)eth_header + sizeof(*eth_header) > data_end) {
        action = XDP_PASS;
        goto out;
    }

    __u16 h_proto = eth_header->h_proto;

    /* anything that is not IPv4 (including ARP) goes up to the kernel */
    if (h_proto != __constant_htons(ETH_P_IP)) {
        action = XDP_PASS;
        goto out;
    }

    ip_header = (void *)(eth_header + 1);
    if ((void *)ip_header + sizeof(*ip_header) > data_end) {
        action = XDP_PASS;
        goto out;
    }

    /* Example of tail-calling into another XDP program */
    __u32 tckey = bpf_get_smp_processor_id();;
    struct tail_call_cache *tailcall;

    tailcall = bpf_map_lookup_elem(&tail_call, &tckey);
    if (tailcall) {
        tailcall->protocol = __constant_ntohs(ip_header->protocol);
    } else {
        action = XDP_DROP;
        goto out;
    }

    if (bpf_map_update_elem(&tail_call, &tckey, tailcall, BPF_ANY) < 0) {
        action = XDP_DROP;
        goto out;
    }

    /* Tail call into hashing program */
    bpf_tail_call(ctx, &jmp_map, 1);

out:
    return xdp_stats_record_action(ctx, action);
}

SEC("xdp")
int xdp_skeleton_1(struct xdp_md *ctx)
{
    __u32 action = XDP_PASS;
    struct tail_call_cache *tailcall;
    __u32 tckey = bpf_get_smp_processor_id();;
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    size_t data_len = ctx->data_end - ctx->data;
    struct ethhdr *eth_header;
    struct iphdr *ip_header;
    struct hdr_cursor nh = {};

    bpf_printk("xdp_skeleton_1: Entering function\n");

    nh.ctx = ctx;
    nh.pos = data;

    eth_header = data;
    if ((void *)eth_header + sizeof(*eth_header) > data_end) {
        action = XDP_PASS;
        goto out;
    }

    __u16 h_proto = eth_header->h_proto;

    /* anything that is not IPv4 (including ARP) goes up to the kernel */
    if (h_proto != __constant_htons(ETH_P_IP)) {
        action = XDP_PASS;
        goto out;
    }

    ip_header = (void *)(eth_header + 1);
    if ((void *)ip_header + sizeof(*ip_header) > data_end) {
        action = XDP_PASS;
        goto out;
    }

    tailcall = bpf_map_lookup_elem(&tail_call, &tckey);

    if (!tailcall) {
        return xdp_stats_record_action(ctx, XDP_DROP);
    }

    /* Validate the data passed in the tailcall is accurate */
    if (ip_header->protocol == __constant_htons(tailcall->protocol)) {
        bpf_printk("xdp_skeleton_1: Validated protocol (%x) successfully\n", tailcall->protocol);
    } else {
        bpf_printk("xdp_skeleton_1: Protocol mismatch\n");
        goto out;
    }

    /* Tailcall into last program */
    bpf_tail_call(ctx, &jmp_map, 2);

out:
    return xdp_stats_record_action(ctx, action);
}

SEC("xdp")
int xdp_skeleton_2(struct xdp_md *ctx)
{
    __u32 action = XDP_PASS;

    bpf_printk("xdp_skeleton_2: Entering function\n");

    return xdp_stats_record_action(ctx, action);
}

char _license[] SEC("license") = "GPL";
