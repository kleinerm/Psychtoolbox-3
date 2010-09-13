function linuxmakeitoctave3_ubuntugutsy(mode)
% This is the GNU/Linux version of makeit to build the Linux
% version of PTB's binary plugins for GNU/Octave V3.2.0 and later.
% This version is adapted to build on the "funky" Laptop under Ubuntu Linux
% 7.1 Gutsy or any Linux system with similar configuration.
%
% CAUTION: You *MUST* make sure that the MEX files are compiled *without*
% compiler optimization, ie., with a level of -O0 !!! Otherwise the files
% will be *miscompiled* !!! At first glance they seem to work, but the will
% malfunction and mysteriously crash in many ways, almost impossible to debug!!
%
if nargin < 1
    mode = 0;
end;

fprintf('Building plugin type %i ...\n\n', mode);

if mode==0
    % Build Screen.mex:
    mex -v -g --output ../Projects/Linux/build/Screen.mex -DPTBVIDEOCAPTURE_LIBDC -DPTBOCTAVE3MEX -I/usr/X11R6/include -ICommon/Base -ICommon/Screen -ILinux/Base -ILinux/Screen -L/usr/X11R6/lib Common/Base/*.cc Linux/Base/*.c Linux/Screen/*.c Common/Screen/*.c Common/Base/*.c -lc -lrt -lGL -lX11 -lXext /usr/lib/libXxf86vm.a /usr/lib/libGLU.a /usr/lib/libdc1394.a /usr/lib/libraw1394.a /usr/lib/libusb-1.0.a 
    unix('mv /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/Screen.mex /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/Octave3LinuxFiles/');
    striplibsfrommexfile([PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles/Screen.mex']);
end;

if mode==1
    % Build GetSecs.mex:
    mex -v -g --output ../Projects/Linux/build/GetSecs.mex -DPTBMODULE_GetSecs -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/GetSecs -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Common/GetSecs/*.c -lc -lrt 
    unix('mv /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/GetSecs.mex /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/Octave3LinuxFiles/');
    striplibsfrommexfile([PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles/GetSecs.mex']);
end;

if mode==2
    % Build WaitSecs.mex:
    mex -v -g --output ../Projects/Linux/build/WaitSecs.mex -DPTBMODULE_WaitSecs -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/WaitSecs -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Common/WaitSecs/*.c -lc -lrt 
    unix('mv /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/WaitSecs.mex /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/Octave3LinuxFiles/');
    striplibsfrommexfile([PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles/WaitSecs.mex']);
end;

if mode==3
    % Build PsychPortAudio.mex:
    mex -v -g --output ../Projects/Linux/build/PsychPortAudio.mex -DPTBMODULE_PsychPortAudio -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/PsychPortAudio -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Common/PsychPortAudio/*.c /usr/local/lib/libportaudio.a -lc -lrt -lasound
    unix('mv /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/PsychPortAudio.mex /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/Octave3LinuxFiles/');
    striplibsfrommexfile([PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles/PsychPortAudio.mex']);
end

if mode==4
    % Build Eyelink.mex:
    mex -v -g --output ../Projects/Linux/build/Eyelink.mex -DPTBMODULE_Eyelink -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/Eyelink -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Common/Eyelink/*.c -leyelink_core -lc -lrt
    unix('mv /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/Eyelink.mex /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/Octave3LinuxFiles/');    
    striplibsfrommexfile([PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles/Eyelink.mex']);
end

if mode==5
    % Build IOPort.mex:
    mex -v -g --output ../Projects/Linux/build/IOPort.mex -DPTBMODULE_IOPort -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/IOPort -ICommon/Screen Common/Base/*.cc Linux/Base/*.c Common/Base/*.c Common/IOPort/*.c -lc -lrt
    unix('mv /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/IOPort.mex /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/Octave3LinuxFiles/');    
    striplibsfrommexfile([PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles/IOPort.mex']);
end

if mode==6
    % Build moglcore.mex:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
    try
       mex -v -g --output moglcore.mex -DLINUX -DGLEW_STATIC -DPTBOCTAVE3MEX -I/usr/X11R6/include -L/usr/X11R6/lib -lc -lGL -lglut /usr/lib/libGLU.a moglcore.c gl_auto.c gl_manual.c glew.c mogl_rebinder.c
    catch
    end
    unix('mv moglcore.mex /home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/Octave3LinuxFiles/');
    cd(curdir);
    striplibsfrommexfile([PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles/moglcore.mex']);
end;

return;
