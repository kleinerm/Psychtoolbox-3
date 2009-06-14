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
% See also: AssertOpenGL, COMPUTER, IsOSX, IsOS9, IsWin

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
%					In line 80/81 badNames was often {''} which is not empty for isempty().
%					Also changed || into | in line 90
% 03/08/05  dgp     Add support for 'WIN'.
% 10/06/05	awi     Fixed bug: Changed "okSupNameMatches"  to match addition of  'WIN' to 
%								to "okSupNames"
% 3/3/06    awi     Rewrote help for improved clarity.
% 6/13/09    mk     Update to handle execution failures and missing files
%                   on Octave-3.2 et al. as well.

persistent okNames mexExtensions;

% Different processing on Octave build.
if IsOctave
    myName = char(varargin{1});
    if isempty(findstr(myName, '.m'))
        return;
    end

    octFilename = [ myName(1:end-1) 'mex'];
    fprintf('\nIn place of the expected Octave .mex binary plugin file this placeholder file was executed:\n\n');
    fprintf(['  ' myName '\n\n']);
    fprintf('This MEX file seems to be missing or inaccessible on your Octave path or it is dysfunctional:\n\n')
    fprintf(['  ' octFilename '\n\n']);

    if ~exist(octFilename, 'file')
        fprintf('Hmm. I cannot find the file on your Octave path?!?\n\n');
        fprintf('One reason could be that your Octave path is wrong or not up to date\n');
        fprintf('for the current Psychtoolbox. You may want to run SetupPsychtoolbox to \n');
        fprintf('fix possible path problems.\n');
        fprintf('Make sure that the path is readable by you as well...\n');
        if IsLinux
            fprintf('The following directory should be the *first one* on your Octave path: %s \n\n', [PsychtoolboxRoot 'PsychBasic/Octave3LinuxFiles/']);
        else
            fprintf('The following directory should be the *first one* on your Octave path: %s \n\n', [PsychtoolboxRoot 'PsychBasic/Octave3OSXFiles/']);
        end
        fprintf('\n\nIf you have just installed Psychtoolbox, it worked properly and suddenly stopped working\n');
        fprintf('after you have restarted Octave, then you may encounter a bug present in Octave 3.2.0.\n');
        fprintf('Try the following at the Octave prompt: First type "savepath" + Enter to save the current Octave path again.\n');
        fprintf('Then exit Octave and restart it. Now try again - It may work now, if the problem was caused by aforementioned bug.\n\n');
    else
        % Check for supported Octave version:
        curversion = sscanf(version, '%i.%i.%i');
        if curversion(1) < 3 | curversion(2) < 2 %#ok<OR2>
            fprintf('Your version of Octave (%s) is incompatible with Psychtoolbox: We support Octave 3.2.0 or later.\n', version);
            error('Tried to run Psychtoolbox on an incompatible Octave version.\n');
        end

        fprintf('A reason could be some missing 3rd party dynamic link shared libraries on your system.\n');
        fprintf('Our default installation also only supports 32 bit versions of operating system and Octave.\n');
        fprintf('Another reason could be some binary incompatibility. You would need to recompile Psychtoolbox from source!\n\n');
    end
    error('Missing, inaccessible or dysfunctional Psychtoolbox Oct file for this system. Read the help text above carefully!!\n');
end

% Initialize the persistent variables.
if isempty(okNames)
    okNames = {'PCWIN', 'PCWIN64', 'SOL2', 'HPUX', 'HP700', 'ALPHA', 'IBM_RS', 'SGI', 'LNX86', 'MAC',    'MAC2', 'MACI', 'i486-pc-linux-gnu'};
    mexExtensions = {'dll', 'dll',  '*',    '*',    '*',     '*',     '*',      '*',   '*',     'mexmac', 'mex', 'mexmaci', 'oct'};
end

inputNames = [];

% Check to see if there should be a mex file for our platform.
if isempty(inputNames) | ismember(computer, inputNames) %#ok<OR2>
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
    extensionName=mexExtensions{extensionNameIndex};
    mExtensionIndices=findstr('.m',callerName);
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
        if IsWin
            fprintf('It is important that the folder which contains the Screen.dll file is located *before*\n');
            fprintf('the PsychBasic folder on your Matlab path. On Matlab prior to V7.4, the folder\n');
            fprintf('%sPsychBasic\\MatlabWindowsFilesR11\\ must be before the folder\n%sPsychBasic\\ \n\n', PsychtoolboxRoot, PsychtoolboxRoot);
            fprintf('On Matlab V7.4 (R2007a) or later versions, the folder\n');
            fprintf('%sPsychBasic\\MatlabWindowsFilesR2007a\\ must be before the folder\n%sPsychBasic\\ \n\n', PsychtoolboxRoot, PsychtoolboxRoot);
            fprintf('type path to display the current path and check for this condition.\nIf the order is wrong, ');
            fprintf('simply cd into your Psychtoolbox root folder\n%s and then run SetupPsychtoolbox again.\n', PsychtoolboxRoot);
            fprintf('That is the simplest way of fixing such path problems - Or to get more diagnostic output.\n\n');
        end

        fprintf('Another reason could be insufficient access permissions or \n');
        fprintf('some missing 3rd party libraries on your system.\n\n');

        if IsWin
            fprintf('On Microsoft Windows with recent Matlab versions (>= V7.4) it could also be that\n');
            fprintf('the required Visual C++ 2005 runtime libraries are missing on your system.\n');
            fprintf('Visit http://www.mathworks.com/support/solutions/data/1-2223MW.html for instructions how to\n');
            fprintf('fix this problem. Make sure you follow the download link to Visual Studio SERVICE PACK 1,\n');
            fprintf('(the latter links), *not* Visual Studio without the SP1.\n\nAfter fixing the problem, retry.\n\n');

            if strcmp(computer,'PCWIN64')
                % 64 bit Matlab running on 64 bit Windows?!? That won't work.
                fprintf('And another possible reason for failure:\n\n');
                fprintf('It seems that you are running a 64-bit version of Matlab on your system.\n');
                fprintf('That won''t work at all! Psychtoolbox currently only supports 32-bit versions\n');
                fprintf('of Matlab.\n');
                fprintf('You can try to exit Matlab and then restart it in 32-bit emulation mode to\n');
                fprintf('make Psychtoolbox work on your 64 bit Windows. You do this by adding the\n');
                fprintf('startup option -win32 to the matlab.exe start command, ie.\n');
                fprintf('matlab.exe -win32\n');
                fprintf('If you do not know how to do this, consult the Matlab help about startup\n');
                fprintf('options for Windows.\n\n');
            end
        end
    end    
    error('Missing or dysfunctional Psychtoolbox Mex file for this operating system. Read the help text above carefully!!');
end
