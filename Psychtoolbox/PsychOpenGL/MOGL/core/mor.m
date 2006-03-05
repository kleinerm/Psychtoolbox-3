function o = mor( varargin )

% MOR  Logical inclusive OR of multiple arguments
% 
% o = mor( arg1, arg2, ... )

% 09-Dec-2005 -- created (RFM)

o=varargin{1};
for i=2:nargin,
    o=bitor(o,varargin{i});
end

return
