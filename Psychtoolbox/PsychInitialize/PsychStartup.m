function PsychStartup
% PsychStartup -- Perform Psychtoolbox related setup at Matlab/Octave startup.
%
% This performs setup of Matlab or Octave at session startup for
% Psychtoolbox.
%
% On MS-Windows, it currently detects if the GStreamer 1.22+ runtime is
% installed, as this is required for Screen() multi-media functions to
% work. It performs GStreamer setup, or outputs a warning if the runtime is
% missing.
%
% This function is normally called from the startup.m Matlab startup file,
% or from the .octaverc startup file on GNU/Octave.
%

% History:
% 14.09.2012  mk  Written.
% 14.01.2013  mk  Make path detection more robust.
% 12.09.2013  mk  Also apply GStreamer-SDK setup to 32-Bit Matlab on Windows.
% 26.08.2014  mk  Switch the 64-Bit setup to the GStreamer-1.4.0+ runtime.
% 17.10.2015  mk  Add warning about complete failure of Screen() for Octave on Windows.
% 30.10.2015  mk  Setenv PSYCH_GSTREAMER_SDK_ROOT to help GStreamer on Octave-4 for Windows.
% 14.01.2016  mk  Make detection more robust, delete dead code for GStreamer 0.10.
% 07.08.2019  mk  Update for Psychtoolbox 3.0.16 - GStreamer 1.16+ MSVC needed.
% 01.09.2020  mk  Update for GStreamer 1.18.0 - GStreamer 1.18+ MSVC detection.
% 26.10.2020  mk  Update for GStreamer 1.18.0+ use only.
% 21.11.2020  mk  Reenable GStreamer on octave-cli for Windows. Workaround no longer needed.
% 11.10.2021  mk  Fix wrong drive letters in Win fallback GStreamer detection, introduced in 3.0.17.0.
% 20.09.2023  mk  Remove all 32-Bit support on MS-Windows.

% Try-Catch protect the function, so Matlab startup won't fail due to
% errors in this function:
try
    % Setup for MS-Windows:
    if IsWin
        % Need to assign a proper install location for the
        % GStreamer-1.22.0+ MSVC runtime libraries, otherwise
        % use of GStreamer based functions would fail due to
        % unresolved link dependencies:

        % Must omit GStreamer runtime path on Octave in CLI mode, to avoid
        % build trouble due to lib clashes between Octave and GStreamer:
        % useGStreamer = ~IsOctave || ~any(strfind(cell2mat (argv'), '--no-gui'));
        % GStreamer disable for building on octave-cli: No longer needed since Octave 5.2+
        useGStreamer = 1;

        % Find path to SDK-Root folder: Should be defined in environment variable
        % by installer. Use the 64-Bit GStreamer runtime for 64-Bit Matlab/Octave
        % on Windows:
        sdkroot = getenv('GSTREAMER_1_0_ROOT_MSVC_X86_64');
        suffix = 'msvc_x86_64\';

        if isempty(sdkroot)
            fprintf('PsychStartup: Environment variable GSTREAMER_1_0_ROOT_MSVC_X86_64 is undefined.\n');
            fprintf('PsychStartup: Either GStreamer-1.22 MSVC or a later version is not installed at all, or if it is installed, then\n');
            fprintf('PsychStartup: something is botched. Trying various common locations for the GStreamer runtime to keep going.\n');
        else
            if ~exist(sdkroot, 'dir')
                % Env variable points to non-existent SDK dir. How peculiar?
                % Invalidate invalid sdkroot, so fallback code can run:
                fprintf('PsychStartup: Environment variable GSTREAMER_1_0_ROOT_MSVC_X86_64 points to non-existent folder?!?\n');
                fprintf('PsychStartup: The missing or inaccessible path to GStreamer is: %s\n', sdkroot);
                fprintf('PsychStartup: Something is botched. Trying various common locations for the runtime to keep going.\n');
                sdkroot = [];
            end
        end

        % Probe standard install location on drives C,D,E,F,G:
        if isempty(sdkroot) && exist(['C:\gstreamer\1.0\' suffix], 'dir')
            sdkroot = ['C:\gstreamer\1.0\' suffix];
        end

        if isempty(sdkroot) && exist(['D:\gstreamer\1.0\' suffix], 'dir')
            sdkroot = ['D:\gstreamer\1.0\' suffix];
        end

        if isempty(sdkroot) && exist(['E:\gstreamer\1.0\' suffix], 'dir')
            sdkroot = ['E:\gstreamer\1.0\' suffix];
        end

        if isempty(sdkroot) && exist(['F:\gstreamer\1.0\' suffix], 'dir')
            sdkroot = ['F:\gstreamer\1.0\' suffix];
        end

        if isempty(sdkroot) && exist(['G:\gstreamer\1.0\' suffix], 'dir')
            sdkroot = ['G:\gstreamer\1.0\' suffix];
        end

        if isempty(sdkroot) || ~exist(sdkroot, 'dir')
            fprintf('\nPsychStartup: Path to GStreamer runtime is undefined! This probably means that\n');
            fprintf('PsychStartup: the 64-Bit GStreamer 1.22+ MSVC runtime is not installed.\n');
            fprintf('PsychStartup: The Psychtoolbox Screen() function will not work at all until you fix\n');
            fprintf('PsychStartup: this! Read ''help GStreamer'' for instructions.\n\n');
            sdkroot = [];
        else
            sdkroot = [sdkroot 'bin'];
        end

        % Get current path:
        path = getenv('PATH');

        if useGStreamer && ~isempty(sdkroot)
            % Matlab, or Octave in GUI mode: Prepend sdkroot to path:
            newpath = [sdkroot ';' path];

            % Check if we have the right flavor of GStreamer, the MSVC variant:
            if ~isdeployed
                addpath(sdkroot);
                if exist('libgstreamer-1.0-0.dll', 'file') && ~exist('gstreamer-1.0-0.dll', 'file')
                    % Wrong type, the MinGW build instead of the MSVC build!
                    fprintf('\n\n');
                    fprintf('PsychStartup: WRONG variant of the GStreamer runtimes installed! This is\n');
                    fprintf('PsychStartup: the MinGW variant, but Psychtoolbox needs the MSVC variant!\n');
                    fprintf('PsychStartup: The Psychtoolbox Screen() function will NOT WORK AT ALL until\n');
                    fprintf('PsychStartup: you fix this! Read ''help GStreamer'' for instructions.\n\n');
                    warning('WRONG TYPE OF GStreamer packages INSTALLED! WE NEED THE MSVC variant, but this is the MINGW variant!');
                end
                rmpath(sdkroot);
            end

            % Also store sdkroot in a separate environment variable, to be used
            % by Screen for Octave internally:
            setenv('PSYCH_GSTREAMER_SDK_ROOT', sdkroot);

            fprintf('\nPsychStartup: Adding path of installed GStreamer runtime to library path. [%s]\n', sdkroot);
        else
            newpath = path;
            fprintf('\nPsychStartup: Use of GStreamer disabled or impossible.');
            fprintf('\nPsychStartup: Only leightweight GStreamer-less Screen() mex file will work.\n');
        end

        if IsOctave
            % For Octave-5.1 and later on Windows, we also need to prepend path to the
            % possible portaudio dll locations to the library path, otherwise PsychPortAudio
            % won't load, as Octave does not allow delay-loading like Matlab. Set path in
            % preference order, so a possible override dll in PsychtoolboxRoot gets priority:
            driverloadpath = [PsychtoolboxRoot ';' PsychtoolboxRoot 'PsychSound'];
            newpath = [driverloadpath ';' newpath];
        end

        % For moglcore we need to prepend the path to the 64-Bit freeglut.dll:
        driverloadpath = [PsychtoolboxRoot 'PsychOpenGL/MOGL/core/x64'];
        newpath = [driverloadpath ';' newpath];

        % For PsychHID we need to prepend the path to the 64-Bit libusb-1.0.dll:
        driverloadpath = [PsychtoolboxRoot 'PsychContributed' filesep 'x64' filesep];
        newpath = [driverloadpath ';' newpath];

        setenv('PATH', newpath);
    end
catch %#ok<*CTCH>
    fprintf('PTB-WARNING: Call to PsychStartup() failed!! Error messages:\n');
    ple;
end

end
