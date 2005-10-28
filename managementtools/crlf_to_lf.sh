#!/bin/bash
#
# Convert line endings from CRLF style to LF style.
echo Converting $1 ...
tr '\r' '\n' < $1 > /tmp/unixfile.txt
cp /tmp/unixfile.txt $1
