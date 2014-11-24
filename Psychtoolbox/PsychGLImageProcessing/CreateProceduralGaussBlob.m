function [blobid, blobrect] = CreateProceduralGaussBlob(windowPtr, width, height, backgroundColorOffset, disableNorm, contrastPreMultiplicator)
% [blobid, blobrect] = CreateProceduralGaussBlob(windowPtr, width, height [, backgroundColorOffset =(0,0,0,0)][, disableNorm=0][, contrastPreMultiplicator=1])
%
% Creates a procedural texture that allows to draw "Gaussian blob" stimulus
% patches in a very fast and efficient manner on modern graphics hardware.
%
% This works on GPU's with support for the OpenGL shading language and
% vertex- and fragment shaders. See ProceduralGarboriumDemo for examples
% on how to use this function.
%
% Parameters and their meaning:
%
% 'windowPtr' A handle to the onscreen window.
% 'width' x 'height' The maximum size (in pixels) of the blob. More
% precise, the size of the mathematical support of the blob. Providing too
% small values here would 'cut off' peripheral parts or your blob. Too big
% values don't hurt wrt. correctness or accuracy, they just hurt
% performance, ie. drawing speed. Use a reasonable size for your purpose.
%
% 'backgroundColorOffset' Optional, defaults to [0 0 0 0]. A RGBA offset
% color to add to the final RGBA colors of the drawn blob, prior to
% drawing it.
%
% 'disableNorm' Optional, defaults to 0. If set to a value of 1, the
% special multiplicative normalization term normf = 1/(sqrt(2*pi) * sc)
% will not be applied to the computed blob. By default (setting 0), it
% will be applied. This term seems to be a reasonable normalization of the
% total amplitude of the blob, but it is not part of the standard
% definition of a blob. Therefore we allow to disable this normalization.
%
% 'contrastPreMultiplicator' Optional, defaults to 1. This value is
% multiplied as a scaling factor to the requested contrast value. If you
% set the 'disableNorm' parameter to 1 to disable the builtin normf
% normalization and then specify contrastPreMultiplicator = 0.5 then the
% per blob 'contrast' value will correspond to what practitioners of the
% field usually understand to be the contrast value of a blob.
%
%
% The function returns a procedural texture handle 'blobid' that you can
% pass to the Screen('DrawTexture(s)', windowPtr, blobid, ...) functions
% like any other texture handle. The 'blobrect' is a rectangle which
% describes the size of the blob support.
%
% A typical invocation to draw a single blob patch may look like this:
%
% Screen('DrawTexture', windowPtr, blobid, [], dstRect, Angle, [], [],
% modulateColor, [], kPsychDontDoRotation, [contrast, sc, aspectratio, 0]);
%
% Draws the blob 'blobid' into window 'windowPtr', at position 'dstRect',
% or in the center if 'dstRect' is set to []. Make sure 'dstRect' has the
% size of 'blobrect' to avoid spatial distortions! You could do, e.g.,
% dstRect = OffsetRect(blobrect, xc, yc) to place the blob centered at
% screen position (xc,yc).
%
% The definition of the blob mostly follows the definition of Wikipedia for a
% gabor patch, just that there isn't a sin() term to define a grating. Instead
% there's assumed to be a constant 1.0, so you only get the shape of the
% exponential function - a gaussian blob.
%
% Wikipedia's definition (better readable): http://en.wikipedia.org/wiki/Gabor_filter
% See http://tech.groups.yahoo.com/group/psychtoolbox/message/9174 for
% Psychtoolbox forum message 9174 with an in-depth discussion of this
% function.
%
% 'Angle' is the optional orientation angle in degrees (0-360), default is zero degrees.
%
% 'modulateColor' is the base color of the blob patch - it defaults to
% white, ie. the blob has only luminance, but no color. If you'd set it to
% [255 0 0] you'd get a reddish blob.
%
% 'sc' is the spatial constant of the gaussian hull function of the blob, ie.
% the "sigma" value in the exponential function.
%
% 'contrast' is the amplitude of your blob in intensity units - A factor
% that is multiplied to the evaluated blob equation before converting the
% value into a color value. 'contrast' may be a bit of a misleading term
% here...
%
% 'aspectratio' Defines the aspect ratio of the hull of the blob.
%
% Make sure to use the Screen('DrawTextures', ...); function properly,
% instead of the Screen('DrawTexture', ...); function, if you want to draw
% many different blobs simultaneously - this is much faster!
%

% History:
% 24-Nov-2014 Written. (MK)

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

if nargin < 5 || isempty(disableNorm)
  disableNorm = 0;
end

if ~isscalar(disableNorm) || ~ismember(disableNorm, [0, 1])
  error('The "disableNorm" flag must be 0 or 1. Invalid flag passed!');
end

if nargin < 6 || isempty(contrastPreMultiplicator)
  contrastPreMultiplicator = 1.0;
end

% Load gaussian blob shader:
blobShader = LoadGLSLProgramFromFiles('BasicGaussBlobShader', debuglevel);

% Setup shader:
glUseProgram(blobShader);

% Set the 'Center' parameter to the center position of the patch [tw/2, th/2]:
glUniform2f(glGetUniformLocation(blobShader, 'Center'), width/2, height/2);
glUniform4f(glGetUniformLocation(blobShader, 'Offset'), backgroundColorOffset(1),backgroundColorOffset(2),backgroundColorOffset(3),backgroundColorOffset(4));

% Assign disable flag for normalization:
glUniform1f(glGetUniformLocation(blobShader, 'disableNorm'), disableNorm);

% Apply contrast premultiplier:
glUniform1f(glGetUniformLocation(blobShader, 'contrastPreMultiplicator'), contrastPreMultiplicator);

% Setup done:
glUseProgram(0);

% Create a purely virtual procedural texture 'blobid' of size width x height virtual pixels.
% Attach the blobShader to it to define its appearance:
blobid = Screen('SetOpenGLTexture', windowPtr, [], 0, GL.TEXTURE_RECTANGLE_EXT, width, height, 1, blobShader);

% Query and return its bounding rectangle:
blobrect = Screen('Rect', blobid);

% Ready!
return;
