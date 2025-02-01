function downloadlexactivator(alsosdk, cleanupafter)
% downloadlexactivator([alsosdk=0][, cleanupafter=0])
%
% Download latest Cryptlex LexActivator client SDK and runtime libraries for
% macOS, Windows and Linux. Unzip them, then copy SDK files and runtime libraries
% into proper folders next to or inside Psychtoolbox.
%
% alsosdk = 0 to only install runtime libraries into Psychtoolbox PsychPlugins
%           folders. 1 = Also install header files and import libs to allow to
%           build license managed mex files.
%
% cleanupafter = 0 Don't clean up after setup. 1 = Delete all zip and temp files.
%

% History:
%
% 25-Nov-2024   mk  Written. Starting off for v3.30.3 SDK and runtimes.
% 31-Jan-2025   mk  Upgrade to v3.31.2 SDK and runtimes for macOS system-wide activations.
%                   Switch from Windows/vc16 to vc14, ie. from MSVC 2019 to MSVC 2015, as
%                   Cryptlex dropped support for MSVC2019 for some reason.

    if nargin < 1 || isempty(alsosdk)
        alsosdk = 0;
    end

    if nargin < 2 || isempty(cleanupafter)
        cleanupafter = 0;
    end

    % On Octave, tell rmdir() to not prompt for confirmation for all rmdir()
    % calls from within this function and subfunctions:
    if exist('confirm_recursive_rmdir')
        confirm_recursive_rmdir(0, 'local');
    end

    try
        dogetit('Win', './lextmp/windows/');
        dogetit('Mac', './lextmp/macos/');
        dogetit('Linux', './lextmp/linux/');
    catch
        if cleanupafter
            delete('LexActivator*.zip');
            rmdir('./lextmp', 's');
        end

        rethrow(lasterror);
    end

    if alsosdk
        % Move header include files into place, next to the Psychtoolbox-3 main folder:
        s1 = copyfile('./lextmp/linux/headers/*.h', [PsychtoolboxRoot '../../'], 'f');

        % Move Windows import lib into place, next to the Psychtoolbox-3 main folder:
        s2 = copyfile('./lextmp/windows/libs/vc14/x64/LexActivator.lib', [PsychtoolboxRoot '../../'], 'f');

        if ~s1 || ~s2
            error('Copy of LexActivator SDK files failed!');
        end
    end

    % Move Windows DLL into PsychPlugins/Intel64/ folder:
    s1 = copyfile('./lextmp/windows/libs/vc14/x64/LexActivator.dll', [PsychtoolboxRoot 'PsychBasic/PsychPlugins/Intel64/'], 'f');

    % Move Linux 64-Bit Intel shared library into PsychPlugins/Intel64/ folder:
    s2 = copyfile('./lextmp/linux/libs/gcc/amd64/libLexActivator.so', [PsychtoolboxRoot 'PsychBasic/PsychPlugins/Intel64/'], 'f');

    % Move Linux 64-Bit ARM shared library into PsychPlugins/ARM64/ folder:
    s3 = copyfile('./lextmp/linux/libs/gcc/arm64/libLexActivator.so', [PsychtoolboxRoot 'PsychBasic/PsychPlugins/ARM64/'], 'f');

    % Move macOS 64-Bit dylib into PsychPlugins/ folder:
    s4 = copyfile('./lextmp/macos/libs/clang/universal/libLexActivator.dylib', [PsychtoolboxRoot 'PsychBasic/PsychPlugins/'], 'f');

    if ~s1 || ~s2 || ~s3 || ~s4
        error('Copy of at least one LexActivator runtime library failed!');
    end

    if cleanupafter
        delete('LexActivator*.zip');
        rmdir('./lextmp', 's');
    end

    if IsOctave
        % Rehash the Octave toolbox cache:
        rehash;
    end

    fprintf('LexActivator installation completed successfully.\n');
end

function dogetit(lextype, target)
    % Base download url:
    url = 'https://dl.cryptlex.com/downloads';

    % Version of the SDK and runtime to fetch:
    lexversion = 'v3.31.2';

    % Assemble zip file name:
    lexzipname = ['LexActivator-' lextype '.zip']

    % Download zip file:
    [f, s] = urlwrite([url '/' lexversion '/' lexzipname], lexzipname);
    if ~s
        error('LexActivator download failed for %s to %s', lextype, lexzipname);
    end

    % Unzip to temp folder:
    unzip(lexzipname, target);
end
