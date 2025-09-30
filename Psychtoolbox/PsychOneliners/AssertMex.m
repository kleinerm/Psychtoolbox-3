function callStack = AssertMex(varargin)
% Handle missing or dysfunctional MEX files on Matlab or Octave.
%
% AssertMex(targetname)
%
% AssertMex detects missing mex files.  Calling AssertMex from
% your help file, e.g. foo.m, asserts the existence of a mex file foo.mex.
% If no such mex file exists then AssertMex will exit with an error.
%
% Assert Mex is useful for detecting the error condition when a .m help
% file is mistakenly executed because the correspondig .mex file is
% missing. When foo.mex is missing, MATLAB silently executes the help file
% foo.m instead.  Calling AssertMex within foo.m detects and reports that
% error. E.g., in Screen.m you'll find AssertMex('Screen.m') to handle
% missing or dysfunctional Screen mex files properly.
%  _________________________________________________________________________
%
% See also: AssertOpenGL, COMPUTER, IsOSX, IsWin

% HISTORY:
%
% mm/dd/yy
%
% 12/14/03  awi     Wrote it
% 07/02/04  awi     Improved documentation: Added See also, explained
%                   behavior when no arguments are supplied.
%                   Changed name to AssertMex from PsychAssertMex
% 10/03/04  awi     Modified to accept 'WINDOWS', 'OS9', 'OSX'
% 01/08/05  fwc     fixed little bug, see comments
% 01/11/05  awi     Merged fwc fix into psychtoolbox.org master copy.
% 01/29/05  dgp     Cosmetic.
% 03/07/05  fwc     Fixed bugs due to which failed Assertmex to accept 'OSX', 'OS9' as valid input.
%                   In line 80/81 badNames was often {''} which is not empty for isempty().
%                   Also changed || into | in line 90
% 03/08/05  dgp     Add support for 'WIN'.
% 10/06/05  awi     Fixed bug: Changed "okSupNameMatches"  to match addition of  'WIN' to
%                   to "okSupNames"
% 3/3/06    awi     Rewrote help for improved clarity.
% 6/13/09    mk     Update to handle execution failures and missing files
%                   on Octave-3.2 et al. as well.
% 5/28/12    mk     Update: 64-Bit OSX and Linux supported, but OSX PowerPC
%                   and Matlab < V7.4 (aka R2007a) no longer supported.
% 23-Feb-2019 mk    Make more robust in case of wrong path order.
% 07-Aug-2019 mk    Update for new MSVC 2019 build system on Windows-10 / Win 10 SDK.
% 13-Feb-2023 mk    Update for PTB 3.0.19.0.
% 22-Apr-2023 mk    Update for PTB 3.0.19.2.
% 25-Sep-2025 mk    Update for PTB 3.0.22.2 and Octave 10+ on macOS

persistent okNames mexExtensions;

% Different processing on Octave build.
if IsOctave
    myName = char(varargin{1});
    if isempty(strfind(myName, '.m')) %#ok<*STREMP>
        return;
    end

    octFilename = [ myName(1:end-1) 'mex'];
    fprintf('\nIn place of the expected Octave .mex binary plugin file this placeholder file was executed:\n\n');
    fprintf(['  ' myName '\n\n']);
    fprintf('This MEX file seems to be missing or inaccessible on your Octave path or it is dysfunctional:\n\n')
    fprintf(['  ' octFilename '\n\n']);

    fpath = which(myName(1:end-2));
    if isempty(fpath)
        fprintf('Hmm. I cannot find the file on your Octave path?!?\n\n');
        fprintf('One reason could be that your Octave path is wrong or not up to date\n');
        fprintf('for the current Psychtoolbox. You may want to run SetupPsychtoolbox to \n');
        fprintf('fix possible path problems.\n');
        fprintf('Make sure that the path is readable by you as well...\n');
    else
        % Some file with such a function exists on the Octave path:
        [dum1, dum2, fpathext] = fileparts(fpath); %#ok<ASGLU>
        % Proper extension for the .mex file?
        if isempty(strfind(fpathext, '.mex'))
            % Nope, wrong file bound:
            if IsARM
                oext = 'ARM';
            else
                oext = '';
            end

            if Is64Bit
                oext = [oext '64' filesep];
            else
                oext = [oext filesep];
            end

            if IsLinux
                if ~IsARM
                    % 64-Bit Intel:
                    fprintf('The following directory should be the *first one* on your Octave path:\n %s \n\n', [PsychtoolboxRoot 'PsychBasic/Octave5LinuxFiles' oext]);
                else
                    % 32-Bit ARM:
                    fprintf('The following directory should be the *first one* on your Octave path:\n %s \n\n', [PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles' oext]);
                end
            end

            if IsOSX
                fprintf('The following directory should be the *first one* on your Octave path:\n %s \n\n', [PsychtoolboxRoot 'PsychBasic/Octave10OSXFiles' oext]);
            end

            if IsWindows
                fprintf('The following directory should be the *first one* on your Octave path:\n %s \n\n', [PsychtoolboxRoot 'PsychBasic\Octave8WindowsFiles' oext]);
            end
        else
            % Correct file with correct extension, still load failure:
            % Check for supported Octave version:
            fprintf('Your version of Octave (%s) might be incompatible with Psychtoolbox: We support Octave 5.0.0 or later\n', version);
            fprintf('on Linux, Octave 7.3 on Windows, and Octave 10.2 and later on macOS.\n');

            fprintf('Another reason could be some missing 3rd party dynamic link shared libraries on your system.\n');
            fprintf('Another reason could be some binary incompatibility. You would need to recompile Psychtoolbox from source!\n\n');
        end
    end

    % Our little ad for our troubleshooting services:
    if exist('PsychPaidSupportAndServices', 'file')
        PsychPaidSupportAndServices(2);
    end

    error('Missing, inaccessible or dysfunctional Psychtoolbox Mex file for this system, or Octave path wrong. Read the help text above carefully!!');
end

% Initialize the persistent variables.
if isempty(okNames)
    okNames = {      'PCWIN',  'PCWIN64', 'SOL2', 'HPUX', 'HP700', 'ALPHA', 'IBM_RS', 'SGI', 'GLNX86', 'MAC',    'MAC2', 'MACI',    'i486-pc-linux-gnu', 'MACI64',    'GLNXA64', 'MACA64'};
    mexExtensions = {'mexw32', 'mexw64',  '*',    '*',    '*',     '*',     '*',      '*',   'mexglx', 'mexmac', 'mex',  'mexmaci', 'mex',               'mexmaci64', 'mexa64',  'mexmaca64'};
end

inputNames = [];

% Check to see if there should be a mex file for our platform.
if isempty(inputNames) || ismember(computer, inputNames)
    % Element 1 will always be AssertMex. Element 2 will be the calling
    % function unless it is invoked from the commnand line.
    callStack = dbstack;

    if length(callStack) > 1
        callerName = callStack(2).name;
    else
        error('AssertMex was invoked from the command line.');
    end

    % Generate error strings
    extensionNameIndex=find(streq(computer,okNames));
    extensionName=mexExtensions{extensionNameIndex}; %#ok<FNDSB>
    mExtensionIndices=strfind('.m',callerName);
    if ~isempty(mExtensionIndices)
        termExtension=mExtensionIndices(end);
        mexFilenameHead=callerName(1:termExtension-1);
    else
        mexFilenameHead = callerName;
    end
    mexFilename=[mexFilenameHead '.' extensionName];
    fprintf('\nIn place of the expected mex file this placeholder file was executed:\n\n');
    fprintf(['  ' callerName '\n\n']);
    fprintf('This mex file seems to be missing or inaccessible on your Matlab path or it is dysfunctional:\n\n')
    fprintf(['  ' mexFilename '\n\n']);

    if isempty(which(mexFilename))
        fprintf('Hmm. I cannot find the file on your Matlab path?!?\n\n');
        fprintf('One reason could be that your Matlab path is wrong or not up to date\n');
        fprintf('for the current Psychtoolbox. You may want to run SetupPsychtoolbox to \n');
        fprintf('fix possible path problems.\n\n');
    else
        if ~isequal(which(mexFilename), which(callerName))
            fprintf('Seems %s.m is in the Matlab path before/above %s instead of after/below it!\n', callerName, mexFilename);
            fprintf('This will cause the mex file to be ignored, leading to this error.\n');
            fprintf('Please run SetupPsychtoolbox again to rectify this problem, or use Matlabs pathtool\n');
            fprintf('to shuffle things into the right order. This folder:\n\n');
            fprintf('%s\n', fileparts(which(mexFilename)));
            fprintf('\nneeds to go above/before this folder:\n\n');
            fprintf('%s\n\n\n', fileparts(which([callerName '.m'])));
        else
            fprintf('Another reason could be insufficient access permissions or \n');
            fprintf('some missing 3rd party libraries on your system.\n\n');
        end

        if IsWin
            fprintf('On Microsoft Windows with supported Matlab versions R2014b or later, it could also mean\n');
            fprintf('that the required Visual C++ 2015-2019 runtime libraries are missing on your system.\n');
            fprintf('The Psychtoolbox/PsychContributed/ subfolder contains installer files for them, which\n');
            fprintf('you can execute after quitting Matlab. The name of the file is:\n');
            if IsWin(1)
                fprintf('vcredist_x64_2015-2019.exe - you maybe also additionally need vcredist_x64_2010.exe.\n');
            else
                fprintf('unknown - no longer ships with this version of Psychtoolbox.\n');
            end
        end
    end

    % Our little ad for our troubleshooting services:
    if exist('PsychPaidSupportAndServices', 'file')
        PsychPaidSupportAndServices(2);
    end

    error('Missing or dysfunctional Psychtoolbox Mex file for this operating system. Read the help text above carefully!!');
end
