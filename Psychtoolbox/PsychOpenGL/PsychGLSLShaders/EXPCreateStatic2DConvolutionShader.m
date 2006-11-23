function shader = EXPCreateStatic2DConvolutionShader(kernel, channels, shadertype, debug)
% EXPCreateStatic2DConvolutionShader(kernel [, channels=4][, shadertype][, debug])
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

if nargin < 2 || isempty(channels)
    channels = 4;
end

if nargin < 3 || isempty(shadertype)
    shadertype = 0;
end

if nargin < 4
    debug = 0;
end;

% Query size of kernel:
kernelw = size(kernel,1);
kernelh = size(kernel,2);

% We only want odd sized kernels of at least 3x3, e.g., 3x3, 5x5, 7x7, 9x9,
% ...
if kernelw < 1 | kernelh < 1 | mod(kernelw,2)~=1 | mod(kernelh,2)~=1
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

%    maxinstructions = glGetIntegerv(GL.MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT)
    maxuniforms = glGetIntegerv(GL.MAX_FRAGMENT_UNIFORM_COMPONENTS)
    
    % We are initialized:
    initialized = 1;
end % of initialization.

% Compute half-width and linear shader coefficient array:
hw = (kernelw - 1) / 2;
shaderkernel = single(reshape(kernel, kernelw * kernelh, 1));

if shadertype == 3
    % Generic shader, using a texture bound to 2nd unit as lookup table for
    % the convolution kernel. This method is way less efficient than
    % lookups of the kernel in internal uniform registers or compiled in
    % constants, but it scales up to very large kernels, whereas the former
    % methods are limited in kernel size by how many constants can be
    % compiled into the shader or stored in uniform registers.

    % Load our shader for convolution blit operations:
    shader = LoadGLSLProgramFromFiles('Convolve2DRectTextureShader.frag.txt', 1);
    % Assign proper texture units for input image and clut:
    glUseProgram(shader);
    shader_image = glGetUniformLocation(shader, 'Image');
    shader_clut  = glGetUniformLocation(shader, 'Kernel');
    shader_kernelsize  = glGetUniformLocation(shader, 'KernelHalfWidth');

    glUniform1i(shader_image, 0);
    glUniform1i(shader_clut, 1);
    glUniform1f(shader_kernelsize, hw);

    glUseProgram(0);

    % Shader ready. Setup 2nd texture unit as LUT unit for our kernel:
    
    % Select the 2nd texture unit (unit 1) for setup:
    glActiveTexture(GL.TEXTURE1);
    luttex = glGenTextures(1);
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, luttex);
    glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, GL.LUMINANCE_FLOAT32_APPLE, kernelw, kernelh, 0, GL.LUMINANCE, GL.FLOAT, shaderkernel);
    %glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, GL.LUMINANCE8, kernelw, kernelh, 0, GL.LUMINANCE, GL.FLOAT, shaderkernel);

    % Make sure we use nearest neighbour sampling:
    glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_MIN_FILTER, GL.NEAREST);
    glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_MAG_FILTER, GL.NEAREST);

    % And that we clamp to edge:
    glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_S, GL.CLAMP);
    glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_T, GL.CLAMP);
    
    % Default CLUT setup done: Switch back to texture unit 0:
    glActiveTexture(GL.TEXTURE0);
end

if shadertype == 2
    % Header section:
    src = ['/* Statically compiled 2D convolution fragment shader for 2D rectangle textures.' char(10) ...
        '// Kernel Size is ' num2str(kernelw) ' by ' num2str(kernelh) char(10) ...
        '// Compiled by Psychtoolbox convolution shader metacompiler.'  char(10) ...
        '*/'  char(10)];

    src = [src '#version 110' char(10)  char(10) ...
        'uniform sampler2DRect Image;' char(10) char(10)];

    switch(channels)
        case 1
            dtype  = '  float sum = float(0.0);';
            ifetch = '.r';
            douti  = '  gl_FragColor.rgba = vec4(sum);';
        case 2
            dtype  = '  vec2 sum = vec2(0.0);';
            ifetch = '.rg';
            douti  = '  gl_FragColor.rg = sum;';
        case 3
            dtype  = '  vec3 sum = vec3(0.0);';
            ifetch = '.rgb';
            douti  = '  gl_FragColor.rgb = sum;';
        case 4
            dtype  = '  vec4 sum = vec4(0.0);';
            ifetch = '.rgba';
            douti  = '  gl_FragColor.rgba = sum;';
    end

    % Now for the program body:
    src =         [src ...
        'void main()' char(10)...
        '{' char(10) ...
        dtype char(10) char(10)];
    
    % Generate unrolled loop:
    i=0;
    for dy=-hw:hw
        for dx=-hw:hw
            i= i + 1;
            src = [src '  sum += ' num2str(shaderkernel(i)) ' * texture2DRect(Image, gl_TexCoord[0].st + vec2(' num2str(dx) '.0, ' num2str(dy) '.0))' ifetch ';' char(10)];
        end
    end

    % Generate tail:
    src = [src douti char(10) '}' char(10) char(10)];
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
end

if shadertype < 2
    % Precompiled shaders, not using textures as lookup tables:
    % Ok, now we synthesize a shader program source code string on the fly:

    % Header section:
    src = ['/* Statically compiled 2D convolution fragment shader for 2D rectangle textures.' char(10) ...
        '// Kernel Size is ' num2str(kernelw) ' by ' num2str(kernelh) char(10) ...
        '// Compiled by Psychtoolbox convolution shader metacompiler.'  char(10) ...
        '*/'  char(10)];

    src = [src '#version 110' char(10)  char(10) ...
        'const int KernelHalfWidth = ' num2str(hw) ';' char(10) ...
        'uniform sampler2DRect Image;' char(10)];

    if shadertype == 0
        src = [src 'float kernel[' num2str(kernelw * kernelh) '];' char(10) char(10)];
    else
        src = [src 'uniform float kernel[' num2str(kernelw * kernelh) '];' char(10) char(10)];
    end

    switch(channels)
        case 1
            dtype  = '  float tmp, sum = float(0.0);';
            ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).r;';
            douti  = '  gl_FragColor.rgba = vec4(sum);';
        case 2
            dtype  = '  vec2 tmp, sum = vec2(0.0);';
            ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).rg;';
            douti  = '  gl_FragColor.rg = sum;';
        case 3
            dtype  = '  vec3 tmp, sum = vec3(0.0);';
            ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).rgb;';
            douti  = '  gl_FragColor.rgb = sum;';
        case 4
            dtype  = '  vec4 tmp, sum = vec4(0.0);';
            ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).rgba;';
            douti  = '  gl_FragColor.rgba = sum;';
    end

    % Now for the program body:
    src =         [src ...
        'void main()' char(10)...
        '{' char(10) ...
        '  int dx, dy, i;' char(10) ...
        dtype char(10) ...
        '  i=0;' char(10) ' '];

    % Type 0: Kernel is compiled into the shader as array of floating
    % point constants:
    if shadertype == 0
        % Output the kernel itself:
        for i=1:(kernelw*kernelh)
            src = sprintf('%s kernel[%i] = %f;', src, i-1, shaderkernel(i));
        end
    end

    src =         [src char(10) char(10) ...
        '  for (dy = -KernelHalfWidth; dy <= KernelHalfWidth; dy++) {' char(10) ...
        '    for (dx = -KernelHalfWidth; dx <= KernelHalfWidth; dx++) {' char(10) ...
        ifetch char(10) ...
        '      sum += tmp * kernel[i];' char(10) ...
        '      i++;' char(10) ...
        '    }' char(10) ...
        '  }' char(10) ...
        douti char(10) ...
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

    % For shadertype 1, set up uniforms with kernel:
    if shadertype == 1
        shader_kernel = glGetUniformLocation(shader, 'kernel[0]');

        % Output the kernel itself:
        for i=1:(kernelw*kernelh)
            glUniform1f(shader_kernel + i - 1, shaderkernel(i));
        end

    end

    % Unbind program:
    glUseProgram(0);
end

% Ok, shader should be ready for use with e.g., Screen('DrawTexture')...

% Check for errors:
err = glGetError;
if err
    fprintf('Error creating static 2D convolution shader! The GL returned: ');
    gluErrorString(err);
    shader = 0;
end;

% Shader should be ready for use.
return;
