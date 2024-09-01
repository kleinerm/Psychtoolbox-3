function macossetmatlabrpath(mexfname, mexpath)
% macossetmatlabrpath(mexfname [, mexpath])
%
% Change the @rpath library search path for some helper libraries inside the
% given Matlab mex file, so they can be found by the dynamic linker inside the
% Psychtoolbox/PsychBasic/PsychPlugins. We change from absolute path to @rpath.
%
% E.g.,
%
% macossetmatlabrpath('Screen'); would rewrite Screen.mexmaci64 on Intel Matlab
% to use the @rpath settings stored in this function. We define one candidate
% rpath as @loader_path/PsychPlugins, so the helper dylibs are expected there,
% e.g., a system library path, or the PsychPlugins folder.

    if ~IsOSX(1) || IsOctave
        error('macossetmatlabrpath only works with 64-Bit Matlab for macOS!');
    end

    % If no mex filename given, iterate over 'mexpath' - or the default install
    % location of mex files - and apply the rpath editing to each mex file there:
    if nargin < 1 || isempty(mexfname)
        if nargin < 2 || isempty(mexpath)
            mexpath = [PsychtoolboxRoot 'PsychBasic/'];
        end

        d = dir (mexpath);
        for j = 1:length(d)
            if ~d(j).isdir
                [~, mexfname, extension] = fileparts(d(j).name);
                if ~isempty(strfind(extension, mexext)) %#ok<STREMP>
                    macossetmatlabrpath(mexfname, mexpath);
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

    % We add the @loader_path/PsychPlugins path to our PsychBasic/PsychPlugins
    % folder, so our mex files can find additional helper dylibs there, which
    % are needed by some of them:
    lpaths = { '@loader_path/PsychPlugins' };

    % Add all paths in lpaths as potential search paths for @rpath:
    for i = 1:length(lpaths)
        system(['install_name_tool -add_rpath ' lpaths{i} ' ' mexfname]);
        fprintf('Added Matlab @rpath %s to mex file %s ...\n', lpaths{i}, mexfname);
    end

return;

