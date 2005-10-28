#!/bin/bash
# Convert CRLF linefeeds to LF linefeeds to make Subversion and a lot of
# other tools happy.
# Usage: ./convertlinefeeds.sh <Path to Psychtoolbox folder (e.g., trunk)>
find $1 -name "*.h" -exec ./crlf_to_lf.sh {} \;
find $1 -name "*.c" -exec ./crlf_to_lf.sh {} \;
find $1 -name "*.m" -exec ./crlf_to_lf.sh {} \;
echo Done.

