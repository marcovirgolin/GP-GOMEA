FROM ubuntu:20.04

# Set console to not use prompts
ARG DEBIAN_FRONTEND=noninteractive

# Upgrade installed packages
RUN apt-get update && apt-get upgrade -y && apt-get clean

# Python package management and basic dependencies
RUN apt-get install -y curl python3-dev python3-distutils python3-numpy

# Upgrade pip to latest version
RUN curl -s https://bootstrap.pypa.io/get-pip.py -o get-pip.py && \
    python3 get-pip.py --force-reinstall && \
    rm get-pip.py

# Install dependencies
RUN apt-get -y install g++ pkg-config libarmadillo-dev make build-essential autotools-dev libicu-dev libbz2-dev wget libboost-all-dev cmake ninja-build ccache

RUN mkdir /GP-GOMEA
WORKDIR /GP-GOMEA
ADD . .
RUN rm -rf build

ENV GEN=ninja

# compile the c++ project
RUN echo ">>> Compiling GP-GOMEA source code..." && make

# Test the installation
RUN echo ">>> Test installation..." &&  python3 test.py
