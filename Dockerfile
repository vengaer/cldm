FROM archlinux:latest
LABEL maintainer="vilhelm.engstrom@tuta.io"

RUN pacman -Syu --needed --noconfirm make clang gcc nasm git python python-pytest valgrind llvm

ENV CC=gcc

COPY . /cldm
WORKDIR /cldm
