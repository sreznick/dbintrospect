FROM ubuntu:22.04

RUN apt update
RUN apt -y upgrade 
RUN apt-get -y install g++
RUN apt-get -y install git
RUN git clone https://github.com/libfuse/libfuse.git
RUN apt-get -y install meson ninja-build
WORKDIR  libfuse
RUN mkdir build
WORKDIR  build
RUN meson setup ..
RUN ninja
RUN ninja install

