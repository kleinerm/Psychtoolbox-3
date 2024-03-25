function osxsetoctaverpath(mexfname, mexpath)
% osxsetoctaverpath(mexfname [, mexpath])
%
% Change the @rpath library search path for the octave
% runtime libraries inside the given mex file.
%
% We change from absolute path to @rpath.
%
% E.g.,
%
% osxsetoctaverpath('Screen'); would rewrite Screen.mex
% to use the @rpath settings stored in this function.
% We define one rpath as @loader_path, so the runtime dylibs
% are expected in loader_path, e.g., a system library path,
% or the Psychtoolbox folder where the mex files are stored.
% PsychtoolboxPostInstallRoutine copies or symlinks the Octave
% runtime libraries into the mex file folder of PTB, so the mex
% files should always find a dylib for the currently running Octave.

    if ~IsOSX(1) || ~IsOctave || ~compare_versions(version, '9.1.0', '==')
        error('osxsetoctaverpath only works with a 64-Bit version of HomeBrew Octave-9.1.0 for macOS!');
    end

    % If no mex filename given, iterate over 'mexpath' - or the default install
    % location of mex files - and apply the rpath editing to each mex file there:
    if nargin < 1 || isempty(mexfname)
        if nargin < 2 || isempty(mexpath)
            if IsARM
                mexpath = [PsychtoolboxRoot 'PsychBasic/Octave8OSXFilesARM64/'];
            else
                mexpath = [PsychtoolboxRoot 'PsychBasic/Octave8OSXFiles64/'];
            end
        end

        d = dir (mexpath);
        for j = 1:length(d)
            if ~d(j).isdir
                [~, mexfname, extension] = fileparts(d(j).name);
                if ~isempty(strfind(extension, mexext)) %#ok<STREMP>
                    osxsetoctaverpath(mexfname, mexpath);
                end
            end
        end
        return;
    end

    % Set default path for finding the mex file to process, if omitted:
    if nargin < 2 || isempty(mexpath)
        mexpath = '../Projects/MacOSX/build/';
    end

    % Build full path to file:
    mexfname = [mexpath mexfname '.' mexext];

    % This is how the libdir should be defined automatically:
    libdir = GetOctlibDir;

    % This is sadly how we have to do it with Octave on macOS 13-14 due to
    % the latest macOS linker crap - Hardcoding the path for a Octave install
    % from HomeBrew. Yes, this is sad...
    if IsARM
        libdir = '/opt/homebrew/opt/octave/lib/octave/9.1.0';
    else
        libdir = '/usr/local/opt/octave/lib/octave/9.1.0';
    end

    % Replace absolute path to liboctinterp.12.dylib with @rpath:
    system(['install_name_tool -change ' libdir '/liboctinterp.12.dylib @rpath/liboctinterp.dylib ' mexfname]);

    % Replace absolute path to liboctave.11.dylib with @rpath:
    system(['install_name_tool -change ' libdir '/liboctave.11.dylib @rpath/liboctave.dylib ' mexfname]);

    % Add one single rpath: @loader_path. This is the path to our folder where our
    % mex file is stored. If we place symlinks to liboctave.dylib and liboctinterp.dylib
    % there, then the linker will find them. In absence, the linker will also search the
    % users $HOME/lib/ directory as a possible fallback:
    lpaths = { '@loader_path' };

    % Add all paths in lpaths as potential search paths for the octave
    % library directories, ie., as settings for @rpath:
    for i = 1:length(lpaths)
        system(['install_name_tool -add_rpath ' lpaths{i} ' ' mexfname]);
        fprintf('Added Octave @rpath %s to mex file %s ...\n', lpaths{i}, mexfname);
    end

return;
