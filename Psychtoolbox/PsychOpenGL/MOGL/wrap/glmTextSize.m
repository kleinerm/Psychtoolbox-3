function xy = glmTextSize( string )

% glmTextSize  Calculate the width and height of a string
% 
% usage:  xy = glmTextSize( string )

% 08-Jan-2006 -- created (RFM)

% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

xy=moglcore('glmTextSize',string);

return
