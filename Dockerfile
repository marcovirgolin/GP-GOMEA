FROM ubuntu:18.04

ADD . .

# Upgrade installed packages
RUN apt-get update && apt-get upgrade -y && apt-get clean

# Python package management and basic dependencies
RUN apt-get install -y curl python3 python3-dev python3-distutils python3-numpy

# Upgrade pip to latest version
RUN curl -s https://bootstrap.pypa.io/get-pip.py -o get-pip.py && \
    python3 get-pip.py --force-reinstall && \
    rm get-pip.py

# Install dependencies
RUN apt-get -y install g++ pkg-config libarmadillo-dev make build-essential autotools-dev libicu-dev libbz2-dev wget libboost-all-dev

# compile the c++ project
RUN echo ">>> Compiling GP-GOMEA source code..." && make LIB_BOOST_PYTHON=-lpython3.6m LIB_BOOST_NUMPY=-lboost_numpy3

# copy the .so library into the python package
RUN cp dist/Python_Release/GNU-Linux/gpgomea pyGPGOMEA/gpgomea.so

# launch pip install
RUN echo ">>> Running pip install..." && pip install -r ./requirements.txt

# Test the installation
RUN echo ">>> Test installation..." &&  python3 test.py
