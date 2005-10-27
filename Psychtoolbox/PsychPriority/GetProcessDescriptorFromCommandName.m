function processDescriptor=GetProcessDescriptorFromCommandName(commandName)
% processDescriptor=GetProcessDescriptorFromCommandName(commandName)
%
% OS X: ___________________________________________________________________
%
% Accept the name of a process and return a structure with fields
% describing that process.  GetProcessDescriptorFromCommandName relies on
% the Unix command "ps" to get information about processes.
%
% Try:
%
%   GetProcessDescriptorFromCommandName('MATLAB');
%
% OS 9: ___________________________________________________________________
%
% GetProcessDescriptorFromCommandName does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% GetProcessDescriptorFromCommandName does not exist in Windows.
% 
% _________________________________________________________________________
%
% SEE ALSO: GetProcessList, GetRawProcessList

% HISTORY
% 12/08/03  awi   Wrote it.
% 7/15/04    awi   Improved documentation.
% 2/17/05		dgp  Changed strcmp to streq.
% 10/10/05  awi   Cosmetic.  Noted change by dgp on 2/17/05.



processList=GetProcessList;
matchIndices=find(streq({processList(:).command}, commandName));
if length(matchIndices) == 0
    processDescriptor=[];
else
    processDescriptor=processList(matchIndices);
end


