FROM ubuntu:18.04

# Upgrade installed packages
RUN apt-get update && apt-get upgrade -y && apt-get clean

ADD . .

# Python package management and basic dependencies
RUN apt-get install -y curl python3.7 python3-dev python3.7-dev python3.7-distutils

# Register the version in alternatives
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.7 1

RUN rm /usr/bin/python3
RUN ln -s python3.7 /usr/bin/python3

# Upgrade pip to latest version
RUN curl -s https://bootstrap.pypa.io/get-pip.py -o get-pip.py && \
    python get-pip.py --force-reinstall && \
    rm get-pip.py

# Install dependencies
RUN apt-get -y install g++ pkg-config libarmadillo-dev make build-essential autotools-dev libicu-dev libbz2-dev wget #libboost-all-dev

# Install latest boost version
# TODO: when all works fine; remove this hastle and just use apt-get libboost-all-dev?
RUN apt-get -y --purge remove libboost-all-dev libboost-doc libboost-dev && rm -f /usr/lib/libboost_*
RUN wget https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.tar.bz2 && \
    tar --bzip2 -xf boost_1_75_0.tar.bz2 && \
    cd boost_1_75_0 && \
    # this will generate ./b2
    ./bootstrap.sh  --prefix=/usr/local && \
    ./b2 --with=all install && \
    sh -c 'echo "/usr/local/lib" >> /etc/ld.so.conf.d/local.conf' && \
    ldconfig

# Check numpy installation
RUN find / -name "numpy.hpp"
RUN find / -name "libboost_numpy37.so"
RUN find / -name "libboost_numpy3.so"
RUN find / -name "libboost_numpy.so"
RUN ls /usr/local/include/boost
RUN ls /usr/local/include/boost/python
RUN ld -lboost_numpy37 --verbose
# TODO: symlink?

# compile the c++ project
# TODO: remove these
#RUN python3 --version
#RUN pkg-config --variable pc_path pkg-config
#RUN ls /usr/lib/pkgconfig
#RUN ls /usr/share/pkgconfig
#
#RUN echo ">>> Compiling GP-GOMEA source code..." && make
#
## copy the .so library into the python package
#RUN cp dist/Python_Release/GNU-Linux/gpgomea pyGPGOMEA/gpgomea.so
#
## launch pip install
#RUN echo ">>> Running pip install..." && pip install .
#
## Test the installation
#RUN echo ">>> Test installation..." &&  python test.py


##### OTHER TODOS
# - Document how this script was built
# - Remove m_ubuntu and m_fedora from docker image?
