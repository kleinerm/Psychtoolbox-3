function r = sed( str, varargin )

% SED  Pass a string through one or more sed filters
% 
% r = sed( str, filter1, [ filter2, ... ] )

% 22-Dec-2005 -- created (RFM)

r=str;
for i=1:(nargin-1),
    [err,r]=unix([ 'echo ''' r ''' | sed -E ''' varargin{i} '''' ]);
    r=r(find(r~=10));
    if err,
        error([ 'invalid filter ''' varargin{i} '''' ]);
    end
end

return
