% Psychtoolbox:PsychMatlabTests.
%
% help Psychtoolbox % For an overview, triple-click me & hit enter.
%
% These scripts document the few bugs in Matlab 5.2.1. We've reported
% all of them to Mathworks (the Case numbers are included), but
% Mathworks won't be further developing Matlab for Mac OS 9, so no fixes
% are expected. Note that Matlab 5.2.1 is the best-ever release of 
% Matlab for Mac OS 8 and 9.
% 
% ConcatenationBug       - Square and curly braces sometimes can't embrace multiple lines. 84454
% fprintfBug             - FPRINTF may interpret the first argument as a filename. 201530
% fprintfEmpty           - FPRINTF terminates when it gets an empty matrix []. 297266
% fprintfLeak            - FPRINTF leaks memory (i.e. uses up the Temp memory heap). 319124
% helpLinelength         - HELP terminates if a line exceeds 131 characters. 41758
% iminfoBuglet           - IMINFO gives misleading error message. 187040
% SpuriousDebuggerError  - The debugger works, but gives a spurious error message. 230201
% StaleDirectoryCacheBug - Matlab's directory cache is not updated when you add files to /toolbox/. 230202
% ylabelBug              - YLABEL command (in Matlab plots) bug and work around. 297565

% Copyright (c) 2000 by David Brainard & Denis Pelli
