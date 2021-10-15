SHELL=/bin/bash

KERN_HEADERS=kern/xdp_maps.h \
	     kern/xdp_parser.h

TC_HEADERS=tc/geneve.h \
	   tc/maps.h \
	   tc/parsers.h \
	   tc/stats.h \
	   tc/tc_helpers.h

all: user/xdploader stats/statswatch kern/xdp.elf tc/tc.elf

goebpf:
	git clone https://github.com/dropbox/goebpf.git

docker: Dockerfile
	docker build -t "xdp-skeleton-build:latest" .
	touch docker

kern/xdp.elf: kern/xdp.c $(KERN_HEADERS) docker goebpf
	docker run --rm -it -v "$(PWD)":/xdp/src -w /xdp/src/kern xdp-skeleton-build:latest clang-9 -g -O2 -target bpf -I/xdp/src/common -I/xdp/src/goebpf -I/usr/include/x86_64-linux-gnu -c xdp.c -o xdp.elf

user/xdploader: user/main.go docker goebpf
	docker run --rm -it -v "$(PWD)":/opt/src/xdploader -w /opt/src/xdploader/user xdp-skeleton-build:latest go build -ldflags="-s -w" -o xdploader

stats/statswatch: stats/main.go docker goebpf
	docker run --rm -it -v "$(PWD)":/opt/src/xdploader -w /opt/src/xdploader/stats xdp-skeleton-build:latest go build -ldflags="-s -w" -o statswatch

tc/tc.elf: tc/tc.c $(TC_HEADERS) docker goebpf
	docker run --rm -it -v "$(PWD)":/xdp/src -w /xdp/src/tc xdp-skeleton-build:latest clang-9 -g -O2 -target bpf -I/xdp/src/common -I/xdp/src/goebpf -I/usr/include/x86_64-linux-gnu -c tc.c -o tc.elf
