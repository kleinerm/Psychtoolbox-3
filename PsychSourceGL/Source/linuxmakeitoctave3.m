function linuxmakeitoctave3(mode)
% This is the GNU/Linux version of makeit to build the Linux
% mex files for Octave on Linux. It also creates copies of
% the mex files build against Octave-3 and modifies them to
% work on Octave-4.0 - 4.2. Files for Octave 4.4 - 5.2 are
% built and stored into a different target folder.

if ~IsLinux || ~IsOctave
    error('This script is for Octave on Linux only!');
end

if nargin < 1
    mode = 0;
end

% Rebuild all request?
if mode == -1
    % Yes: Call ourselves recursively on all plugins/modes to rebuild
    % everything:
    tic;

    % Build plugin types 0-15 by default:
    modes = 0:15;

    if IsARM
        % Do not build plugin 15 == PsychVulkanCore on ARM / RaspberryPi:
        modes = setdiff (modes, 15);
    end

    for mode = modes
        linuxmakeitoctave3(mode);
    end
    elapsedsecs = toc;
    fprintf('Total rebuild time for all mex files was %f seconds. Bye.\n\n', elapsedsecs);
    return;
end

fprintf('Building plugin type %i ...\n\n', mode);

% Target folder depends if this is a 64 bit or 32 bit runtime:
if ~isempty(strfind(computer, '_64'))
    v = sscanf(version, '%i.%i.%i');

    % Octave 4.4.0 or later?
    if (v(1) >= 5) || (v(1) == 4 && v(2) >= 4)
        % Some backwards incompatible mex api changes. Treat it as Octave-5:
        target = 'PsychBasic/Octave5LinuxFiles64/';
    else
        % Good old <= Octave 4.2. Like Octave 3.8 - 4.2:
        target = 'PsychBasic/Octave3LinuxFiles64/';
    end
else
    target = 'PsychBasic/Octave3LinuxFiles/';
end

% Special folder for ARM binaries:
if ~isempty(strfind(computer, 'arm'))
    target = 'PsychBasic/Octave3LinuxFilesARM/';
end

if mode==0
    % Build Screen.mex:

    % Build against system installed GStreamer-1.8+, ideally 1.18+.
    mex "-W -std=gnu99" --output ../Projects/Linux/build/Screen.mex -Wno-date-time -DPTBMODULE_Screen -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC -DPTB_USE_NVSTUSB -DGLEW_STATIC -DPTBOCTAVE3MEX -D_GNU_SOURCE -I/usr/X11R6/include -I/usr/include/gstreamer-1.0 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include -I/usr/lib/i386-linux-gnu/gstreamer-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include -I/usr/include/libxml2 -I../Cohorts/libnvstusb-code-32/include -ICommon/Base -ICommon/Screen -ILinux/Base -ILinux/Screen -L/usr/X11R6/lib Linux/Base/*.c Linux/Screen/*.c Common/Screen/*.c Common/Base/*.c Common/Screen/tinyexr.cc -lc -ldl -lrt -lGL -lGLU -lX11 -lXext -lX11-xcb -lxcb -lxcb-dri3 -lgstreamer-1.0 -lgstbase-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lgobject-2.0 -lgmodule-2.0 -lxml2 -lgthread-2.0 -lglib-2.0 -lXxf86vm -ldc1394 -lusb-1.0 -lpciaccess -lXi -lXrandr -lXfixes
    
    unix(['cp ../Projects/Linux/build/Screen.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'Screen.mex']);
end

if mode==100
    % Build Screen.mex with Wayland display backend, for desktop Linux:
    fprintf('Building Screen() for native Wayland.\n');
    mex "-W -std=gnu99" --output ../Projects/Linux/build/Screen.mex -Wno-date-time -DPTBMODULE_Screen -DPTB_USE_WAYLAND -DPTB_USE_WAFFLE -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC -DGLEW_STATIC -DPTBOCTAVE3MEX -D_GNU_SOURCE -I/usr/local/include/waffle-1 -L/usr/local/lib/x86_64-linux-gnu/ -I/usr/X11R6/include -I/usr/include/gstreamer-1.0 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include -I/usr/lib/i386-linux-gnu/gstreamer-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/libxml2 -I/usr/include/colord-1 -ICommon/Base -ICommon/Screen -ILinux/Base -ILinux/Screen -L/usr/X11R6/lib   Linux/Base/*.c Linux/Screen/*.c Common/Screen/*.c Common/Base/*.c Common/Screen/tinyexr.cc -lc -ldl -lrt -lGL -lGLU -lX11 -lXext -lX11-xcb -lxcb -lxcb-dri3 -lgstreamer-1.0 -lgstbase-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lgobject-2.0 -lgmodule-2.0 -lxml2 -lgthread-2.0 -lglib-2.0 -lXxf86vm -ldc1394 -lusb-1.0 -lpciaccess -lXi -lXrandr -lXfixes -lwaffle-1 -lwayland-cursor -lxkbcommon -lcolord

    unix(['cp ../Projects/Linux/build/Screen.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'Screen.mex']);
end

if mode==101
    % Build Screen.mex with Waffle display backend, for desktop Linux:
    fprintf('Hmm, me likes some Waffle with this Screen :-)\n');
    mex "-W -std=gnu99" --output ../Projects/Linux/build/Screen.mex -Wno-date-time -DPTBMODULE_Screen -DPTB_USE_WAYLAND_PRESENT -DPTB_USE_WAFFLE -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC -DGLEW_STATIC -DPTBOCTAVE3MEX -D_GNU_SOURCE -I/usr/local/include/waffle-1 -L/usr/local/lib/x86_64-linux-gnu/ -I/usr/X11R6/include -I/usr/include/gstreamer-1.0 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include -I/usr/lib/i386-linux-gnu/gstreamer-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/libxml2 -ICommon/Base -ICommon/Screen -ILinux/Base -ILinux/Screen -L/usr/X11R6/lib   Linux/Base/*.c Linux/Screen/*.c Common/Screen/*.c Common/Base/*.c Common/Screen/tinyexr.cc -lc -ldl -lrt -lGL -lGLU -lX11 -lXext -lX11-xcb -lxcb -lxcb-dri3 -lgstreamer-1.0 -lgstbase-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lgobject-2.0 -lgmodule-2.0 -lxml2 -lgthread-2.0 -lglib-2.0 -lXxf86vm -ldc1394 -lusb-1.0 -lpciaccess -lXi -lXrandr -lXfixes -lwaffle-1

    unix(['cp ../Projects/Linux/build/Screen.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'Screen.mex']);
end

if mode==1000
    % Build Screen.mex with Waffle display backend, for embedded/android devices:
    fprintf('Hmm, me likes some mobile Waffle with this Screen :-)\n');
    mex "-W -std=gnu99" --output ../Projects/Linux/build/Screen.mex -Wno-date-time -DPTBMODULE_Screen -DPTB_USE_WAFFLE -DPTB_USE_EGL -DPTB_USE_GLES1 -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC -DGLEW_STATIC -DPTBOCTAVE3MEX -D_GNU_SOURCE -I/usr/local/include/waffle-1 -L/usr/local/lib/arm-linux-gnueabihf/ -I/usr/X11R6/include -I/usr/include/gstreamer-1.0 -I/usr/lib/arm-linux-gnueabihf/gstreamer-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include -I/usr/include/libxml2 -ICommon/Base -ICommon/Screen -ILinux/Base -ILinux/Screen -L/usr/X11R6/lib   Linux/Base/*.c Linux/Screen/*.c Common/Screen/*.c Common/Base/*.c Common/Screen/tinyexr.cc -lc -ldl -lrt -lGLESv1_CM -lGL -lGLU -lX11 -lXext -lX11-xcb -lxcb -lxcb-dri3 -lgstreamer-1.0 -lgstbase-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lgobject-2.0 -lgmodule-2.0 -lxml2 -lgthread-2.0 -lglib-2.0 -lXxf86vm -ldc1394 -lusb-1.0 -lpciaccess -lXi -lXrandr -lXfixes -lwaffle-1

    unix(['cp ../Projects/Linux/build/Screen.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'Screen.mex']);
end

if mode==1
    % Build GetSecs.mex:
    mex --output ../Projects/Linux/build/GetSecs.mex -Wno-date-time -DPTBMODULE_GetSecs -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/GetSecs -ICommon/Screen  Linux/Base/*.c Common/Base/*.c Common/GetSecs/*.c -lc -lrt -ldl
    unix(['cp ../Projects/Linux/build/GetSecs.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'GetSecs.mex']);
end

if mode==2
    % Build WaitSecs.mex:
    mex --output ../Projects/Linux/build/WaitSecs.mex -Wno-date-time -DPTBMODULE_WaitSecs -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/WaitSecs -ICommon/Screen  Linux/Base/*.c Common/Base/*.c Common/WaitSecs/*.c -lc -lrt -ldl
    unix(['cp ../Projects/Linux/build/WaitSecs.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'WaitSecs.mex']);
end

if mode==3
    % Build PsychPortAudio.mex:
    mex --output ../Projects/Linux/build/PsychPortAudio.mex -Wno-date-time -DPTBMODULE_PsychPortAudio -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/PsychPortAudio -ICommon/Screen  Linux/Base/*.c Common/Base/*.c Common/PsychPortAudio/*.c -lportaudio -lasound -lc -lrt -ldl
    unix(['cp ../Projects/Linux/build/PsychPortAudio.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'PsychPortAudio.mex']);
end

if mode==4
    % Build Eyelink.mex:
    mex --output ../Projects/Linux/build/Eyelink.mex -Wno-date-time -DPTBMODULE_Eyelink -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/Eyelink -ICommon/Screen  Linux/Base/*.c Common/Base/*.c Common/Eyelink/*.c -leyelink_core -lc -lrt -ldl
    unix(['cp ../Projects/Linux/build/Eyelink.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'Eyelink.mex']);
end

if mode==5
    % Build IOPort.mex:
    mex --output ../Projects/Linux/build/IOPort.mex -Wno-date-time -DPTBMODULE_IOPort -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/IOPort -ICommon/Screen  Linux/Base/*.c Common/Base/*.c Common/IOPort/*.c -lc -lrt -ldl
    unix(['cp ../Projects/Linux/build/IOPort.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'IOPort.mex']);
end

if mode==6
    % Build moglcore.mex:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
    try
       mex --output moglcore.mex -Wno-incompatible-pointer-types -Wno-discarded-qualifiers -Wno-int-conversion -DPTB_USE_WAYLAND -DLINUX -DGLEW_STATIC -DPTBOCTAVE3MEX -I/usr/X11R6/include -L/usr/X11R6/lib -lc -lGL -lGLU -lglut moglcore.c gl_auto.c gl_manual.c glew.c mogl_rebinder.c ftglesGlue.c
    catch %#ok<*CTCH>
    end
    unix(['cp moglcore.mex ' PsychtoolboxRoot target]);
    delete('moglcore.mex');
    cd(curdir);
    striplibsfrommexfile([PsychtoolboxRoot target 'moglcore.mex']);
end

if mode==6000
    % Build moglcore.mex for OpenGL-ES:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
    try
       mex --output moglcore.mex -DPTB_USE_WAFFLE -DLINUX -DGLEW_STATIC -DPTBOCTAVE3MEX -I/usr/X11R6/include -L/usr/X11R6/lib -lc -lGLESv1_CM -lGL -lGLU -lglut moglcore.c gl_auto.c gl_manual.c glew.c mogl_rebinder.c ftglesGlue.c
    catch
    end
    unix(['cp moglcore.mex ' PsychtoolboxRoot target]);
    delete('moglcore.mex');
    cd(curdir);
    striplibsfrommexfile([PsychtoolboxRoot target 'moglcore.mex']);
end

if mode==7
    % Build PsychKinectCore.mex:
    % Official build method: mex --output ../Projects/Linux/build/PsychKinectCore.mex -DPTBMODULE_PsychKinectCore -DPTBOCTAVE3MEX -I/usr/include/libusb-1.0 -I/usr/include/libfreenect -ICommon/Base -ILinux/Base -ICommon/PsychKinect -ICommon/Screen  Linux/Base/*.c Common/Base/*.c Common/PsychKinect/*.c -lc -lrt -lfreenect -lusb-1.0
    % Test build against libfreenect-0.5 from GitHub repo:
    mex --output ../Projects/Linux/build/PsychKinectCore.mex -Wno-date-time -DPTBMODULE_PsychKinectCore -DPTBOCTAVE3MEX -I/usr/include/libusb-1.0 -I/usr/local/include/libfreenect -L/usr/local/lib/ -ICommon/Base -ILinux/Base -ICommon/PsychKinect -ICommon/Screen  Linux/Base/*.c Common/Base/*.c Common/PsychKinect/*.c -lc -lrt -ldl -lfreenect -lusb-1.0
    unix(['cp ../Projects/Linux/build/PsychKinectCore.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'PsychKinectCore.mex']);
end

if mode==8
    % Build PsychHID.mex:
    mex --output ../Projects/Linux/build/PsychHID.mex -Wno-date-time -Wno-deprecated-declarations -DPTBMODULE_PsychHID -DPTBOCTAVE3MEX -I/usr/include/libusb-1.0 -ICommon/Base -ILinux/Base -ICommon/PsychHID -ILinux/PsychHID -ICommon/Screen  Linux/Base/*.c Common/Base/*.c Common/PsychHID/*.c Linux/PsychHID/*.c -lc -ldl -lrt -lusb-1.0 -lX11 -lXi -lutil
    unix(['cp ../Projects/Linux/build/PsychHID.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'PsychHID.mex']);
end

if mode==9
    % Build moalcore.mex:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychSound/MOAL/source/')
    try
       mex --output moalcore.mex -DLINUX -DPTBOCTAVE3MEX -lc -lopenal moalcore.c al_auto.c al_manual.c alm.c 
    catch
    end
    unix(['cp moalcore.mex ' PsychtoolboxRoot target]);
    delete('moalcore.mex');
    cd(curdir);
    striplibsfrommexfile([PsychtoolboxRoot target 'moalcore.mex']);
end

if mode == 10
    % Build PsychCV
    mex --output ../Projects/Linux/build/PsychCV.mex -Wno-date-time -DPTBMODULE_PsychCV -DPTBOCTAVE3MEX -ICommon/Base -ICommon/PsychCV -ILinux/Base -I../Cohorts/ARToolkit/include  Common/Base/*.c Linux/Base/*.c Common/PsychCV/*.c -lc -lrt -ldl /usr/local/lib/libARMulti.a /usr/local/lib/libARgsub.a /usr/local/lib/libARgsub_lite.a /usr/local/lib/libARgsubUtil.a /usr/local/lib/libAR.a -lglut
    %mex --output ../Projects/Linux/build/PsychCV.mex -Wno-date-time -DPTBMODULE_PsychCV -DPTBOCTAVE3MEX -DPSYCHCV_USE_OPENCV -ICommon/Base -ICommon/PsychCV -ILinux/Base -I../Cohorts/ARToolkit/include -I/usr/include/opencv Common/Base/*.c Linux/Base/*.c Common/PsychCV/*.c Common/PsychCV/OpenEyesCVEyeTracker/*.cc -lc -lrt -ldl -lopencv_core -lopencv_imgproc -lopencv_highgui /usr/local/lib/libARMulti.a /usr/local/lib/libARgsub.a /usr/local/lib/libARgsub_lite.a /usr/local/lib/libARgsubUtil.a /usr/local/lib/libAR.a -lglut
    unix(['cp ../Projects/Linux/build/PsychCV.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'PsychCV.mex']);
end

if mode == 11
    % Build pnet
    curdir = pwd;
    cd('../../Psychtoolbox/PsychHardware/iViewXToolbox/tcp_udp_ip/')
    try
        mex  -Wno-date-time pnet.c
    catch
    end
    unix(['cp pnet.mex ' PsychtoolboxRoot target]);
    delete('pnet.mex');
    cd(curdir);
    striplibsfrommexfile([PsychtoolboxRoot target 'pnet.mex']);
end

if mode==12
    % Build PsychOculusVRCore.mex:
    try
        % Pure C compile:
        mex --output ../Projects/Linux/build/PsychOculusVRCore.mex -Wno-date-time -DPTBMODULE_PsychOculusVRCore -DPTBOCTAVE3MEX -D_GNU_SOURCE -L/usr/local/lib/ -I/usr/local/include -ICommon/Base -ILinux/Base -ICommon/PsychOculusVRCore Linux/Base/*.c Common/Base/*.c Common/PsychOculusVRCore/*.c -lc -lrt -ldl /usr/local/lib/libOVR.a
    catch
        disp(psychlasterror);
    end

    unix(['cp ../Projects/Linux/build/PsychOculusVRCore.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'PsychOculusVRCore.mex']);
end

if mode == 13 && IsARM
    % Build RPiGPIOMex for RaspberryPi. Needs libwiringPi
    curdir = pwd;
    cd('../../Psychtoolbox/PsychContributed/')
    try
        mex RPiGPIOMex.c -lwiringPi
    catch
    end
    unix(['mv RPiGPIOMex.mex ' PsychtoolboxRoot target]);
    delete('RPiGPIOMex.o');
    cd(curdir);
    striplibsfrommexfile([PsychtoolboxRoot target 'RPiGPIOMex.mex']);
end

if mode==14
    % Build PsychOpenHMDVRCore.mex:
    try
        mex --output ../Projects/Linux/build/PsychOpenHMDVRCore.mex -Wno-date-time -DPTBMODULE_PsychOpenHMDVRCore -DPTBOCTAVE3MEX -D_GNU_SOURCE -L/usr/local/lib/ -I/usr/local/include -I/usr/local/include/openhmd -I/usr/include/openhmd -ICommon/Base -ILinux/Base -ICommon/PsychOpenHMDVRCore Linux/Base/*.c Common/Base/*.c Common/PsychOpenHMDVRCore/*.c -lc -lrt -ldl -lopenhmd
    catch
        disp(psychlasterror);
    end

    unix(['cp ../Projects/Linux/build/PsychOpenHMDVRCore.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'PsychOpenHMDVRCore.mex']);
end

if mode==15
    % Build PsychVulkanCore.mex:
    try
        mex --output ../Projects/Linux/build/PsychVulkanCore.mex -Wno-date-time -DPTBMODULE_PsychVulkanCore -DPTBOCTAVE3MEX -ICommon/Base -ILinux/Base -ICommon/PsychVulkanCore Linux/Base/*.c Common/Base/*.c Common/PsychVulkanCore/*.c -lc -lrt -ldl -lX11 -lvulkan
    catch
        disp(psychlasterror);
    end

    unix(['cp ../Projects/Linux/build/PsychVulkanCore.mex ' PsychtoolboxRoot target]);
    striplibsfrommexfile([PsychtoolboxRoot target 'PsychVulkanCore.mex']);
end

if str2num(version()(1)) < 5
    % Remove stale object files from current dir on Octave 4 and earlier:
    delete('*.o');
end

return;
end

% Our own override implementation of mex(), shadowing
% octave's mex.m . This one uses glob() to glob-expand all
% *.c shell patterns to corresponding lists of source
% filenames, so Octave 3.8's mkoctfile can "handle" such
% wildcards. Older mkoctfile implementations did this,
% but Octave 3.8.1's mkoctfile is reimplemented from scratch
% as a C++ piece of art, which can't expand wildcards anymore.
function mex(varargin)
  inargs = {varargin{:}};
  outargs = {"--mex"};
  outargs = {outargs{:}, "-s"};

  for i = 1:length(inargs)
    if ~isempty(strfind(inargs{i}, '*'))
      outargs = {outargs{:}, glob(inargs{i})};
    else
      outargs = {outargs{:}, inargs{i}};
    end
  end
  
  args = cellstr(char(outargs));
  mkoctfile (args{:});
end
