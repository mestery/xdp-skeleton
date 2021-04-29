SHELL=/bin/bash

all: kern/xdp.elf

goebpf:
	git clone https://github.com/dropbox/goebpf.git

docker: Dockerfile
	docker build -t "xdp-skeleton-build:latest" .
	touch docker

kern/xdp.elf: kern/xdp.c docker goebpf
	docker run --rm -it -v "$(PWD)":/xdp/src -w /xdp/src/kern xdp-skeleton-build:latest clang-9 -g -O2 -target bpf -I/xdp/src/common -I/xdp/src/goebpf -I/usr/include/x86_64-linux-gnu -c xdp.c -o xdp.elf
