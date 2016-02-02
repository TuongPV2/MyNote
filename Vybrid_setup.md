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

Reference: 
https://falstaff.agner.ch/2014/07/10/libopencm3-bare-metal-vybrid-examples/
https://github.com/libopencm3/libopencm3-examples
http://embedded-computing.com/guest-blogs/bare-metal-firmware-for-vybrids-asymmetrical-multicore-architecture/
==============
#U-boot
u-boot on the Vybrid tower board in a few commands
##Get u-boot sources
$ git clone git://git.denx.de/u-boot.git
This should create a u-boot directory with all the latest sources.
 
Note that for more stability you might want to checkout a release instead of the latest version; to do so, list the available release tags with e.g. git tag -l 'v2*', and git checkout <the-desired-tag>.
Vybrid support in "mainline" u-boot is fairly recent so you will need a tag no older than v2013.07.

##Compile
Assuming your cross compiler is called e.g. arm-linux-gnueabihf-gcc, you can compile by doing:
 
  $ cd u-boot
  $ export CROSS_COMPILE=arm-linux-gnueabihf-
  $ make vf610twr_config
  $ make
 
This should create a number of files, including u-boot.imx.

##Put on SD
 
U-boot should reside at offset 1024B of your SD card. To put it there, do:
 
  $ dd if=u-boot.imx of=/dev/<your-sd-card> bs=1k seek=1
  $ sync
 
Your SD card device is typically something in /dev/sd<X> or /dev/mmcblk<X>. Note that you need write permissions on the SD card for the command to succeed, so you might need to su - as root, or use sudo, or do a chmod a+w as root on the SD card device node to grant permissions to users.

##Boot!
 
Your SD card is ready for booting. Insert it in the SD card slot of your Vybrid tower board, connect to the USB to UART port with a serial terminal set to 115200 baud, no parity, 8bit data, power up the platform and you should see something like:
 
U-Boot 2013.04-00300-g3d9138e (Jul 09 2013 - 11:57:02)
 
 
CPU:   Freescale Vybrid VF610 at 396 MHz
Reset cause: WDOG
Board: vf610twr
DRAM:  128 MiB
WARNING: Caches not enabled
MMC:   FSL_SDHC: 0
In:    serial
Out:   serial
Err:   serial
Net:   Phy not found
PHY reset timed out
FEC
Warning: failed to set MAC address
 
 
Hit any key to stop autoboot:  0

Reference:
https://community.freescale.com/docs/DOC-95248
=======================================
https://github.com/falstaff84/vf610m4bootldr
https://github.com/falstaff84/m4boot
https://falstaff.agner.ch/2014/10/05/make-it-two-tuxes-on-one-soc/
https://community.freescale.com/docs/DOC-93628
https://community.freescale.com/docs/DOC-95017
https://community.freescale.com/docs/DOC-95043
https://community.freescale.com/docs/DOC-95015
https://linuxlink.timesys.com/docs/gsg/twr_vf600
https://github.com/tsfs/Vybrid-U-Boot
http://developer.toradex.com/knowledge-base/flashing-linux-on-vybrid-modules
http://cms.applegriff.no/makepdf.php?tid=33&pk=2262
https://lists.yoctoproject.org/pipermail/meta-freescale/2013-June/003123.html
http://archlinuxarm.org/forum/viewtopic.php?f=23&t=9338



==
sth else
https://github.com/BuFran/blackmagic
https://github.com/chrysn/summon-arm-toolchain
