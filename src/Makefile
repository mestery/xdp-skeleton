SHELL=/bin/bash

KERN_HEADERS=kern/xdp_maps.h \
	     kern/xdp_parser.h

TC_HEADERS=tc/geneve.h \
	   tc/maps.h \
	   tc/parsers.h \
	   tc/stats.h \
	   tc/tc_helpers.h

# Detect architecture and set architecture-specific include paths
ARCH := $(shell uname -m)

# Set include paths based on architecture
ifeq ($(ARCH), x86_64)
    ARCH_INCLUDE = -I/usr/include/x86_64-linux-gnu
	CLANG_ARGS = -m64
else ifeq ($(ARCH), arm64)
    ARCH_INCLUDE = -I/usr/include/aarch64-linux-gnu
	CLANGS_ARGS =
else
    $(error Unsupported architecture: $(ARCH))
endif

all: user/xdploader stats/statswatch kern/xdp.elf tc/tc.elf

goebpf:
	git clone https://github.com/dropbox/goebpf.git

docker: Dockerfile
	docker build --progress=plain -t "xdp-skeleton-build:latest" .
	touch docker

kern/xdp.elf: kern/xdp.c $(KERN_HEADERS) docker goebpf
	docker run --rm -v "$(PWD)":/xdp/src -w /xdp/src/kern xdp-skeleton-build:latest clang $(CLANG_ARGS) -g -O2 -target bpf -I/xdp/src/common -I/xdp/src/goebpf $(ARCH_INCLUDE) -I/opt/src/xdploader/goebpf -c xdp.c -o xdp.elf

user/xdploader: user/main.go docker goebpf
	docker run --rm -v "$(PWD)":/opt/src/xdploader -w /opt/src/xdploader/user xdp-skeleton-build:latest go build -ldflags="-s -w" -o xdploader

stats/statswatch: stats/main.go docker goebpf
	docker run --rm -v "$(PWD)":/opt/src/xdploader -w /opt/src/xdploader/stats xdp-skeleton-build:latest go build -ldflags="-s -w" -o statswatch

tc/tc.elf: tc/tc.c $(TC_HEADERS) docker goebpf
	docker run --rm -v "$(PWD)":/xdp/src -w /xdp/src/tc xdp-skeleton-build:latest clang $(CLANG_ARGS) -g -O2 -target bpf -I/xdp/src/common -I/xdp/src/goebpf $(ARCH_INCLUDE) -I/opt/src/xdploader/goebpf -c tc.c -o tc.elf
