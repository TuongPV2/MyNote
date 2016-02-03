Setup and Build examples for Vybrid
$ export PATH=/opt/gcc-arm-none-eabi-4_6-2012q4/bin/:${PATH}
$ git clone https://github.com/falstaff84/libopencm3-examples.git
...
$ cd libopencm3-examples
$ git checkout fsl-vf6xx
...
$ git submodule init
..
$ git submodule update

$ cd libopencm3
$ git remote add falstaff https://github.com/falstaff84/libopencm3.git
$ git fetch falstaff
...
$ git checkout fsl-vf6xx
...




https://falstaff.agner.ch/2014/07/10/libopencm3-bare-metal-vybrid-examples/