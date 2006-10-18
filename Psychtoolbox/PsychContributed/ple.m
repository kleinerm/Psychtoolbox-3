function ple(s)
% PLE "Print Last Error"
%
% ple prints the last error message issued by Matlab, including a complete
% backtrace of the call-sequence of functions that led to the error
% condition. Each line in the backtrace includes M-Filename and line number
% and allows you to open that file on the specified line by simply clicking
% on it with the mouse.
%
% ple is only supported on Matlab version 7 (Release 14 service pack 3) and later.
%
% Usage:
% ple     - Print last error, as contained in error structure 'psychlasterror'.
% ple(s)  - Print error and backtrace contained in error structure 's'.
%
% Copyright: This implementation of 'ple' is a slightly modified derivate of
% the original public domain implementation of ple.m by Malcolm Wood (the MathWorks).
%
% The original file can be downloaded from Matlab Central at:
% http://www.mathworks.com/matlabcentral/fileexchange/loadFile.do?objectId=
% 9525&objectType=file
%
% Thanks to David Fencsik for pointing us to this useful file and Malcolm
% Wood for writing it.
%

% History:
% 10/17/06 Derived from ple.m by MK.

% Error structure s provided?
if nargin<1
    % No. Fetch it from psychlasterror.
    s = psychlasterror;
end

if isempty(s.message)
   fprintf(1,'No error message stored\n');
   return;
end

fprintf(1,'Last Error: %s (%s)\n',s.message,s.identifier);

% Do we have a stack?
if ~isfield(s, 'stack')
    % Nope. This is not Matlab 7 or later. We're done.
    fprintf(1,'Error message does not contain a stack.\n');
    return;
end

% Stack available. Pretty print a nice backtrace.
for i=1:numel(s.stack)
   e = s.stack(i);
   ff = which(e.file);
   [ignore_dir,command] = fileparts(ff);
   n = e.name;
   href = sprintf('matlab:opentoline(''%s'',%d)',ff,e.line);
   if strcmp(command,n)
       % main function in this file
       fprintf(1,'    <a href="%s">%s,%d</a>\n',href,ff,e.line);
   else
       % subfunction in this file
       fprintf(1,'    <a href="%s">%s >%s,%d</a>\n',href,ff,n,e.line);
   end
end
fprintf(1,'\n');
