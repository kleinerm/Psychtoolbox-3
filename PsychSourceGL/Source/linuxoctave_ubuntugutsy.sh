# Build GetSecs.oct ...
#./mex2oct_ubuntugutsy -r Octave/GetSecs.cc -IOctave -ICommon/Base -ILinux/Base -ICommon/GetSecs -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Common/GetSecs/*.c -g -lc -lrt -v

# Build WaitSecs.oct ...
#./mex2oct_ubuntugutsy -r Octave/WaitSecs.cc -IOctave -ICommon/Base -ILinux/Base -ICommon/WaitSecs Common/Base/*.cc -ICommon/Screen Linux/Base/*.c Common/Base/*.c Common/WaitSecs/*.c -g -lc -lrt -v

# Build Screen.oct with libdc1394 video capture support...
#./mex2oct_ubuntugutsy -r Octave/Screen.cc -DPTBVIDEOCAPTURE_LIBDC -IOctave -I/usr/X11R6/include -ICommon/Base -ILinux/Base -ILinux/Screen -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Linux/Screen/*.c Common/Screen/*.c /usr/lib/libXxf86vm.a /usr/lib/libGLU.a /usr/local/lib/libdc1394.a /usr/local/lib/libraw1394.a -L/usr/X11R6/lib -g -lc -lrt -lGL -lX11 -lXext -v 

# Build Screen.oct without libdc1394 video capture support...
#./mex2oct_ubuntugutsy -r Octave/Screen.cc -IOctave -I/usr/X11R6/include -ICommon/Base -ILinux/Base -ILinux/Screen -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Linux/Screen/*.c Common/Screen/*.c /usr/lib/libXxf86vm.a /usr/lib/libGLU.a -L/usr/X11R6/lib -g -lc -lrt -lGL -lX11 -lXext -v 

# Build Screen.oct without libdc1394 video capture support and without vidmode extensions!!!...
#./mex2oct_ubuntugutsy -r Octave/Screen.cc -DNO_VIDMODEEXTS -IOctave -I/usr/X11R6/include -ICommon/Base -ILinux/Base -ILinux/Screen -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Linux/Screen/*.c Common/Screen/*.c /usr/lib/libGLU.a -L/usr/X11R6/lib -g -lc -lrt -lGL -lX11 -lXext -v 

# Build PsychPortAudio.oct
#./mex2oct_ubuntugutsy -r Octave/PsychPortAudio.cc -IOctave -ICommon/Base -ILinux/Base -ICommon/PsychPortAudio Common/Base/*.cc -ICommon/Screen Linux/Base/*.c Common/Base/*.c Common/PsychPortAudio/*.c /usr/local/lib/libportaudio.a -g -lc -lrt -lasound -v

# Build Eyelink.oct
#./mex2oct_ubuntugutsy -r Octave/Eyelink.cc -IOctave -ICommon/Base -ILinux/Base -ICommon/Eyelink Common/Base/*.cc -ICommon/Screen Linux/Base/*.c Common/Base/*.c Common/Eyelink/*.c -leyelink_core -g -lc -lrt -v

# Build IOPort.oct
./mex2oct_ubuntugutsy -r Octave/IOPort.cc -IOctave -ICommon/Base -ILinux/Base -ICommon/IOPort Common/Base/*.cc -ICommon/Screen Linux/Base/*.c Common/Base/*.c Common/IOPort/*.c -g -lc -lrt -v
