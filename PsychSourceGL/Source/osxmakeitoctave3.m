function osxmakeitoctave3(mode)
% This is the macOS Octave version of makeit: It is meant for building PTB for
% 64-Bit Octave-10+ on macOS 13.7.5 Ventura or later, using the 14.x or later SDK.
%
% The rpath Octave version independent encoding is done by the helper
% function osxsetoctaverpath().
dst = [PsychtoolboxRoot 'PsychBasic/Octave10OSXFiles64/'];

if IsARM
    dst = [PsychtoolboxRoot 'PsychBasic/Octave10OSXFilesARM64/'];
end

if ~IsOSX(1) || ~IsOctave
    error('osxmakeitoctave3 only works with a 64-Bit version of Octave 10 or later for macOS!');
end

if nargin < 1 || isempty(mode)
    mode = 0;
end

% Rebuild all request?
if mode == -1
    % Yes: Call ourselves recursively on all plugins/modes to rebuild
    % everything:
    tic;
    for mode = 0:15
        osxmakeitoctave3(mode);
    end
    elapsedsecs = toc;
    fprintf('Total rebuild time for all mex files was %f seconds. Bye.\n\n', elapsedsecs);
    return;
end

% Make our helper scripts available even if we change working directory:
addpath(pwd);

fprintf('Building plugin type %i ...\n\n', mode);

if mode==0
    % Build Screen:
    % Depends: GStreamer-1.18+, libdc1394-2

    % Need to build the PsychCocoaGlue.c separately as Objective-C code,
    % whereas the mex call below only compiles C-Code and links it with the
    % PsychCocoaGlue.o object code created here. This to avoid linker
    % trouble with unknown/illegal Objective-C language command line switches:
    mex -c -DPTBMODULE_Screen -DPTBVIDEOCAPTURE_LIBDC -DPTBOCTAVE3MEX ...
        -I/usr/include -ICommon/Base -ICommon/Screen -IOSX/Base -IOSX/Screen -IOSX/Fonts -IOSX/EthernetAddress ...
        "-x objective-c -mmacosx-version-min='10.13'" "OSX/Screen/PsychCocoaGlue.c"

    movefile('./OSX/Screen/PsychCocoaGlue.c', './OSX/Screen/PsychCocoaGlue.m');

    % Build with weak linking of GStreamer and libdc1394 via -weak_library
    % flag. This means that a missing GStreamer or libdc1394 installation
    % will not cause linker failure at Screen() load time -- Screen
    % continues to work as long as no GStreamer-dependent functions are
    % used. In that case it will abort with a helpful error message:
    % GStreamer 1.4.x+:
    mex --output ../Projects/MacOSX/build/Screen -DPTBMODULE_Screen -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC -DGLEW_STATIC -DPTBOCTAVE3MEX ...
    "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" ...
    "-Wl,-headerpad_max_install_names,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstreamer-1.0.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstbase-1.0.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstapp-1.0.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstpbutils-1.0.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgstvideo-1.0.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgobject-2.0.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgmodule-2.0.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libxml2.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libgthread-2.0.dylib,\
    -weak_library,/Library/Frameworks/GStreamer.framework/Versions/Current/lib/libglib-2.0.dylib,\
    -weak_library,/usr/local/lib/libdc1394.dylib,\
    -rpath,/Library/Frameworks/GStreamer.framework/Versions/Current/lib,\
    -framework,CoreServices,-framework,CoreFoundation,-framework,ApplicationServices,-framework,CoreAudio,-framework,OpenGL,-framework,CoreVideo,\
    -framework,IOKit,-framework,SystemConfiguration,-framework,Carbon,-framework,CoreText,-framework,QuartzCore,\
    -framework,CoreDisplay,-F/System/Library/PrivateFrameworks,-framework,DisplayServices,\
    -U,CoreDisplay_Display_SetUserBrightness,-U,CoreDisplay_Display_GetUserBrightness,\
    -U,DisplayServicesGetBrightness,-U,DisplayServicesSetBrightness,\
    -U,DisplayServicesCanChangeBrightness,-U,DisplayServicesBrightnessChanged,\
    -framework,Cocoa" ...
    -I/usr/include -I/usr/local/include -I/opt/homebrew/include -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/gstreamer-1.0 ...
    -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/libxml2 -I/Library/Frameworks/GStreamer.framework/Versions/Current/include/glib-2.0 ...
    -I/Library/Frameworks/GStreamer.framework/Versions/Current/lib/glib-2.0/include ...
    -I/Library/Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0/include/ ...
    -ICommon/Base -ICommon/Screen -IOSX/Base -IOSX/Screen -IOSX/Fonts -IOSX/EthernetAddress ...
    "OSX/Screen/*.c" "Common/Screen/*.c" "OSX/Base/*.c" "OSX/Fonts/*FontGlue*.c" "OSX/Fonts/FontInfo.c" "OSX/EthernetAddress/*.c" "Common/Base/*.c" ...
    "Common/Screen/tinyexr.cc" ...
    -L/usr/lib -L/usr/local/lib ./PsychCocoaGlue.o

    movefile('./OSX/Screen/PsychCocoaGlue.m', './OSX/Screen/PsychCocoaGlue.c');

    osxsetoctaverpath('Screen');
    unix(['mv ../Projects/MacOSX/build/Screen.' mexext ' ' dst]);
end

if mode==1
    % Build GetSecs:
    mex --output ../Projects/MacOSX/build/GetSecs -DPTBMODULE_GetSecs -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio" -I/usr/include -ICommon/Base -IOSX/Base -ICommon/GetSecs "OSX/Base/*.c" "Common/Base/*.c" "Common/GetSecs/*.c"
    osxsetoctaverpath('GetSecs');
    unix(['mv ../Projects/MacOSX/build/GetSecs.' mexext ' ' dst]);
end

if mode==2
    % Build WaitSecs:
    mex --output ../Projects/MacOSX/build/WaitSecs -DPTBMODULE_WaitSecs -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio" -I/usr/include -ICommon/Base -IOSX/Base -ICommon/WaitSecs  "OSX/Base/*.c" "Common/Base/*.c" "Common/WaitSecs/*.c"
    osxsetoctaverpath('WaitSecs');
    unix(['mv ../Projects/MacOSX/build/WaitSecs.' mexext ' ' dst]);
end

if mode==3
    % Depends: PortAudio
    % Build PsychPortAudio:
    mex --output ../Projects/MacOSX/build/PsychPortAudio -DPTBMODULE_PsychPortAudio -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio,-framework,AudioToolbox,-framework,AudioUnit" -I/usr/include -ICommon/Base -IOSX/Base -ICommon/PsychPortAudio  "OSX/Base/*.c" "Common/Base/*.c" "Common/PsychPortAudio/*.c" ../Cohorts/PortAudio/libportaudio_osx_64.a
    osxsetoctaverpath('PsychPortAudio');
    unix(['mv ../Projects/MacOSX/build/PsychPortAudio.' mexext ' ' dst]);
end

if mode==4
    % Depends: eyelink-SDK: eyelink_core.framework installed under default location /Library/Frameworks/
    % Build Eyelink:
    mex --output ../Projects/MacOSX/build/Eyelink -DPTBMODULE_Eyelink -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio,-F/Library/Frameworks/,-framework,eyelink_core,-rpath,/Library/Frameworks/" -I/Library/Frameworks/eyelink_core.framework/Headers -ICommon/Base -IOSX/Base -ICommon/Eyelink  "OSX/Base/*.c" "Common/Base/*.c" "Common/Eyelink/*.c"
    osxsetoctaverpath('Eyelink');
    unix(['mv ../Projects/MacOSX/build/Eyelink.' mexext ' ' dst]);
end

if mode==5
    % Build IOPort:
    mex --output ../Projects/MacOSX/build/IOPort -DPTBMODULE_IOPort -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio" -I/usr/include  -ICommon/Base -IOSX/Base -ICommon/IOPort  "OSX/Base/*.c" "Common/Base/*.c" "Common/IOPort/*.c"
    osxsetoctaverpath('IOPort');
    unix(['mv ../Projects/MacOSX/build/IOPort.' mexext ' ' dst]);
end

if mode==6
    % Build moglcore:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
    try
        mex --output ./moglcore -DMACOSX -DGLEW_STATIC -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -Wno-tautological-pointer-compare -Wno-incompatible-pointer-types -Wno-incompatible-pointer-types-discards-qualifiers -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,OpenGL,-framework,GLUT" -I/usr/include moglcore.c gl_auto.c gl_manual.c glew.c mogl_rebinder.c ftglesGlue.c
    catch %#ok<*CTCH>
    end
    osxsetoctaverpath('moglcore', './');
    unix(['mv moglcore.' mexext ' ' dst]);
    cd(curdir);
end

if mode==7
    % Depends: libfreenect, libusb-1.0
    % Build PsychKinectCore:
    mex --output ../Projects/MacOSX/build/PsychKinectCore -DPTBMODULE_PsychKinectCore -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio" -I/usr/include -I/usr/local/include/libusb-1.0 -I/usr/local/include/libfreenect -I/opt/homebrew/include/libusb-1.0 -I/opt/homebrew/include/libfreenect -ICommon/Base -IOSX/Base -ICommon/PsychKinect  "OSX/Base/*.c" "Common/Base/*.c" "Common/PsychKinect/*.c" -L/usr/local/lib -L/opt/homebrew/lib -lfreenect -lusb-1.0
    osxsetoctaverpath('PsychKinectCore');
    unix(['mv ../Projects/MacOSX/build/PsychKinectCore.' mexext ' ' dst]);
end

if mode==8
    % Build PsychHID:
    mex --output ../Projects/MacOSX/build/PsychHID -DPTBMODULE_PsychHID -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,ApplicationServices,-framework,CoreServices,-framework,CoreFoundation,-framework,Carbon,-framework,CoreAudio,-framework,IOKit,-weak_library,/usr/local/lib/libusb-1.0.dylib" -I/usr/local/include/libusb-1.0 -I/opt/homebrew/include/libusb-1.0 -ICommon/Base -IOSX/Base -ICommon/PsychHID -IOSX/PsychHID -I../Cohorts/HID_Utilities_64Bit/ -I../Cohorts/HID_Utilities_64Bit/IOHIDManager  "Common/PsychHID/*.c" "OSX/PsychHID/*.c" "OSX/Base/*.c" "Common/Base/*.c" ../Cohorts/HID_Utilities_64Bit/build/Release/libHID_Utilities64.a
    osxsetoctaverpath('PsychHID');
    unix(['mv ../Projects/MacOSX/build/PsychHID.' mexext ' ' dst]);
end

if mode==9
    % Build moalcore:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychSound/MOAL/source/')
    try
        mex --output ./moalcore  -DMACOSX -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -Wno-tautological-pointer-compare -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,OpenAL,-framework,ApplicationServices,-framework,Carbon" -I/usr/include moalcore.c al_auto.c al_manual.c alm.c
    catch
    end
    osxsetoctaverpath('moalcore', './');
    unix(['mv ./moalcore.' mexext ' ' dst]);

    cd(curdir);
end

if mode==10
    % Build FontInfo:
    mex --output ../Projects/MacOSX/build/FontInfo -DPTBMODULE_FontInfo -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,ApplicationServices,-framework,CoreAudio" -I/usr/include -ICommon/Base -ICommon/Fonts -IOSX/Base -IOSX/Fonts  "Common/Base/*.c" "OSX/Base/*.c" "OSX/Fonts/*.c"
    osxsetoctaverpath('FontInfo');
    unix(['mv ../Projects/MacOSX/build/FontInfo.' mexext ' ' dst]);
end

if mode==11
    % Build MachAbsoluteTimeClockFrequency, MachGetPriorityMex,
    % MachSetPriorityMex, which are needed by the Priority() command on
    % OSX:

    % MachAbsoluteTimeClockFrequency
    mex --output ../Projects/MacOSX/build/MachAbsoluteTimeClockFrequency -DPTBMODULE_MachAbsoluteTimeClockFrequency -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio" -I/usr/include -ICommon/Base -IOSX/Base -ICommon/MachAbsoluteTimeClockFrequency  "OSX/Base/*.c" "Common/Base/*.c" "Common/MachAbsoluteTimeClockFrequency/*.c"
    osxsetoctaverpath('MachAbsoluteTimeClockFrequency');
    unix(['mv ../Projects/MacOSX/build/MachAbsoluteTimeClockFrequency.' mexext ' ' dst]);

    % MachGetPriorityMex
    mex --output ../Projects/MacOSX/build/MachGetPriorityMex -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio" -I/usr/include -ICommon/MachPriorityMex "Common/MachPriorityMex/MachPriorityCommonMex.c" "Common/MachPriorityMex/MachGetPriorityMex.c"
    osxsetoctaverpath('MachGetPriorityMex');
    unix(['mv ../Projects/MacOSX/build/MachGetPriorityMex.' mexext ' ' dst]);

    % MachSetPriorityMex
    mex --output ../Projects/MacOSX/build/MachSetPriorityMex -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio" -I/usr/include -ICommon/MachPriorityMex "Common/MachPriorityMex/MachPriorityCommonMex.c" "Common/MachPriorityMex/MachSetPriorityMex.c"
    osxsetoctaverpath('MachSetPriorityMex');
    unix(['mv ../Projects/MacOSX/build/MachSetPriorityMex.' mexext ' ' dst]);
end

if mode==12
    % Build Gestalt:
    mex --output ../Projects/MacOSX/build/Gestalt -DPTBMODULE_Gestalt -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio" -I/usr/include -ICommon/Base -IOSX/Base -IOSX/Gestalt -IOSX/OS9ToolboxFragments  "OSX/Base/*.c" "Common/Base/*.c" "OSX/Gestalt/*.c"
    osxsetoctaverpath('Gestalt');
    unix(['mv ../Projects/MacOSX/build/Gestalt.' mexext ' ' dst]);
end

if mode==13
    % Build pnet:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychHardware/iViewXToolbox/tcp_udp_ip/')
    try
        mex --output ./pnet pnet.c "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names"
        osxsetoctaverpath('pnet', './');
        unix(['mv ./pnet.' mexext ' ' dst]);
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
    try
        %mex --output ../Projects/MacOSX/build/PsychVulkanCore -DPTBMODULE_PsychVulkanCore -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-F/Library/Frameworks/,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio,-framework,OpenGL" -ICommon/Base -IOSX/Base -ICommon/PsychVulkanCore -I/usr/local/include Common/PsychVulkanCore/*.c OSX/Base/*.c Common/Base/*.c -lvulkan -lMoltenVK
        % Build against Headers in GStreamer 1.24+ development framework, link statically against libMoltenVK.a from GStreamer:
        mex --output ../Projects/MacOSX/build/PsychVulkanCore -DPTBMODULE_PsychVulkanCore -DPTBOCTAVE3MEX "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-F/Library/Frameworks/,-framework,AppKit,-framework,IOKit,-framework,IOSurface,-framework,Metal,-framework,Foundation,-framework,QuartzCore,-framework,CoreGraphics,-framework,CoreServices,-framework,CoreFoundation,-framework,CoreAudio,-framework,OpenGL" -ICommon/Base -IOSX/Base -ICommon/PsychVulkanCore -I/usr/local/include -I/Library/Frameworks/GStreamer.framework/Headers Common/PsychVulkanCore/*.c OSX/Base/*.c Common/Base/*.c /Library/Frameworks/GStreamer.framework/Libraries/libMoltenVK.a
    catch
        disp(psychlasterror);
    end

    osxsetoctaverpath('PsychVulkanCore');
    unix(['mv ../Projects/MacOSX/build/PsychVulkanCore.mex ' dst]);
end

return;
end

% Our own override implementation of mex(), shadowing
% octave's mex.m . This one uses glob() to glob-expand all
% *.c shell patterns to corresponding lists of source
% filenames, so Octave 10's mkoctfile can "handle" such
% wildcards. Older mkoctfile implementations did this, but
% since Octave 3.8.1 mkoctfile is reimplemented from scratch
% as a C++ piece of art, which can't expand wildcards anymore.
function mex(varargin)
  inargs = {varargin{:}};
  outargs = {"--mex"};
  % Stripping of mex files does not work on Octave for OSX: outargs = {outargs{:}, "-s"};

  for i = 1:length(inargs)
    filtered = inargs{i};
    if ~isempty(strfind(filtered, '-Wl,'))
        % Remove any internal whitespace in -Wl, linker option strings:
        filtered = filtered(~isspace(filtered));
    end

    if ~isempty(strfind(filtered, '*'))
      outargs = {outargs{:}, glob(filtered)};
    else
      outargs = {outargs{:}, filtered};
    end
  end

  % Link all mex files against the libLexActivator.dylib in PsychPlugins:
  outargs = {outargs{:}, sprintf('-L%sPsychBasic/PsychPlugins', PsychtoolboxRoot) };
  outargs = {outargs{:}, '-lLexActivator'};

  args = cellstr(char(outargs));
  mkoctfile (args{:});
end
