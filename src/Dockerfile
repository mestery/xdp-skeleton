FROM debian:bookworm

# Update and install basic dependencies
RUN apt-get -y update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y \
    tzdata clang libclang-dev llvm-dev libelf-dev git golang gcc flex bison cmake python3 libpcap-dev linux-libc-dev

# Install kernel headers for the architecture
RUN apt-get -y install linux-headers-$(uname -r) || \
    apt-get -y install linux-headers-generic

# Create working directory
RUN mkdir -p /opt/src
WORKDIR /opt/src
