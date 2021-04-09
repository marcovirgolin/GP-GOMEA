FROM fedora:33

# Upgrade installed packages
RUN dnf update -y

# Python package management and basic dependencies
RUN dnf install -y curl wget python3-devel python3-setuptools python3-numpy python3-Cython

# Upgrade pip to latest version
RUN curl -s https://bootstrap.pypa.io/get-pip.py -o get-pip.py && \
    python3 get-pip.py --force-reinstall && \
    rm get-pip.py

# Install dependencies
RUN dnf groupinstall -y "Development Tools"
RUN dnf install -y g++ armadillo-devel automake autoconf libicu-devel bzip2-devel boost-devel boost-program-options boost-python3-devel cmake ninja-build ccache

RUN mkdir /GP-GOMEA
WORKDIR /GP-GOMEA
ADD . .

# Install sklearn
RUN python3 -m pip install -r pythonpkg/requirements.txt --user

ENV GEN=ninja

# compile the c++ project
RUN echo ">>> Compiling GP-GOMEA source code..." && make

# Test the installation
RUN echo ">>> Test installation..." &&  python3 test.py

