function [ screensize, fps, bitsperpixel ] = glmGetScreenInfo

% glmGetScreenInfo  Get information about the screen
% 
% usage:  [ screensize, fps, bitsperpixel ] = glmGetScreenInfo

% 09-Dec-2005 -- created (RFM)

% ---protected---

[screensize,fps,bitsperpixel]=moglcore('glmGetScreenInfo');

return
