function osxmakeit(mode)
% This is the macOS version of makeit: It is meant for building PTB for
% 64-Bit Matlab on macOS 13+, using the 13.3 SDK.

if ~IsOSX(1) || IsOctave
    error('osxmakeit only works with a 64-Bit version of Matlab for macOS!');
end

if nargin < 1
    mode = 0;
end

% Rebuild all request?
if mode == -1
    % Yes: Call ourselves recursively on all plugins/modes to rebuild
    % everything:
    tic;
    for mode = 0:15
        osxmakeit(mode);
    end
    elapsedsecs = toc;
    fprintf('Total rebuild time for all mex files was %f seconds. Bye.\n\n', elapsedsecs);
    return;
end

fprintf('Building plugin type %i ...\n\n', mode);

%  LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio" CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" CXXFLAGS=""

if mode==0
    % Build Screen:
    % Depends: GStreamer-1.0 (actually 1.18.0+), libdc1394-2

    % Must build tinyexr separately, because it needs C++ compile
    % incompatible with the C/Obj-C compile in the main mex cmd below:
    mex -c CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" Common/Screen/tinyexr.cc

    % Build with weak linking of GStreamer and libdc1394 via -weak_library
    % flag. This means that a missing GStreamer or libdc1394 installation
    % will not cause linker failure at Screen() load time -- Screen
    % continues to work as long as no GStreamer-dependent functions are
    % used. In that case it will abort with a helpful error message:
    mex -outdir ../Projects/MacOSX/build -output Screen -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_Screen -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC -DGLEW_STATIC ...
        LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstreamer-1.0.0.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstbase-1.0.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstapp-1.0.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstvideo-1.0.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstpbutils-1.0.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgobject-2.0.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgmodule-2.0.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libxml2.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgthread-2.0.dylib ...
        -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libglib-2.0.dylib ...
        -rpath /Library/Frameworks/GStreamer.framework/Versions/Current/lib ...
        -weak_library /usr/local/lib/libdc1394.dylib ...
        -framework CoreServices -framework CoreFoundation -framework ApplicationServices -framework CoreAudio -framework OpenGL -framework CoreVideo ...
        -framework CoreDisplay -F /System/Library/PrivateFrameworks -framework DisplayServices ...
        -U CoreDisplay_Display_SetUserBrightness -U CoreDisplay_Display_GetUserBrightness ...
        -U DisplayServicesCanChangeBrightness -U DisplayServicesBrightnessChanged ...
        -U CoreDisplay_Display_SetAutoBrightnessIsEnabled ...
        -framework IOKit -framework SystemConfiguration -framework Carbon -framework Cocoa -framework CoreText -framework QuartzCore" ...
        CFLAGS="\$CFLAGS -x objective-c -mmacosx-version-min=10.11" -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/gstreamer-1.0 ...
        -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/libxml2 -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/glib-2.0 ...
        -I/Library/Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0/include/ ...
        -I/Library/Frameworks/GStreamer.framework/Versions/Current/lib/glib-2.0/include -I/usr/local/include ...
        -ICommon/Base -ICommon/Screen -IOSX/Base -IOSX/Screen -IOSX/Fonts -IOSX/EthernetAddress ...
         "OSX/Screen/*.c" "Common/Screen/*.c" "OSX/Base/*.c" "OSX/Fonts/*FontGlue*.c" "OSX/Fonts/FontInfo.c" "OSX/EthernetAddress/*.c" "Common/Base/*.c" ...
         tinyexr.o ...
        -L/usr/local/lib

    delete tinyexr.o;
    movefile(['../Projects/MacOSX/build/Screen.' mexext], [PsychtoolboxRoot 'PsychBasic/']);
end

if mode==1
    % Build GetSecs:
    mex -outdir ../Projects/MacOSX/build -output GetSecs -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_GetSecs CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/Base -IOSX/Base -ICommon/GetSecs  "OSX/Base/*.c" "Common/Base/*.c" "Common/GetSecs/*.c"
    unix(['mv ../Projects/MacOSX/build/GetSecs.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==2
    % Build WaitSecs:
    mex -outdir ../Projects/MacOSX/build -output WaitSecs -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_WaitSecs CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/Base -IOSX/Base -ICommon/WaitSecs  "OSX/Base/*.c" "Common/Base/*.c" "Common/WaitSecs/*.c"
    unix(['mv ../Projects/MacOSX/build/WaitSecs.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==3
    % Depends: PortAudio
    % Build PsychPortAudio:
    mex -outdir ../Projects/MacOSX/build -output PsychPortAudio -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychPortAudio CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework AudioToolbox -framework AudioUnit" -ICommon/Base -IOSX/Base -ICommon/PsychPortAudio  "OSX/Base/*.c" "Common/Base/*.c" "Common/PsychPortAudio/*.c" ../Cohorts/PortAudio/libportaudio_osx_64.a
    unix(['mv ../Projects/MacOSX/build/PsychPortAudio.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==4
    % Depends: eyelink-SDK: eyelink_core.framework installed under default location /Library/Frameworks/
    % Build Eyelink:
    mex -outdir ../Projects/MacOSX/build -output Eyelink -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_Eyelink CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework eyelink_core -rpath /Library/Frameworks/" -I/Library/Frameworks/eyelink_core.framework/Headers -ICommon/Base -IOSX/Base -ICommon/Eyelink  "OSX/Base/*.c" "Common/Base/*.c" "Common/Eyelink/*.c"
    unix(['mv ../Projects/MacOSX/build/Eyelink.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==5
    % Build IOPort:
    mex -outdir ../Projects/MacOSX/build -output IOPort -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_IOPort CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio"  -ICommon/Base -IOSX/Base -ICommon/IOPort  "OSX/Base/*.c" "Common/Base/*.c" "Common/IOPort/*.c"
    unix(['mv ../Projects/MacOSX/build/IOPort.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==6
    % Build moglcore:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
    try
        mex -outdir ./ -output moglcore -DMACOSX -DGLEW_STATIC -largeArrayDims -DMEX_DOUBLE_HANDLE CFLAGS="\$CFLAGS -Wno-deprecated-declarations -Wno-tautological-pointer-compare -Wno-incompatible-pointer-types -Wno-incompatible-pointer-types-discards-qualifiers -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework OpenGL -framework GLUT" -I./ moglcore.c gl_auto.c gl_manual.c glew.c mogl_rebinder.c ftglesGlue.c
    catch %#ok<*CTCH>
        ple;
    end
    unix(['mv moglcore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    cd(curdir);
end

if mode==7
    % Depends: libfreenect, libusb-1.0
    % Build PsychKinectCore:
    mex -outdir ../Projects/MacOSX/build -output PsychKinectCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychKinectCore CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -I/usr/local/include/libusb-1.0 -I/usr/local/include/libfreenect -ICommon/Base -IOSX/Base -ICommon/PsychKinect  "OSX/Base/*.c" "Common/Base/*.c" "Common/PsychKinect/*.c" -L/usr/local/lib -lfreenect -lusb-1.0
    unix(['mv ../Projects/MacOSX/build/PsychKinectCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==8
    % Build PsychHID:
    % Dynamic link:   mex -outdir ../Projects/MacOSX/build -output PsychHID -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychHID CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework ApplicationServices -framework Carbon -framework CoreAudio -framework IOKit" -ICommon/Base -IOSX/Base -ICommon/PsychHID -IOSX/PsychHID -I../Cohorts/HID_Utilities_64Bit/ -I../Cohorts/HID_Utilities_64Bit/IOHIDManager  "Common/PsychHID/*.c" "OSX/PsychHID/*.c" "OSX/Base/*.c" "Common/Base/*.c" -L../Cohorts/HID_Utilities_64Bit/build/Release -lHID_Utilities
    mex -outdir ../Projects/MacOSX/build -output PsychHID -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychHID CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework ApplicationServices -framework CoreServices -framework CoreFoundation -framework Carbon -framework CoreAudio -framework IOKit -weak_library /usr/local/lib/libusb-1.0.dylib" -ICommon/Base -IOSX/Base -ICommon/PsychHID -IOSX/PsychHID -I../Cohorts/HID_Utilities_64Bit/ -I../Cohorts/HID_Utilities_64Bit/IOHIDManager -I/usr/local/include/libusb-1.0 "Common/PsychHID/*.c" "OSX/PsychHID/*.c" "OSX/Base/*.c" "Common/Base/*.c" ../Cohorts/HID_Utilities_64Bit/build/Release/libHID_Utilities64.a
    movefile(['../Projects/MacOSX/build/PsychHID.' mexext], [PsychtoolboxRoot 'PsychBasic/']);
end

if mode==9
    % Build moalcore:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychSound/MOAL/source/')
    try
        mex -outdir ./ -output moalcore -largeArrayDims -DMEX_DOUBLE_HANDLE -DMACOSX CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework OpenAL -framework ApplicationServices -framework Carbon" moalcore.c al_auto.c al_manual.c alm.c
    catch
        ple;
    end
    unix(['mv ./moalcore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);

    cd(curdir);
end

if mode==10
    % Build FontInfo:
    mex -outdir ../Projects/MacOSX/build -output FontInfo -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_FontInfo CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework ApplicationServices -framework CoreAudio" -ICommon/Base -ICommon/Fonts -IOSX/Base -IOSX/Fonts  "Common/Base/*.c" "OSX/Base/*.c" "OSX/Fonts/*.c"
    unix(['mv ../Projects/MacOSX/build/FontInfo.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==11
    % Build MachAbsoluteTimeClockFrequency, MachGetPriorityMex,
    % MachSetPriorityMex, which are needed by the Priority() command on
    % OSX:

    % MachAbsoluteTimeClockFrequency
    mex -outdir ../Projects/MacOSX/build -output MachAbsoluteTimeClockFrequency -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_MachAbsoluteTimeClockFrequency CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/Base -IOSX/Base -ICommon/MachAbsoluteTimeClockFrequency  "OSX/Base/*.c" "Common/Base/*.c" "Common/MachAbsoluteTimeClockFrequency/*.c"
    unix(['mv ../Projects/MacOSX/build/MachAbsoluteTimeClockFrequency.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);

    % MachGetPriorityMex
    mex -outdir ../Projects/MacOSX/build -output MachGetPriorityMex -largeArrayDims -DMEX_DOUBLE_HANDLE CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/MachPriorityMex "Common/MachPriorityMex/MachPriorityCommonMex.c" "Common/MachPriorityMex/MachGetPriorityMex.c"
    unix(['mv ../Projects/MacOSX/build/MachGetPriorityMex.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);

    % MachSetPriorityMex
    mex -outdir ../Projects/MacOSX/build -output MachSetPriorityMex -largeArrayDims -DMEX_DOUBLE_HANDLE CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/MachPriorityMex "Common/MachPriorityMex/MachPriorityCommonMex.c" "Common/MachPriorityMex/MachSetPriorityMex.c"
    unix(['mv ../Projects/MacOSX/build/MachSetPriorityMex.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==12
    % Build Gestalt:
    mex -outdir ../Projects/MacOSX/build -output Gestalt -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_Gestalt CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/Base -IOSX/Base -IOSX/Gestalt -IOSX/OS9ToolboxFragments  "OSX/Base/*.c" "Common/Base/*.c" "OSX/Gestalt/*.c"
    unix(['mv ../Projects/MacOSX/build/Gestalt.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==13
    % Build pnet:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychHardware/iViewXToolbox/tcp_udp_ip/')
    try
        mex -output pnet pnet.c -largeArrayDims -DMEX_DOUBLE_HANDLE CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11"
        unix(['mv ./pnet.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    catch
    end
    cd(curdir);
end

if mode==14 && false
    % Build PsychOculusVRCore:
    % Depends on Oculus VR SDK v0.5 - Does not work on macOS 10.15 or later
    % due to the VR runtime being unsupported 32-Bit Intel only.
    mex -outdir ../Projects/MacOSX/build -output PsychOculusVRCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychOculusVRCore CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework LibOVR" -ICommon/Base -IOSX/Base -ICommon/PsychOculusVRCore -I/Library/Frameworks/LibOVR.framework/Versions/Current/Headers/ "OSX/Base/*.c" "Common/Base/*.c" "Common/PsychOculusVRCore/*.c"
    unix(['mv ../Projects/MacOSX/build/PsychOculusVRCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==15
    % Build PsychVulkanCore:
    % Depends on a system level (/usr/local/[share/include/lib]/
    % installation of the Vulkan SDK and MoltenVK for macOS from
    % https://vulkan.lunarg.com for prebuilt SDK and Vulkan ICD,
    % https://github.com/KhronosGroup/MoltenVK for source code.
    %
    % We link directly to MoltenVK, as we use some MVK functions which are
    % not yet part of the Vulkan standard, and thereby not supported by the
    % Vulkan loader and layers, as of Mid-May 2021.
    %
    % We link to OpenGL to implement OpenGL-Vulkan interop.
    try
        mex -outdir ../Projects/MacOSX/build/ -output PsychVulkanCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychVulkanCore CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework OpenGL" -ICommon/Base -IOSX/Base -ICommon/PsychVulkanCore -I/usr/local/include "Common/PsychVulkanCore/*.c" "OSX/Base/*.c" "Common/Base/*.c" -lvulkan -lMoltenVK
    catch
        disp(psychlasterror);
    end
    movefile(['../Projects/MacOSX/build/PsychVulkanCore.' mexext], [PsychtoolboxRoot 'PsychBasic/']);
end

return;
