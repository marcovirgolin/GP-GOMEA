# install dependencies 
# replace dnf with apt if working from Debian/Ubuntu
echo ">>> Installing g++..."
dnf install gcc-c++

echo ">>> Installing pkg-config..."
dnf install pkg-config

echo ">>> Installing armadillo..."
dnf install armadillo-devel

echo ">>> Installing python developer..."
dnf install python3-devel 

echo ">>> Installing boost developer..."
dnf install boost-devel

echo ">>> Installing boost python3 developer..."
dnf install boost-python3-devel

# compile the c++ project
echo ">>> Compiling GP-GOMEA source code..."
make

# copy the .so library into the python package
cp dist/Python_Release/GNU-Linux/gpgomea pyGPGOMEA/gpgomea.so

# launch pip install
echo ">>> Running pip install..."
pip3 install .

# done
echo ">>> ...Done."
