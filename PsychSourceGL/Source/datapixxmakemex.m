function datapixxmakemex()

    if (IsOSX)
        VPIXXDIR = '/Users/kleinerm/projects/';
        CPYCMD = 'cp ';
        DELCMD = 'rm ';
    elseif (IsLinux)
        if Is64Bit || IsARM
          VPIXXDIR = '/home/kleinerm/projects/';
        else
          VPIXXDIR = '/media/sf_kleinerm/projects/';
        end

        CPYCMD = 'cp ';
        DELCMD = 'rm ';
    elseif (IsWin)
        VPIXXDIR = 'T:/projects/';
        if exist('C:/Users/kleinerm/Documents/GitHub/', 'dir')
            VPIXXDIR = 'C:/Users/kleinerm/Documents/GitHub/';
        end

        if exist('C:/Users/mario/Documents/GitHub/', 'dir')
            VPIXXDIR = 'C:/Users/mario/Documents/GitHub/';
        end

        CPYCMD = 'copy ';
        DELCMD = 'del ';
    end

    if ~IsWin
        PTBDIR = [VPIXXDIR 'OpenGLPsychtoolbox/Psychtoolbox-3/'];
    else
        PTBDIR = [VPIXXDIR 'Psychtoolbox-3/'];
    end

    % Start constructing mex command
    S = 'mex -v';   % -v for verbose output.

    if IsOctave
        % -s for stripping the files under Octave:
        S = [S ' -s'];
    end

    S = [S ' -DPTBMODULE_Datapixx'];

    if IsOctave
        S = [S ' -DPTBOCTAVE3MEX'];
    else
        % Build it backwards compatible with Matlab releases before R2014b:
        S = [S ' -largeArrayDims -DMEX_DOUBLE_HANDLE'];
    end

    if IsLinux
        if ~IsOctave
            S = [S ' CFLAGS=''$CFLAGS -fPIC -std=gnu99 -fexceptions'' '];
        else
            S = [S ' -fexceptions'];
        end
    end

    S = [S ' -I' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/src'];
    S = [S ' -I' VPIXXDIR 'VPixx_Software_Tools/libdpx/src'];
    S = [S ' -I' PTBDIR 'PsychSourceGL/Source/Common/Base'];
    S = [S ' -I' PTBDIR 'PsychSourceGL/Source/Common/Screen'];

    if (IsOSX)
        S = [S ' -I' VPIXXDIR 'VPixx_Software_Tools/libusb'];
        S = [S ' -I' PTBDIR 'PsychSourceGL/Source/OSX/Base'];
        S = [S ' -I' PTBDIR 'PsychSourceGL/Source/OSX/Screen'];
        S = [S ' -I' PTBDIR 'PsychSourceGL/Source/OSX/Fonts'];
    elseif (IsLinux)
        S = [S ' -I' VPIXXDIR 'VPixx_Software_Tools/libusb'];
        S = [S ' -I' PTBDIR 'PsychSourceGL/Source/Linux/Base'];
        S = [S ' -I' PTBDIR 'PsychSourceGL/Source/Linux/Screen'];
    elseif (IsWin)
        if ~IsOctave
            % Include this folder only on Matlab (or other MSVC builds). It contains a stdint.h file
            % previously located in the src/ folder directly. This stdint.h file lacks definitions of
            % int32_t, int64_t et al., but overrides Octave's / MinGW's stdint.h files which do define
            % those needed data types! Strangely this override was never a problem on Octave 4.2.x and
            % earlier. Apparently the type defines came from an alternate source, but i could not track
            % it down in multiple hours, so screw this - a hack it is :(
            S = [S ' -I' VPIXXDIR 'VPixx_Software_Tools/libusb_win32/libusb-win32-src-0.1.12.2/src/include'];
        end
        S = [S ' -I' VPIXXDIR 'VPixx_Software_Tools/libusb_win32/libusb-win32-src-0.1.12.2/src'];
        S = [S ' -I' PTBDIR 'PsychSourceGL/Source/Windows/Base'];
        S = [S ' -I' PTBDIR 'PsychSourceGL/Source/Windows/Screen'];
        S = [S ' -DWIN_BUILD'];    % libdpx DPxOpen mod under Windows
    end

    S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/src/PsychDatapixx.c'];
    S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/src/RegisterProject.c'];
    S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libdpx/src/libdpx.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/MiniBox.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/ProjectTable.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychAuthors.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychCellGlue.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychError.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychHelp.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychInit.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychInstrument.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychMemory.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychRegisterProject.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychStructGlue.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychVersioning.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychScriptingGlue.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/PsychScriptingGlueMatlab.c'];
    S = [S ' ' PTBDIR 'PsychSourceGL/Source/Common/Base/MODULEVersion.c'];

    if (IsOSX)
        S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb/descriptors.c'];
        S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb/error.c'];
        S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb/usb.c'];
        S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb/darwin.c'];
        S = [S ' ' PTBDIR 'PsychSourceGL/Source/OSX/Base/PsychTimeGlue.c'];
        if IsOctave
            S = [S ' ''-mmacosx-version-min=10.11'' '];
            S = [S ' ''-Wl,-headerpad_max_install_names,-framework,ApplicationServices,-framework,CoreServices,-framework,CoreFoundation,-framework,Carbon,-framework,CoreAudio,-framework,IOKit'' '];
        else
            S = [S ' CFLAGS="\$CFLAGS -mmacosx-version-min=10.11" LDFLAGS="\$LDFLAGS -mmacosx-version-min=10.11 -framework ApplicationServices -framework CoreServices -framework CoreFoundation -framework Carbon -framework CoreAudio -framework IOKit" '];
        end
        if (IsOctave)
            S = [S ' --output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/macosx/Datapixx.mex'];
        else
            if (Is64Bit)
                S = [S ' -output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/macosx64/Datapixx'];
            else
                S = [S ' -output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/macosx/Datapixx'];
            end
        end
    elseif (IsLinux)
        S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb/descriptors.c'];
        S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb/error.c'];
        S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb/usb.c'];
        S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb/linux.c'];
        S = [S ' ' PTBDIR 'PsychSourceGL/Source/Linux/Base/PsychTimeGlue.c'];
        S = [S ' -lc'];     % Because Mario says so
        S = [S ' -lrt'];    % for clock_getres() and other clock_*()
        S = [S ' -ldl'];    % for dlsym() etc. use in PsychTimeGlue.c.

        if (IsOctave)
            if (Is64Bit)
                S = [S ' --output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/linux64/Datapixx.mex'];
            else
                S = [S ' --output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/linux/Datapixx.mex'];
            end
        else
            if (Is64Bit)
                S = [S ' -output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/linux64/Datapixx'];
            else
                S = [S ' -output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/linux/Datapixx'];
            end
        end
    elseif (IsWin)
        S = [S ' ' PTBDIR 'PsychSourceGL/Source/Windows/Base/PsychTimeGlue.c'];
        if (Is64Bit)
            if IsOctave
                % We use a stub-loader which runtime loads and manually links the DLL.
                % For unknown reasons, simply using the libusb0.dll for 64-Bit MSVC does
                % not work, but crash. Manually loading this way otoh. works fine with the
                % same DLL. Go figure!
                S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb_win32/libusb-win32-device-bin-0.1.12.2/lib/dynamic/libusb_dyn.c'];
            else
                S = [S ' -L' VPIXXDIR 'VPixx_Software_Tools/libusb_win32/libusb-win32-device-bin-0.1.12.2/lib/msvc_x64 -llibusb'];
            end
        else
            if IsOctave
                S = [S ' ' VPIXXDIR 'VPixx_Software_Tools/libusb_win32/libusb-win32-device-bin-0.1.12.2/lib/gcc/libusb.a'];
            else
                S = [S ' -L' VPIXXDIR 'VPixx_Software_Tools/libusb_win32/libusb-win32-device-bin-0.1.12.2/lib/msvc -llibusb'];
            end
        end
        if (IsOctave)
            S = [S ' -lWinmm']; % for timeGetTime() et al
            S = [S ' --output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/win32/Datapixx.mex'];
        else
            S = [S ' -L"C:/Program Files/Microsoft Visual Studio/VC98/Lib" -lWinmm']; % for timeGetTime() et al
            if (Is64Bit)
                S = [S ' -output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/win64/Datapixx.mexw64'];
            else
                S = [S ' -output ' VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/win32/Datapixx.mexw32'];
            end
        end
    end

    eval(strrep(S, '/', filesep));

    %   Move mex file to final destination, and clean up any temporary build files.
    %   Octave puts object files in same folders as source files, and we have to delete them manually.
    %   Matlab is a bit smarter, putting the object files in the current directory, then immediately cleaning them up.
    if (IsOctave)
        v = sscanf(version, '%i.%i.%i');
        system(strrep([DELCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/src/*.o'], '/', filesep));
        system(strrep([DELCMD VPIXXDIR 'VPixx_Software_Tools/libdpx/src/*.o'], '/', filesep));
        system(strrep([DELCMD PTBDIR 'PsychSourceGL/Source/Common/Base/*.o'], '/', filesep));
        % system(strrep([DELCMD PTBDIR 'PsychSourceGL/Source/Common/Screen/*.o'], '/', filesep));
        if IsOSX(1)
            system(strrep([DELCMD VPIXXDIR 'VPixx_Software_Tools/libusb/*.o'], '/', filesep));
            system(strrep([DELCMD PTBDIR 'PsychSourceGL/Source/OSX/Base/*.o'], '/', filesep));
            if IsARM
                system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/macosx/Datapixx.mex ' PTBDIR 'Psychtoolbox/PsychBasic/Octave10OSXFilesARM64'], '/', filesep));
                osxsetoctaverpath('Datapixx', [PTBDIR 'Psychtoolbox/PsychBasic/Octave10OSXFilesARM64/']);
            else
                system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/macosx/Datapixx.mex ' PTBDIR 'Psychtoolbox/PsychBasic/Octave10OSXFiles64'], '/', filesep));
                osxsetoctaverpath('Datapixx', [PTBDIR 'Psychtoolbox/PsychBasic/Octave10OSXFiles64/']);
            end
        elseif IsLinux
            system(strrep([DELCMD VPIXXDIR 'VPixx_Software_Tools/libusb/*.o'], '/', filesep));
            system(strrep([DELCMD PTBDIR 'PsychSourceGL/Source/Linux/Base/*.o'], '/', filesep));
            % We require Octave-5 or later:
            if Is64Bit
                system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/linux64/Datapixx.mex ' PTBDIR 'Psychtoolbox/PsychBasic/Octave5LinuxFiles64'], '/', filesep));
            else
                if IsARM
                    system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/linux/Datapixx.mex ' PTBDIR 'Psychtoolbox/PsychBasic/Octave3LinuxFilesARM'], '/', filesep));
                else
                    system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/linux/Datapixx.mex ' PTBDIR 'Psychtoolbox/PsychBasic/Octave3LinuxFiles'], '/', filesep));
                end
            end
        elseif IsWin
            system(strrep([DELCMD PTBDIR 'PsychSourceGL/Source/Windows/Base/*.o'], '/', filesep));
            if Is64Bit
                system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/win32/Datapixx.mex ' PTBDIR 'Psychtoolbox/PsychBasic/Octave8WindowsFiles64'], '/', filesep));
            else
                system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/octave/win32/Datapixx.mex ' PTBDIR 'Psychtoolbox/PsychBasic/Octave8WindowsFiles'], '/', filesep));
            end
        end
    else
        if (IsOSX(1))           % 64-bit MATLAB on OS X
            system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/macosx64/Datapixx.' mexext ' ' PTBDIR 'Psychtoolbox/PsychBasic'], '/', filesep));
        elseif (IsOSX)          % 32-bit MATLAB on OS X
            system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/macosx/Datapixx.' mexext ' ' PTBDIR 'Psychtoolbox/PsychBasic'], '/', filesep));
            system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/macosx/Datapixx.' mexext ' ' VPIXXDIR 'git_Psychtoolbox-3/Psychtoolbox/PsychBasic'], '/', filesep));
        elseif (IsLinux(1))     % 64-bit MATLAB on Linux
            system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/linux64/Datapixx.' mexext ' ' PTBDIR 'Psychtoolbox/PsychBasic'], '/', filesep));
        elseif (IsLinux)        % 32-bit MATLAB on Linux
            system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/linux/Datapixx.' mexext ' ' PTBDIR 'Psychtoolbox/PsychBasic'], '/', filesep));
        elseif (IsWin(1))       % 64-bit MATLAB on Windows
            system(strrep([CPYCMD VPIXXDIR 'VPixx_Software_Tools/DatapixxToolbox_trunk/mexdev/build/matlab/win64/Datapixx.mexw64 ' PTBDIR 'Psychtoolbox/PsychBasic'], '/', filesep));
        end
    end

return;
