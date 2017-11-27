function [gratingid, gratingrect] = CreateProceduralSineSmoothedGrating(windowPtr, width, height, backgroundColorOffset, radius, contrastPreMultiplicator, sigma, useAlpha, method)
% [gratingid, gratingrect] = CreateProceduralSineGrating(windowPtr, width, height [, backgroundColorOffset =(0,0,0,0)] [, radius=inf][, contrastPreMultiplicator=1])
%
% Creates a procedural texture that allows to draw sine grating stimulus patches
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
% 'sigma' smoothing SD in pixels
%
% 'useAlpha' whether to use colour (0) or alpha (1) for smoothing channel
%
% 'method' whether to use cosine (0) or smoothstep(1) functions
%
% The function returns a procedural texture handle 'gratingid' that you can
% pass to the Screen('DrawTexture(s)', windowPtr, gratingid, ...) functions
% like any other texture handle. The 'gratingrect' is a rectangle which
% describes the size of the support.
%
% A typical invocation to draw a grating patch looks like this:
%
% Screen('DrawTexture', windowPtr, gratingid, [], dstRect, Angle, [], [],
% modulateColor, [], [], [phase+180, freq, contrast, 0]);
%
% Draws the grating 'gratingid' into window 'windowPtr', at position 'dstRect'
% or in the center if dstRect is set to []. Make sure 'dstRect' has the
% size of 'gratingrect' to avoid spatial distortions! You could do, e.g.,
% dstRect = OffsetRect(gratingrect, xc, yc) to place the grating centered at
% screen position (xc,yc). 'Angle' is the optional orientation angle,
% default is zero degrees. 'modulateColor' is the base color of the grating
% patch - it defaults to white, ie. the grating has only luminance, but no
% color. If you'd set it to [255 0 0] you'd get a reddish grating. 'phase' is
% the phase of the grating in degrees, 'freq' is its spatial frequency in
% cycles per pixel, 'contrast' is the contrast of your grating.
%
% For a zero degrees grating:
% g(x,y) = modulatecolor * contrast * contrastPreMultiplicator * sin(x*2*pi*freq + phase) + Offset.
%
% Make sure to use the Screen('DrawTextures', ...); function properly to
% draw many different gratings simultaneously - this is much faster!
%

% History:
% 06/06/2011 Modified from PTB function (iandol).

debuglevel = 1;

% Global GL struct: Will be initialized in the LoadGLSLProgramFromFiles
% below:
global GL;

% Make sure we have support for shaders, abort otherwise:
AssertGLSL;

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

if nargin < 7 || isempty(sigma)
	sigma = 0.0;
end

if nargin < 8 || isempty(useAlpha)
	useAlpha = 0.0;
end

if nargin < 9 || isempty(method)
	method = 0.0;
end

if isinf(radius)
	% Load standard grating shader:
	gratingShader = LoadGLSLProgramFromFiles('BasicSineGratingShader', debuglevel);
else
	% Load grating shader with circular aperture and smoothing support:
	gratingShader = LoadGLSLProgramFromFiles('SmoothedAperture', debuglevel);
end

% Setup shader:
glUseProgram(gratingShader);

% Set the 'Center' parameter to the center position of the gabor image
% patch [tw/2, th/2]:
glUniform2f(glGetUniformLocation(gratingShader, 'Center'), width/2, height/2);
glUniform4f(glGetUniformLocation(gratingShader, 'Offset'), backgroundColorOffset(1),backgroundColorOffset(2),backgroundColorOffset(3),backgroundColorOffset(4));

if ~isinf(radius)
	% Set radius of circular aperture:
	glUniform1f(glGetUniformLocation(gratingShader, 'Radius'), radius);
	% Apply sigma:
	glUniform1f(glGetUniformLocation(gratingShader, 'Sigma'), sigma);
	% Apply useAlpha:
	glUniform1f(glGetUniformLocation(gratingShader, 'useAlpha'), useAlpha);
	% Apply method:
	glUniform1f(glGetUniformLocation(gratingShader, 'Method'), method);
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
