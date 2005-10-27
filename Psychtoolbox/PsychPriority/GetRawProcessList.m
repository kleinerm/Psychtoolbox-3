function rawProcessList=GetRawProcessList
% processListString=GetRawProcessList
%
% OS X: ___________________________________________________________________
%
% Return in a single string the process list as retuned by the BSD ps
% command.  Specifically: ps -caxwl
%
% OS 9: ___________________________________________________________________
%
% GetRawProcessList does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% GetRawProcessList does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: GetProcessDescriptorFromCommandName, GetProcessList

% HISTORY
% 12/5/03   awi     Wrote it.
% 7/15/04   awi     Improved documentation, added AssertOSX.  AssertUnix
%                   would be better, but we can worry about that in the event
%                   of Linux.

AssertOSX;
rawProcessList=evalc('! ps -caxwl');

