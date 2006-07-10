# Build GetSecs.oct ...
#./mex2oct Octave/GetSecs.cc -IOctave -ICommon/Base -ILinux/Base -ICommon/GetSecs Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Common/GetSecs/*.c -g -lc -v

# Build WaitSecs.oct ...
#./mex2oct Octave/WaitSecs.cc -IOctave -ICommon/Base -ILinux/Base -ICommon/WaitSecs Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Common/WaitSecs/*.c -g -lc -v

# Build Screen.oct ...
./mex2oct Octave/Screen.cc -IOctave -I/usr/X11R6/include -ICommon/Base -ILinux/Base -ILinux/Screen -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Linux/Screen/*.c Common/Screen/*.c /usr/X11R6/lib/libXxf86vm.a /usr/X11R6/lib/libGLU.a /usr/local/lib/libdc1394.a /usr/local/lib/libraw1394.a -L/usr/X11R6/lib -g -lc -lGL -lX11 -lXext -v 

# Copy all oct files to the PsychBasic folder:
/bin/cp -f ./*.oct ./../../Psychtoolbox/PsychBasic/