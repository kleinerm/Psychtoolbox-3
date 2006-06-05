# Build GetSecs.oct ...
./osx2oct Octave/GetSecs.cc -ICommon/Base -IOSX/Base -IOSX/GetSecs -IOSX/DictionaryGlue Common/Base/*.cc ../Projects/MacOSX/PsychToolbox/build/PsychToolbox.build/Development-Octavebuild/GetSecs.build/Objects-normal/ppc/*.o -v

# Build WaitSecs.oct ...
./osx2oct Octave/WaitSecs.cc -ICommon/Base -IOSX/Base -IOSX/WaitSecs -IOSX/DictionaryGlue Common/Base/*.cc ../Projects/MacOSX/PsychToolbox/build/PsychToolbox.build/Development-Octavebuild/WaitSecs.build/Objects-normal/ppc/*.o -v

# Build Screen.oct ...
./osx2oct Octave/Screen.cc -ICommon/Base -IOSX/Base -IOSX/Screen -IOSX/Fonts -IOSX/Gestalt -IOSX/DictionaryGlue -IOSX/EthernetAddress -ICommon/Screen Common/Base/*.cc ../Projects/MacOSX/PsychToolbox/build/PsychToolbox.build/Development-Octavebuild/Screen.build/Objects-normal/ppc/*.o -v

# Copy all oct files to the PsychBasic folder:
/bin/cp -f ./*.oct ./../../Psychtoolbox/PsychBasic/