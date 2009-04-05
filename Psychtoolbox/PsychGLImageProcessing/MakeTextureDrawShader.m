function glsl = MakeTextureDrawShader(windowPtr, shadertype, backgroundColorOffset)
% glsl = MakeTextureDrawShader(windowPtr, shadertype [, backgroundColorOffset = [0,0,0,0]]);
%
% Create a special GLSL shader that can apply some special image processing
% operations that modify the normal drawing of textures via
% Screen('DrawTexture(s)'). Return a handle 'glsl' to the shader.
%
% The created shader can be attached to specific textures via passing the
% returned 'glsl' argument as the optional 'textureShader' argument for the
% Screen('MakeTexture', ....., textureShader); function. It can be also
% applied on a per-draw basis by passing the 'glsl' handle as optional
% 'textureShader' argument to the Screen('DrawTexture(s)', ...); function.
%
%
% Mandatory arguments:
%
% 'windowPtr' A handle to the onscreen window to which the shader should be
% associated.
%
% 'shadertype' A name string that defines the type of shader / image
% processing operation to apply. It can be one of the following:
%
% - 'SeparateAlphaChannel': Alpha values are looked up at the regular
% locations defined via the 'srcRect' parameter in a Screen('DrawTexture')
% call. RGB color values are looked up at srcRect, offset by some (dx,dy)
% offset, as provided by the 'auxParameters' vector [dx, dy, 0, 0].
% Providing non-zero offset values for dx and dy allows to "shift" or
% "scroll" the RGB or Luminance image inside a texture during drawing,
% while keeping the alpha image at a fixed location, solely defined by
% 'srcRect'. This is mostly useful if you want to draw some drifting
% stimulus with a fixed alpha channel mask applied, ie., the color image
% should move, but the alpha mask should stay fixed.
%
% - 'PremultipliedAlphaChannel': Like 'SeparateAlphaChannel', but the alpha
% value is not written to the framebuffer, but premultiplied to the RGB
% color pixel before writeout.
%
% 'backgroundColorOffset' Optional, defaults to [0 0 0 0]. A RGBA offset
% color to add to the final RGBA colors of the drawn texture, prior to
% drawing it.
%

% History:
% 03/31/2009 Written. (MK)

% Global GL struct: Will be initialized in the LoadGLSLProgramFromFiles below:
global GL; %#ok<NUSED>

% Make sure we have support for shaders, abort otherwise:
AssertGLSL;

if nargin < 2
    error('You must provide the "windowPtr" and "shadertype" arguments!');
end

if nargin < 3
    backgroundColorOffset = [];
end

if isempty(backgroundColorOffset)
    backgroundColorOffset = [0 0 0 0];
else
    if length(backgroundColorOffset) < 4
        error('The "backgroundColorOffset" must be a 4 component RGBA vector [red green blue alpha]!');
    end
end

% This dummy-call just makes sure the context of the proper window 'windowPtr' is bound:
Screen('GetWindowInfo', windowPtr);

switch shadertype
    case {'SeparateAlphaChannel'}
        % Load shader for separate lookup of RGB and alpha channels:
        glsl = LoadGLSLProgramFromFiles('SeparateAlphaTextureShader', 1);
        
    case {'PremultipliedAlphaChannel'}
        % Load shader for separate lookup of RGB and alpha channels with premultiplied alpha:
        glsl = LoadGLSLProgramFromFiles('PremultipliedAlphaTextureShader', 1);

    otherwise
        error('Unknown or unsupported "shadertype" string provided!');
end

% Setup shader:
glUseProgram(glsl);

% Set backgroundColorOffset:
glUniform4f(glGetUniformLocation(glsl, 'Offset'), backgroundColorOffset(1),backgroundColorOffset(2),backgroundColorOffset(3),backgroundColorOffset(4));

% Setup done.
glUseProgram(0);

% Ready!
return;
