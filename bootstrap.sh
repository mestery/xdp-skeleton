#!/usr/bin/env bash

sudo apt-get update && DEBIAN_FRONTEND=noninteractive sudo apt-get install -y \
            make \
            gcc \
            clang \
            git \
            pkg-config \
            autoconf \
            automake \
            gettext \
            libtool \
            libev-dev \
            libpcre3-dev \
            libudns-dev \
            debhelper \
            dh-autoreconf \
            dpkg-dev \
            procps \
            git \
            cmake \
            clang \
            libclang-dev \
            libelf-dev \
            llvm-dev \
            flex \
            bison \
            cmake \
            python \
            libpcap-dev \
            python3 \
            python3-dev \
            python3-setuptools \
            python3-virtualenv \
            python3-venv \
            python3-pip \
            linux-tools-common \
            libdw1 \
            linux-tools-$(uname -r) \
            linux-cloud-tools-$(uname -r)

# Set some default network buffer values
sudo sysctl -w net.core.rmem_default=134217728
sudo sysctl -w net.core.rmem_max=268435456
sudo sysctl -w net.core.wmem_default=134217728
sudo sysctl -w net.core.wmem_max=268435456
sudo sysctl -w net.ipv4.tcp_rmem="134217728 134217728 134217728"
sudo sysctl -w net.ipv4.tcp_wmem="134217728 134217728 134217728"
sudo sysctl -w net.ipv4.tcp_mem="134217728 134217728 134217728"
sudo sysctl -w net.ipv4.udp_mem="134217728 134217728 134217728"
sudo sysctl -w net.ipv4.udp_rmem_min=134217728
sudo sysctl -w net.ipv4.udp_wmem_min="134217728 134217728 134217728"

# Update pahole
wget http://archive.ubuntu.com/ubuntu/pool/universe/d/dwarves-dfsg/dwarves_1.17-1_amd64.deb
DEBIAN_FRONTEND=noninteractive sudo apt-get install ./dwarves_1.17-1_amd64.deb
