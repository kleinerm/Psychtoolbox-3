#!/bin/bash

# setupdate -- start or stop /usr/sbin/update
#   setupdate 0 .......... stop
#   setupdate 1 .......... start
#   setupdate permit ..... change /etc/sudoers to allow anyone to run setupdate
#   setupdate ............ report number of instances of 'update' running

# 26-Nov-2005 -- created (RFM)

# name of current command
THISCMD=$( basename $0 )

# function to count instances of 'update'
function updates_running () {
    ps -acx | tr -s " " | sed 's/^ *//' | cut -d " " -f5 | grep "^update$" | wc -l ; }

# function to report usage
function usage() { printf "usage:  %s [01]\n" $THISCMD ; }

# commands
STARTCMD='/usr/sbin/update'
STOPCMD='/usr/bin/killall update'

# count instances of 'update' running
RUNNING=$( updates_running )

# if no arguments, report number of 'update' processes running
if test $# -eq 0 ; then
    printf "%d instance(s) of 'update' running\n" $RUNNING
    exit 0
fi

# check number of arguments
if test $# -gt 1 ; then
    printf "%s:  invalid number of arguments\n" $THISCMD 1>&2
    usage 1>&2
    exit -1
fi

# print warning if multiple instances
if test $RUNNING -gt 1 ; then
    printf "%s:  warning -- %d instances of 'update' running\n" $THISCMD $RUNNING 1>&2
fi

# kill update
if test $1 = 0 ; then

    # if any instances running, then kill them
    if test $RUNNING -gt 0 ; then

        # kill 'update'
        sudo $STOPCMD

        # check whether they're gone
        RUNNING=$( updates_running )
        if test $RUNNING -gt 0 ; then
            printf "%s:  warning -- tried to kill 'update', but %d instance(s) still running\n" $THISCMD $RUNNING 1>&2
            exit -1
        fi

    fi

#    printf "now %d instance(s) of 'update' running\n" $RUNNING
    exit 0

# start update
elif test $1 = 1 ; then

     # if none running, then start one
    if test $RUNNING -eq 0 ; then

         # start an 'update' process
        sudo $STARTCMD

         # check that it's running
        RUNNING=$( updates_running )
        if test $RUNNING -eq 0 ; then
	    printf "%s:  warning -- tried to start 'update', but failed\n" $THISCMD 1>&2
            exit -1
        fi

    fi

#    printf "now %d instance(s) of 'update' running\n" $RUNNING
    exit 0

# change /etc/sudoers to allow anyone to start or stop 'update'
elif test $1 = permit ; then

    # see whether we have permission
    eval 'echo "" >> /etc/sudoers' 2> /dev/null
    if test $? -ne 0 ; then
        printf "%s permit:  error -- you must run this command as root\n" $THISCMD 1>&2
        printf "e.g., sudo %s permit\n" $THISCMD 1>&2
	exit -1
    fi

    echo "# allow anyone to start and stop the 'update' process" >> /etc/sudoers
    echo "# (important for allowing MATLAB to stop disk writes during experiments)" >> /etc/sudoers
    printf "# added by %s using '%s' script (%s)\n" $USER $THISCMD "$( date )"  >> /etc/sudoers
    printf "ALL ALL = NOPASSWD: %s\n" "$STARTCMD" >> /etc/sudoers
    printf "ALL ALL = NOPASSWD: %s\n" "$STOPCMD"  >> /etc/sudoers
    echo "" >> /etc/sudoers

# invalid argument
else

    printf "%s:  invalid argument\n" $THISCMD 1>&2
    usage 1>&2
    exit -1

fi
