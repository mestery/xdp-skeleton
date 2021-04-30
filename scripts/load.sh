#!/bin/bash

set -x

ulimit -l unlimited

mount bpffs /sys/fs/bpf -t bpf

/git/src/user/xdploader -file /git/src/kern/xdp.elf -i veth1
