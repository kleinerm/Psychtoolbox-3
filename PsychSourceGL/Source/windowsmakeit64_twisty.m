function windowsmakeit64_twisty(what, onoctave)
% Builds the 64-Bit Psychtoolbox on MS-Windows for Octave-7 and Matlab.
% This script is customized for MK's build machines "darlene" and "touchy",
% building against the Windows-10 SDK on Windows-10 64-Bit.
%

if ~IsWin(1)
    error('%s must be run on MS-Windows within 64 Bit Octave 7 or within 64-Bit Matlab!', mfilename);
end

if nargin < 1
    what = 0;
end

if nargin < 2
    onoctave = IsOctave;
end

% Rebuild all request?
if what == -1
    % Yes: Call ourselves recursively on all plugins/modes to rebuild
    % everything:
    tic;
    for what = 0:16
        windowsmakeit64_twisty(what);
    end
    elapsedsecs = toc;
    fprintf('Total rebuild time for all mex files was %f seconds. Bye.\n\n', elapsedsecs);
    return;
end

try

fprintf('Building plugin type %i ...\n', what);

% Matlab or Octave build?
if onoctave == 0
    % Matlab build:
    if what == 0
        % Default: Build Screen with GStreamer-1 support: Needs the
        % www.gstreamer.freedesktop.org GStreamer-1.22.0+ SDK in the MSVC
        % variant for 64-Bit Windows, or a later version of GStreamer. Use
        % this for verbose linker output: /VERBOSE:LIB
        clear Screen
        % Does not work with GStreamer 1.18.0 MSVC SDK yet due to glib-2.0.0 incompatibility with delay loading: mex -outdir ..\Projects\Windows\build -output Screen -DPTBMODULE_Screen -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTB_USE_GSTREAMER -DGLEW_STATIC -LC:\gstreamer\1.0\msvc_x86_64\lib -IC:\gstreamer\1.0\msvc_x86_64\include -IC:\gstreamer\1.0\msvc_x86_64\include\gstreamer-1.0 -IC:\gstreamer\1.0\msvc_x86_64\include\glib-2.0 -IC:\gstreamer\1.0\msvc_x86_64\include\glib-2.0\include -IC:\gstreamer\1.0\msvc_x86_64\lib\glib-2.0\include -IC:\gstreamer\1.0\msvc_x86_64\lib\gstreamer-1.0\include -IC:\gstreamer\1.0\msvc_x86_64\include\libxml2 -ICommon\Base -ICommon\Screen -IWindows\Base -IWindows\Screen Windows\Screen\*.c Windows\Base\*.c Common\Base\*.c Common\Screen\*.c Common\Screen\tinyexr.cc kernel32.lib user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib winmm.lib Dwmapi.lib PowrProf.lib delayimp.lib -lgobject-2.0 -lglib-2.0 -lgstreamer-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 LINKFLAGS="$LINKFLAGS /DELAYLOAD:gobject-2.0-0.dll /DELAYLOAD:glib-2.0-0.dll /DELAYLOAD:gstreamer-1.0-0.dll /DELAYLOAD:gstapp-1.0-0.dll /DELAYLOAD:gstvideo-1.0-0.dll /DELAYLOAD:gstpbutils-1.0-0.dll"
        mex -outdir ..\Projects\Windows\build -output Screen -DPTBMODULE_Screen -largeArrayDims -DMEX_DOUBLE_HANDLE -DPTB_USE_GSTREAMER -DGLEW_STATIC -LC:\gstreamer\1.0\msvc_x86_64\lib -IC:\gstreamer\1.0\msvc_x86_64\include -IC:\gstreamer\1.0\msvc_x86_64\include\gstreamer-1.0 -IC:\gstreamer\1.0\msvc_x86_64\include\glib-2.0 -IC:\gstreamer\1.0\msvc_x86_64\include\glib-2.0\include -IC:\gstreamer\1.0\msvc_x86_64\lib\glib-2.0\include -IC:\gstreamer\1.0\msvc_x86_64\lib\gstreamer-1.0\include -IC:\gstreamer\1.0\msvc_x86_64\include\libxml2 -ICommon\Base -ICommon\Screen -IWindows\Base -IWindows\Screen Windows\Screen\*.c Windows\Base\*.c Common\Base\*.c Common\Screen\*.c Common\Screen\tinyexr.cc kernel32.lib user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib winmm.lib Dwmapi.lib PowrProf.lib delayimp.lib -lgobject-2.0 -lglib-2.0 -lgstreamer-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 LINKFLAGS="$LINKFLAGS /DELAYLOAD:gobject-2.0-0.dll /DELAYLOAD:gstreamer-1.0-0.dll /DELAYLOAD:gstapp-1.0-0.dll /DELAYLOAD:gstvideo-1.0-0.dll /DELAYLOAD:gstpbutils-1.0-0.dll"
        movefile(['..\Projects\Windows\build\Screen.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 100
        % Optional: Build Screen without GStreamer support:
        clear Screen
        mex -outdir ..\Projects\Windows\build -output ScreenLight -DPTBMODULE_Screen -DGLEW_STATIC -ICommon\Base -ICommon\Screen -IWindows\Base -IWindows\Screen Windows\Screen\*.c Windows\Base\*.c Common\Base\*.c Common\Screen\*.c Common\Screen\tinyexr.cc kernel32.lib user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib winmm.lib Dwmapi.lib PowrProf.lib
        movefile(['..\Projects\Windows\build\ScreenLight.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 1
        % Build WaitSecs
        clear WaitSecs
        mex -outdir ..\Projects\Windows\build -output WaitSecs -DPTBMODULE_WaitSecs -largeArrayDims -DMEX_DOUBLE_HANDLE -ICommon\Base -ICommon\WaitSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\WaitSecs\*.c kernel32.lib user32.lib winmm.lib
        movefile(['..\Projects\Windows\build\WaitSecs.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 2
        % Build PsychPortAudio
        % If PsychPortAudio should support the proprietary ASIO sound backend, then add a: -DPTB_USE_ASIO
        clear PsychPortAudio
        mex -outdir ..\Projects\Windows\build -output PsychPortAudio -DPTBMODULE_PsychPortAudio -largeArrayDims -DMEX_DOUBLE_HANDLE -L..\Cohorts\PortAudio -ICommon\Base -ICommon\PsychPortAudio -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\PsychPortAudio\*.c kernel32.lib user32.lib advapi32.lib winmm.lib delayimp.lib -lportaudio_x64 LINKFLAGS="$LINKFLAGS /DELAYLOAD:portaudio_x64.dll"
        movefile(['..\Projects\Windows\build\PsychPortAudio.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 3
        % Build GetSecs
        clear GetSecs
        mex -outdir ..\Projects\Windows\build -output GetSecs -DPTBMODULE_GetSecs -largeArrayDims -DMEX_DOUBLE_HANDLE -ICommon\Base -ICommon\GetSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\GetSecs\*.c kernel32.lib user32.lib winmm.lib
        movefile(['..\Projects\Windows\build\GetSecs.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 4
        % Build IOPort
        clear IOPort
        mex -outdir ..\Projects\Windows\build -output IOPort -DPTBMODULE_IOPort -largeArrayDims -DMEX_DOUBLE_HANDLE -ICommon\Base -ICommon\IOPort -IWindows\Base -IWindows\IOPort Windows\Base\*.c Common\Base\*.c Common\IOPort\*.c Windows\IOPort\*.c kernel32.lib user32.lib winmm.lib
        movefile(['..\Projects\Windows\build\IOPort.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 5 && exist('..\..\..\SRAnipalCSDK', 'dir')
        % Build SRAnipalMex on MS-Windows for Matlab 64 Bit:
        %
        % This requires the proprietary HTC SRAnipal SDK installed side by side
        % to the Psychtoolbox-3 folder in a folder called SRAnipalCSDK.
        %
        % The SDK can be found at the time of this writing under the following URL
        % (free sign up for a developer account with HTC and agreeing to a license
        % required):
        %
        % https://developer-express.vive.com/resources/vive-sense/eye-and-facial-tracking-sdk/
        %
        % Specifically, e.g., for the v1.3.6.8 SDK, you have to unzip the SDK zip file,
        % then move the "01_C" subfolder from "...\SRAnipalSDK-v1.3.6.8\SDK-v1.3.6.8\SDK\01_C"
        % into a parent folder next to the Psychtoolbox-3 folder and rename it from "01_C" to
        % "SRAnipalCSDK". This is a one type preparation step on a new build system.
        %
        clear SRAnipalMex;
        mex -outdir ..\Projects\Windows\build -output SRAnipalMex -I..\..\..\SRAnipalCSDK\include -L..\..\..\SRAnipalCSDK\lib ..\Cohorts\SRAnipalMex.cpp -lSRanipal
        movefile(['..\Projects\Windows\build\SRAnipalMex.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 6
        % Build moglcore on MS-Windows for Matlab 64 Bit:
        %
        % Requires freeglut import libraries and header files which are bundled
        % inside the PsychSourceGL\Cohorts\freeglut folder of the PTB source distro.
        %
        % Requires freeglut dll's included within Psychtoolbox distribution.
        %
        curdir = pwd;
        cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/');
        clear moglcore;

        try
            % 64-Bit build:
            mex -outdir . -output moglcore -largeArrayDims -DMEX_DOUBLE_HANDLE -DWINR2007a -DWINDOWS -DGLEW_STATIC -I..\..\..\..\PsychSourceGL\Cohorts\freeglut\include -L..\..\..\..\PsychSourceGL\Cohorts\freeglut\lib\x64 -I. windowhacks.c gl_auto.c gl_manual.c mogl_rebinder.c moglcore.c glew.c ftglesGlue.c user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib -lfreeglut

            movefile(['moglcore.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
        catch
            lasterr %#ok<LERR>
        end
        cd(curdir);
    end

    if what == 7
        % Build Eyelink:
        clear Eyelink
        mex -outdir ..\Projects\Windows\build -output Eyelink -DPTBMODULE_Eyelink -largeArrayDims -DMEX_DOUBLE_HANDLE -I"C:\Program Files (x86)\SR Research\EyeLink\Includes\eyelink" -ICommon\Base -ICommon\Eyelink -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\Eyelink\*.c user32.lib gdi32.lib advapi32.lib winmm.lib "C:\Program Files (x86)\SR Research\EyeLink\libs\x64\eyelink_core64.lib" "C:\Program Files (x86)\SR Research\EyeLink\libs\x64\eyelink_w32_comp64.lib"
        movefile(['..\Projects\Windows\build\Eyelink.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 8
        % Build PsychKinectCore:
        clear PsychKinectCore
        mex -outdir ..\Projects\Windows\build -output PsychKinectCore -DPTBMODULE_PsychKinectCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DWIN32 -L..\Cohorts\Kinect-v16-withsource\libusb\lib\msvc_x64 -I..\Cohorts\Kinect-v16-withsource -ICommon\Base -IWindows\Base -ICommon\PsychKinect Windows\Base\*.c Common\Base\*.c Common\PsychKinect\*.c ..\Cohorts\Kinect-v16-withsource\*.cpp kernel32.lib user32.lib winmm.lib -lusb
        movefile(['..\Projects\Windows\build\PsychKinectCore.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 9
        % Build PsychHID:
        clear PsychHID % make sure not in use
        mex -outdir ..\Projects\Windows\build -output PsychHID -DPTBMODULE_PsychHID -largeArrayDims -DMEX_DOUBLE_HANDLE -DWIN32 -L..\Cohorts\libusb1-win32\MS64\dll -I..\Cohorts\libusb1-win32\include\libusb-1.0 -ICommon\Base -IWindows\Base -ICommon\PsychHID Windows\PsychHID\*.c Windows\Base\*.c Common\Base\*.c Common\PsychHID\*.c -ldinput8 kernel32.lib user32.lib winmm.lib -lusb-1.0 setupapi.lib
        movefile(['..\Projects\Windows\build\PsychHID.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 10
        % Makefile for building the moalcore MEX file for Matlab+OpenAL under
        % MS-Windows. Call it while inside the .../MOAL/source folder. You'll
        % have to install the Creative labs OpenAL SDK into ...
        % C:\Program Files (x86)\OpenAL 1.1 SDK\
        % ... which is the default install location on Windows-7.
        %
        % The SDK is currently available from:
        % http://connect.creativelabs.com/openal
        %
        curdir = pwd;
        cd('../../Psychtoolbox/PsychSound/MOAL/source/');

        try
            clear moalcore;
            % 64-Bit R2007a or later build:
            mex -outdir . -output moalcore -largeArrayDims -DWINDOWS -I"C:\Program Files (x86)\OpenAL 1.1 SDK\include" -L"C:\Program Files (x86)\OpenAL 1.1 SDK\libs\Win64" moalcore.c al_auto.c al_manual.c alm.c user32.lib -lOpenAL32

            movefile(['moalcore.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
        catch
            lasterr %#ok<LERR>
        end

        cd(curdir);
    end

    if what == 11
        % Build pnet
        curdir = pwd;
        cd('../../Psychtoolbox/PsychHardware/iViewXToolbox/tcp_udp_ip/');
        clear pnet
        try
            mex -output pnet -DWIN32 pnet.c ws2_32.lib winmm.lib
            movefile(['pnet.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
        catch
        end

        cd(curdir);
    end

    if what == 12
        % Build PsychOculusVRCore:
        clear PsychOculusVRCore
        % Needs the Oculus VR SDK v 0.5.01 installed in side-by-side to
        % the Psychtoolbox-3 folder, ie., in the same parent directory, and
        % renamed from OculusSDK to OculusSDKWin.
        %
        % For building with R2019a and MSVC 2019 we had to hack the OVR_CAPI.h
        % file to #define OVR_ALIGN(n) as nothing. Hopefully this still
        % works, as i don't have a Rift DK2 around for testing atm., and my
        % Windows 10 box refuses installation of the full runtime drive
        % anyway.
        %
        % For the Matlab build, we compile the OVR_CAPIshim.c shim file and
        % other helper C files from the SDK directly into our mex file,
        % instead of statically linking against LibOVR.lib, as that .lib
        % import file only works with up to MSVC 2013, but not with our
        % MSVC 2019 build system for R2019a. The shim will locate and
        % runtime-link against the libOVRRT_64_0_5.dll of the installed
        % Oculus VR runtime during initialization:
        mex -outdir ..\Projects\Windows\build -output PsychOculusVRCore -DPTBMODULE_PsychOculusVRCore -largeArrayDims -DMEX_DOUBLE_HANDLE -DWIN32 -I..\..\..\OculusSDKWin\LibOVR\Include -ICommon\Base -IWindows\Base -ICommon\PsychOculusVRCore Common\PsychOculusVRCore\*.c Windows\Base\*.c Common\Base\*.c ..\..\..\OculusSDKWin\LibOVR\Src\*.c ..\..\..\OculusSDKWin\LibOVR\Src\*.cpp kernel32.lib user32.lib winmm.lib
        movefile(['..\Projects\Windows\build\PsychOculusVRCore.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 13
        % Build PsychOculusVRCore1 for 64-Bit Matlab:
        % Needs the Oculus VR SDK v1.16 installed side-by-side to the Psychtoolbox-3
        % folder, so that it shares the same parent folder as Psychtoolbox-3,
        % and the SDK must be renamed from OculusSDK to OculusSDK1Win.
        % CAUTION: Need exactly v1.16 SDK, no earlier or later versions, due to
        % backwards incompatible API changes in v1.17+
        mex -outdir ..\Projects\Windows\build -output PsychOculusVRCore1 -DPTBMODULE_PsychOculusVRCore1 -largeArrayDims -DMEX_DOUBLE_HANDLE -DWIN32 -I..\..\..\OculusSDK1Win\LibOVR\Include -ICommon\Base -IWindows\Base -ICommon\PsychOculusVRCore1 Common\PsychOculusVRCore1\*.c Windows\Base\*.c Common\Base\*.c kernel32.lib user32.lib winmm.lib ..\..\..\OculusSDK1Win\LibOVR\Lib\Windows\x64\Release\VS2015\LibOVR.lib
        movefile(['..\Projects\Windows\build\PsychOculusVRCore1.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 14 && false
        % Build PsychOpenHMDVRCore.mexa64:
        % Needs the OpenHMD v0.3.0+ SDK installed side-by-side to the Psychtoolbox-3
        % folder, so that it shares the same parent folder as Psychtoolbox-3,
        % and the SDK must be renamed to OpenHMD.
        mex -outdir ..\Projects\Windows\build -output PsychOpenHMDVRCore -DPTBMODULE_PsychOpenHMDVRCore -largeArrayDims -DMEX_DOUBLE_HANDLE -I..\..\..\OpenHMD -L..\..\..\OpenHMD -ICommon\Base -IWindows\Base -ICommon\PsychOpenHMDVRCore Windows\Base\*.c Common\Base\*.c Common\PsychOpenHMDVRCore\*.c kernel32.lib user32.lib winmm.lib gdi32.lib -lhidapi -lopenhmd-0
        movefile(['..\Projects\Windows\build\PsychOpenHMDVRCore.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 15
        % Build PsychVulkanCore for 64-Bit Matlab:
        % Needs the official Vulkan SDK for 64-Bit Windows for at least
        % Vulkan 1.1 installed under C:\VulkanSDK\1.3.246.1
        mex -outdir ..\Projects\Windows\build -output PsychVulkanCore -DPTBMODULE_PsychVulkanCore -largeArrayDims -DMEX_DOUBLE_HANDLE -L"C:\VulkanSDK\1.3.246.1\Lib" -I"C:\VulkanSDK\1.3.246.1\Include" -ICommon\Base -IWindows\Base -ICommon\PsychVulkanCore Windows\Base\*.c Common\Base\*.c Common\PsychVulkanCore\*.c kernel32.lib user32.lib winmm.lib gdi32.lib vulkan-1.lib dxgi.lib dxguid.lib
        movefile(['..\Projects\Windows\build\PsychVulkanCore.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end

    if what == 16
        % Build PsychOpenXRCore for 64-Bit Matlab:
        % Needs the official Khronos OpenXR SDK for 64-Bit Windows from
        % https://github.com/KhronosGroup/OpenXR-SDK
        % installed side-by-side to the Psychtoolbox-3 folder, so that it
        % shares the same parent folder as Psychtoolbox-3.
        mex -outdir ..\Projects\Windows\build -output PsychOpenXRCore -DPTBMODULE_PsychOpenXRCore -largeArrayDims -DMEX_DOUBLE_HANDLE -L..\..\..\OpenXR-SDK\build\win64\src\loader\Release -I..\..\..\OpenXR-SDK\include\ -ICommon\Base -IWindows\Base -ICommon\PsychOpenXRCore Windows\Base\*.c Common\Base\*.c Common\PsychOpenXRCore\*.c kernel32.lib user32.lib winmm.lib opengl32.lib openxr_loader.lib
        movefile(['..\Projects\Windows\build\PsychOpenXRCore.' mexext], [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
    end
else
    % Octave build:
    target = [PsychtoolboxRoot 'PsychBasic\Octave8WindowsFiles64\'];

    if what == 0
        % Default: Build Screen.mex
        clear Screen;

        try
            % Set override linker search path: GStreamer lib dir comes before Octaves mingw64 dir:
            [rc, oldoctavelinkoptions] = system('mkoctfile --print LDFLAGS');
            oldoctavelinkoptions = deblank(oldoctavelinkoptions);
            setenv('LDFLAGS', ['-LC:\gstreamer\1.0\msvc_x86_64\lib ' oldoctavelinkoptions]);

            mexoctave -fno-exceptions --output ..\Projects\Windows\build\Screen.mex -DPTBMODULE_Screen -DPTB_USE_GSTREAMER -DPTBOCTAVE3MEX -DGLEW_STATIC -LC:\gstreamer\1.0\msvc_x86_64\lib -IC:\gstreamer\1.0\msvc_x86_64\include -IC:\gstreamer\1.0\msvc_x86_64\include\gstreamer-1.0 -IC:\gstreamer\1.0\msvc_x86_64\include\glib-2.0 -IC:\gstreamer\1.0\msvc_x86_64\include\glib-2.0\include -IC:\gstreamer\1.0\msvc_x86_64\lib\glib-2.0\include -IC:\gstreamer\1.0\msvc_x86_64\lib\gstreamer-1.0\include -IC:\gstreamer\1.0\msvc_x86_64\include\libxml2 -ICommon\Base -ICommon\Screen -IWindows\Base -IWindows\Screen Windows\Screen\*.c Windows\Base\*.c Common\Base\*.c Common\Screen\*.c Common\Screen\tinyexr.cc kernel32.lib user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib winmm.lib Dwmapi.lib PowrProf.lib delayimp.lib -lgobject-2.0 -lglib-2.0 -lgstreamer-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstpbutils-1.0
        catch
        end

        % Restore old linker path:
        setenv('LDFLAGS', oldoctavelinkoptions);

        % Move new mex file into its place:
        movefile(['..\Projects\Windows\build\Screen.' mexext], target);
    end

    if what == 1
        % Build WaitSecs.mex
        clear WaitSecs
        mexoctave --output ../Projects/Windows/build/WaitSecs.mex -DPTBMODULE_WaitSecs -DPTBOCTAVE3MEX -ICommon\Base -ICommon\WaitSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\WaitSecs\*.c kernel32.lib user32.lib winmm.lib
        movefile(['..\Projects\Windows\build\WaitSecs.' mexext], target);
    end

    if what == 2
        % Build PsychPortAudio.mex
        % If PsychPortAudio should support the proprietary ASIO sound backend, then add a: -DPTB_USE_ASIO
        %
        % This needs a libportaudio_x64 compatible with mingw64 gcc. The way we do this
        % is by taking the libportaudio_x64.dll from the PsychSound folder and copying
        % it into the PsychSourceGL/Cohorts/PortAudio/MinGW64 folder, but with the file extension
        % changed from .dll to .lib, ie. libportaudio_x64.lib - Weird but true, this makes
        % mingws linker accept the file as input.
        clear PsychPortAudio
        mexoctave --output ..\Projects\Windows\build\PsychPortAudio.mex -DPTBMODULE_PsychPortAudio -DPTBOCTAVE3MEX -L..\Cohorts\PortAudio\MinGW64 -ICommon\Base -ICommon\PsychPortAudio -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\PsychPortAudio\*.c kernel32.lib user32.lib advapi32.lib winmm.lib delayimp.lib -lportaudio_x64
        movefile(['..\Projects\Windows\build\PsychPortAudio.' mexext], target);
    end

    if what == 3
        % Build GetSecs.mex
        clear GetSecs
        mexoctave --output ..\Projects\Windows\build\GetSecs.mex -DPTBMODULE_GetSecs -DPTBOCTAVE3MEX -ICommon\Base -ICommon\GetSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\GetSecs\*.c kernel32.lib user32.lib winmm.lib
        movefile(['..\Projects\Windows\build\GetSecs.' mexext], target);
    end

    if what == 4
        % Build IOPort.mex
        clear IOPort
        mexoctave --output ..\Projects\Windows\build\IOPort.mex -DPTBMODULE_IOPort -DPTBOCTAVE3MEX -ICommon\Base -ICommon\IOPort -IWindows\Base -IWindows\IOPort Windows\Base\*.c Common\Base\*.c Common\IOPort\*.c Windows\IOPort\*.c kernel32.lib user32.lib winmm.lib
        movefile(['..\Projects\Windows\build\IOPort.' mexext], target);
    end

    if what == 5 && exist('..\..\..\SRAnipalCSDK', 'dir')
        % Build SRAnipalMex on MS-Windows for Octave 64 Bit:
        %
        % This requires the proprietary HTC SRAnipal SDK installed side by side
        % to the Psychtoolbox-3 folder in a folder called SRAnipalCSDK. See above
        % Matlab build path for the SDK download location and setup instructions.
        %
        clear SRAnipalMex;
        mexoctave --output ..\Projects\Windows\build\SRAnipalMex.mex -I..\..\..\SRAnipalCSDK\include -L..\..\..\SRAnipalCSDK\lib ..\Cohorts\SRAnipalMex.cpp -lSRanipal
        movefile(['..\Projects\Windows\build\SRAnipalMex.' mexext], target);
    end

    if what == 6
        % Build moglcore.mex:
        curdir = pwd;
        cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
        clear moglcore
        try
            mexoctave --output moglcore.mex -DWINDOWS -DGLEW_STATIC -I..\..\..\..\PsychSourceGL\Cohorts\freeglut\include -L..\..\..\..\PsychSourceGL\Cohorts\freeglut\lib\x64 -I. windowhacks.c gl_manual.c mogl_rebinder.c moglcore.c glew.c ftglesGlue.c gl_auto.c user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib -lfreeglut
            movefile(['moglcore.' mexext], target);
        catch
            lasterr
        end
        cd(curdir);
    end

    if what == 7
        % Build Eyelink.mex
        clear Eyelink
        try
            copyfile('C:\Program Files (x86)\SR Research\EyeLink\libs\x64\*.dll', '..\..\..\');
            mexoctave -W'l,--enable-stdcall-fixup' --output ..\Projects\Windows\build\Eyelink.mex -DPTBMODULE_Eyelink -DPTBOCTAVE3MEX -I'C:\Program Files (x86)\SR Research\EyeLink\Includes\eyelink' -L..\..\.. -ICommon\Base -ICommon\Eyelink -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\Eyelink\*.c user32.lib gdi32.lib advapi32.lib winmm.lib -leyelink_core64 -leyelink_w32_comp64
            movefile(['..\Projects\Windows\build\Eyelink.' mexext], target);
        catch
        end
        delete('..\..\..\*.dll');
    end

    if what == 8
        % Build PsychKinectCore.mex:
        clear PsychKinectCore
        mexoctave --output ..\Projects\Windows\build\PsychKinectCore.mex -DPTBMODULE_PsychKinectCore -DPTBOCTAVE3MEX -L..\Cohorts\Kinect-v16-withsource\libusb\lib\msvc_x64 -I..\Cohorts\Kinect-v16-withsource -ICommon\Base -IWindows\Base -ICommon\PsychKinect Windows\Base\*.c Common\Base\*.c Common\PsychKinect\*.c ..\Cohorts\Kinect-v16-withsource\*.cpp kernel32.lib user32.lib winmm.lib libusb.lib
        movefile(['..\Projects\Windows\build\PsychKinectCore.' mexext], target);
    end

    if what == 9
        % Build PsychHID.mex:
        clear PsychHID
        mexoctave --output ..\Projects\Windows\build\PsychHID.mex -DPTBMODULE_PsychHID -DPTBOCTAVE3MEX -L..\Cohorts\libusb1-win32\MinGW64\dll -I..\Cohorts\libusb1-win32\include\libusb-1.0 -ICommon\Base -IWindows\Base -ICommon\PsychHID Windows\PsychHID\*.c Windows\Base\*.c Common\Base\*.c Common\PsychHID\*.c dinput8.lib kernel32.lib user32.lib winmm.lib libusb-1.0.lib setupapi.lib
        movefile(['..\Projects\Windows\build\PsychHID.' mexext], target);
    end

    if what == 10
        % Build moalcore 64-Bit:
        curdir = pwd;
        cd('../../Psychtoolbox/PsychSound/MOAL/source/')
        try
            clear moalcore
            copyfile('C:\Program Files (x86)\OpenAL 1.1 SDK\libs\Win64\OpenAL32.lib', '.\');

            mexoctave --output moalcore.mex -DWINDOWS -I'C:\Program Files (x86)\OpenAL 1.1 SDK\include' -L. moalcore.c al_auto.c al_manual.c alm.c user32.lib -lOpenAL32
            movefile(['moalcore.' mexext], target);
        catch
            lasterr
        end
        % Remove stale object files:
        delete('OpenAL32.lib');
        cd(curdir);
    end

    if what == 11
        % Build pnet
        curdir = pwd;
        cd('../../Psychtoolbox/PsychHardware/iViewXToolbox/tcp_udp_ip/')
        clear pnet
        try
            mexoctave --output pnet.mex -DWIN32 pnet.c ws2_32.lib winmm.lib
            movefile(['pnet.' mexext], target);
        catch
        end

        cd(curdir);
    end

    if what == 12
        % Build PsychOculusVRCore 64 Bit:
        % Needs the Oculus VR SDK v 0.5.01 installed in side-by-side to
        % the Psychtoolbox-3 folder, ie., in the same parent directory, and
        % renamed from OculusSDK to OculusSDKWin. Needs to Oculus VR runtime
        % for Windows installed as well. Then we copy the installed runtime DLL
        % C:\windows\system32\LibOVRRT64_0_5.dll into the SDK folder and abuse that
        % as import library for our mex file. Weird but true - this seems to work!
        clear PsychOculusVRCore
        try
            copyfile('C:\windows\system32\LibOVRRT64_0_5.dll', '..\..\..\OculusSDKWin\LibOVR\Lib\Windows\Win32\Release\VS2010\');
        end
        mexoctave --output ..\Projects\Windows\build\PsychOculusVRCore.mex -DPTBMODULE_PsychOculusVRCore -DPTBOCTAVE3MEX -I..\..\..\OculusSDKWin\LibOVR\Include -ICommon\Base -IWindows\Base -ICommon\PsychOculusVRCore Common\PsychOculusVRCore\*.c Windows\Base\*.c Common\Base\*.c kernel32.lib user32.lib winmm.lib -L..\..\..\OculusSDKWin\LibOVR\Lib\Windows\Win32\Release\VS2010 -lOVRRT64_0_5
        movefile(['..\Projects\Windows\build\PsychOculusVRCore.' mexext], target);
    end

    if what == 13
        % Build PsychOculusVRCore1 for 64-Bit Octave-4.4.1 or later:
        % Needs the Oculus VR SDK v1.16 installed side-by-side to the Psychtoolbox-3
        % folder, so that it shares the same parent folder as Psychtoolbox-3,
        % and the SDK must be renamed from OculusSDK to OculusSDK1Win.
        % CAUTION: Need exactly v1.16 SDK, no earlier or later versions, due to
        % backwards incompatible API changes in v1.17+
        try
            % For the Octave build, we compile the OVR_CAPIshim.c shim file and
            % other helper C files from the SDK directly into our mex file, instead
            % of statically linking against LibOVR.lib, as that .lib import file only
            % works with MSVC, but not with Octave's gcc based build system. The shim
            % will locate and runtime-link against the libOVRRT_64_1.dll of the installed
            % Oculus VR runtime during initialization:
            mexoctave --output ..\Projects\Windows\build\PsychOculusVRCore1.mex -DPTBMODULE_PsychOculusVRCore1 -DPTBOCTAVE3MEX -I..\..\..\OculusSDK1Win\LibOVR\Include -ICommon\Base -IWindows\Base -ICommon\PsychOculusVRCore1 Common\PsychOculusVRCore1\*.c Windows\Base\*.c Common\Base\*.c ..\..\..\OculusSDK1Win\LibOVR\Src\*.c* kernel32.lib user32.lib winmm.lib
            movefile(['..\Projects\Windows\build\PsychOculusVRCore1.' mexext], target);
        catch %#ok<*CTCH>
            % Empty. We just want to make sure the delete() call below is executed
            % in both success and failure case.
        end
    end

    if what == 14 && false
        % Build PsychOpenHMDVRCore.mex:
        % Needs the OpenHMD v0.3.0+ SDK installed side-by-side to the Psychtoolbox-3
        % folder, so that it shares the same parent folder as Psychtoolbox-3,
        % and the SDK must be renamed to OpenHMD.
        mexoctave --output ..\Projects\Windows\build\PsychOpenHMDVRCore.mex -DPTBMODULE_PsychOpenHMDVRCore -DPTBOCTAVE3MEX -I..\..\..\OpenHMD -L..\..\..\OpenHMD -ICommon\Base -IWindows\Base -ICommon\PsychOpenHMDVRCore Windows\Base\*.c Common\Base\*.c Common\PsychOpenHMDVRCore\*.c kernel32.lib user32.lib winmm.lib gdi32.lib -lhidapi -lopenhmd-0
        movefile(['..\Projects\Windows\build\PsychOpenHMDVRCore.' mexext], target);
    end

    if what == 15
        % Build PsychVulkanCore.mex for 64-bit Octave:
        % Needs the official Vulkan SDK for 64-Bit Windows for at least
        % Vulkan 1.1 installed under C:\VulkanSDK\1.3.246.1
        try
            mexoctave --output ..\Projects\Windows\build\PsychVulkanCore.mex -DPTBMODULE_PsychVulkanCore -DPTBOCTAVE3MEX -LC:\VulkanSDK\1.3.246.1\Lib -IC:\VulkanSDK\1.3.246.1\Include -ICommon\Base -IWindows\Base -ICommon\PsychVulkanCore Windows\Base\*.c Common\Base\*.c Common\PsychVulkanCore\*.c kernel32.lib user32.lib winmm.lib gdi32.lib vulkan-1.lib dxgi.lib dxguid.lib
            movefile(['..\Projects\Windows\build\PsychVulkanCore.' mexext], target);
        catch
            disp(psychlasterror);
        end
    end

    if what == 16
        % Build PsychOpenXRCore.mex for 64-Bit Octave:
        % Needs the official Khronos OpenXR SDK for 64-Bit Windows from
        % https://github.com/KhronosGroup/OpenXR-SDK
        % installed side-by-side to the Psychtoolbox-3 folder, so that it
        % shares the same parent folder as Psychtoolbox-3.
        %
        % Note: A statically linked openxr_loader.lib, built with MSVC does not
        % work with Octave, because it depends on MSVC runtime libraries, so
        % linking during build-time would fail. Therefore we MSVC build a dynamic
        % openxr_loader.dll in a separate builddll\ subdirectory and link against
        % that library via a pure openxr_loader.lib import library. This works
        % with Octave when storing the openxr_loader.dll alongside PsychOpenXRCore.mex.
        %
        % It may also work with Matlab, but this is so far not verified.
        % TODO: Verify and reunite by Matlab also using the openxr_loader.dll.
        try
            mexoctave --output ..\Projects\Windows\build\PsychOpenXRCore.mex -DPTBMODULE_PsychOpenXRCore -DPTBOCTAVE3MEX -L..\..\..\OpenXR-SDK\builddll\win64\src\loader\RelWithDebInfo -I..\..\..\OpenXR-SDK\include -ICommon\Base -IWindows\Base -ICommon\PsychOpenXRCore Windows\Base\*.c Common\Base\*.c Common\PsychOpenXRCore\*.c kernel32.lib user32.lib winmm.lib opengl32.lib openxr_loader.lib
            movefile(['..\Projects\Windows\build\PsychOpenXRCore.' mexext], target);
        catch
            disp(psychlasterror);
        end
    end
end

catch err
    % Empty. We just want to make sure the delete() call below is executed
    % in both success and failure case.
    disp(err.message)
end

return;

% Special mex wrapper for Octave compile on Windows:
function mexoctave(varargin)
debugme = 0;
callmex = 1;

if (debugme), fprintf('nargin = %i\n', nargin); end
myvararg = cell();
myvararg(end+1) = '-DPTBOCTAVE3MEX';
myvararg(end+1) = '-Wno-multichar';
myvararg(end+1) = '-Wno-unknown-pragmas';
myvararg(end+1) = '-s'; % Strip mex/oct files to get them down to a reasonable size.

outarg = '';
quoted = 0;
emitarg = 0;
for i=1:nargin
    curarg = char(varargin(i));

    if debugme, fprintf('Preparse Arg %i: %s\n', i, curarg); end
    if (~isempty(strfind(curarg, '"')))
        if ~quoted
            % Start of quoted string:
            quoted = 1;
            outarg = [curarg];
            emitarg = 0;
        else
            % End of quoted string: Emit!
            quoted = 0;
            outarg = [ outarg curarg ];
            emitarg = 1;
        end
    else
        % Not start or end string of a quoted piece:
        if quoted
            % Within a quoted segment!
            outarg = [ outarg curarg ];
            emitarg = 0;
        else
            % Outside a quoted segment:
            % Expansion needed?
            ppos = strfind(curarg, '*.c');
            if ~isempty(ppos)
                prefix  = curarg(1:ppos(1)-1);
                allfiles = dir(curarg);
                for j=1:length(allfiles)
                    expandedfiles = [ prefix allfiles(j).name ];
                    emitarg = 0;
                    myvararg(end+1) = expandedfiles;
                    if (debugme), fprintf('Emitted Arg %i : %s\n', length(myvararg), char(myvararg(end))); end;
                end
            else
                % Regular chunk: Emit it
                ppos = strfind(curarg, '.lib');
                if ~isempty(ppos)
                    curarg = [ '-l' curarg(1:ppos(1)-1) ];
                end

                outarg = curarg;
                emitarg = 1;
            end
        end
    end

    if emitarg
        emitarg = 0;
        myvararg(end+1) = outarg;
        if (debugme), fprintf('Emitted Arg %i : %s\n', length(myvararg), char(myvararg(end))); end;
    end
end

final = myvararg;

if (debugme), outargtype = class(myvararg), end

if (callmex)
    mex(myvararg{:});
end

return;
