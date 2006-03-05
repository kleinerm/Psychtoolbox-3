function [ xy, b ] = glmGetMouse

% glmGetMouse  Get mouse cursor position and button state
% 
% usage:  [ xy, b ] = glmGetMouse

% 11-Dec-2005 -- created (RFM)

% ---protected---

[xy,b]=moglcore('glmGetMouse');

return
