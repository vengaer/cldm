FROM archlinux:latest
LABEL maintainer="vilhelm.engstrom@tuta.io"

RUN pacman -Syu --needed --noconfirm make clang {,aarch64-linux-gnu-}gcc git python{,-pytest} valgrind llvm qemu{,-arch-extra}

ENV CC=gcc

COPY . /cldm
WORKDIR /cldm
