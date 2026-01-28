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

%  LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio" CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" CXXFLAGS=""

if mode==0
    % Build Screen:
    % Depends: GStreamer-1.0 (actually 1.18.0+), libdc1394-2

    % Must build tinyexr separately, because it needs C++ compile
    % incompatible with the C/Obj-C compile in the main mex cmd below:
    mex -c CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" Common/Screen/tinyexr.cc

    % Build with weak linking of GStreamer and libdc1394 via -weak_library
    % flag. This means that a missing GStreamer or libdc1394 installation
    % will not cause linker failure at Screen() load time -- Screen
    % continues to work as long as no GStreamer-dependent functions are
    % used. In that case it will abort with a helpful error message:
    mex -outdir ../Projects/MacOSX/build -output Screen -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_Screen -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC -DGLEW_STATIC ...
        LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -weak_library /Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstreamer-1.0.0.dylib ...
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
        -U DisplayServicesGetBrightness -U DisplayServicesSetBrightness ...
        -framework IOKit -framework SystemConfiguration -framework Carbon -framework Cocoa -framework CoreText -framework QuartzCore" ...
        CFLAGS="\$CFLAGS -x objective-c -Wno-gnu-folding-constant -mmacosx-version-min=10.13" ...
        -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/gstreamer-1.0 ...
        -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/libxml2 -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/glib-2.0 ...
        -I/Library/Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0/include/ ...
        -I/Library/Frameworks/GStreamer.framework/Versions/Current/lib/glib-2.0/include -I/usr/local/include -I/opt/homebrew/include ...
        -ICommon/Base -ICommon/Screen -IOSX/Base -IOSX/Screen -IOSX/Fonts -IOSX/EthernetAddress ...
         "OSX/Screen/*.c" "Common/Screen/*.c" "OSX/Base/*.c" "OSX/Fonts/*FontGlue*.c" "OSX/Fonts/FontInfo.c" "OSX/EthernetAddress/*.c" "Common/Base/*.c" ...
         tinyexr.o ...
        -L/usr/local/lib -L../../Psychtoolbox/PsychBasic/PsychPlugins/ ...
        -lLexActivator

    delete tinyexr.o;

    macossetmatlabrpath('Screen');
    movefile(['../Projects/MacOSX/build/Screen.' mexext], [PsychtoolboxRoot 'PsychBasic/']);
end

if mode==1
    % Build GetSecs:
    mex -outdir ../Projects/MacOSX/build -output GetSecs -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_GetSecs -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/Base -IOSX/Base -ICommon/GetSecs  "OSX/Base/*.c" "Common/Base/*.c" "Common/GetSecs/*.c" -lLexActivator
    macossetmatlabrpath('GetSecs');
    unix(['mv ../Projects/MacOSX/build/GetSecs.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==2
    % Build WaitSecs:
    mex -outdir ../Projects/MacOSX/build -output WaitSecs -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_WaitSecs -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/Base -IOSX/Base -ICommon/WaitSecs  "OSX/Base/*.c" "Common/Base/*.c" "Common/WaitSecs/*.c" -lLexActivator
    macossetmatlabrpath('WaitSecs');
    unix(['mv ../Projects/MacOSX/build/WaitSecs.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==3
    % Depends: PortAudio
    % Build PsychPortAudio:
    mex -outdir ../Projects/MacOSX/build -output PsychPortAudio -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychPortAudio -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework AudioToolbox -framework AudioUnit" -ICommon/Base -IOSX/Base -ICommon/PsychPortAudio  "OSX/Base/*.c" "Common/Base/*.c" "Common/PsychPortAudio/*.c" ../Cohorts/PortAudio/libportaudio_osx_64.a -lLexActivator
    macossetmatlabrpath('PsychPortAudio');
    unix(['mv ../Projects/MacOSX/build/PsychPortAudio.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==4
    % Depends: eyelink-SDK: eyelink_core.framework installed under default location /Library/Frameworks/
    % Build Eyelink:
    mex -outdir ../Projects/MacOSX/build -output Eyelink -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_Eyelink -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework eyelink_core -rpath /Library/Frameworks/" -I/Library/Frameworks/eyelink_core.framework/Headers -ICommon/Base -IOSX/Base -ICommon/Eyelink  "OSX/Base/*.c" "Common/Base/*.c" "Common/Eyelink/*.c"
    macossetmatlabrpath('Eyelink');
    unix(['mv ../Projects/MacOSX/build/Eyelink.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==5
    % Build IOPort:
    mex -outdir ../Projects/MacOSX/build -output IOPort -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_IOPort -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio"  -ICommon/Base -IOSX/Base -ICommon/IOPort  "OSX/Base/*.c" "Common/Base/*.c" "Common/IOPort/*.c" -lLexActivator
    macossetmatlabrpath('IOPort');
    unix(['mv ../Projects/MacOSX/build/IOPort.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==6
    % Build moglcore:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
    try
        mex -outdir ./ -output moglcore -DMACOSX -DGLEW_STATIC -largeArrayDims -DMEX_DOUBLE_HANDLE CFLAGS="\$CFLAGS -Wno-deprecated-declarations -Wno-tautological-pointer-compare -Wno-incompatible-pointer-types -Wno-incompatible-pointer-types-discards-qualifiers -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework OpenGL -framework GLUT" -I./ moglcore.c gl_auto.c gl_manual.c glew.c mogl_rebinder.c ftglesGlue.c
    catch %#ok<*CTCH>
        ple;
    end
    unix(['mv moglcore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    cd(curdir);
end

if mode==7
    % Depends: libfreenect, libusb-1.0
    % Build PsychKinectCore:
    mex -outdir ../Projects/MacOSX/build -output PsychKinectCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychKinectCore -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -I/usr/local/include/libusb-1.0 -I/usr/local/include/libfreenect  -I/opt/homebrew/include/libusb-1.0 -I/opt/homebrew/include/libfreenect -ICommon/Base -IOSX/Base -ICommon/PsychKinect  "OSX/Base/*.c" "Common/Base/*.c" "Common/PsychKinect/*.c" -L/usr/local/lib  -L/opt/homebrew/lib -lfreenect -lusb-1.0 -lLexActivator
    macossetmatlabrpath('PsychKinectCore');
    unix(['mv ../Projects/MacOSX/build/PsychKinectCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==8
    % Build PsychHID:
    mex -outdir ../Projects/MacOSX/build -output PsychHID -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychHID -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework ApplicationServices -framework CoreServices -framework CoreFoundation -framework Carbon -framework CoreAudio -framework IOKit -weak_library /usr/local/lib/libusb-1.0.dylib" -ICommon/Base -IOSX/Base -ICommon/PsychHID -IOSX/PsychHID -I../Cohorts/HID_Utilities_64Bit/ -I../Cohorts/HID_Utilities_64Bit/IOHIDManager -I/usr/local/include/libusb-1.0  -I/opt/homebrew/include/libusb-1.0 "Common/PsychHID/*.c" "OSX/PsychHID/*.c" "OSX/Base/*.c" "Common/Base/*.c" ../Cohorts/HID_Utilities_64Bit/build/Release/libHID_Utilities64.a -lLexActivator
    macossetmatlabrpath('PsychHID');
    movefile(['../Projects/MacOSX/build/PsychHID.' mexext], [PsychtoolboxRoot 'PsychBasic/']);
end

if mode==9
    % Build moalcore:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychSound/MOAL/source/')
    try
        mex -outdir ./ -output moalcore -largeArrayDims -DMEX_DOUBLE_HANDLE -DMACOSX CFLAGS="\$CFLAGS -Wno-tautological-pointer-compare -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework OpenAL -framework ApplicationServices -framework Carbon" moalcore.c al_auto.c al_manual.c alm.c
    catch
        ple;
    end
    unix(['mv ./moalcore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);

    cd(curdir);
end

if mode==10
    % Build FontInfo:
    mex -outdir ../Projects/MacOSX/build -output FontInfo -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_FontInfo -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework ApplicationServices -framework CoreAudio" -ICommon/Base -ICommon/Fonts -IOSX/Base -IOSX/Fonts  "Common/Base/*.c" "OSX/Base/*.c" "OSX/Fonts/*.c" -lLexActivator
    macossetmatlabrpath('FontInfo');
    unix(['mv ../Projects/MacOSX/build/FontInfo.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==11
    % Build MachAbsoluteTimeClockFrequency, MachGetPriorityMex,
    % MachSetPriorityMex, which are needed by the Priority() command on
    % OSX:

    % MachAbsoluteTimeClockFrequency
    mex -outdir ../Projects/MacOSX/build -output MachAbsoluteTimeClockFrequency -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_MachAbsoluteTimeClockFrequency -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/Base -IOSX/Base -ICommon/MachAbsoluteTimeClockFrequency  "OSX/Base/*.c" "Common/Base/*.c" "Common/MachAbsoluteTimeClockFrequency/*.c" -lLexActivator
    macossetmatlabrpath('MachAbsoluteTimeClockFrequency');
    unix(['mv ../Projects/MacOSX/build/MachAbsoluteTimeClockFrequency.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);

    % MachGetPriorityMex
    mex -outdir ../Projects/MacOSX/build -output MachGetPriorityMex -largeArrayDims -DMEX_DOUBLE_HANDLE -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/MachPriorityMex "Common/MachPriorityMex/MachPriorityCommonMex.c" "Common/MachPriorityMex/MachGetPriorityMex.c" -lLexActivator
    macossetmatlabrpath('MachGetPriorityMex');
    unix(['mv ../Projects/MacOSX/build/MachGetPriorityMex.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);

    % MachSetPriorityMex
    mex -outdir ../Projects/MacOSX/build -output MachSetPriorityMex -largeArrayDims -DMEX_DOUBLE_HANDLE -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/MachPriorityMex "Common/MachPriorityMex/MachPriorityCommonMex.c" "Common/MachPriorityMex/MachSetPriorityMex.c" -lLexActivator
    macossetmatlabrpath('MachSetPriorityMex');
    unix(['mv ../Projects/MacOSX/build/MachSetPriorityMex.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==12
    % Build Gestalt:
    mex -outdir ../Projects/MacOSX/build -output Gestalt -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_Gestalt -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework CoreServices -framework CoreFoundation -framework CoreAudio" -ICommon/Base -IOSX/Base -IOSX/Gestalt -IOSX/OS9ToolboxFragments  "OSX/Base/*.c" "Common/Base/*.c" "OSX/Gestalt/*.c" -lLexActivator
    macossetmatlabrpath('Gestalt');
    unix(['mv ../Projects/MacOSX/build/Gestalt.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==13
    % Build pnet:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychHardware/iViewXToolbox/tcp_udp_ip/')
    try
        mex -output pnet pnet.c -largeArrayDims -DMEX_DOUBLE_HANDLE CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13"
        unix(['mv ./pnet.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    catch
    end
    cd(curdir);
end

if mode==14
    % Used to be PsychOculusVRCore, but this is unsupportable on any macOS
    % since 10.15 Catalina, as it required 32-Bit Intel architecture
    % support for the VR runtime, which is dead since 10.15+.
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
    %
    %
    % New style statically links against a MoltenVK static build installed
    % along the Psychtoolbox-3 folder, ie. in the same parent folder, as
    % provided from https://github.com/KhronosGroup/MoltenVK releases page.
    try
        % Old style: Build against Headers in GStreamer 1.24+ development framework, link statically against libMoltenVK.a from GStreamer:
        % mex -outdir ../Projects/MacOSX/build/ -output PsychVulkanCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychVulkanCore -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -mmacosx-version-min=10.13" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.13 -framework AppKit -framework IOKit -framework IOSurface -framework Metal -framework Foundation -framework QuartzCore -framework CoreGraphics -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework OpenGL" -I/Library/Frameworks/GStreamer.framework/Headers -ICommon/Base -IOSX/Base -ICommon/PsychVulkanCore -I/usr/local/include "Common/PsychVulkanCore/*.c" "OSX/Base/*.c" "Common/Base/*.c" /Library/Frameworks/GStreamer.framework/Libraries/libMoltenVK.a -lLexActivator
        % Current style: Link statically against current MoltenVK release installed side by side to Psychtoolbox-3 folder, needs CFLAGS -std=c2x -Wno-constant-logical-operand to compile on macOS 13 with clang 15.0 compiler to activate C23 standard handling:
        mex -outdir ../Projects/MacOSX/build/ -output PsychVulkanCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTBMODULE_PsychVulkanCore -L../../Psychtoolbox/PsychBasic/PsychPlugins/ CFLAGS="\$CFLAGS -std=c2x -Wno-constant-logical-operand -Wno-deprecated-declarations -mmacosx-version-min=11.0" LDFLAGS="\$LDFLAGS -mmacosx-version-min=11.0 -framework AppKit -framework IOKit -framework IOSurface -framework Metal -framework Foundation -framework QuartzCore -framework CoreGraphics -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework OpenGL" -I../../../MoltenVK/MoltenVK/include -ICommon/Base -IOSX/Base -ICommon/PsychVulkanCore "Common/PsychVulkanCore/*.c" "OSX/Base/*.c" "Common/Base/*.c" ../../../MoltenVK/MoltenVK/static/MoltenVK.xcframework/macos-arm64_x86_64/libMoltenVK.a -lLexActivator
    catch
        disp(psychlasterror);
    end
    macossetmatlabrpath('PsychVulkanCore');
    movefile(['../Projects/MacOSX/build/PsychVulkanCore.' mexext], [PsychtoolboxRoot 'PsychBasic/']);
end

return;
