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
