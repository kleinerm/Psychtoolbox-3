function glmText( string )

% glmText  Write a string to the screen
% 
% usage:  glmText( string )

% 09-Dec-2005 -- created (RFM)

% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

moglcore('glmText',string);

return
