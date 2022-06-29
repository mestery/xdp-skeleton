# XDP Skeleton

Sample repository to learn XDP code. Includes a Vagrantfile to spinup a VM to
run the XDP programs.

## How To Use This

This repository contains all the bits and pieces necessary to try out the XDP
code inside of it. You need to have Vagrant installed, and some hypvervisor
platform to use with Vagrant. Virtualbox works great here.

### Run Vagrant

```console
vagrant up
```

### Build the Code

Log into the VM:

```console
vagrant ssh
```

Now, once inside the VM, run the following commands:

```console
cd /git/src
make
```

### Load the Code

Log into the VM:

```console
vagrant ssh
```

Now, once inside the VM, run the following commands:

```console
$ sudo /git/scripts/start.sh
+ for i in /proc/sys/net/ipv4/conf/*/rp_filter
+ echo 0
+ for i in /proc/sys/net/ipv4/conf/*/rp_filter
+ echo 0
+ for i in /proc/sys/net/ipv4/conf/*/rp_filter
+ echo 0
+ for i in /proc/sys/net/ipv4/conf/*/rp_filter
+ echo 0
+ for i in /proc/sys/net/ipv4/conf/*/rp_filter
+ echo 0
+ ip netns add xdp
+ ip link add veth1 type veth peer name veth2
+ ip link set veth2 netns xdp
+ ip add add 10.2.2.1/24 dev veth1
+ ip link set dev veth1 up addr 52:54:27:27:27:25
+ ip netns exec xdp ip addr add 10.2.2.2/24 dev veth2
+ ip netns exec xdp ip link set dev veth2 up addr 52:54:27:27:27:26
+ ip link set dev veth1 mtu 9000
+ ip netns exec xdp ip link set dev veth2 mtu 9000
+ sysctl -w net.core.rmem_default=134217728
net.core.rmem_default = 134217728
+ sysctl -w net.core.rmem_max=268435456
net.core.rmem_max = 268435456
+ sysctl -w net.core.wmem_default=134217728
net.core.wmem_default = 134217728
+ sysctl -w net.core.wmem_max=268435456
net.core.wmem_max = 268435456
+ sysctl -w 'net.ipv4.tcp_rmem=134217728 134217728 134217728'
net.ipv4.tcp_rmem = 134217728 134217728 134217728
+ sysctl -w 'net.ipv4.tcp_wmem=134217728 134217728 134217728'
net.ipv4.tcp_wmem = 134217728 134217728 134217728
+ sysctl -w 'net.ipv4.tcp_mem=134217728 134217728 134217728'
net.ipv4.tcp_mem = 134217728 134217728 134217728
+ sysctl -w 'net.ipv4.udp_mem=134217728 134217728 134217728'
net.ipv4.udp_mem = 134217728 134217728 134217728
+ sysctl -w net.ipv4.udp_rmem_min=134217728
net.ipv4.udp_rmem_min = 134217728
+ sysctl -w 'net.ipv4.udp_wmem_min=134217728 134217728 134217728'
net.ipv4.udp_wmem_min = 134217728 134217728 134217728
+ ip netns exec xdp sysctl -w 'net.ipv4.tcp_rmem=134217728 134217728 134217728'
net.ipv4.tcp_rmem = 134217728 134217728 134217728
+ ip netns exec xdp sysctl -w 'net.ipv4.tcp_wmem=134217728 134217728 134217728'
net.ipv4.tcp_wmem = 134217728 134217728 134217728
+ ip netns exec xdp sysctl -w net.ipv4.udp_rmem_min=134217728
net.ipv4.udp_rmem_min = 134217728
+ ip netns exec xdp sysctl -w 'net.ipv4.udp_wmem_min=134217728 134217728 134217728'
net.ipv4.udp_wmem_min = 134217728 134217728 134217728
+ ethtool -K veth1 tso off
+ ip netns exec xdp ethtool -K veth2 tso off
+ ulimit -l unlimited
+ '[' '!' -d /sys/fs/bpf ']'
+ /git/src/user/xdploader -file /git/src/kern/xdp.elf -i veth1
Dumping info for ELF file /git/src/kern/xdp.elf
Maps:
        tail_call: Per-CPU array, Fd 8
        stats_map: Per-CPU array, Fd 5
        if_redirect: Device map, Fd 6
        jmp_map: Array of programs, Fd 7

Programs:
        xdp_skeleton_2: XDP, size 344, license "GPL"
        xdp_skeleton_1: XDP, size 1272, license "GPL"
        xdp_skeleton: XDP, size 736, license "GPL"

Loading tail call jumps
Loading program name xdp_skeleton_2
Converting string (2)
Trying to insert (3) into position (2) in jmp_map
Loading program name xdp_skeleton_1
Converting string (1)
Trying to insert (9) into position (1) in jmp_map
Loading program name xdp_skeleton
Error scanning unexpected EOF: xdp_skeleton
Attaching program xdp_skeleton
Looking for interface veth1
Inserting ifIndex 17
XDP code attached to veth1. To turn it off, use `ip -f link set dev %!s(MISSING) xdp off`
$
```

Verify the code is loaded by running the following command inside the VM:

```console
$ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 5000
    link/ether 08:00:27:1c:65:67 brd ff:ff:ff:ff:ff:ff
    inet 10.0.2.15/24 brd 10.0.2.255 scope global dynamic eth0
       valid_lft 86000sec preferred_lft 86000sec
3: docker0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN group default
    link/ether 02:42:64:b8:88:a4 brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.1/16 brd 172.17.255.255 scope global docker0
       valid_lft forever preferred_lft forever
17: veth1@if16: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 9000 xdpgeneric/id:125 qdisc noqueue state UP group default qlen 1000
    link/ether 52:54:27:27:27:25 brd ff:ff:ff:ff:ff:ff link-netns xdp
    inet 10.2.2.1/24 scope global veth1
       valid_lft forever preferred_lft forever
```

Note in the above that veth1 has the xdp program loaded. It's highlighted below:

```console
17: veth1@if16: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 9000 xdpgeneric/id:125 qdisc noqueue state UP group default qlen 1000
```

### Try a Ping

Log into the VM:

```console
vagrant ssh
```

Now, once inside the VM, run the following commands, which will ping from veth1
on the host into veth2 in the xdp namespace:

```console
$ ping 10.2.2.2 -c 5
PING 10.2.2.2 (10.2.2.2) 56(84) bytes of data.
64 bytes from 10.2.2.2: icmp_seq=1 ttl=64 time=0.038 ms
64 bytes from 10.2.2.2: icmp_seq=2 ttl=64 time=0.031 ms
64 bytes from 10.2.2.2: icmp_seq=3 ttl=64 time=0.039 ms
64 bytes from 10.2.2.2: icmp_seq=4 ttl=64 time=0.062 ms
64 bytes from 10.2.2.2: icmp_seq=5 ttl=64 time=0.031 ms

--- 10.2.2.2 ping statistics ---
5 packets transmitted, 5 received, 0% packet loss, time 4098ms
rtt min/avg/max/mdev = 0.031/0.040/0.062/0.011 ms
$
```

### Verify bpfmap

Log into the VM:

```console
vagrant ssh
```

Now, once inside the VM, run the following commands:

```console
$ $ sudo /git/src/stats/statswatch -file /sys/fs/bpf/stats_map
Found map stats_map of type Per-CPU array
    XDP_ABORTED:  0 packets / 0 bytes
    XDP_DROP:     0 packets / 0 bytes
    XDP_PASS:     7 packets / 574 bytes
    XDP_TX:       0 packets / 0 bytes
    XDP_REDIRECT: 0 packets / 0 bytes
$ sudo bpftool map show
1: percpu_array  name stats_map  flags 0x0
        key 4B  value 16B  max_entries 5  memlock 4096B
2: devmap  name if_redirect  flags 0x80
        key 4B  value 4B  max_entries 64  memlock 4096B
$ sudo bpftool map dump id 1
key:
00 00 00 00
value (CPU 00): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 01): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 02): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 03): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 04): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 05): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
key:
01 00 00 00
value (CPU 00): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 01): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 02): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 03): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 04): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 05): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
key:
02 00 00 00
value (CPU 00): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 01): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 02): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 03): 01 00 00 00 00 00 00 00  62 00 00 00 00 00 00 00
value (CPU 04): 06 00 00 00 00 00 00 00  dc 01 00 00 00 00 00 00
value (CPU 05): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
key:
03 00 00 00
value (CPU 00): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 01): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 02): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 03): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 04): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 05): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
key:
04 00 00 00
value (CPU 00): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 01): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 02): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 03): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 04): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
value (CPU 05): 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
Found 5 elements
```

Notice that in the case above, the third map entry (Key 02 00 00 00) shows
that some packets are being processed by CPUs 3 and 4.
