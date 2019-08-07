function striplibsfrommexfile(filename, testrun, alsorenameit)
% striplibsfrommexfile(filename [, testrun=0][, alsorenameit=1]);
%
% Remove a certain set of dynamic library dependencies from MEX/OCT file
% given via 'filename'. Optional 'testrun' if set to non-zero value will
% perform a dry-run.
%
% The stripped libraries (see source of this file) are library dependencies
% added by default by Octave's build process for OCT/MEX files. They make
% sense for mathematical routines, but are totally redundant for PTB files.
%
% They also do harm, as they are versioned for some odd reason, so trying
% to load the mex files on Linux distributions with different versions of
% those libraries installed will fail.
%
% -> This script loads the binary code, searches for occurence of the
% superfluous library name strings and null's them out to remove the
% dependency. Then it rewrites the stripped mex binary.
%

if nargin < 2 || isempty(testrun)
    testrun = 0;
end

if nargin < 3 || isempty(alsorenameit)
    alsorenameit = 1;
end

if exist(filename, 'file')
    % Read original image:
    [fd, msg] = fopen(filename, 'r+');
    if fd == -1
        error('ERROR! MEX file %s could not be read [%s]! Can''t strip it!\n', filename, msg);
    end
    image = uint8(fread(fd));

    % Strip all following libraries from image:
    image = stripLibrary(image, 'libreadline');
    image = stripLibrary(image, 'libncurses');
    image = stripLibrary(image, 'libfft');
    image = stripLibrary(image, 'liblapack');
    image = stripLibrary(image, 'libblas');
    image = stripLibrary(image, 'libhdf');
    image = stripLibrary(image, 'libgfortran');
    image = stripLibrary(image, 'libz');
    image = stripLibrary(image, 'libcruft');
    image = stripLibrary(image, 'liboctave');

    if alsorenameit
        % Rename the following libraries in image: We rename from .so.2 to .so,
        % etc. to make these mex files built on/against Octave-3/4 compatible with
        % at least Octave-3 and Octave-4. This works because while the
        % liboctinterp's so-name/version has changed, the api/abi of
        % Octave-4's C-Mex interface has not changed. Or so we hope until proven
        % wrong... -- Well, there was an ABI change between v4.2 and v4.4, in that
        % mwSize was redefined from unsigned long (uint32_t) to size_t, so on 64-Bit
        % architectures mwSize is no longer 32 bits, but 64 bits! As we pass values
        % around via mwSize*, this ends badly if one tries to use a mex file built
        % for < 4.4 with a >= 4.4 Octave and vice versa. But other than that, the
        % general principle still holds.
        if str2num(version ()(1)) == 4 && str2num(version ()(3)) == 4
            % Octave 4.4.1 - liboctinterp.so.6:
            image = renameLibrary(image, 'liboctinterp.so.6', 'liboctinterp.so');
        elseif str2num(version ()(1)) == 4 && str2num(version ()(3)) == 2
            % Octave 4.2 - liboctinterp.so.4:
            image = renameLibrary(image, 'liboctinterp.so.4', 'liboctinterp.so');
        elseif str2num(version ()(1)) == 4 && str2num(version ()(3)) == 0
            % Octave 4.0 - liboctinterp.so.3:
            image = renameLibrary(image, 'liboctinterp.so.3', 'liboctinterp.so');
        else
            % Octave 3.8.x, maybe earlier Octave 3.x.y as well - liboctinterp.so.2:
            image = renameLibrary(image, 'liboctinterp.so.2', 'liboctinterp.so');
        end
    end

    % Write stripped image:
    frewind(fd);
    if ~testrun
        fwrite(fd, image);
    end

    if (fclose(fd) ~= 0)
        error('ERROR! MEX file %s could not be closed [%s]! Can''t strip it!\n', filename, ferror(fd));
    end

    fprintf('MEX file %s stripped from superfluous libraries.\n', filename);
else
    fprintf('WARNING! MEX file %s does not exist! Can''t strip it!\n', filename);
end

return

function image = stripLibrary(image, library)

    dodebug = 0;

    % Find first character of library in image:
    pStart = strfind(char(image'), library);
    if ~isempty(pStart)
        % Iterate over all occurences, kill each of them:
        for cStart = pStart
            % Kill occurence at cStart:

            if dodebug
                disp(char(image(cStart:cStart + 20)))
            end

            % Start at the start:
            p = cStart;

            % Strip-Loop: Strip until zero-byte:
            while (image(p) ~= 0)
                % Null-out this character:
                image(p) = 0;
                % Next one...
                p = p + 1;
            end

            if dodebug
                disp(char(image(cStart:cStart + 20)))
            end
        end
    end

    if ~isa(image, 'uint8')
        error('After strip op, image is no longer of uint8 class as required! Stripping failed!!');
    end

return

function image = renameLibrary(image, oldlibrary, newlibrary)

    dodebug = 0;

    if length(newlibrary) > length(oldlibrary)
        error('New replacement library name longer than to be replaced old name! Forbidden.');
    end

    % Find first character of library in image:
    pStart = strfind(char(image'), oldlibrary);
    if ~isempty(pStart)
        % Iterate over all occurences, kill each of them:
        for cStart = pStart
            % Kill occurence at cStart:

            if dodebug
                disp(char(image(cStart:cStart + 20)))
            end

            % Start at the start:
            p = cStart;
            c = 1;

            % Rename-Loop: Replace chars until zero-byte:
            while (image(p) ~= 0)
                if c <= length(newlibrary)
                    % Replace this character:
                    image(p) = newlibrary(c);
                else
                    % 0-out this character:
                    image(p) = 0;
                end

                % Next one...
                p = p + 1;
                c = c + 1;
            end

            if dodebug
                disp(char(image(cStart:cStart + 20)))
            end
        end
    end

    if ~isa(image, 'uint8')
        error('After rename op, image is no longer of uint8 class as required! Renaming failed!!');
    end
return
