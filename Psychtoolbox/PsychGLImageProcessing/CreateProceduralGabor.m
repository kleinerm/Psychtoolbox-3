function [gaborid, gaborrect] = CreateProceduralGabor(windowPtr, width, height, nonSymmetric, backgroundColorOffset)
% [gaborid, gaborrect] = CreateProceduralGabor(windowPtr, width, height [, nonSymmetric=0][, backgroundColorOffset =(0,0,0,0)])
%
% Creates a procedural texture that allows to draw Gabor stimulus patches
% in a very fast and efficient manner on modern graphics hardware.
%
% 'windowPtr' A handle to the onscreen window.
% 'width' x 'height' The maximum size (in pixels) of the gabor. More
% precise, the size of the mathematical support of the gabor. Providing too
% small values here would 'cut off' peripheral parts or your gabor. Too big
% values don't hurt wrt. correctness or accuracy, they just hurt
% performance, ie. drawing speed. Use a reasonable size for your purpose.
%
% 'nonSymmetric' Optional, defaults to zero. A non-zero value means that
% you intend to draw gabors whose gaussian hull is not perfectly circular
% symmetric, but a more general ellipsoid. The generated procedural texture
% will honor an additional 'spatial aspect ratio' parameter, at the expense
% of a higher computational effort and therefore slower drawing speed.
%
% 'backgroundColorOffset' Optional, defaults to [0 0 0 0]. A RGBA offset
% color to add to the final RGBA colors of the drawn gabor, prior to
% drawing it.
%
% The function returns a procedural texture handle 'gaborid' that you can
% pass to the Screen('DrawTexture(s)', windowPtr, gaborid, ...) functions
% like any other texture handle. The 'gaborrect' is a rectangle which
% describes the size of the gabor support.
%
% A typical invocation to draw a gabor patch looks like this:
%
% Screen('DrawTexture', windowPtr, gaborid, [], dstRect, Angle, [], [],
% modulateColor, [], kPsychDontDoRotation, [phase+180, freq, sc,
% contrast]);
%
% Draws the gabor 'gaborid' into window 'windowPtr', at position 'dstRect'
% or in the center if dstRect is set to []. Make sure 'dstRect' has the
% size of 'gaborrect' to avoid spatial distortions! You could do, e.g.,
% dstRect = OffsetRect(gaborrect, xc, yc) to place the gabor centered at
% screen position (xc,yc). 'Angle' is the optional orientation angle,
% default is zero degrees. 'modulateColor' is the base color of the gabor
% patch - it defaults to white, ie. the gabor has only luminance, but no
% color. If you'd set it to [255 0 0] you'd get a reddish gabor. 'phase' is
% the phase of the gabors sine grating, 'freq' is its spatial frequency,
% 'sc' is the spatial constant of the gaussian hull function, 'contrast' is
% the contrast of your gabor.
%
%
% Make sure to use the Screen('DrawTextures', ...); function properly to
% draw many different gabors simultaneously - this is much faster!
%
% 

% History:
% 11/25/2007 Written. (MK)
% 01/03/2008 Enable support for asymmetric gabor shading. (MK)

debuglevel = 1;

% Global GL struct: Will be initialized in the LoadGLSLProgramFromFiles
% below:
global GL;

% Make sure we have support for shaders, abort otherwise:
AssertGLSL;

if nargin < 3 || isempty(windowPtr) || isempty(width) || isempty(height)
    error('You must provide "windowPtr", "width" and "height"!');
end

if nargin < 4 || isempty(nonSymmetric)
    nonSymmetric = 0;
end

if nargin < 5 || isempty(backgroundColorOffset)
    backgroundColorOffset = [0 0 0 0];
else
    if length(backgroundColorOffset) < 4
        error('The "backgroundColorOffset" must be a 4 component RGBA vector [red green blue alpha]!');
    end
end

if ~nonSymmetric
    % Load standard symmetric support shader - Faster!
    gaborShader = LoadGLSLProgramFromFiles('BasicGaborShader', debuglevel);
else
    % Load extended asymmetric support shader - Slower!
    gaborShader = LoadGLSLProgramFromFiles('NonSymetricGaborShader', debuglevel);
end

% Setup shader:
glUseProgram(gaborShader);
% Set the 'Center' parameter to the center position of the gabor image
% patch [tw/2, th/2]:
glUniform2f(glGetUniformLocation(gaborShader, 'Center'), width/2, height/2);
glUniform4f(glGetUniformLocation(gaborShader, 'Offset'), backgroundColorOffset(1),backgroundColorOffset(2),backgroundColorOffset(3),backgroundColorOffset(4));
glUseProgram(0);

% Create a purely virtual procedural texture 'gaborid' of size width x height virtual pixels.
% Attach the GaborShader to it to define its appearance:
gaborid = Screen('SetOpenGLTexture', windowPtr, [], 0, GL.TEXTURE_RECTANGLE_EXT, width, height, 1, gaborShader);

% Query and return its bounding rectangle:
gaborrect = Screen('Rect', gaborid);

% Ready!
return;
