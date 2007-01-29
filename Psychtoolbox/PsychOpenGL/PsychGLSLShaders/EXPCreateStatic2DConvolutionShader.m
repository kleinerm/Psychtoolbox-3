function shader = EXPCreateStatic2DConvolutionShader(kernel, inputchannels, filteredoutchannels, shadertype, debug)
% EXPCreateStatic2DConvolutionShader(kernel [, inputchannels=3][, filteredoutchannels=3][, shadertype][, debug])
% 
% Creates a GLSL fragment shader for 2D convolution of textures with
% the 2D or 1D convolution kernel 'kernel' and returns a handle 'shader'
% to it. The shader can then be applied to any texture by
% calling glUseProgram(shader) before drawing the texture.
%
% The kernel is a simple m-by-n matrix of floating point numbers with m and
% n being odd numbers, e.g., 1x1, 3x3, 5x5, 7x7, 9x9,..., 1x3, 1x9, 7x1 ...
% Each entry in the kernel is used as a weight factor.
%
% The simplest way to get a 2D kernel is to use the function
% kernel = fspecial(...); fspecial is part of the Matlab image
% processing toolbox, see "help fspecial" for more information.
%
% 'inputchannels' = The number of texture channels to use as input for the
% convolution. Possible values are: 3 = Red,Green and Blue color channels are
% provided as part of a true-color texture, don't use the alpha channel (if
% any) for convolution but just pass it through unmodified. 1 = The texture
% only defines a luminance channel for convolution, an (optional) alpha
% channel is passed through unmodified. 4 = Use all four channels (Red,
% green, blue, alpha) for convolution.
%
% 'filteredoutchannels' = The number of channels to convolve as output: 3 =
% Convolve each of the three color channels red, green and blue separately
% by the kernel. An (optional) alpha channel is passed through unmodified.
% 1 = Output a filtered luminance channel, and an (optional) unmodified
% alpha channel. If input is a 3 channel RGB image, then the rgb image will
% get converted to luminance before convolution. 4 = Filter all four
% channels independently.
%
% Typical settings:
% Filter a RGB(A) image: inputchannels = 3, filteredoutchannels = 3.
% Filter a RGB(A) image into greyscale: inputchannels = 3,
% filteredoutchannels = 1.
% Filter a luminance(A) image: inputchannels = 1, filteredoutchannels = 1.
% Generic filtering of 4-channel data: inputchannels = 4,
% filteredoutchannels = 4.
%
%
% CAUTION:
%
% This feature is in early alpha stage. It may fail on your system and its
% future implementation may change significantly! Don't trust its results
% without validation against a known good reference implementation!
%
% NOTES: Filtermode 2 and 3 not fully implemented for the general case!
% ---> No separable 1D kernels, no support for all input->output mappings

% History:
% 27.11.2006 written by Mario Kleiner.

global GL;
persistent initialized;

if nargin < 1
    error('CreateStatic2DConvolutionShader: No kernel provided!');
end;

if isempty(kernel)
    error('CreateStatic2DConvolutionShader: No kernel provided!');
end

if nargin < 2 || isempty(inputchannels)
    inputchannels = 3;
end

if nargin < 3 || isempty(filteredoutchannels)
    filteredoutchannels = 3;
end

if nargin < 4 || isempty(shadertype)
    shadertype = 0;
end

if nargin < 5 || isempty(debug);
    debug = 0;
end;

% Query size of kernel:
kernelw = size(kernel,1);
kernelh = size(kernel,2);

% We only want odd sized kernels of at least 3x3, e.g., 3x3, 5x5, 7x7, ...
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
hwx = (kernelw - 1) / 2;
hwy = (kernelh - 1) / 2;
shaderkernel = single(reshape(kernel, kernelw * kernelh, 1));

if shadertype == 3
    % Generic shader, using a texture bound to 2nd unit as lookup table for
    % the convolution kernel. This method is way less efficient than
    % lookups of the kernel in internal uniform registers or compiled-in
    % constants, but it scales up to very large kernels, whereas the former
    % methods are limited in kernel size by how many constants can be
    % compiled into the shader or stored in uniform registers of a specific
    % GPU.

    % Load our shader for convolution blit operations:
    shader = LoadGLSLProgramFromFiles('Convolve2DRectTextureShader.frag.txt', 1);
    % Assign proper texture units for input image and clut:
    glUseProgram(shader);
    shader_image = glGetUniformLocation(shader, 'Image');
    shader_clut  = glGetUniformLocation(shader, 'Kernel');
    shader_kernelsize  = glGetUniformLocation(shader, 'KernelHalfWidth');

    glUniform1i(shader_image, 0);
    glUniform1i(shader_clut, 1);
    glUniform1f(shader_kernelsize, hwx);

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
    % Generate a linear shader program, i.e., all loops are completely
    % unrolled. This may be a good workaround on old GPU's, but it didn't
    % yield any speed improvements on GF-7000 or X1600 ...
    
    % Header section:
    src = ['/* Statically compiled 2D convolution fragment shader for 2D rectangle textures.' char(10) ...
        '// Kernel Size is ' num2str(kernelw) ' by ' num2str(kernelh) char(10) ...
        '// Compiled by Psychtoolbox convolution shader metacompiler.'  char(10) ...
        '*/'  char(10)];

    src = [src '#version 110' char(10)  char(10) ...
        'uniform sampler2DRect Image;' char(10) char(10)];

    switch(inputchannels)
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
    for dy=-hwy:hwy
        for dx=-hwx:hwx
            i= i + 1;
%            src = [src '  sum += ' num2str(shaderkernel(i)) ' * texture2DRect(Image, gl_TexCoord[0].st + vec2(' num2str(dx) '.0, ' num2str(dy) '.0))' ifetch ';' char(10)];
            src = [src '  sum += ' sprintf('%.16f',shaderkernel(i)) ' * texture2DRect(Image, gl_TexCoord[0].st + vec2(' num2str(dx) '.0, ' num2str(dy) '.0))' ifetch ';' char(10)];
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
        '// Auto-Generated by Psychtoolbox convolution shader generator.'  char(10) ...
        '*/'  char(10)];

    src = [src '#version 110' char(10)  char(10) ...
        'const int KernelHalfWidthX = ' num2str(hwx) ';' char(10) ...
        'const int KernelHalfWidthY = ' num2str(hwy) ';' char(10) ...
        'uniform sampler2DRect Image;' char(10)];

    if (inputchannels==3) && (filteredoutchannels==1)
        src = [src 'const vec3 rgb2grayweights = vec3(0.3, 0.59, 0.11);' char(10) ];
    end

    if shadertype == 0
        src = [src 'float kernel[' num2str(kernelw * kernelh) '];' char(10) char(10)];
    else
        src = [src 'uniform float kernel[' num2str(kernelw * kernelh) '];' char(10) char(10)];
    end

    switch(inputchannels)
        case 1 % Filter luminance channel only, but replicate filtered result to RGB. Alpha is passed through.
            dtype  = '  float tmp, sum = float(0.0);';
            ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).r;';
            iupdate= '      sum += tmp * kernel[i];';
            douti  = '  gl_FragColor.rgb = vec3(sum); gl_FragColor.a = texture2DRect(Image, gl_TexCoord[0].st).a;';
        case 2
            dtype  = '  vec2 tmp, sum = vec2(0.0);';
            ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).rg;';
            iupdate= '      sum += tmp * vec2(kernel[i]);';
            douti  = '  gl_FragColor.rg = sum; gl_FragColor.a = texture2DRect(Image, gl_TexCoord[0].st).a;';
        case 3 % Input is RGB. Alpha is passed through, ...
            if (filteredoutchannels == 3)
                % Filter each R,G,B channel separately...
                dtype  = '  vec3 tmp, sum = vec3(0.0);';
                ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).rgb;';
                iupdate= '      sum += tmp * vec3(kernel[i]);';
                douti  = '  gl_FragColor.rgb = sum; gl_FragColor.a = texture2DRect(Image, gl_TexCoord[0].st).a;';
            else
                % Convert RGB to Luminance, filter that, replicate result
                % into RGB out...
                dtype  = '  vec3 tmp; float sum = float(0.0);';
                ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).rgb;';
                iupdate= '      sum += dot(tmp, rgb2grayweights) * kernel[i];';
                douti  = '  gl_FragColor.rgb = vec3(sum); gl_FragColor.a = texture2DRect(Image, gl_TexCoord[0].st).a;';
            end
        case 4 % Filter all RGBA channels. Mostly useful for generic 4-channel data processing, not for imaging.
            dtype  = '  vec4 tmp, sum = vec4(0.0);';
            ifetch = '      tmp = texture2DRect(Image, gl_TexCoord[0].st + vec2(float(dx), float(dy))).rgba;';
            iupdate= '      sum += tmp * vec4(kernel[i]);';
            douti  = '  gl_FragColor.rgba = sum;';
    end

    % Now for the program body:
    src =         [src ...
        'void main()' char(10)...
        '{' char(10) ...
        '  int dx, dy, i;' char(10) ...
        dtype char(10) ...
        '  i=0;' char(10) ...
        ' '];

    % Type 0: Kernel is compiled into the shader as array of floating
    % point constants:
    if shadertype == 0
        % Output the kernel itself:
        for i=1:(kernelw*kernelh)
            src = sprintf('%s kernel[%i] = %.16f;', src, i-1, shaderkernel(i));
        end
    end

    src =         [src char(10) char(10) ...
        '  for (dy = -KernelHalfWidthY; dy <= KernelHalfWidthY; dy++) {' char(10) ...
        '    for (dx = -KernelHalfWidthX; dx <= KernelHalfWidthX; dx++) {' char(10) ...
        ifetch char(10) ...
        iupdate char(10) ...
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
