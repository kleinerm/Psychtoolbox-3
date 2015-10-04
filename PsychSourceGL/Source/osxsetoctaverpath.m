function osxsetoctaverpath(mexfname, mexpath)
% osxsetoctaverpath(mexfname [, mexpath])
%
% Change the @rpath library search path for the octave
% runtime libraries inside the given mex file.
%
% e.g.,
%
% osxsetoctaverpath('Screen'); would rewrite Screen.mex
% to use the @rpath settings stored in this function.
%

    if ~IsOSX(1) || ~IsOctave
        error('osxsetoctaverpath only works with a 64-Bit version of Octave for OSX!');
    end

    % Set default path for finding the mex file to process, if omitted:
    if nargin < 2 || isempty(mexpath)
        mexpath = '../Projects/MacOSX/build/';
    end

    % Build full path to file:
    mexfname = [mexpath mexfname '.' mexext];

    % Replace absolute path to liboctinterp.2.dylib with @rpath:
    system(['install_name_tool -change ' octave_config_info.octlibdir '/liboctinterp.2.dylib @rpath/liboctinterp.2.dylib ' mexfname]);

    % Replace absolute path to liboctave.2.dylib with @rpath:
    system(['install_name_tool -change ' octave_config_info.octlibdir '/liboctave.2.dylib @rpath/liboctave.2.dylib ' mexfname]);

    % Replace absolute path to libcruft.2.dylib with @rpath:
    system(['install_name_tool -change ' octave_config_info.octlibdir '/libcruft.2.dylib @rpath/libcruft.2.dylib ' mexfname]);

    % Ok, now all octave library locations are encoded relative to @rpath.
    % Encode a whole bunch of rpath runtime pathes into the mex file, for
    % different versions of octave, installed via different package
    % managers:

    % Start with Octave-Forge stuff, followed by HomeBrew stuff, followed by Fink stuff:
    % This would give current lib path on devel system: octave_config_info.octlibdir
    lpaths = {  ...
                '/usr/local/octave/3.8.2/lib/octave/3.8.2', ...
                '/usr/local/octave/3.8.0/lib/octave/3.8.0', ...
                '/usr/local/Cellar/octave/3.8.2_1/lib/octave/3.8.2', ...
                '/usr/local/Cellar/octave/3.8.2/lib/octave/3.8.2', ...
                '/usr/local/Cellar/octave/3.8.2_2/lib/octave/3.8.2', ...
                '/sw/lib/octave/3.8.2', ...
                '/sw/lib/octave/3.8.1', ...
                '/sw/lib/octave/3.8.0', ...
             };

    % Add all paths in lpaths as potential search paths for the octave
    % library directories, ie., as settings for @rpath:
    for i = 1:length(lpaths)
        system(['install_name_tool -add_rpath ' lpaths{i} ' ' mexfname]);
        fprintf('Added Octave-3 @rpath %s to mex file %s ...\n', lpaths{i}, mexfname);
    end

return;
