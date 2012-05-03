function osxmakeit(mode)
% This is the MacOS/X version of makeit:

if nargin < 1
    mode = 0
end;

fprintf('Building plugin type %i ...\n\n', mode);

%  LDFLAGS="\$LDFLAGS -framework CoreServices -framework CoreFoundation -framework CoreAudio" CFLAGS="" CXXFLAGS="" -I/usr/include 

if mode==0
    %TODO: Depends -- All kind of shit: GStreamer, libdc1394
    % Build Screen:
    % mex -v -outdir ../Projects/MacOSX/build -output Screen -largeArrayDims -DPTBMODULE_Screen -DPTB_USE_GSTREAMER -DPTBVIDEOCAPTURE_LIBDC LDFLAGS="\$LDFLAGS -framework CoreServices -framework CoreFoundation -framework CoreAudio" CFLAGS="-framework CoreServices -framework CoreFoundation -framework CoreAudio" CXXFLAGS="-framework CoreServices" -I/usr/include -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -ICommon/Base -ICommon/Screen -IOSX/Base -IOSX/Screen "Common/Base/*.cc" "OSX/Base/*.c" "OSX/Screen/*.c" "Common/Screen/*.c" "Common/Base/*.c" -lc -lrt -ldl -lGL -lGLU -lX11 -lXext -lgstreamer-0.10 -lgstbase-0.10 -lgstapp-0.10 -lgstinterfaces-0.10 -lgobject-2.0 -lgmodule-2.0 -lxml2 -lgthread-2.0 -lglib-2.0 -ldc1394 -lusb-1.0
    mex -v -outdir ../Projects/MacOSX/build -output Screen -largeArrayDims -DPTBMODULE_Screen LDFLAGS="\$LDFLAGS -framework CoreServices -framework CoreFoundation -framework ApplicationServices -framework CoreAudio -framework OpenGL -framework CoreVideo -framework IOKit -framework AGL -framework SystemConfiguration -framework Carbon" CFLAGS="" CXXFLAGS="" -I/usr/include -ICommon/Base -ICommon/Screen -IOSX/Base -IOSX/Screen -IOSX/Fonts -IOSX/EthernetAddress "Common/Base/*.cc" "OSX/Base/*.c" "OSX/Screen/*.c" "OSX/Fonts/*FontGlue*.c" "OSX/Fonts/FontInfo.c" "OSX/EthernetAddress/*.c" "Common/Screen/*.c" "Common/Base/*.c" % -lgstreamer-0.10 -lgstbase-0.10 -lgstapp-0.10 -lgstinterfaces-0.10 -lgobject-2.0 -lgmodule-2.0 -lxml2 -lgthread-2.0 -lglib-2.0 -ldc1394 -lusb-1.0
    unix(['mv ../Projects/MacOSX/build/Screen.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end;

if mode==1
    % Build GetSecs:
    mex -v -outdir ../Projects/MacOSX/build -output GetSecs -largeArrayDims -DPTBMODULE_GetSecs LDFLAGS="\$LDFLAGS -framework CoreServices -framework CoreFoundation -framework CoreAudio" CFLAGS="" CXXFLAGS="" -I/usr/include -ICommon/Base -IOSX/Base -ICommon/GetSecs "Common/Base/*.cc" "OSX/Base/*.c" "Common/Base/*.c" "Common/GetSecs/*.c"
    unix(['mv ../Projects/MacOSX/build/GetSecs.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end;

if mode==2
    % Build WaitSecs:
    mex -v -outdir ../Projects/MacOSX/build -output WaitSecs -largeArrayDims -DPTBMODULE_WaitSecs LDFLAGS="\$LDFLAGS -framework CoreServices -framework CoreFoundation -framework CoreAudio" CFLAGS="" CXXFLAGS="" -I/usr/include -ICommon/Base -IOSX/Base -ICommon/WaitSecs "Common/Base/*.cc" "OSX/Base/*.c" "Common/Base/*.c" "Common/WaitSecs/*.c"
    unix(['mv ../Projects/MacOSX/build/WaitSecs.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end;

if mode==3
    %TODO -- Depends: PortAudio
    % Build PsychPortAudio:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions' -v -outdir ../Projects/MacOSX/build/ -output PsychPortAudio -largeArrayDims -DPTBMODULE_PsychPortAudio -ICommon/Base -IOSX/Base -ICommon/PsychPortAudio -ICommon/Screen Common/Base/*.cc OSX/Base/*.c Common/Base/*.c Common/PsychPortAudio/*.c /usr/local/lib/libportaudio.a -lc -lrt -lasound
    unix(['mv ../Projects/MacOSX/build/PsychPortAudio.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==4
    %TODO -- Depends: eyelink-SDK
    % Build Eyelink:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions' -v -outdir ../Projects/MacOSX/build/ -output Eyelink -largeArrayDims -DPTBMODULE_Eyelink -ICommon/Base -IOSX/Base -ICommon/Eyelink -ICommon/Screen Common/Base/*.cc OSX/Base/*.c Common/Base/*.c Common/Eyelink/*.c -leyelink_core -lc -lrt
    unix(['mv ../Projects/MacOSX/build/Eyelink.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==5
    % Build IOPort:
    mex -v -outdir ../Projects/MacOSX/build -output IOPort -largeArrayDims -DPTBMODULE_IOPort LDFLAGS="\$LDFLAGS -framework CoreServices -framework CoreFoundation -framework CoreAudio" CFLAGS="" CXXFLAGS="" -I/usr/include  -ICommon/Base -IOSX/Base -ICommon/IOPort "Common/Base/*.cc" "OSX/Base/*.c" "Common/Base/*.c" "Common/IOPort/*.c"
    unix(['mv ../Projects/MacOSX/build/IOPort.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==6
    %TODO
    % Build moglcore:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychOpenGL/MOGL/source/')
    try
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions' -v -outdir ./ -output moglcore -DOSX -DGLEW_STATIC -largeArrayDims -I/usr/X11R6/include -L/usr/X11R6/lib -lc -lGL -lGLU -lglut moglcore.c gl_auto.c gl_manual.c glew.c mogl_rebinder.c
    catch
    end
    unix(['mv moglcore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
    cd(curdir);
end

if mode==7
    %TODO -- Depends: libfreenect
    % Build PsychKinectCore:
    mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions' -v -outdir ../Projects/MacOSX/build/ -output PsychKinectCore -largeArrayDims -DPTBMODULE_PsychKinectCore -I/usr/include/libusb-1.0 -I/usr/include/libfreenect -ICommon/Base -IOSX/Base -ICommon/PsychKinect -ICommon/Screen Common/Base/*.cc OSX/Base/*.c Common/Base/*.c Common/PsychKinect/*.c -lc -lrt -lfreenect -lusb-1.0
    unix(['mv ../Projects/MacOSX/build/PsychKinectCore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==8
    % Build PsychHID:
    % Dynamic link:   mex -v -outdir ../Projects/MacOSX/build -output PsychHID -largeArrayDims -DPTBMODULE_PsychHID LDFLAGS="\$LDFLAGS -framework ApplicationServices -framework Carbon -framework CoreAudio -framework IOKit" CFLAGS="" CXXFLAGS="" -I/usr/include -ICommon/Base -IOSX/Base -ICommon/PsychHID -IOSX/PsychHID -I../Cohorts/HID_Utilities_64Bit/ -I../Cohorts/HID_Utilities_64Bit/IOHIDManager "Common/Base/*.cc" "Common/PsychHID/*.c" "OSX/PsychHID/*.c" "OSX/Base/*.c" "Common/Base/*.c" -L../Cohorts/HID_Utilities_64Bit/build/Release -lHID_Utilities
    mex -v -outdir ../Projects/MacOSX/build -output PsychHID -largeArrayDims -DPTBMODULE_PsychHID LDFLAGS="\$LDFLAGS -framework ApplicationServices -framework Carbon -framework CoreAudio -framework IOKit" CFLAGS="" CXXFLAGS="" -I/usr/include -ICommon/Base -IOSX/Base -ICommon/PsychHID -IOSX/PsychHID -I../Cohorts/HID_Utilities_64Bit/ -I../Cohorts/HID_Utilities_64Bit/IOHIDManager "Common/Base/*.cc" "Common/PsychHID/*.c" "OSX/PsychHID/*.c" "OSX/Base/*.c" "Common/Base/*.c" ../Cohorts/HID_Utilities_64Bit/build/Release/libHID_Utilities64.a
    unix(['mv ../Projects/MacOSX/build/PsychHID.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

if mode==9
    %TODO
    % Build moalcore:
    curdir = pwd;
    cd('../../Psychtoolbox/PsychSound/MOAL/source/')
    try
       mex CFLAGS='$CFLAGS -fPIC -std=gnu99 -fexceptions' -v -outdir ./ -output moalcore -largeArrayDims -DOSX -lc -lopenal moalcore.c al_auto.c al_manual.c alm.c 
    catch
    end
    unix(['mv ./moalcore.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);

    cd(curdir);
end

if mode==10
    % Build FontInfo:
    mex -v -outdir ../Projects/MacOSX/build -output FontInfo -largeArrayDims -DPTBMODULE_FontInfo LDFLAGS="\$LDFLAGS -framework ApplicationServices -framework CoreAudio" CFLAGS="" CXXFLAGS="" -I/usr/include -ICommon/Base -ICommon/Fonts -IOSX/Base -IOSX/Fonts "Common/Base/*.cc" "Common/Base/*.c" "OSX/Base/*.c" "OSX/Fonts/*.c"
    unix(['mv ../Projects/MacOSX/build/FontInfo.' mexext ' ' PsychtoolboxRoot 'PsychBasic/']);
end

return;
