#!/bin/bash
#
# psychregistrationserver.sh -- Minimalistic server for Psychtoolbox online registration.
#
# You need to make this script executable via "chmod a+x psychregistrationserver.sh"
#
# Start by entering "./psychregistrationserver.sh &" in a command window.
# Stop via "kill -9 <PID OF psychregistrationserver.sh>", you get the pid via "ps"
# command.
#

# This is an infinite loop. It runs until the psychserver.sh script
# gets kill'ed by the user. The nc command opens a listening TCP server
# on port 2000. The server will wait for and serve exactly one client
# connection. It will write all received data into /tmp/ptbregistrationlog. It will
# exit when the connection is closed. The while loop will then start a
# new instance of the server to serve the next client.

# You'll have to set a matching port number in the PsychtoolboxRegistration.m
# script. You also have to open the network port in the "Firewall" settings
# tab of SystemSettings->Network->Sharing for incoming connections.

while true; do nc -l -p 2000 >> /tmp/ptbregistrationlog ; done
