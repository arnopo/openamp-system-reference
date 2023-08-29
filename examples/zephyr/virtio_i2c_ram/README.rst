.. _openAMP_sample:

OpenAMP Sample Application
##########################

Overview
========

This application demonstrates how to use of virtio msg to share an I2C bus between
Zephyr and a remote processor. It is designed to implement an memory device
connected on an I2C virtio bus.

requested Hardware
==================

- compatible with:
  - STM32MP157c-dk2 board

Generate the stm32mp15 image
============================

Install stm32mp1_distrib_oss Scarthgap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

From the stm32mp15-demo directory

.. code-block:: console

   export YOCTO_VER=scarthgap
   cd stm32mp1_distrib_oss
   mkdir -p layers/meta-st

   git clone https://git.yoctoproject.org/git/poky layers/poky
   cd layers/poky
   git checkout -b WORKING origin/$YOCTO_VER
   cd -

   git clone https://github.com/openembedded/meta-openembedded.git layers/meta-openembedded
   cd layers/meta-openembedded
   git checkout -b WORKING origin/$YOCTO_VER
   cd -

   git clone https://github.com/STMicroelectronics/meta-st-stm32mp-oss.git layers/meta-st/meta-st-stm32mp-oss
   cd layers/meta-st/meta-st-stm32mp-oss
   git checkout -b WORKING origin/$YOCTO_VER
   cd -

Initialize the Open Embedded build environment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The OpenEmbedded environment setup script must be run once in each new working terminal in which you
use the BitBake or devtool tools (see later) from stm32mp15-demo/stm32mp1_distrib_oss directory

.. code-block:: console

   source ./layers/poky/oe-init-build-env build-stm32mp15-disco-oss

   bitbake-layers add-layer ../layers/meta-openembedded/meta-oe
   bitbake-layers add-layer ../layers/meta-openembedded/meta-perl
   bitbake-layers add-layer ../layers/meta-openembedded/meta-python
   bitbake-layers add-layer ../layers/meta-st/meta-st-stm32mp-oss

   echo "MACHINE = \"stm32mp15-disco-oss\"" >> conf/local.conf
   echo "DISTRO = \"nodistro\"" >> conf/local.conf
   echo "PACKAGE_CLASSES = \"package_deb\" " >> conf/local.conf

Build stm32mp1_distrib_oss image
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

From stm32mp15-demo/stm32mp1_distrib_oss/build-stm32mp15-disco-oss/ directory

.. code-block:: console

   bitbake core-image-base

Note that

   - to build around 30 GB is needed
   - building the distribution can take more than 2 hours depending on performance of the PC.

Install stm32mp1_distrib_oss
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

From 'stm32mp15-demo/stm32mp1_distrib_oss/build-stm32mp15-disco-oss/' directory,populate your
microSD card inserted on your HOST PC using command

.. code-block:: console

   cd tmp-glibc/deploy/images/stm32mp15-disco-oss/
   # Unmount potential media mounted from the sdcard. Replace <device> by mmcblk<X> (X = 0,1..) or
   #sd<Y>( Y = b,c,d,..) depending on the connection
   sudo eject /dev/<device>
   # Flash wic image on your sdcard. Replace <device> by mmcblk<X> (X = 0,1..) or
   #sd<Y>( Y = b,c,d,..) depending on the connection
   sudo dd if=core-image-base-stm32mp15-disco-oss.rootfs.wic of=/dev/<device> bs=8M conv=fdatasync status=progress


Update the linux kernel
^^^^^^^^^^^^^^^^^^^^^^^
The Linux kernel image should be updated to support the virtio-ms protocol.

Get the linux kernel cloning the following repo git:

https://github.com/arnopo/linux/tree/virtio-msg

Rebuild the image and update the SDcard

Zephyr
======

Initialization
^^^^^^^^^^^^^^
The first step is to initialize the workspace folder (``my-workspace``) where
the examples and all Zephyr modules will be cloned. You can do
that by running:

.. code-block:: console

  # initialize my-workspace for the example-application (main branch)
  west init -m https://github.com/arnopo/openamp-system-reference --mf west-virtio-msg.yml --mr virtio-msg  my-workspace
  # update modules
  cd my-workspace
  west update

Build
^^^^^
.. code-block:: console

   west build -b stm32mp157c_dk2 openamp-system-reference/examples/zephyr/virtio_i2c_ram

Copy the binary file on the SDCard

Linux console
=============

Open a serial Linux terminal (minicom, putty, etc.) and connect the board with the
following settings:

- Speed: 115200
- Data: 8 bits
- Parity: odd
- Stop bits: 1

Load and start the firmware:

.. code-block:: console

  echo -n <firmware_name.elf> > /sys/class/remoteproc/remoteproc0/firmware
  echo start >/sys/class/remoteproc/remoteproc0/state


This is the Linux console:

1. Verify that the virtio I2C bus is preent

.. code-block:: console

  root@stm32mp1-disco-oss:~# i2cdetect -l
  i2c-0   i2c             STM32F7 I2C(0x40012000)                 I2C adapter
  i2c-1   i2c             STM32F7 I2C(0x5c002000)                 I2C adapter
  i2c-2   i2c             i2c-0-mux (chan_id 0)                   I2C adapter
  i2c-3   i2c             i2c_virtio at virtio bus 0              I2C adapter

2. list devices on the virtio bus

Two memory devices should be connected:
- one 20 bytes memory at address 0x54 initialized with "123456789abcdefghij"
- one 20 bytes memory at address 0x56 initialized with "klmnopqrstuvwxyz!:;"
Verify that the virtio I2C bus is present

.. code-block:: console

  root@stm32mp1-disco-oss:~# i2cdetect -y 3
       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
  00:                         -- -- -- -- -- -- -- --
  10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  50: -- -- -- -- 54 -- 56 -- -- -- -- -- -- -- -- --
  60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  70: -- -- -- -- -- -- -- --

3. Update RAM device 0x54 at address 0x14 with value 4

.. code-block:: console

  root@stm32mp1-disco-oss:~# i2cget -y 3 0x54 14
  0x66
  root@stm32mp1-disco-oss:~# i2cset -y 3 0x54 14 0x25
  root@stm32mp1-disco-oss:~# i2cget -y 3 0x54 14
  0x25
