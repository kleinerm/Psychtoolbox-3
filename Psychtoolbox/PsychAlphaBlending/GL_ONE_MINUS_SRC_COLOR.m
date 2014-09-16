function constantString=GL_ONE_MINUS_SRC_COLOR

% constantString=GL_ONE_MINUS_SRC_COLOR
%
% Return the string 'GL_ONE_MINUS_SRC_COLOR', which specifies an alpha
% blending factor to Screen('BlendFunction', ...).  
%
% "Alpha" is a factor which weights RGB values when combining pixels by
% drawing or copying.  Alpha values weight the pixels drawn, the "source
% surface".   Separate alpha values weight the pixels drawn onto, the
% "destination surface".   A given pixel's alpha factor is not necessarily
% the alpha component of that pixel's [rgba] color vector, but may instead
% be the other combined surface's alpha component, a function of either or
% both both alpha compoenents, a constant, or it may derive from RGB values
% of the other combined surface.  The Psychtoolbox command
% Screen('BlendFunction') selects which. It implements the OpenGL function
% "glBlendFunc".
%
% In MATLAB, Screen('BlendFunction') accepts strings named for C constants
% passed to the OpenGL function glBlendFunc(). Enter "Help
% PsychAlphaBlending" in the MATLAB command window for a list of blending
% constants (and other functions related to alpha blending).  
%
% GL_ONE_MINUS_SRC_COLOR may only be used as a destination factor, not a
% source factor.
% 
%
% see also: PsychAlphaBlending, AlphaDemo, AlphaBlendingTest.


constantString='GL_ONE_MINUS_SRC_COLOR';
return;


% HISTORY
%
% 2/9/05  awi   Wrote it

% NOTES
% 
% 2/9/05  awi   These alpha blending factor functions are an excuse
%               for documentation, but I don't see anytying wrong with that.
%               By defining them as functions we get them to appear upon "Help
%               PsychAlphBlendning", which is exaclty the right place to look.
%               In MATLAB, There is a way to get help on a function.  There is no way
%               to get help on a string.     


% All constants:
% 	"GL_ZERO",
% 	"GL_ONE",
% 	"GL_SRC_COLOR",
% 	"GL_ONE_MINUS_SRC_COLOR",
% 	"GL_DST_COLOR",
% 	"GL_ONE_MINUS_DST_COLOR",
% 	"GL_SRC_ALPHA",
% 	"GL_ONE_MINUS_SRC_ALPHA",
% 	"GL_DST_ALPHA",
% 	"GL_ONE_MINUS_DST_ALPHA",
% 	"GL_SRC_ALPHA_SATURATE"
