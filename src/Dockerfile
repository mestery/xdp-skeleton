FROM debian:bookworm

# Update and install basic dependencies
RUN apt-get -y update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y \
    tzdata clang libclang-dev llvm-dev libelf-dev git golang gcc flex bison cmake python3 libpcap-dev linux-libc-dev

# Install architecture-specific packages
RUN ARCH=$(uname -m) && \
    if [ "$ARCH" = "x86_64" ]; then \
        apt-get install -y gcc-multilib g++-multilib libc6-dev-i386 linux-headers-amd64; \
    elif [ "$ARCH" = "aarch64" ]; then \
        apt-get install -y linux-headers-arm64; \
    else \
        echo "Unsupported architecture: $ARCH"; exit 1; \
    fi

# Create working directory
RUN mkdir -p /opt/src
WORKDIR /opt/src
