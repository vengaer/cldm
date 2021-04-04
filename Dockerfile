FROM archlinux:latest
LABEL maintainer="vilhelm.engstrom@tuta.io"

RUN pacman -Syu --needed --noconfirm make clang gcc git python python-pytest valgrind

ENV CC=gcc

COPY . /cldm
WORKDIR /cldm
