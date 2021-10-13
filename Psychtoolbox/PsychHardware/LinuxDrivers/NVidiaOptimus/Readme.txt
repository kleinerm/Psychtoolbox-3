CAUTION: THIS WILL ONLY WORK ON DISTRIBUTIONS WITH X-SERVER 1.19, NOT
WITH UBUNTU 20.04-LTS AND LATER AT THE MOMENT!

How to setup research grade NVidia Optimus support with the proprietary
drivers:

First you need a Linux distribution with XOrg X-Server 1.19.0 or later.
As of December 2016, that would be "Debian unstable" or "Fedora 25".
Both of these distros are completely untested with our driver and PTB.

The only way to get this working on a Ubuntu Linux flavor older than 17.04
is currently to download, compile and install an X-Server 1.19 yourself.
This is how this driver was developed and tested. If you are clever and
adventurous enough to try this route, instead of waiting for the release of
Ubuntu 17.04 in April 2017, you will have enough Google and Computer skills
to find out how to get such an X-Server downloaded, compiled and installed.

Once you have the X-Server 1.19 up and running, you need to install NVidia
proprietary display drivers of version 375.20 or preferrably any later and
more modern version. You can get those from www.nvidia.com, or via an
installation method specific to your Linux distribution.

Next you need to setup various configuration files and scripts. Many of
these steps are also explained in the release notes of the NVidia proprietary
driver.

This folder contains templates of these scripts, to be copied into the
proper folders on your machine. Ubuntu also provides a package "nvidia-prime"
which, once installed, will automate parts of this setup.

The following config files exist in this folder:

".xinitrc" --> To be copied into /root/.xinitrc
"lightdm.conf" --> To be copied into /etc/lightm/lightdm.conf on Ubuntu 16.04-LTS.

--> lightdm.conf tells the LightDM login manager ro execute the .xinitrc script
    found in /root.xinitrc. If you use a different login manager, find out what
    kind of config file is needed to execute the .xinitrc script at X-Server
    startup. You can copy the .xinitrc file under a different name into a different
    location if you like, you just need to adapt the path/name in lightdm.conf etc.
    accordingly.

--> The Ubuntu nvidia-prime package will perform these setup steps for you
    automatically if it works for you, but this has not been tested with PTB.

"optimusproprietary.conf" --> To be copied into /etc/modprobe.d/optimusproprietary.conf

"xorg.conf" needs to be copied into /etc/X11/xorg.conf
However, read the instructions at the top of xorg.conf on how you will have to
customize the file for your specific machine.

If, after a logout/login or machine restart you still enjoy a working display,
congratulations! You did everything right.

Finally copy the file modesetting_drv.so into the system folder
/usr/lib/x86_64-linux-gnu/xorg/extra-modules/

Then logout and login again, or maybe restart the machine, so the X-Server can pick
up the new driver. If your display still works, then that's a good sign :)

You will also need Linux kernel version 4.6 or later for timing to work
correctly. However, as of NVidia driver version 375.20, you are restricted
to Linux 4.6 and Linux 4.7, as the proprietary 375.20 driver does not work
Linux 4.8 or later!

As you can see, the setup procedure is quite involved at the moment, and
even small mistakes can lead to a completely non-functional GUI, so tread
carefully and only execute these instructions if you have some experience
working on a Unix command line and finding your way around compilers and
Makefiles.

Credits/Licenses:

The modesetting_drv.so module and modesetting_drv.so_highlag module are
modified versions of the modesetting ddx driver which ships as part of
X-Server 1.19.0 by default. Iow. they are created by ...

1. Installing the source code of X-Server 1.19.0 downloaded from
   https://cgit.freedesktop.org/xorg/xserver/ at the xorg-server-1.19.0 tag

2. Applying the patches included in this folder to the source tree:
   0001-modesetting-Add-custom-UDP-Prime-Sync-protocol-for-P.patch
   0002-modesetting-Add-custom-low-lag-optimizations-for-Psy.patch

3. Building the X-Server and thereby modesetting driver.

4. This results in the modesetting_drv.so files included in this folder.

-> There are two variants: modesetting_drv.so is the result of applying
   both patches in step 2.

   modesetting_drv.so_highlag is the result of only applying the patch
   0001-modesetting-Add-custom-UDP-Prime-Sync-protocol-for-P.patch
   which only provides the timestamping funcionality, but not the
   frame lag reduction of patch 0002.

The source code of the modesetting driver, and thereby modesetting_drv.so itself,
is licensed under a license as posted below. For the individual copyrights of
each sourc file of the driver, look at the top of the files contained in the
following part of the X-Server git tree:

https://cgit.freedesktop.org/xorg/xserver/tree/hw/xfree86/drivers/modesetting

The license always says this - with changing copyrights assigned to different
entities for the different files:

e.g., for driver.c:

/*

Copyright assignment -- different for different files, here for driver.c:

 * Copyright 2008 Tungsten Graphics, Inc., Cedar Park, Texas.
 * Copyright 2011 Dave Airlie
 * All Rights Reserved.
 *

Actual license, identical for the whole driver:

 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *

Different authors for the different files, here for driver.c:

 *
 * Original Author: Alan Hourihane <alanh@tungstengraphics.com>
 * Rewrite: Dave Airlie <airlied@redhat.com>
 *
 */
