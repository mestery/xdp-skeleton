FROM debian:bullseye

RUN apt-get -y update
RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata
RUN DEBIAN_FRONTEND="noninteractive" apt-get install clang-9 libclang-9-dev libllvm9 libelf-dev git golang llvm-9-dev gcc flex bison cmake python libpcap-dev -y 

RUN mkdir -p /opt/src
WORKDIR /opt/src
