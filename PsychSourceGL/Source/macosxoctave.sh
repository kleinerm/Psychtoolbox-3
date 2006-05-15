# Build GetSecs.oct ...
#ls Linux/Base/*.c Common/Base/*.c Common/GetSecs/*.c | xargs ./mex2oct Octave/GetSecs.c -DPTBOCTAVE -ICommon/Base -ILinux/Base -ICommon/GetSecs -lc 

# Build WaitSecs.oct ...
#ls Linux/Base/*.c Common/Base/*.c Common/WaitSecs/*.c | xargs ./mex2oct Octave/WaitSecs.c -DPTBOCTAVE -ICommon/Base -ILinux/Base -ICommon/WaitSecs -lc 

# Build Screen.oct ...
./osx2oct Octave/Screen.cc -ICommon/Base -IOSX/Base -IOSX/Screen -IOSX/Fonts -IOSX/Gestalt -IOSX/DictionaryGlue -IOSX/EthernetAddress -ICommon/Screen Common/Base/*.cc OSX/Base/*.c Common/Base/*.c OSX/Screen/*.c OSX/EthernetAddress/*.c OSX/Fonts/*.c OSX/DictionaryGlue/*.c OSX/Gestalt/*.c Common/Screen/*.c -v

# Copy all oct files to the PsychBasic folder:
/bin/cp -f ./*.oct ./../../Psychtoolbox/PsychBasic/