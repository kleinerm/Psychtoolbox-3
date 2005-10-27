function strArray=BreakLines(str)

% strArray=BreakLines(str)
%
% OS X: ___________________________________________________________________
%
% Accept a string, "str",  and return a cell array of strings
% broken at the line terminators in str
%
% OS 9: ___________________________________________________________________
%
% BreakLines does not yet exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% BreakLines does not yet exist in Windows.
% 
% _________________________________________________________________________
%
% see also: ReplaceLineTerminators
 

% HISTORY
%
% 12/9/03   awi     Wrote it.
% 7/12/04   awi     Added platform sections.

%first substitue in the unix break char no matter what we start out with.
%this makes BreakLines platform independent 
unixBreakChar=char(10);
unixStr=ReplaceLineTerminators(str, 'unix');

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




