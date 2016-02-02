Setup and build bare-metal code for Vybrid (on Linux)

1. Setup compiler
wget https://launchpad.net/gcc-arm-embedded/5.0/5-2015-q4-major/+download/gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2

tar -xjf gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2 (gcc-arm-none-eabi-x_x-xxxxqx-xxxxxxxx-linux.tar.bz2)

mv gcc-arm-none-eabi-5_2-2015q4 /opt/

2. Install 
git clone https://github.com/falstaff84/libopencm3-examples.git

cd libopencm3-examples

git checkout fsl-vf6xx

git submodule init

git submodule update

cd libopencm3

git remote add falstaff https://github.com/falstaff84/libopencm3.git

git fetch falstaff

git checkout fsl-vf6xx

Compile lib:

export PATH=/opt/gcc-arm-none-eabi-5_2-2015q4/bin/:${PATH}

cd libopencm3 (if you are standing on another folder )

make

Build examples:

cd examples/vf6xx/colibri-vf61/uart/

make uart.bin

ls -la uart.bin (check)

Reference: https://falstaff.agner.ch/2014/07/10/libopencm3-bare-metal-vybrid-examples/

