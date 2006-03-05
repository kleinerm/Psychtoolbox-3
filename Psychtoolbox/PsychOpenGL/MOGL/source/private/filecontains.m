function b = filecontains( fname, regexp, wordflag )

% FILECONTAINS  See whether a file contains an expression
% 
% b = filecontains( fname, regexp, wordflag )

% 23-Dec-2005 -- created (RFM)

if nargin<3,
    wordflag=1;
end

if wordflag,
    b=(unix(sprintf('grep -qsw -e %s %s',regexp,fname))==0);
else
    b=(unix(sprintf('grep -qs  -e %s %s',regexp,fname))==0);
end

return
