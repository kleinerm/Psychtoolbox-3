function osxsetoctaverpath4(mexfname, mexpath)
% osxsetoctaverpath4([mexfname][, mexpath])
%
% Change the @rpath library search path for the octave
% runtime libraries inside the given mex file to make
% them compatible with GNU/Octave version 4.
%
% e.g.,
%
% osxsetoctaverpath4('Screen'); would rewrite Screen.mex
% to use the @rpath settings stored in this function.
%
% osxsetoctaverpath4() would rewrite all mex files in the
% standard folder for mex files aka Octave4OSXFiles64

    if ~IsOSX(1) || ~IsOctave
        error('osxsetoctaverpath4 only works with a 64-Bit version of Octave for OSX!');
    end

    % Set default path for finding the mex file to process, if omitted:
    if nargin < 2 || isempty(mexpath)
        mexpath = '../Projects/MacOSX/build/';
    end

    % If no mex filename given, iterate over 'mexpath' - or the default install
    % location of mex files - and apply the rpath editing to each mex file there:
    if nargin < 1 || isempty(mexfname)
        if nargin < 2 || isempty(mexpath)
            mexpath = [PsychtoolboxRoot 'PsychBasic/Octave4OSXFiles64/'];
        end

        d = dir (mexpath);
        for j = 1:length(d)
            if ~d(j).isdir
                [a, mexfname, extension] = fileparts(d(j).name);
                if ~isempty(strfind(extension, mexext))
                    osxsetoctaverpath4(mexfname, mexpath);
                end
            end
        end
        return;
    end

    % Build full path to file:
    mexfname = [mexpath mexfname '.' mexext];

    % Replace absolute path to liboctinterp.2.dylib with @rpath:
    system(['install_name_tool -change @rpath/liboctinterp.2.dylib @rpath/liboctinterp.3.dylib ' mexfname]);

    % Replace absolute path to liboctave.2.dylib with @rpath:
    system(['install_name_tool -change @rpath/liboctave.2.dylib @rpath/liboctave.3.dylib ' mexfname]);

    % Ok, now all octave library locations are encoded relative to @rpath.
    % Encode a whole bunch of rpath runtime pathes into the mex file, for
    % different versions of octave, installed via different package
    % managers:

    % Delete old rpath's from mex file:
    lpaths = {  '/usr/local/Cellar/octave/4.0.0_1/lib/octave/4.0.0', ...
                '/usr/local/Cellar/octave/4.0.0_5/lib/octave/4.0.0', ...
                '/usr/local/Cellar/octave/4.0.0_6/lib/octave/4.0.0', ...
                '/usr/local/Cellar/octave/4.0.0_7/lib/octave/4.0.0', ...
                '/usr/local/Cellar/octave/4.0.2/lib/octave/4.0.2', ...
                '/usr/local/Cellar/octave/4.0.2_1/lib/octave/4.0.2', ...
                '/sw/lib/octave/4.0.0', ...
                '/usr/local/octave/3.8.2/lib/octave/3.8.2', ...
                '/usr/local/octave/3.8.0/lib/octave/3.8.0', ...
                '/usr/local/Cellar/octave/3.8.2_1/lib/octave/3.8.2', ...
                '/usr/local/Cellar/octave/3.8.2/lib/octave/3.8.2', ...
                '/usr/local/Cellar/octave/3.8.2_2/lib/octave/3.8.2', ...
                '/sw/lib/octave/3.8.2', ...
                '/sw/lib/octave/3.8.1', ...
                '/sw/lib/octave/3.8.0', ...
             };

    for i = 1:length(lpaths)
        system(['install_name_tool -delete_rpath ' lpaths{i} ' ' mexfname]);
        fprintf('Removed Octave-4 @rpath %s from mex file %s ...\n', lpaths{i}, mexfname);
    end

    % Add one single rpath: @loader_path. This is the path to our folder where our
    % mex file is stored. If we place copies of liboctave.3.dylib and liboctinterp.3.dylib
    % there, then the linker will find them. In absence, the linker will also search the
    % users $HOME/lib/ directory as a possible fallback:
    lpaths = { '@loader_path' };

    % Add all paths in lpaths as potential search paths for the octave
    % library directories, ie., as settings for @rpath:
    for i = 1:length(lpaths)
        system(['install_name_tool -add_rpath ' lpaths{i} ' ' mexfname]);
        fprintf('Added Octave-4 @rpath %s to mex file %s ...\n', lpaths{i}, mexfname);
    end

return;
