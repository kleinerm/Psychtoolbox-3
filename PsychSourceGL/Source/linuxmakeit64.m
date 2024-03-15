function linuxmakeit64(mode, cibuild)
% This is the GNU/Linux version of makeit to build the Linux
% mex files for 64-Bit Matlab on Linux.

if ~IsLinux(1) || IsOctave
    error('This script is for 64-Bit Linux on Matlab only!');
end

if nargin < 1 || isempty(mode)
    mode = 0;
end

if nargin < 2 || isempty(cibuild)
    cibuild = 0;
end

% Rebuild all request?
if mode == -1
    % Yes: Call ourselves recursively on all plugins/modes to rebuild
    % everything:
    tic;
    for mode = 0:16
        linuxmakeit64(mode, cibuild);
    end
    elapsedsecs = toc;
    fprintf('Total rebuild time for all mex files was %f seconds. Bye.\n\n', elapsedsecs);
    return;
end

fprintf('Building plugin type %i ...\n\n', mode);

if mode==0
    % Build Screen.mexa64: We build against GStreamer-1
    mex -c Common/Screen/tinyexr.cc
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output Screen -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_Screen -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC -DPTB_USE_NVSTUSB -DGLEW_STATIC -D_GNU_SOURCE -I/usr/include -I/usr/include/gstreamer-1.0 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/libxml2 -I../Cohorts/libnvstusb-code-32/include -ICommon/Base -ICommon/Screen -ILinux/Base -ILinux/Screen "Linux/Base/*.c" "Linux/Screen/*.c" "Common/Screen/*.c" "Common/Base/*.c" tinyexr.o -lc -lrt -ldl -lGL -lGLU -lX11 -lXext -lX11-xcb -lxcb -lxcb-dri3 -lxcb-present -lgstreamer-1.0 -lgstbase-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lgobject-2.0 -lgmodule-2.0 -lxml2 -lgthread-2.0 -lglib-2.0 -lXxf86vm -ldc1394 -lusb-1.0 -lpciaccess -lXi -lXrandr -lXfixes -lXcomposite
    unix(['mv ../Projects/Linux/build/Screen.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==1
    % Build GetSecs.mexa64:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output GetSecs -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_GetSecs -ICommon/Base -ILinux/Base -ICommon/GetSecs -ICommon/Screen  "Linux/Base/*.c" "Common/Base/*.c" "Common/GetSecs/*.c" -lc -lrt -ldl
    unix(['mv ../Projects/Linux/build/GetSecs.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==2
    % Build WaitSecs.mexa64:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output WaitSecs -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_WaitSecs -ICommon/Base -ILinux/Base -ICommon/WaitSecs -ICommon/Screen  "Linux/Base/*.c" "Common/Base/*.c" "Common/WaitSecs/*.c" -lc -lrt -ldl
    unix(['mv ../Projects/Linux/build/WaitSecs.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==3
    % Build PsychPortAudio.mexa64:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output PsychPortAudio -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychPortAudio -ICommon/Base -ILinux/Base -ICommon/PsychPortAudio -ICommon/Screen  "Linux/Base/*.c" "Common/Base/*.c" "Common/PsychPortAudio/*.c" -lportaudio -lasound -lc -lrt -ldl
    unix(['mv ../Projects/Linux/build/PsychPortAudio.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==4
    % Build Eyelink.mexa64: Needs Eyelink SDK (eyelink_core component) from SR-Research.
    % See https://www.sr-research.com/support/docs.php?topic=linuxsoftware for install instructions.
    try
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output Eyelink -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_Eyelink -I/usr/include/EyeLink -ICommon/Base -ILinux/Base -ICommon/Eyelink -ICommon/Screen  "Linux/Base/*.c" "Common/Base/*.c" "Common/Eyelink/*.c" -leyelink_core -lc -lrt -ldl
    unix(['mv ../Projects/Linux/build/Eyelink.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    catch
        disp(psychlasterror);
    end
end

if mode==5
    % Build IOPort.mexa64:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output IOPort -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_IOPort -ICommon/Base -ILinux/Base -ICommon/IOPort -ICommon/Screen  "Linux/Base/*.c" "Common/Base/*.c" "Common/IOPort/*.c" -lc -lrt -ldl
    unix(['mv ../Projects/Linux/build/IOPort.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==6
    % Build moglcore.mexa64:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
    try
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ./ -output moglcore -DLINUX -DGLEW_STATIC -largeArrayDims -DMEX_DOUBLE_HANDLE -I./ -I/usr/include -lc -lGL -lGLU -lglut moglcore.c gl_auto.c gl_manual.c glew.c mogl_rebinder.c ftglesGlue.c
    catch %#ok<*CTCH>
    end
    unix(['mv moglcore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    cd(curdir);
end

if mode==7
    % Build PsychKinectCore.mexa64:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output PsychKinectCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychKinectCore -I/usr/include/libusb-1.0 -I/usr/local/include/libfreenect -L/usr/local/lib -ICommon/Base -ILinux/Base -ICommon/PsychKinect -ICommon/Screen  "Linux/Base/*.c" "Common/Base/*.c" "Common/PsychKinect/*.c" -lc -lrt -ldl -lfreenect -lusb-1.0
    unix(['mv ../Projects/Linux/build/PsychKinectCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==8
    % Build PsychHID.mexa64:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output PsychHID -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychHID -I/usr/include/libusb-1.0 -ICommon/Base -ILinux/Base -ICommon/PsychHID -ILinux/PsychHID -ICommon/Screen  "Linux/Base/*.c" "Common/Base/*.c" "Common/PsychHID/*.c" "Linux/PsychHID/*.c" -lc -ldl -lrt -lusb-1.0 -lX11 -lXi -lutil
    unix(['mv ../Projects/Linux/build/PsychHID.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==9
    % Build moalcore.mexa64:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychSound/MOAL/source/')
    try
       mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ./ -output moalcore -largeArrayDims -DMEX_DOUBLE_HANDLE -DLINUX -lc -lopenal moalcore.c al_auto.c al_manual.c alm.c
    catch
    end
    unix(['mv ./moalcore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    cd(curdir);
end

if mode == 10
    % Build PsychCV.mexa64:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output PsychCV -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychCV -DPSYCHCV_USE_APRILTAGS -I/usr/include/apriltag -ICommon/Base -ILinux/Base -ICommon/PsychCV "Common/Base/*.c" "Linux/Base/*.c" "Common/PsychCV/*.c" -lc -lrt -ldl -lapriltag -lapriltag-utils
    unix(['mv ../Projects/Linux/build/PsychCV.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode == 11
    % Build pnet.mexa64:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychHardware/iViewXToolbox/tcp_udp_ip/')
    try
        mex -O -g CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -largeArrayDims -DMEX_DOUBLE_HANDLE pnet.c
    catch
    end
    unix(['mv ./pnet.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    cd(curdir);
end

if mode==12
    % Build PsychOculusVRCore.mexa64:
    try
        mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output PsychOculusVRCore -largeArrayDims -DMEX_DOUBLE_HANDLE -D_GNU_SOURCE -DPTBMODULE_PsychOculusVRCore -L/usr/local/lib/ -I/usr/local/include -ICommon/Base -ILinux/Base -ICommon/PsychOculusVRCore "Linux/Base/*.c" "Common/Base/*.c" "Common/PsychOculusVRCore/*.c" -lc -lrt -ldl /usr/local/lib/libOVR.a
    catch
        disp(psychlasterror);
    end
    unix(['mv ../Projects/Linux/build/PsychOculusVRCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==14
    % Build PsychOpenHMDVRCore.mexa64:
    try
        mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output PsychOpenHMDVRCore -largeArrayDims -DMEX_DOUBLE_HANDLE -D_GNU_SOURCE -DPTBMODULE_PsychOpenHMDVRCore -L/usr/local/lib/ -I/usr/local/include -I/usr/local/include/openhmd -I/usr/include/openhmd -ICommon/Base -ILinux/Base -ICommon/PsychOpenHMDVRCore "Linux/Base/*.c" "Common/Base/*.c" "Common/PsychOpenHMDVRCore/*.c" -lc -lrt -ldl -lopenhmd
    catch
        disp(psychlasterror);
    end
    unix(['mv ../Projects/Linux/build/PsychOpenHMDVRCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==15
    % Build PsychVulkanCore.mexa64:
    % Needs at least Vulkan SDK version 1.2.189
    try
        mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output PsychVulkanCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychVulkanCore -ICommon/Base -ILinux/Base -ICommon/PsychVulkanCore "Linux/Base/*.c" "Common/Base/*.c" "Common/PsychVulkanCore/*.c" -lc -lrt -ldl -lX11 -lvulkan
    catch
        disp(psychlasterror);
    end
    unix(['mv ../Projects/Linux/build/PsychVulkanCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==16
    % Build PsychOpenXRCore.mexa64:
    try
        mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions -pthread' -outdir ../Projects/Linux/build/ -output PsychOpenXRCore -largeArrayDims -DMEX_DOUBLE_HANDLE -D_GNU_SOURCE -DPTBMODULE_PsychOpenXRCore -ICommon/Base -ILinux/Base -ICommon/PsychOpenXRCore -ICommon/PsychOpenXRCore/nanopb "Linux/Base/*.c" "Common/Base/*.c" "Common/PsychOpenXRCore/*.c" "Common/PsychOpenXRCore/nanopb/*.c" -lc -lrt -ldl -lopenxr_loader
    catch
        disp(psychlasterror);
    end
    unix(['mv ../Projects/Linux/build/PsychOpenXRCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

return;
