function shader = EXPCreateStatic2DConvolutionShader(kernel, textarget, debug)
% EXPCreateStatic2DConvolutionShader(kernel [, textarget][, debug])
% 
% Creates a GLSL fragment shader for 2D convolution of textures with
% the 2D convolution kernel 'kernel' and returns a handle 'shader'
% to it. The shader can then be applied to any texture by
% calling glUseProgram(shader) before drawing the texture.
%
% The simplest way to get a kernel is to use the function
% kernel = fspecial(...); fspecial is part of the Matlab image
% processing toolbox.
%
% CAUTION:
%
% This feature is in early alpha stage. It may fail on your system and its
% future implementation may change significantly! Don't trust its results
% without validation against a known good reference implementation!

% History:
% 15.04.2006 written by Mario Kleiner.

global GL;
persistent initialized;

if nargin < 1
    error('CreateStatic2DConvolutionShader: No kernel provided!');
end;

if isempty(kernel)
    error('CreateStatic2DConvolutionShader: No kernel provided!');
end

% MK: TODO texture target specific setup.
textarget = 0;

if nargin < 3
    debug = 0;
end;

% Query size of kernel:
kernelw = size(kernel,1);
kernelh = size(kernel,2);

% We only want odd sized kernels of at least 3x3, e.g., 3x3, 5x5, 7x7, 9x9,
% ...
if kernelw < 3 | kernelh < 3 | mod(kernelw,2)~=1 | mod(kernelh,2)~=1
    error('CreateStatic2DConvolutionShader: Only odd-sized kernels of at least size 3x3 supported!');
end;

% First time invocation?
if isempty(initialized)
    % Make sure GLSL and fragmentshaders are supported on first call:
    AssertGLSL;

    % Query supported extensions:
    extensions = glGetString(GL.EXTENSIONS);
    if isempty(findstr(extensions, 'GL_ARB_fragment_shader'))
        % No fragment shaders: This is a no go!
        error('Sorry, this function does not work on your graphics hardware due to lack of sufficient support for fragment shaders.');
    end

    % Clear any OpenGL error state.
    while (glGetError~=GL.NO_ERROR); end;

    % We are initialized:
    initialized = 1;
end % of initialization.

% Ok, now we synthesize a shader program source code string on the fly:

% Header section:
src = ['/* Statically compiled 2D convolution fragment shader for 2D rectangle textures.' char(10) ...
       '// Kernel Size is ' num2str(kernelw) ' by ' num2str(kernelh) char(10) ...
       '// Compiled by Psychtoolbox convolution shader metacompiler.'  char(10) ...
       '*/'  char(10)];
   
% Compute half-width and linear shader coefficient array:
hw = (kernelw - 1) / 2;
shaderkernel = single(reshape(kernel, kernelw * kernelh, 1));

src = [src '#version 110' char(10)  char(10) ...
           'const int KernelHalfWidth = ' num2str(hw) ';' char(10) ...
           'uniform sampler2DRect Image;' char(10) ...
           'float kernel[' num2str(kernelw * kernelh) '];' char(10) char(10)];


% Now for the program body:
src =         [src ...
              'void main()' char(10)...
              '{' char(10) ...
              '  int dx, dy, i;' char(10) ...
              '  vec4 sum = vec4(0.0);' char(10) ...
              '  vec4 tmp;' char(10) ...
              '  i=0;' char(10) ' '];

% Output the kernel itself:
for i=1:(kernelw*kernelh)
    src = sprintf('%s kernel[%i] = %f;', src, i-1, shaderkernel(i));
end
          
src =         [src char(10) char(10) ...   
              '  for (dy = -KernelHalfWidth; dy <= KernelHalfWidth; dy++) {' char(10) ...
              '    for (dx = -KernelHalfWidth; dx <= KernelHalfWidth; dx++) {' char(10) ...
              '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx) + 0.5, float(dy) + 0.5));' char(10) ...
              '      sum += tmp * kernel[i];' char(10) ...
              '      i++;' char(10) ...
              '    }' char(10) ...
              '  }' char(10) ...
              '  gl_FragColor = sum;' char(10) ...
              '}' char(10) char(10)];
  
% Ok, we have our shader source string:
if debug > 0
    src
end

% Create shader object, assign sourcecode and compile it:
shandle = glCreateShader(GL.FRAGMENT_SHADER);
if debug > 1
    glShaderSource(shandle, src, debug);
else
    glShaderSource(shandle, src);
end;

% This would abort if the shader would be invalid:
glCompileShader(shandle);

% Create new program object and get handle to it:
shader = glCreateProgram;
glAttachShader(shader, shandle);

% Link it: This will abort if linker pass fails.
glLinkProgram(shader);

% Should be ready. Setup texture unit to be always unit zero:
glUseProgram(shader);
shader_image = glGetUniformLocation(shader, 'Image');
glUniform1i(shader_image, 0);
% Unbind program:
glUseProgram(0);

% Ok, should be ready for use with e.g., Screen('DrawTexture')...

% Check for errors:
err = glGetError;
if err
    fprintf('Error creating static 2D convolution shader! The GL returned: ');
    gluErrorString(err);
    shader = 0;
end;

% Shader should be ready for use.
return;
