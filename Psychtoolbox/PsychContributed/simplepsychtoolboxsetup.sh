#!/bin/bash
#
# Name: simplepsychtoolboxsetup.sh
#
# Description: When run as admin user, modifies system
# configuration in order to allow *ALL* users of the machine
# to run the following commands without having to enter the
# root/admin password:
# sudo /usr/bin/killall update
# sudo /usr/sbin/update
#
# Usage: Run once as sytem administrator via sudo command when setting up
# Psychtoolbox for MacOS-X, e.g. as part of installation.
#
# Step by Step instructions: You need to be admin user to do this!
#
# 1. Download script into a directory:
# 2. Open a terminal window ( Finder -> Go -> Utilities -> Terminal ) 
# 3. Go to directory where you downloaded the scripts via cd - command.
# 4. Execute script by entering: sudo /bin/bash ./simplepsychtoolboxsetup.sh
#    You will probably have to enter your admin password...
# 5. Follow the instructions on the screen...
#
# You're done.
#
# Date:   4.7.2004
# Author: Mario Kleiner (mario.kleiner@tuebingen.mpg.de)

echo ""
echo ""
echo "Modifying system configuration, so that Psychtoolbox killupdate - workaround"
echo "for the Matlab on OS-X timing bug works without root password..."
echo ""

# Child protection - This needs to be run as root or sudo root:
if !(test $USER = "root"); then
	echo "Sorry, this script needs to be run as root or with the sudo"
	echo "command as admin user. Invocation without admin privileges"
	echo "will not work. Please retry as administrator or sudo admin."
	echo "This command should do the job: sudo /bin/bash ./simplepsychtoolboxsetup.sh"
	echo "Script aborted."
	exit
fi

# Child protection - We check if /etc/sudoers setup has been done already.
if test -e /etc/sudoers_pre_psychtoolbox; then
	echo "The file /etc/sudoers_pre_psychtoolbox already exists."
	echo "This indicates that the system has been already"
	echo "modified for the purpose of Psychtoolbox."
	echo ""
	echo "Modifying it more than once is not necessary."
	echo ""
	echo "Aborting now. If you think i'm wrong, then manually delete the"
	echo "file via sudo rm -f /etc/sudoers_pre_psychtoolbox and retry."
	echo ""
	echo "Bye."
	exit
fi

# Make backup copy of /etc/sudoers file:
echo "Creating a backup copy of the file /etc/sudoers as /etc/sudoers_pre_psychtoolbox ..."
echo "In case you want to undo this scripts effects, just ask the system administrator to restore /etc/sudoers"
echo "from that backup copy."
echo ""
cp /etc/sudoers /etc/sudoers_pre_psychtoolbox

# Make sure that the backup was successfull:
if !(test -f /etc/sudoers_pre_psychtoolbox); then
	echo "Creating backup copy of /etc/sudoers failed!"
	echo "It is not safe to continue - I will abort."
	echo ""
	echo "Please ask your system administrator for help."
	echo ""
	exit
fi

# Add the additional lines to the sudoers file, which allow all
# users with a valid login account to issue the sudo
# commands mentioned above without having to enter *any* password:
echo -n "Modifying the /etc/sudoers file ..."
echo "ALL ALL= NOPASSWD: /usr/bin/killall update" >> /etc/sudoers
echo "ALL ALL= NOPASSWD: /usr/sbin/update" >> /etc/sudoers
sync
echo " ...Done."

echo ""
echo ""
echo "Basic setup of machine for Psychtoolbox finished."
echo ""
echo "Script successfully finished! Bye."
