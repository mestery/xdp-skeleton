/* * Copyright (c) 2021, Kyle Mestery
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
package main

import (
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"

	"github.com/dropbox/goebpf"
)

type DataRec struct {
	Rx_packets uint64
	Rx_bytes   uint64
}

func main() {
	var xdpMapFile, xdpMap string

	flag.StringVar(&xdpMapFile, "file", "", "xdp map to open")
	flag.StringVar(&xdpMap, "m", "", "map name to dump, for example stats_map")
	flag.Parse()

	if xdpMapFile == "" {
		panic("Please enter a valid map filename.")
	}

	mapmap, err := goebpf.NewMapFromExistingMapByPath(xdpMapFile)
	if err != nil {
		panic(err)
	}

	fmt.Printf("Found map %s of type %s\n", mapmap.Name, mapmap.Type)

	var datarec_aborted, datarec_drop, datarec_pass, datarec_tx, datarec_redirect DataRec

	xdp_aborted, err := mapmap.Lookup(0)
	if err != nil {
		panic(err)
	}
	if err := binary.Read(bytes.NewBuffer(xdp_aborted[:]), binary.LittleEndian, &datarec_aborted); err != nil {
		panic(err)
	}

	xdp_drop, err := mapmap.Lookup(1)
	if err != nil {
		panic(err)
	}
	if err := binary.Read(bytes.NewBuffer(xdp_drop[:]), binary.LittleEndian, &datarec_drop); err != nil {
		panic(err)
	}

	xdp_pass, err := mapmap.Lookup(2)
	if err != nil {
		panic(err)
	}
	if err := binary.Read(bytes.NewBuffer(xdp_pass[:]), binary.LittleEndian, &datarec_pass); err != nil {
		panic(err)
	}

	xdp_tx, err := mapmap.Lookup(3)
	if err != nil {
		panic(err)
	}
	if err := binary.Read(bytes.NewBuffer(xdp_tx[:]), binary.LittleEndian, &datarec_tx); err != nil {
		panic(err)
	}

	xdp_redirect, err := mapmap.Lookup(4)
	if err != nil {
		panic(err)
	}
	if err := binary.Read(bytes.NewBuffer(xdp_redirect[:]), binary.LittleEndian, &datarec_redirect); err != nil {
		panic(err)
	}

	fmt.Printf("    XDP_ABORTED:  %d packets / %d bytes\n", datarec_aborted.Rx_packets, datarec_aborted.Rx_bytes)
	fmt.Printf("    XDP_DROP:     %d packets / %d bytes\n", datarec_drop.Rx_packets, datarec_drop.Rx_bytes)
	fmt.Printf("    XDP_PASS:     %d packets / %d bytes\n", datarec_pass.Rx_packets, datarec_pass.Rx_bytes)
	fmt.Printf("    XDP_TX:       %d packets / %d bytes\n", datarec_tx.Rx_packets, datarec_tx.Rx_bytes)
	fmt.Printf("    XDP_REDIRECT: %d packets / %d bytes\n", datarec_redirect.Rx_packets, datarec_redirect.Rx_bytes)
}
