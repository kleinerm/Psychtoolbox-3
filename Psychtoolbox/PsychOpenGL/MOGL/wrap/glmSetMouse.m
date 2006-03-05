function glmSetMouse( xy, visible )

% glmSetMouse  Set position and visibility of mouse cursor
% 
% usage:  glmSetMouse( xy, visible )

% 11-Dec-2005 -- created (RFM)

% ---protected---

if nargin<1,
    xy=[];
end
if nargin<2,
    visible=[];
end

moglcore('glmSetMouse',xy,visible);

return
