FROM archlinux:latest
LABEL maintainer="vilhelm.engstrom@tuta.io"

RUN pacman -Syu --needed --noconfirm make clang gcc git python python-pytest

ENV CC=gcc

COPY . /cmock
WORKDIR /cmock
