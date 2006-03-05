function s = commalist( varargin )

% COMMALIST  Make a comma-delimited list containing a number of strings
% 
% s = commalist( str1, [ str2, ... ] )

% 19-Dec-2005 -- created (RFM)

if nargin==0,
	s='';
else
    s=varargin{1};
    if nargin>1,
        s=[ s sprintf(', %s',varargin{2:end}) ];
    end
end

return
