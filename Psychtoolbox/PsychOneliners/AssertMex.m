function callStack = AssertMex(varargin)

% AssertMex([platform1,] [platform2,] ...)
%
% OSX and OS9: _____________________________________________________________
%
% AssertMex detects missing mex files.  Calling AssertMex from
% your help file, e.g. foo.m, asserts the existence of a mex file foo.mex.
% If no such mex file exists then AssertMex will exit with an error.
%
% You may specify on which platforms your help file expects a corresponding
% mex file by using optional "platform" arguments. If no arguments are
% supplied then AssertMex will issue an error on any platform on which it
% cannot find a mex file.  If any platform names are supplied then
% AssertMex will issue errors only if MATLAB fails to find the mex file on
% the named platforms.
%
% Assert Mex is useful for detecting the error condition when a .m help
% file is mistakenly executed because the correspondig .mex file is
% missing. When foo.mex is missing, MATLAB silently executes the help file
% foo.m instead.  Calling AssertMex within foo.m detects and reports that
% error. 
% 
% AssertMex accepts platform names as returned by Matlab's COMPUTER
% command:
% 'PCWIN', 'SOL2', 'HPUX', 'HP700', 'ALPHA', 'IBM_RS', 'SGI', 'LNX86',
% 'MAC', 'MAC2'
%
% NOTE: Counterintuitively, "MAC" refers to OS X and "MAC2" refers to its 
% predecessor, OS9.
%
% AssertMex also accepts these synonyms in place of MATLAB's names:
%  'OSX', 'OS9', 'Win' and 'Windows'
%
% WIN: ________________________________________________________________
% 
% AssertMex does not yet exist in Windows.
% 
% _________________________________________________________________________
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

persistent okNames mexExtensions okSupNames okSupNameMatches;

% Do not do anything on Octave build.
if IsOctave
    myName = char(varargin{1});
    if isempty(findstr(myName, '.m'))
        return;
    end

    octFilename = [ myName(1:end-1) 'oct'];
    fprintf('\nIn place of the expected Octave .oct binary plugin file this placeholder file was executed:\n\n');
    fprintf(['  ' myName '\n\n']);
    fprintf('This OCT file seems to be missing or inaccessible on your Octave path or it is dysfunctional:\n\n')
    fprintf(['  ' octFilename '\n\n']);

    if ~exist(octFilename, 'file')
        fprintf('Hmm. I cannot find the file on your Octave path?!?\n\n');
        fprintf('One reason could be that your Octave path is wrong or not up to date\n');
        fprintf('for the current Psychtoolbox. You may want to run SetupPsychtoolbox to \n');
        fprintf('fix possible path problems.\n');
        fprintf('Make sure that the path is readable by you as well...\n');
        if IsLinux
            fprintf('The following directory should be on your Octave path: %s \n\n', [PsychtoolboxRoot 'PsychBasic/OctaveLinuxFiles/']);
        else
            fprintf('The following directory should be on your Octave path: %s \n\n', [PsychtoolboxRoot 'PsychBasic/OctaveOSXFiles/']);
        end
    else
        % Check for supported Octave version:
        curversion = sscanf(version, '%i.%i.%i');
        if curversion(1)~=2 | curversion(2)~=1 | curversion(3)<73
            fprintf('Your version of Octave (%s) is incompatible with Psychtoolbox: We support Octave 2.1.73 or later,\n', version);
            fprintf('i.e., a version with 2.1.x x>=73 in its name, but *not* Octave 2.9 or later!!\n');
            error('Tried to run Psychtoolbox on an incompatible Octave version.\n');
        end

        fprintf('A reason could be some missing 3rd party dynamic link shared libraries on your system.\n');
        fprintf('Our default installation also only supports 32 bit versions of operating system and Octave.\n');
        fprintf('Another reason could be some binary incompatibility. You would need to recompile Psychtoolbox from source!\n\n');
    end
    error('Missing, inaccessible or dysfunctional Psychtoolbox Oct file for this system. Read the help text above carefully!!\n');
end;

% Initialize the persistent variables.
if isempty(okNames)
    okNames = {'PCWIN', 'PCWIN64', 'SOL2', 'HPUX', 'HP700', 'ALPHA', 'IBM_RS', 'SGI', 'LNX86', 'MAC',    'MAC2', 'MACI', 'i486-pc-linux-gnu'};
    mexExtensions = {'dll', 'dll',  '*',    '*',    '*',     '*',     '*',      '*',   '*',     'mexmac', 'mex', 'mexmaci', 'oct'};
    okSupNames = {'WINDOWS', 'WIN', 'OS9', 'OSX'};
    okSupNameMatches = {'PCWIN', 'PCWIN', 'MAC2', 'MAC'};
end

% This code-chunk disabled: Doesn't make sense anymore for current PTB-3:
% Code left for documentation...

% Replace any non-standard platform names in the argument list with their
% official equivalents.  Our non-standard names
% match our shorthand platform tests: IsOS9, IsWin, and IsOSX.

% inputNames = upper(varargin);
% 
% for i = 1:length(okSupNames)
%     foundices=find(streq(okSupNames{i},upper(inputNames)));
%     if foundices
%         inputNames{foundices}=okSupNameMatches{i};
%     end
% end
% 
% % Check for invalid arguments.
% badNames = setdiff(inputNames, okNames); 
% if ~(isempty(badNames) | streq(badNames,'')) % badNames was often {''} which is not empty for isempty()
%     nameList=[];
%     for i = 1:length(badNames)
%         nameList = [nameList ' ' badNames{i}];
%     end
%     error(['Invalid OS names: ' nameList '\n']);
% end

inputNames = [];

% Check to see if there should be a mex file for our platform.
if isempty(inputNames) | ismember(computer, inputNames)
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
        fprintf('A reason could be insufficient access permissions or \n');
        fprintf('some missing 3rd party libraries on your system.\n\n');

        if IsWin & ~IsOctave
            fprintf('On Microsoft Windows with recent Matlab versions (>= V7.4) it could also be that\n');
            fprintf('the required Visual C++ 2005 runtime libraries are missing on your system.\n');
            fprintf('Visit http://www.mathworks.com/support/solutions/data/1-2223MW.html for instructions how to\n');
            fprintf('fix this problem. After fixing the problem, retry.\n\n');

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
