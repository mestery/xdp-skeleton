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
	"strconv"
	"strings"

	"github.com/dropbox/goebpf"
	"github.com/vishvananda/netlink"
)

const maxprogs int = 16

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

func LoadJmpMap(bpf goebpf.System, jmp goebpf.Map) error {
	var id string

	fmt.Printf("Loading tail call jumps\n")

	for _, prog := range bpf.GetPrograms() {
		fmt.Printf("Loading program name %s\n", prog.GetName())
		name := prog.GetName()
		_, err := fmt.Sscanf(name, "xdp_skeleton_%s", &id)
		if err != nil {
			fmt.Printf("Error scanning %s: %s\n", err, name)
			continue
		}
		intid, err := strconv.Atoi(id)
		if err != nil {
			fmt.Printf("Error converting string\n")
			return err
		}
		if intid > maxprogs {
			panic("Reached maximum number of programs, failing")
		}
		fmt.Printf("Looking at program slot (%d)\n", intid)
		if id != "0" {
			err = prog.Load()
			if err != nil {
				fmt.Printf("Error loading program %s:\n", name)
				//fmt.Printf("%s\n", err)
				panic(err)
			}
			fd := prog.GetFd()
			fmt.Printf("Converting string (%s)\n", id)

			fmt.Printf("Trying to insert FD (%d) into position (%d) in jmp_map\n", uint32(fd), intid)

			if err := jmp.Upsert(intid, uint32(fd)); err != nil {
				panic(err)
			}
		}
	}
	return nil
}

func FetchMaps(prog goebpf.System) (devMap, jmp goebpf.Map, err error) {
	devMap = prog.GetMapByName("if_redirect")
	if devMap == nil {
		err = fmt.Errorf("failed fetching map if_redirect from program.\n")
	}
	jmp = prog.GetMapByName("jmp_map")
	if jmp == nil {
		err = fmt.Errorf("failed fetching map jmp_map from program.\n")
	}

	return
}

func DumpEbpfInfo(bpf goebpf.System, elf string) {
	fmt.Printf("Dumping info for ELF file %s\n", elf)
	fmt.Println("Maps:")
	for _, item := range bpf.GetMaps() {
		m := item.(*goebpf.EbpfMap)
		fmt.Printf("\t%s: %v, Fd %v\n", m.Name, m.Type, m.GetFd())
	}
	fmt.Println("\nPrograms:")
	for _, prog := range bpf.GetPrograms() {
		fmt.Printf("\t%s: %v, size %d, license \"%s\"\n",
			prog.GetName(), prog.GetType(), prog.GetSize(), prog.GetLicense(),
		)
	}
	fmt.Println()

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

	DumpEbpfInfo(bpf, xdpProgFile)

	devMap, jmp, err := FetchMaps(bpf)
	if err != nil {
		panic(err)
	}

	LoadJmpMap(bpf, jmp)

	xdpProg := bpf.GetProgramByName("xdp_skeleton")
	if xdpProg == nil {
		panic(fmt.Sprintf("Could not find xdp_skeleton program in %s", xdpProgFile))
	}
	if err := xdpProg.Load(); err != nil {
		panic(err)
	}
	fmt.Printf("Attaching program %s\n", xdpProg.GetName())
	if err := AttachXdp(xdpProg, devMap, intfList); err != nil {
		panic(err)
	}
}
