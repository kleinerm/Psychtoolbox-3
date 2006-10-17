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
    return;
end;

% Initialize the persistent variables.
if isempty(okNames)
    okNames = {'PCWIN', 'SOL2', 'HPUX', 'HP700', 'ALPHA', 'IBM_RS', 'SGI', 'LNX86', 'MAC',    'MAC2'};
    mexExtensions = {'dll',   '*',    '*',    '*',     '*',     '*',      '*',   '*',     'mexmac', 'mex'};
    okSupNames = {'WINDOWS', 'WIN', 'OS9', 'OSX'};
    okSupNameMatches = {'PCWIN', 'PCWIN', 'MAC2', 'MAC'};
end

% Replace any non-standard platform names in the argument list with their
% official equivalents.  Our non-standard names
% match our shorthand platform tests: IsOS9, IsWin, and IsOSX.

inputNames = upper(varargin);

for i = 1:length(okSupNames)
    foundices=find(streq(okSupNames{i},upper(inputNames)));
    if foundices
        inputNames{foundices}=okSupNameMatches{i};
    end
end

% Check for invalid arguments.
badNames = setdiff(inputNames, okNames); 
if ~(isempty(badNames) | streq(badNames,'')) % badNames was often {''} which is not empty for isempty()
    nameList=[];
    for i = 1:length(badNames)
        nameList = [nameList ' ' badNames{i}];
    end
    error(['Invalid OS names: ' nameList '\n']);
end

% Check to see if there should be a mex file for our platform.
if isempty(inputNames) | ismember(computer, inputNames)
    % Element 1 will always be AssertMex. Element 2 will be the calling
    % function unless it is invoked from the commnand line.
    callStack = dbstack;
    if length(callStack) > 1
        callerName = callStack(2).name;
    else
        error('PsychAssertMex was invoked from the command line.');
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
    fprintf('In place of the expected mex file this file was executed:\n');
    fprintf(['  ' callerName '\n']);
    fprintf('This mex file seems to be missing:\n')
    fprintf(['  ' mexFilename '\n']);
    error('Missing Psychtoolbox mex file for this operating system. Unsupported?');
end
        


    
    



 
