function [gratingid, gratingrect] = CreateProceduralSquareWaveGrating(windowPtr, width, height, backgroundColorOffset, radius, contrastPreMultiplicator)
% [gratingid, gratingrect] = CreateProceduralSquareWaveGrating(windowPtr, width, height [, backgroundColorOffset =(0,0,0,0)] [, radius=inf][, contrastPreMultiplicator=1])
%
% Creates a procedural texture that allows to draw square wave grating stimulus patches
% in a very fast and efficient manner on modern graphics hardware.
%
% 'windowPtr' A handle to the onscreen window.
% 'width' x 'height' The maximum size (in pixels) of the grating. More
% precise, the size of the mathematical support of the grating. Providing too
% small values here would 'cut off' peripheral parts or your grating. Too big
% values don't hurt wrt. correctness or accuracy, they just hurt
% performance, ie. drawing speed. Use a reasonable size for your purpose.
%
% 'backgroundColorOffset' Optional, defaults to [0 0 0 0]. A RGBA offset
% color to add to the final RGBA colors of the drawn grating, prior to
% drawing it.
%
% 'radius' Optional parameter. If specified, a circular aperture of
% 'radius' pixels is applied to the grating. By default, no aperture is
% applied.
%
% 'contrastPreMultiplicator' Optional, defaults to 1. This value is
% multiplied as a scaling factor to the requested contrast value. If you
% specify contrastPreMultiplicator = 0.5 then the per grating 'contrast'
% value will correspond to what practitioners of the field usually
% understand to be the contrast value of a grating.
%
% The function returns a procedural texture handle 'gratingid' that you can
% pass to the Screen('DrawTexture(s)', windowPtr, gratingid, ...) functions
% like any other texture handle. The 'gratingrect' is a rectangle which
% describes the size of the support.

% History:
% 06/06/2011 Modified from PTB function (Ian Andolina).

debuglevel = 1;

% Global GL struct: Will be initialized in the LoadGLSLProgramFromFiles
% below:
global GL;

if nargin < 3 || isempty(windowPtr) || isempty(width) || isempty(height)
    error('You must provide "windowPtr", "width" and "height"!');
end

if nargin < 4 || isempty(backgroundColorOffset)
    backgroundColorOffset = [0 0 0 0];
else
    if length(backgroundColorOffset) < 4
        error('The "backgroundColorOffset" must be a 4 component RGBA vector [red green blue alpha]!');
    end
end

if nargin < 5 || isempty(radius)
    % Don't apply circular aperture if no radius given:
    radius = inf;
end

if nargin < 6 || isempty(contrastPreMultiplicator)
    contrastPreMultiplicator = 1.0;
end

% Switch to windowPtr OpenGL context:
Screen('GetWindowInfo', windowPtr);

% Make sure we have support for shaders, abort otherwise:
AssertGLSL;

if isinf(radius)
    % Load standard grating shader:
    gratingShader = LoadGLSLProgramFromFiles('SquareWaveShader', debuglevel);
else
    % Load grating shader with circular aperture support:
    gratingShader = LoadGLSLProgramFromFiles('SquareWaveApertureShader', debuglevel);
end

% Setup shader:
glUseProgram(gratingShader);

% Set the 'Center' parameter to the center position of the grating image
% patch [tw/2, th/2]:
glUniform2f(glGetUniformLocation(gratingShader, 'Center'), width/2, height/2);
glUniform4f(glGetUniformLocation(gratingShader, 'Offset'), backgroundColorOffset(1),backgroundColorOffset(2),backgroundColorOffset(3),backgroundColorOffset(4));

if ~isinf(radius)
    % Set radius of circular aperture:
    glUniform1f(glGetUniformLocation(gratingShader, 'Radius'), radius);
end

% Apply contrast premultiplier:
glUniform1f(glGetUniformLocation(gratingShader, 'contrastPreMultiplicator'), contrastPreMultiplicator);

glUseProgram(0);

% Create a purely virtual procedural texture 'gaborid' of size width x height virtual pixels.
% Attach the GaborShader to it to define its appearance:
gratingid = Screen('SetOpenGLTexture', windowPtr, [], 0, GL.TEXTURE_RECTANGLE_EXT, width, height, 1, gratingShader);

% Query and return its bounding rectangle:
gratingrect = Screen('Rect', gratingid);

% Ready!
return;
