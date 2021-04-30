#!/bin/bash

set -x

# Unload XDP programs
ip link set dev eth1 xdp off
ip link set dev veth1 xdp off

# Remove veth5 from the namespace
ip netns exec xdp ip l s veth2 netns 1

# Delete the veth pair
ip link delete veth1

# Delete the namespace
ip netns delete xdp
