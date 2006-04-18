function shader = Create2DConvolutionShader(kernel, textarget)
% Create2DConvolutionShader(kernel, textarget)
% 
% Creates a GLSL shader for 2D convolution of textures with
% the convolution kernel 'kernel' and returns a handle 'shader'
% to it. The shader can then be applied to any texture by
% calling glUseProgram(shader) before drawing the texture.
%
% The simplest way to get a kernel is to use the function
% kernel = fspecial(...); fspecial is part of the Matlab image
% processing toolbox.
%
% Limitations: Currently we only support kernels of size 5 x 5.
% This feature is in early beta stage. It may fail on your system!

% History:
% 15.04.2006 written by Mario Kleiner.

global GL;
if isempty(GL)
    InitializeMatlabOpenGL;
end;

if nargin < 1
    error('Create2DConvolutionShader: No kernel provided!');
end;

if nargin < 2 | isempty(textarget)
    % MK: TODO texture target specific setup.
    textarget = 0;
end;

if size(kernel,1)~=5 | size(kernel,2)~=5
    error('Create2DConvolutionShader: Sorry, only 5x5 kernels supported!');
end;

% Clear any OpenGL error state.
glGetError;

% Load, compile and link shader:
shader = LoadGLSLProgramFromFiles('Convolve2DRectTextureShader',1);

% Activate shader:
glUseProgram(shader);

% Compute gauss-weight mask and offset-mask and upload them
% into shaders uniform-arrays:
hw = 5;
shaderkernel = reshape(kernel, size(kernel,1)*size(kernel,2), 1);
shaderweights = glGetUniformLocation( shader, 'KernelValue[0]');
shaderoffsets = glGetUniformLocation( shader, 'Offset[0]');
for i=0:(hw*hw)-1
    glUniform1f(shaderweights + i, shaderkernel(i+1));
end;

i=0;
for dx=-2:+2
    for dy=-2:+2
        glUniform2f(shaderoffsets + i, dx, dy);
        i=i+1;
    end;
end;

err = glGetError;
if err
    fprintf('Error creating convolution shader! The GL returned: ');
    gluErrorString(err);
end;

% Shader should be ready for use.
return;
