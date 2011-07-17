function [shader, varargout] = CreateSinglePassImageProcessingShader(windowPtr, shaderType, varargin) %#ok<STOUT>
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

% History:
% 17.07.2011  mk    Written.
%

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

% Unhandled:
error('Unknown or unsupported "shaderType" provided!');

end
