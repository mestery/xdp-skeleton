#!/bin/bash

# Debug
set -x

# Turn off reverse path filtering
for i in /proc/sys/net/ipv4/conf/*/rp_filter ; do echo 0 > $i ; done

# Create the namespace
ip netns add xdp

# Setup the veth pair linking xdp namespace to host
ip link add veth1 type veth peer name veth2
ip link set veth2 netns xdp
ip add add 10.2.2.1/24 dev veth1
ip link set dev veth1 up addr 52:54:27:27:27:25
ip netns exec xdp ip addr add 10.2.2.2/24 dev veth2
ip netns exec xdp ip link set dev veth2 up addr 52:54:27:27:27:26

# Set MTUs
ip link set dev veth1 mtu 9000
ip netns exec xdp ip link set dev veth2 mtu 9000

# Run this on the VM itself
sysctl -w net.core.rmem_default=134217728
sysctl -w net.core.rmem_max=268435456
sysctl -w net.core.wmem_default=134217728
sysctl -w net.core.wmem_max=268435456
sysctl -w net.ipv4.tcp_rmem="134217728 134217728 134217728"
sysctl -w net.ipv4.tcp_wmem="134217728 134217728 134217728"
sysctl -w net.ipv4.tcp_mem="134217728 134217728 134217728"
sysctl -w net.ipv4.udp_mem="134217728 134217728 134217728"
sysctl -w net.ipv4.udp_rmem_min=134217728
sysctl -w net.ipv4.udp_wmem_min="134217728 134217728 134217728"

# Bump up UDP buffer sizes
ip netns exec xdp sysctl -w net.ipv4.tcp_rmem="134217728 134217728 134217728"
ip netns exec xdp sysctl -w net.ipv4.tcp_wmem="134217728 134217728 134217728"
ip netns exec xdp sysctl -w net.ipv4.udp_rmem_min=134217728
ip netns exec xdp sysctl -w net.ipv4.udp_wmem_min="134217728 134217728 134217728"

# Turn off TSO
ethtool -K veth1 tso off
ip netns exec xdp ethtool -K veth2 tso off
