function glmGetConst

% glmGetConst  Load OpenGL constants as globally defined variables
% 
% usage:  glmGetConst

% 09-Dec-2005 -- created (RFM)

% ---protected---

% load constants as fields of a struct, e.g., GL.COLOR_BUFFER_BIT
evalin('caller','global AGL GL GLU');
evalin('caller','load oglconst.mat AGL GL GLU');

return
