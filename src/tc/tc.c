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

#include <linux/if_ether.h>
#include <linux/stddef.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/pkt_cls.h>
#include <linux/tcp.h>
#include <bpf_helpers.h>

#include "geneve.h"
#include "maps.h"
#include "parsers.h"
#include "tc_helpers.h"

static __always_inline
__u32 tc_stats_record_action(struct __sk_buff *skb, __u32 action)
{
    if (action >= TC_ACT_VALUE_MAX)
        return TC_ACT_SHOT;

    /* Lookup in kernel BPF-side return pointer to actual data record */
    struct datarec *rec = bpf_map_lookup_elem(&tc_stats_map, &action);
    if (!rec)
        return TC_ACT_SHOT;

    /*
     * BPF_MAP_TYPE_PERCPU_ARRAY returns a data record specific to current
     * CPU and TC hooks runs under Softirq, which makes it safe to update
     * without atomic operations.
     */
    rec->rx_packets++;
    rec->rx_bytes += skb->len;

    return action;
}

SEC("tc_prog")
int tc_prog_example(struct __sk_buff *skb)
{
    __u32 action = TC_ACT_OK;
    void *data_end = (void *)(unsigned long long)skb->data_end;
    void *data = (void *)(unsigned long long)skb->data;
    struct ethhdr *eth_header;
    struct iphdr *ip_header;
    struct hdr_cursor nh = {};
    struct bpf_tunnel_key key;
    struct geneve_opt gopt = {};

    nh.pos = data;

    if (parse_headers_ip(&nh, data_end, &eth_header, &ip_header) != 0) {
        action = TC_ACT_PIPE; /* will iterate to the next action, if available */
        goto out;
    }

    /* Only want IP packets here for the example */
    if (eth_header->h_proto != __constant_htons(ETH_P_IP)) {
        action = TC_ACT_PIPE; /* will iterate to the next action, if available */
        goto out;
    }

    if (bpf_skb_get_tunnel_key(skb, &key, sizeof(key), 0) < 0) {
        bpf_printk("Failed looking up GENEVE VNI\n");
        action = TC_ACT_PIPE; /* will iterate to the next action, if available */
        goto out;
    }

    if (bpf_skb_get_tunnel_opt(skb, &gopt, sizeof(gopt)) < 0) {
        bpf_printk("Failed looking up GENEVE options\n");
        action = TC_ACT_PIPE; /* will iterate to the next action, if available */
        goto out;
    }

    bpf_printk("geneve opts: Class %x, type: %x, len: %x\n",
            gopt.opt_class, gopt.proto_type, gopt.opt_len);

out:
    return tc_stats_record_action(skb, action);
}

char __license[] SEC("license") = "GPL";
