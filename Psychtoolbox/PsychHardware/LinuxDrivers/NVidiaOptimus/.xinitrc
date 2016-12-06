#!/bin/bash
#
# Copy this file into /root/ iff you use the bundled
# lightdm.conf file. Otherwise copy it to the location
# to which you pointed your login manager to find it.
#
xrandr --setprovideroutputsource modesetting NVIDIA-0
xrandr --auto
#xrandr --output eDP-1 --set "PRIME Synchronization" 1
