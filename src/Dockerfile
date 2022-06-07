FROM ubuntu:jammy

RUN apt-get -y update
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y tzdata build-essential git cmake zlib1g-dev libevent-dev libelf-dev llvm clang libc6-dev-i386 golang

RUN mkdir -p /opt/src
WORKDIR /opt/src
