function strArray=BreakLines(str)

% strArray=BreakLines(str)
%
% Accept a string, "str",  and return a cell array of strings
% broken at the line terminators in str. The terminators used in the string
% are automatically detected (see platforms list in ReplaceLineTerminators
% for supported line terminators).
%
%
% see also: ReplaceLineTerminators
 

% HISTORY
%
% 12/09/03  awi     Wrote it.
% 07/12/04  awi     Added platform sections.
% 21/10/11  dcn     Actually this function works for all platforms. Edited
%                   help and comments in code.

%first substitute in the unix break char no matter what we start out with.
%this makes BreakLines platform independent
unixStr=ReplaceLineTerminators(str, 'unix');
unixBreakChar=char(10);

%find indices of line bounds
breakIndices=find(unixStr==unixBreakChar);
lineStartIndices=[1 breakIndices];
lineEndIndices=[breakIndices length(unixStr)];

% build cell array of strings by gathering between the breakpoints.
strArray={};
for i=1:length(lineStartIndices)
    tempLine=unixStr(lineStartIndices(i):lineEndIndices(i)); %divide between line breaks
    strArray{i}=strrep(tempLine, unixBreakChar, '');         %remove line break characters
end