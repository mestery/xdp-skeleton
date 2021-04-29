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
package main

import (
	"flag"
	"fmt"
	"strings"

	"github.com/dropbox/goebpf"
	"github.com/vishvananda/netlink"
)

func AttachXdp(x goebpf.Program, devMap goebpf.Map, iList []string) error {
	for _, intf := range iList {
		fmt.Printf("Looking for interface %s\n", intf)
		ifData, err := netlink.LinkByName(intf)
		if err != nil {
			fmt.Printf("1\n")
			return err
		}
		ifIndex := ifData.Attrs().Index
		if err := x.Attach(&goebpf.XdpAttachParams{Interface: intf, Mode: goebpf.XdpAttachModeSkb}); err != nil {
			return err
		}
		fmt.Printf("Inserting ifIndex %d\n", ifIndex)
		if err := devMap.Upsert(ifIndex, ifIndex); err != nil {
			return err
		}
		fmt.Printf("XDP code attached to %s. To turn it off, use `ip -f link set dev %s xdp off`\n", intf)
	}
	return nil
}

func FetchMaps(prog goebpf.System) (devMap goebpf.Map, err error) {
	devMap = prog.GetMapByName("if_redirect")
	if devMap == nil {
		err = fmt.Errorf("failed fetching map if_redirect from program.\n")
	}
	return
}

func main() {
	var xdpProgFile, xdpIntf string

	flag.StringVar(&xdpProgFile, "file", "", "xdp binary to attach")
	flag.StringVar(&xdpIntf, "i", "", "interfaces to attach xdp code to. comma separated")
	flag.Parse()

	if xdpProgFile == "" {
		panic("Please enter a valid filename.")
	}
	if xdpIntf == "" {
		panic("Please enter a valid interface name.")
	}
	intfList := strings.Split(xdpIntf, ",")

	bpf := goebpf.NewDefaultEbpfSystem()
	if err := bpf.LoadElf(xdpProgFile); err != nil {
		panic(err)
	}
	xdpProg := bpf.GetProgramByName("xdp_skeleton")
	if xdpProg == nil {
		panic(fmt.Sprintf("Could not find xdp_skeleton program in %s", xdpProgFile))
	}
	devMap, err := FetchMaps(bpf)
	if err != nil {
		panic(err)
	}
	if err := xdpProg.Load(); err != nil {
		panic(err)
	}
	fmt.Printf("Attaching program %s\n", xdpProg.GetName())
	if err := AttachXdp(xdpProg, devMap, intfList); err != nil {
		panic(err)
	}
}
