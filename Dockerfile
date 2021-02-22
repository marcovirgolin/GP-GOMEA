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


# Check numpy installation
RUN ld -lpython3.6m -lboost_numpy3 --verbose
# TODO: Remove?

# compile the c++ project
# TODO: remove these
#RUN python3 --version
#RUN pkg-config --variable pc_path pkg-config
#RUN ls /usr/lib/pkgconfig
#RUN ls /usr/share/pkgconfig

RUN echo ">>> Compiling GP-GOMEA source code..." && make

# copy the .so library into the python package
RUN cp dist/Python_Release/GNU-Linux/gpgomea pyGPGOMEA/gpgomea.so

# launch pip install
RUN echo ">>> Running pip install..." && pip install -r ./requirements.txt

# Test the installation
RUN echo ">>> Test installation..." &&  python3 test.py


##### OTHER TODOS
# - Document how this script was built
# - Remove m_ubuntu and m_fedora from docker image?
