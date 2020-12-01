FROM ubuntu:18.04

# Upgrade installed packages
RUN apt-get update && apt-get upgrade -y && apt-get clean

ADD . .

# Python package management and basic dependencies
RUN apt-get install -y curl python3.7 python3.7-dev python3.7-distutils

# Register the version in alternatives
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.7 1

# Set python 3 as the default python and python3
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.6 1 #TODO: remove?
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.7 2

RUN rm /usr/bin/python3
RUN ln -s python3.7 /usr/bin/python3

#RUN update-alternatives --set python /usr/bin/python3.7
#RUN update-alternatives --config python3
#RUN update-alternatives --set python3 /usr/bin/python3.7

# Upgrade pip to latest version
RUN curl -s https://bootstrap.pypa.io/get-pip.py -o get-pip.py && \
    python get-pip.py --force-reinstall && \
    rm get-pip.py

# Install dependencies
RUN apt-get -y install g++ pkg-config libarmadillo-dev libboost-all-dev make

# compile the c++ project
RUN echo python --version
RUN echo python3 --version
RUN echo ">>> Compiling GP-GOMEA source code..." && make

# copy the .so library into the python package
RUN cp dist/Python_Release/GNU-Linux/gpgomea pyGPGOMEA/gpgomea.so

# launch pip install
RUN echo ">>> Running pip install..." && pip install .

# Test the installation
RUN echo ">>> Test installation..." &&  python test.py
