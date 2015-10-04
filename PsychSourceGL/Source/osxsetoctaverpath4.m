function osxsetoctaverpath4(mexfname, mexpath)
% osxsetoctaverpath4(mexfname [, mexpath])
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

    if ~IsOSX(1) || ~IsOctave
        error('osxsetoctaverpath4 only works with a 64-Bit version of Octave for OSX!');
    end

    % Set default path for finding the mex file to process, if omitted:
    if nargin < 2 || isempty(mexpath)
        mexpath = '../Projects/MacOSX/build/';
    end

    % Build full path to file:
    mexfname = [mexpath mexfname '.' mexext];

    % Replace absolute path to liboctinterp.2.dylib with @rpath:
    system(['install_name_tool -change @rpath/liboctinterp.2.dylib @rpath/liboctinterp.3.dylib ' mexfname]);

    % Replace absolute path to liboctave.2.dylib with @rpath:
    system(['install_name_tool -change @rpath/liboctave.2.dylib @rpath/liboctave.3.dylib ' mexfname]);

    % Replace absolute path to libcruft.2.dylib with @rpath:
    system(['install_name_tool -change @rpath/libcruft.2.dylib @rpath/libcruft.3.dylib ' mexfname]);

    % Ok, now all octave library locations are encoded relative to @rpath.
    % Encode a whole bunch of rpath runtime pathes into the mex file, for
    % different versions of octave, installed via different package
    % managers:

    % Start with HomeBrew stuff, followed by Fink stuff:
    % This would give current lib path on devel system: octave_config_info.octlibdir
    lpaths = {  '/usr/local/Cellar/octave/4.0.0/lib/octave/4.0.0', ...
                '/usr/local/Cellar/octave/4.0.0_1/lib/octave/4.0.0', ...
                '/sw/lib/octave/4.0.0', ...
             };

    % Add all paths in lpaths as potential search paths for the octave
    % library directories, ie., as settings for @rpath:
    for i = 1:length(lpaths)
        system(['install_name_tool -add_rpath ' lpaths{i} ' ' mexfname]);
        fprintf('Added Octave-4 @rpath %s to mex file %s ...\n', lpaths{i}, mexfname);
    end

return;
