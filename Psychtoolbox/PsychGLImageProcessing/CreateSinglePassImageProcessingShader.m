function [shader, varargout] = CreateSinglePassImageProcessingShader(windowPtr, shaderType, varargin)
% Create a single-pass image processing shader for direct use with Screen('DrawTexture')
%
% Usage:
%
% [shader, varargout] = CreateSinglePassImageProcessingShader(windowPtr, shaderType, varargin)
%
% Creates a shader for window 'windowPtr' of type 'shaderType' with
% optional, type specific, parameters. Returns a handle 'shader' and
% optional other properties.
%
% The following types are currently supported:
%
% 'BackgroundMaskOut':
% --------------------
%
%  shader = CreateSinglePassImageProcessingShader(windowPtr, 'BackgroundMaskOut', backgroundColor [, tolerance]);
%  - This shader draws a texture, but removes all "backgroundColor" pixels
%  during drawing, effectively masking them out. 'backgroundColor' is a 3
%  component [R, G, B] vector with the RGB color values of the color to be
%  considered a background to remove. All color values around an euclidean
%  distance of 'tolerance' in 3D color space around the backgroundColor are
%  considered background color. The default tolerance is 0.001 units, which
%  for 8 bit colors means a perfect match -- zero tolerance.
%
% 'WeightedColorComponentSum':
% ----------------------------
%
%  shader = CreateSinglePassImageProcessingShader(windowPtr, 'WeightedColorComponentSum');
%  - This shader draws a texture by computing a weighted sum of the color values of all
%  four color components (Red, Green, Blue, Alpha) for each input texel location (x,y),
%  then outputs the resulting scalar value into all four components (Red, Green, Blue, Alpha)
%  at location (x,y). The optional 'modulateColor' parameter of Screen('DrawTexture', ...);
%  is used to define the weights used for the weighted sum:
%
%  For location (x,y) and color channels R,G,B,A, and Rin, Gin, Bin, Ain = input color values
%  from texture, and Rout, Bout, Gout, Aout the drawn output colors:
%
%  sum = Rin(x,y) * modulateColor(1) + Gin(x,y) * modulateColor(2) + Bin(x,y) * modulateColor(3) + Ain(x,y) * modulateColor(4);
%  Rout(x,y) = Gout(x,y) = Bout(x,y) = Aout(x,y) = sum;
%
%  One interesting application is computing linear combinations of up to four
%  "grayscale" images or "alpha masks" by storing the four different grayscale
%  or alpha images into the four color channels of a texture, then drawing that
%  texture, using the four components of 'modulateColor' as linear combination
%  weights to compute a "composite" output image which is mixed from the four
%  input layers, according to their weights. The optional 'colorMaskNew' parameter
%  of Screen('Blendfunction') can be used to select which output color channels
%  should then be actually written to by the result of this linear combination, e.g.,
%  only RGB channels for a grayscale image, or only Alpha channel for a "morphed"
%  alpha mask image.
%  The moglmorpher('morphTexture') function allows more general image morphing with
%  an arbitrary number of input image textures, but as long as no more than four
%  "monochromatic" images are required, this shader is especially efficient for this task.
%
% History:
% 17.07.2011  mk    Written.
% 24.11.2014  mk    Add 'WeightedColorComponentSum' shader support.


if nargin < 1 || isempty(windowPtr)
    error('Required "windowPtr" argument missing!');
end

if nargin < 2 || isempty(shaderType)
    error('Required "shaderType" argument missing!');
end

% Detect texture background by color key and mask it out by discarding such
% texels:
if strcmpi(shaderType, 'BackgroundMaskOut')
    
    if length(varargin) < 1
        error('Required "backgroundColor" argument missing!');
    end
    
    backgroundColor = varargin{1};
    if isempty(backgroundColor) || (length(backgroundColor) < 3)
        error('"backgroundColor" vector must have at least 3 components for RGB!');
    end

    if length(varargin) > 1 && ~isempty(varargin{2})
        tolerance = varargin{2};
    else
        tolerance = 0.001;
    end
    
    % Remap tolerance and backgroundColor to selected colorRange:
    cr = Screen('ColorRange', windowPtr);
    if cr ~= 1
       backgroundColor = backgroundColor / cr;
       tolerance = tolerance / cr;
    end
    
    shader = LoadGLSLProgramFromFiles(which('ColorMaskedTextureBlitShader.frag.txt'), 1);

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, 'Image'), 0);
    glUniform1f(glGetUniformLocation(shader, 'epsilon'), tolerance);
    glUniform3fv(glGetUniformLocation(shader, 'backgroundColor'), 1, backgroundColor);
    glUseProgram(0);
    
    return;
end

if strcmpi(shaderType, 'WeightedColorComponentSum')
    shader = LoadGLSLProgramFromFiles('WeightedSumOfFourChannelsTextureShader', 1);

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, 'Image'), 0);
    glUseProgram(0);

    return;
end

% Unhandled:
error('Unknown or unsupported "shaderType" provided!');

end
