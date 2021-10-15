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

#ifndef __PARSERS_H
#define __PARSERS_H

#include <stddef.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>

struct hdr_cursor {
    void *pos;
};

static __always_inline int parse_eth_header(struct hdr_cursor *nh, void *data_end, struct ethhdr **ethhdr)
{
    struct ethhdr *eth = nh->pos;
    int hdrsize = sizeof(*eth);

    if (nh->pos + hdrsize <= data_end) {
        nh->pos += hdrsize;
        *ethhdr = eth;
        return 0;
    }

    return -1;
}

static __always_inline int parse_ip_header(struct hdr_cursor *nh, void *data_end, struct iphdr **ip_header) {
    struct iphdr *ip = nh->pos;
    int hdrsize = sizeof(*ip);

    if (nh->pos + hdrsize <= data_end) {
        nh->pos += hdrsize;
        *ip_header = ip;
        return 0;
    }

    return -1;
}

static __always_inline int parse_headers_ip(struct hdr_cursor *nh, void *data_end, struct ethhdr **eth_header, struct iphdr **iphdr) {
    return parse_eth_header(nh, data_end, eth_header) || parse_ip_header(nh, data_end, iphdr);
}

static __always_inline int parse_udp_header(struct hdr_cursor *nh, void *data_end, struct udphdr **udp_header) {
    struct udphdr *udp = nh->pos;
    int hdrsize = sizeof(*udp);

    if (nh->pos + hdrsize <= data_end) {
        nh->pos += hdrsize;
        *udp_header = udp;
        return 0;
    }

    return -1;
}

static __always_inline int parse_headers_udp(struct hdr_cursor *nh, void *data_end, struct ethhdr **eth_header, struct iphdr **ip_header, struct udphdr **udp_header) {
    return parse_eth_header(nh, data_end, eth_header) || parse_ip_header(nh, data_end, ip_header) || parse_udp_header(nh, data_end, udp_header);;
}

#endif /* __PARSERS_H */
