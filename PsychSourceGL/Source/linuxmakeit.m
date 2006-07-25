function linuxmakeit(mode)
% This is the GNU/Linux version of makeit to build the Linux
% version of PTB's Screen - command.
if nargin < 1
    mode = 0
end;

if mode==0
    mex -v -outdir ../Projects/Linux/build/ -output Screen -I/usr/X11R6/include -ICommon/Base -ICommon/Screen -ILinux/Base -ILinux/Screen -L/usr/X11R6/lib Common/Base/*.cc Linux/Base/*.c Linux/Screen/*.c Common/Screen/*.c Common/Base/*.c -lc -lGL -lX11 -lXext /usr/X11R6/lib/libXxf86vm.a /usr/X11R6/lib/libGLU.a /usr/local/lib/libdc1394.a /usr/local/lib/libraw1394.a
    unix('cp /mnt/home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/Screen.mexglx /mnt/home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/');
end;

if mode==1
    mex -v -outdir ../Projects/Linux/build/ -output GetSecs -ICommon/Base -ILinux/Base -ICommon/GetSecs Linux/Base/*.c Common/Base/*.c Common/GetSecs/*.c -lc
    unix('cp /mnt/home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/GetSecs.mexglx /mnt/home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/');
end;

if mode==2
    mex -v -outdir ../Projects/Linux/build/ -output WaitSecs -ICommon/Base -ILinux/Base -ICommon/WaitSecs Linux/Base/*.c Common/Base/*.c Common/WaitSecs/*.c -lc
    unix('cp /mnt/home/kleinerm/projects/OpenGLPsychtoolbox/trunk/PsychSourceGL/Projects/Linux/build/WaitSecs.mexglx /mnt/home/kleinerm/projects/OpenGLPsychtoolbox/trunk/Psychtoolbox/PsychBasic/');
end;

return;
