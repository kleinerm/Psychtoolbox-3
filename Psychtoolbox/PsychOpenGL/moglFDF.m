function varargout = moglFDF(cmd, varargin)
% moglFDF(cmd [, arg1][, arg2][, ...]) - "MOGL FormlessDotFields"
%
% Implementation of Sheinberg et al. inspired random dot structure from motion
% rendering. This routine is a fast implementation of "Formless dot field
% structure-from-motion stimuli". It is based on - and very similar in
% behaviour, although not identical in implementation - the algorithm
% proposed by Jedediah M. Singer and David L. Sheinberg in their
% Journal of Vision paper "A method for the real-time rendering of
% formless dot field structure-from-motion stimuli" (Journal of Vision, 8,
% 1-8)
%
% This algorithm takes the idea of the above mentioned paper and pushes it
% one step further by moving nearly all stimulus computation onto the GPU.
%
% All compute intense tasks are carried out by vertex- and fragment-shaders
% on the GPU and all heavy data structures are stored within the GPU's fast
% local VRAM memory, reducing the amount of communication between host
% system and graphics card to an absolute minimum. The Matlab code on the
% CPU only controls the flow of operations on the GPU and generates a
% matrix with random numbers to update the sample distribution. This should
% provide a significant speedup beyond what the Singer et al. algorithm
% achieves, at least for complex and demanding stimuli.
%
%
% The algorithm makes heavy use of GPU based image processing for maximum
% speed, so it needs at least NVidia Geforce 6000 series or ATI Radeon
% X1000 series graphics hardware (and any later models or equivalent
% hardware) to work. It also needs the PTB imaging pipeline enabled, at
% least fast offscreen window support. You do this, e.g., by replacing a
% call to ...
%
% [win, winRect] = Screen('OpenWindow', screenid, 0);
%
% ... with a call sequence like this ...
%
% PsychImaging('PrepareConfiguration');
% PsychImaging('AddTask', 'General', 'UseFastOffscreenWindows');
% [win , winRect] = PsychImaging('OpenWindow', screenid, 0);
%
%
% A minimal example of how to use moglFDF to render a "dotfield"
% representation of a rotating 3D sphere can be found in the FDFDemo.m
% file.
%
%
% Subcommands, their syntax & meaning:
% ====================================
%
% moglFDF('DebugFlag', flag [, debugGain]);
% - Set debug flag to value 'flag'. Default is zero. Non-zero values enable
% different visualizations that may aid debugging non-working setups.
% 1 = Show silhouette buffer, 2 = Show trackbuffer, 3 = Show random noise
% sampling texture, 4 = Show sampleBuffer, 5 = Show FGDots buffer.
%
% The optional 'debugGain' parameter must be a 4 component [R G B A] color
% vector with modulation gains for the drawn "debug images" - simply to
% scale each color channel in intensity to allow for display of values
% outside the standard displayable range between zero and one.
%
%
% context = moglFDF('CreateContext', window, rect, texCoordMin, texCoordMax, texResolution, maxFGDots, maxBGDots, dotLifetime [,zThreshold=Off]); 
% - Create a "rendercontext" for a single 3D object. Returns a 'context'
% handle to it which needs to be passed in to all other functions as
% reference. All following parameters are required and don't have any
% defaults:
%
% 'window' Handle of masterwindow - The onscreen window used for rendering.
% This is not neccessarily the window to which final stimulus will be drawn
% to, but it is needed as a "parent" for all ressources.
%
% 'rect' A Psychtoolbox rectangle [left top right bottom] that describes
% the size and shape of the final stimulus window. This rect must have the
% same size as the 3D window and final stimulus window -- Lots of internal
% calculations depend on this geometry spec.
%
% 'texCoordMin' Two element vector which contains the minimum texture
% coordinate values contained in the 3D scene for x- resp. y-direction.
%
% 'texCoordMax' Two element vector which contains the maximum texture
% coordinate values contained in the 3D scene for x- resp. y-direction.
%
% 'texResolution' Two element vector which contains the internal resolution
% for x- resp. y-direction of the 3D object surface. Higher values mean finer
% resolution and less aliasing, but also higher storage requirements and
% longer processing times.
%
% 'maxFGDots' Maximum number of foreground (object shape) dots to use for
% random shape sampling.
%
% 'maxBGDots' Maximum number of background dots to use for random background
% sampling.
%
% 'dotLifetime' Lifetime of each foreground- or background dot in 'Update'
% cycles. Each dot is replace by a new random sample after that many
% invocations of the 'Update' function.
%
% 'zThreshold' Optional zThreshold for occlusion test: By default, it is
% 10.0 ie. occlusion test disabled. A value between 0.0 and 1.0 will enable
% occlusion testing -- Dots that would correspond to occluded surfaces are
% not drawn. Small numbers (close to zero) make the test more sensitive but
% can cause artifacts due to internal numeric roundoff errors. Bigger
% numbers (closer to one) make it more robust but less powerful. The
% "sweet-spot" depends on your hardware and 3D scene. Empirically a setting
% of 0.0001 is a good value for ATI Radeon X1000 series hardware.
% The default setting (bigger than 1.0) will disable occlusion test --
% "Hidden dots" are not hidden, but drawn.
%
%
% context = moglFDF('SetRenderCallback', context, callbackEvalString);
% - Define the 'eval' string for this context to be used as rendercallback.
% Pass in a Matlab command string (for evaluation via eval() function in the
% Workspace of the calling function). This string is called/executed during
% each 'Update' cycle. It has to contain the code that performs the actual
% rendering of the 3D scene or object.
%
% The called rendering code *must not* glClear() the framebuffer or mess
% around with alpha-blending state or depth-buffer/depth-test settings, nor
% should it bind any shaders! It makes sense to disable any kind of
% lighting or texture mapping, as no photorealistic image is rendered, so
% it would be a waste of computation time.
%
%
% context = moglFDF('DestroyContext', context);
% - Destroy a processing context, release all of its ressources.
%
%
% context = moglFDF('ResetState', context);
% - Reset processing contexts state to initial state, just as if it was
% just created. Useful at start of a new trial.
%
%
% context = moglFDF('Update', context);
% - Perform an 'update' cycle for given context. A new "3D frame" is rendered
% via the rendercallback function, then analysed, resampled etc. to create
% a new complete distribution of 2D random dots, ready for drawing or
% readback.
%
%
% context = moglFDF('Render', context [, targetWindow] [, drawSpec=[1,1]]);
% - Render current 2D random dot cloud (as defined by processing of last
% 'Update' call) quickly and efficiently into window 'targetWindow'.
% 'targetWindow' can be any onscreen- or offscreen window and is allowed to
% change at each invocation of 'Render'. By default, the 'window' from the
% 'CreateContext' call is used.
%
% 'drawSpec' is an optional two-element vector to select if only foreground
% dots should be rendered [1 0], only background dots should be rendered [0 1],
% or both [1 1] -- which is the default.
%
% Before calling 'Render' you can define dot sizes, colors, alpha-blending
% state, texture coordinates, anti-aliasing settings, or define texture
% mapping, point-sprite modes or texture mapping setups however you like.
% The internal 'Render' routine just defines 2D point locations, then
% invokes the render op.
%
%
% [xyFGdots, xyBGdots] = moglFDF('GetResults', context); - Returns a 2 row
% by n columns vector of all random dot positions, for processing within
% Matlab/Octave. Row 1 is x-locations, Row 2 is y-locations of dots, each
% column defines one dot. The 'xyFGDots' contains all foreground dots which
% define the object, whereas the 'xyBGdots' vector contains the background
% dots. These vectors are suitable for direct drawing via
% Screen('DrawDots'); However, invocation of moglFDF('Render',...); is a
% more efficient method of rendering these dot fields, unless you have very
% special needs.

% History:
%  05/02/08  Initial "proof of concept" implementation (MK).
% -05/10/08  Various improvements (MK).
%  11/03/08  Documentation update, preparation for public release (MK).

% Need OpenGL constants:
global GL;
global moglFDF_OriginalContext;

% Internal state:
persistent initialized;
persistent debug;
persistent debugGain;

if nargin < 1
    error('You must provide a "cmd" subcommand to execute!');
end

if isempty(initialized)
    initialized = 0;
    moglFDF_OriginalContext = [];
    debug = 0;
    
    if isempty(GL)
        % If OpenGL not initialized, do a full init for 3D mode:
        error('OpenGL mode not initialized! You *must* call InitializeMatlabOpenGL before the first call to this routine!')
    end
end

% Subcommand dispatch:

% Initialization of a new context: Allocate and setup all ressources:
if strcmpi(cmd, 'CreateContext')
    % Fetch all arguments - They are all required.
    if nargin < 9
        error('Some input arguments to "CreateContext" are missing. They are all required!');
    end
    
    if initialized
        error('Called "CreateContext" although already initialized! Call "Shutdown" first.');
    end
    
    % Parent window: Provides OpenGL master-/slave- contexts for our
    % operations, shaders and buffers, as well as reference for
    % rendertarget size:
    ctx.parentWin = varargin{1};
    
    if ~isscalar(ctx.parentWin) | ~ismember(ctx.parentWin, Screen('Windows'))
        disp(ctx.parentWin);
        error('Invalid "window" argument provided to "CreateContext" - No such window (see above)!');
    end
    
    % Get all other arguments and perform parameter type and range checks:
    ctx.rect = varargin{2};
    if ~isnumeric(ctx.rect) | length(ctx.rect)~=4
        disp(ctx.rect);
        error('Invalid "rect" argument provided to "CreateContext" - Must be a 4 component vector that describes the size and shape of the target rectangle [left top right bottom]');        
    end
    
    ctx.rect = double(ctx.rect);
    if IsEmptyRect(ctx.rect)
        disp(ctx.rect);
        error('Invalid "rect" argument provided to "CreateContext" - Must be a non-empty rect that describes the size and shape of the target rectangle [left top right bottom]');        
    end
    
    ctx.texCoordMin = varargin{3};
    if ~isnumeric(ctx.texCoordMin) | length(ctx.texCoordMin)~=2
        disp(ctx.texCoordMin);
        error('Invalid "texCoordMin" argument provided to "CreateContext" - Must be a 2 component vector of minimal texture coordinates in x- and y- direction!');
    end
    
    ctx.texCoordMax = varargin{4};
    if ~isnumeric(ctx.texCoordMax) | length(ctx.texCoordMax)~=2
        disp(ctx.texCoordMax);
        error('Invalid "texCoordMax" argument provided to "CreateContext" - Must be a 2 component vector of maximal texture coordinates in x- and y- direction!');
    end
    
    ctx.texResolution = varargin{5};
    if ~isnumeric(ctx.texResolution) | length(ctx.texResolution)~=2
        disp(ctx.texResolution);
        error('Invalid "texResolution" argument provided to "CreateContext" - Must be a 2 component vector of integral numbers with processing resolution in x- and y- direction!');
    end
   
    if round(ctx.texResolution) ~= ctx.texResolution
        disp(ctx.texResolution);
        error('Invalid "texResolution" argument provided to "CreateContext" - Must be integral numbers for resolution in x- and y- direction!');
    end

    if min(ctx.texResolution) < 2
        disp(ctx.texResolution);
        error('Invalid "texResolution" argument provided to "CreateContext" - Each component must be at least 2 units!');
    end
    
    ctx.maxFGDots = round(varargin{6});
    if ~isscalar(ctx.maxFGDots)
        disp(ctx.maxFGDots);
        error('Invalid "maxFGDots" argument provided to "CreateContext" - Must be a positive integral number of maximum foreground dots!');
    end
    
    if ctx.maxFGDots < 1
        disp(ctx.maxFGDots);
        error('Invalid "maxFGDots" argument provided to "CreateContext" - Must be at least 1!');
    end
    
    ctx.maxBGDots = round(varargin{7});
    if ~isscalar(ctx.maxBGDots)
        disp(ctx.maxBGDots);
        error('Invalid "maxBGDots" argument provided to "CreateContext" - Must be a positive integral number of maximum background dots!');
    end
    
    if ctx.maxBGDots < 0
        disp(ctx.maxBGDots);
        error('Invalid "maxBGDots" argument provided to "CreateContext" - Must be at least zero!');
    end

    ctx.dotLifetime = round(varargin{8});
    if ~isscalar(ctx.dotLifetime)
        disp(ctx.dotLifetime);
        error('Invalid "dotLifetime" argument provided to "CreateContext" - Must be a positive integral number of how many update cycles a dot lives before replacement!');
    end
    
    if ctx.dotLifetime < 1
        disp(ctx.dotLifetime);
        error('Invalid "dotLifetime" argument provided to "CreateContext" - Must be at least 1!');
    end

    % Basic checks passed: Now check for inter-parameter consistency:
    if rem(ctx.maxFGDots, ctx.dotLifetime)
        % Doesn't divide without remainder.
        fprintf('maxFGDots=%i , dotLifetime=%i --> remainder of maxFGDots / dotLifetime is not zero!\n', ctx.maxFGDots, ctx.dotLifetime);
        error('In "CreateContext": "maxFGDots" is not an integral multiple of "dotLifetime". That condition must hold!');
    end
    
    if nargin >= 10
        ctx.zThreshold = varargin{9};
        if ~isscalar(ctx.zThreshold)
            disp(ctx.zThreshold);
            error('Invalid "zThreshold" argument provided to "CreateContext" - Must be a positive number in range 0.0 - 1.0 for z-Test, and bigger for z-Test disabled!');
        end
    else
        % Default: z-Test disabled:
        ctx.zThreshold = 10.0;
    end
    
    % Backup current GL context binding:
    BackupGL;
    
    % Make sure our Screen context is active:
    SwitchToPTB;
    
    % Retrieve info about our hosting window. This will implicitely enable
    % our parents OpenGL context, so we can do GL query commands safely:
    winfo = Screen('GetWindowInfo', ctx.parentWin);
    
    % Retrieve maximum width or height of textures and offscreen windows
    % supported by this GL implementation:
    maxtexsize = glGetIntegerv(GL.MAX_RECTANGLE_TEXTURE_SIZE_EXT);
    
    % Width of a line in samplebuffer:
    ctx.samplesPerLine = min(maxtexsize, 2048);
    ctx.samplesPerLine = min(ctx.samplesPerLine, round(ctx.maxFGDots / ctx.dotLifetime));
    
    % Check requested internal resolution against hw-limit:
    if max(ctx.texResolution) > maxtexsize
        disp(ctx.texResolution)
        error(sprintf('Requested "texResolution" parameter too big in at least one dimension - Your graphics card can not handle that! Maximum is %i\n', maxtexsize));
    end
    
    % Imaging pipeline active in at least minimum configuration?
    if ~bitand(winfo.ImagingMode, mor(kPsychNeedFastBackingStore, kPsychNeedFastOffscreenWindows))
        % Neither basic pipeline, nor fast offscreen window support
        % activated in parent window. This is a no-go!
        error('In "CreateContext": The PTB imaging pipeline is not active for provided parent window - this will not work! Need at least support for fast offscreen windows.');
    end
    
    if winfo.GLSupportsFBOUpToBpc < 32 | winfo.GLSupportsTexturesUpToBpc < 32
        error('In "CreateContext": Your gfx-hardware is not capable of handling textures and buffers with the required precision - this function will not work on your hardware!');
    end
    
    % Check for all required extensions:
    if ~(~isempty(findstr(glGetString(GL.EXTENSIONS), '_vertex_buffer_object')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), '_pixel_buffer_object')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), '_framebuffer_object')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_ARB_shading_language')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_ARB_shader_objects')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_ARB_fragment_shader')) && ...
            ~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_ARB_vertex_shader')) && ...
            (~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_APPLE_float_pixels')) || ...
             ~isempty(findstr(glGetString(GL.EXTENSIONS), '_color_buffer_float'))))
         % At least one of the required extensions is missing!
        error('In "CreateContext": Your gfx-hardware does not support all required OpenGL extensions - this function will not work on your hardware!');
         
    end

    % Ok, all checks passed.
    
    % Create all relevant FBO buffers, aka Offscreen windows:

    % Sample buffer: Contains all foreground random dot samples.
    % The buffer is split into ctx.dotLifetime blocks, each
    % ctx.samplesPerLine wide and ctx.sampleLinesperBatch high. Each
    % pixel in the buffer encodes one foreground sampe - and thereby one
    % potential foreground dot in the next rendered frame.
    %
    % The algorithm will at each update cycle replace one of these blocks
    % or "batches" with a new distribution of samples -- the old dots in
    % that batch will die and get replaced by new samples from the
    % foreground distribution iff they actually hit the silhouette of the
    % 3D object to be "visualized" - otherwise that samples will be invalid
    % and inactive. Each update cycle a different batch is selected for
    % update in a round-robin fashion, so each sample has a lifetime of
    % ctx.dotLifetime update cycles as requested.
    %
    % Each pixel codes as follows: Red channel == x position of 3D object
    % surface to which the sample is attached, encoded as 32 bit float --
    % technically it is the s-texture coordinate of the 3D surface at the
    % point of impact of our sample. Green channel == y position, aka
    % t-texture coordinate on objects surface. Blue channel == "Valid"
    % flag: Non-zero means: Visualize this samples final tracked position
    % -- draw corresponding dot. Zero means: Ignore this sample.
    %
    % Our buffer is implemented as a FBO backed floating point offscreen
    % window with a pixel size of 128 bits, aka 32 bpc float.
    ctx.sampleLinesPerBatch = ceil((round(ctx.maxFGDots / ctx.dotLifetime)) / ctx.samplesPerLine);
    ctx.sampleLinesTotal = ctx.sampleLinesPerBatch * ctx.dotLifetime;
    ctx.sampleBuffer = Screen('OpenOffscreenWindow', ctx.parentWin, [0 0 0 0], double([0 0 ctx.samplesPerLine ctx.sampleLinesTotal]), 128);
    ctx.maxFGDots = ctx.sampleLinesTotal * ctx.samplesPerLine;
    
    % Silhouette buffer: Contains the "perspective correct image space"
    % image of the rendered 3D object. The object is rendered "normally" in
    % a first render pass to get its silhouette, except that the color of
    % each rendered pixel is not a shaded/lit color, but its encoded
    % interpolated surface texture coordinate. This will be done by a
    % proper fragment shader during render pass.
    % The buffer is an FBO backed offscreen window the same size as the
    % parent window, but with 32bpc float format to store accurate texture
    % coordinates in the pixel colors. Red channel encodes s-coord, Green
    % channel encodes t-coord of 2D texture coordinate, blue encodes
    % interpolated z-buffer depths.
    [ctx.silhouetteWidth, ctx.silhouetteHeight] = RectSize(ctx.rect);
    ctx.silhouetteBuffer = Screen('OpenOffscreenWindow', ctx.parentWin, [0 0 0 0], [0, 0, ctx.silhouetteWidth, ctx.silhouetteHeight], 128);

    % Retrieve OpenGL texture handle for the sihouetteBuffer:
    ctx.silhouetteTexture = Screen('GetOpenGLTexture', ctx.parentWin, ctx.silhouetteBuffer);
    
    
    % Tracking buffer: Contains the unwarped/flattened image of the 3D
    % objects surface, created by a 2nd rendering pass of the 3D object,
    % but with special vertex-/fragment shaders attached.
    %
    % Pixel location (x,y) encodes for object surface texture coordinate
    % (s,t): Its R, G and B channels encode interpolated X, Y, Z coordinate
    % of the unwarped object surface. This way a lookup at position (s,t)
    % provides the image space 3D coordinate of surface point (s,t) in
    % "normal" camera centered and projected (X,Y,Z) space -- (X,Y) are
    % projected image coordinates, (Z) is depths component. This allows to
    % lookup the tracked/updated (X,Y) image position of a sample dot.
    %
    % Again a 32bpc float offscreen window FBO, but the resolution is
    % chosen per user spec to be fine enough in texture coordinate space to
    % avoid aliasing artifacts as good as possible:
    ctx.trackingBuffer = Screen('OpenOffscreenWindow', ctx.parentWin, [0 0 0 0], [0, 0, ctx.texResolution(1), ctx.texResolution(2)], 128);

    % Final buffer with foreground dot positions. This one will get filled
    % by the createFGDotShader. It will later get either read back to
    % Matlab on usercode request, or converted to a VBO and then rendered.
    ctx.FGDotsBuffer = Screen('OpenOffscreenWindow', ctx.parentWin, [0 0 0 0], Screen('Rect', ctx.sampleBuffer), 128);
    

    % Final buffer with background dot positions. This one will get filled
    % by the createBGDotShader. It will later get either read back to
    % Matlab on usercode request, or converted to a VBO and then rendered.
    
    % Width of a line in buffer:
    ctx.BGsamplesPerLine = min(maxtexsize, 2048);
    ctx.BGsamplesPerLine = min(ctx.BGsamplesPerLine, round(ctx.maxBGDots / ctx.dotLifetime));
    ctx.BGsampleLinesPerBatch = ceil((round(ctx.maxBGDots / ctx.dotLifetime)) / ctx.BGsamplesPerLine);
    ctx.BGsampleLinesTotal = ctx.BGsampleLinesPerBatch * ctx.dotLifetime;
    ctx.BGDotsBuffer = Screen('OpenOffscreenWindow', ctx.parentWin, [0 0 0 0], double([0 0 ctx.BGsamplesPerLine ctx.BGsampleLinesTotal]), 128);
    ctx.maxBGDots = ctx.BGsampleLinesTotal * ctx.BGsamplesPerLine;
    ctx.BGSampleSet = zeros(ctx.BGsampleLinesTotal, ctx.BGsamplesPerLine, 2);

    % Load and setup all our shaders:
    
    % Basepath to shaders:
    % shaderpath = [fileparts(mfilename('fullpath')) filesep ];
    shaderpath = '';

    % Shader for 1st object renderpass: Encode texcoords and depths into
    % color channel -- to fill silhouetteBuffer:
    ctx.silhouetteRenderShader = LoadGLSLProgramFromFiles([shaderpath 'moglFDFSilhouetteRenderShader'], 1);
    
    % Shader for 2nd object renderpass: Fill trackingBuffer
    ctx.trackingRenderShader = LoadGLSLProgramFromFiles([shaderpath 'moglFDFTrackingRenderShader'], 1);
    glUseProgram(ctx.trackingRenderShader)

    % Compute texture coordinate offset and multiplier to apply in order to
    % remap the real texture coordinate range into the normalized [-1:+1]
    % interval that can pass through vertex clipping:
    glUniform4f(glGetUniformLocation(ctx.trackingRenderShader, 'TextureOffsetBias'), ctx.texCoordMin(1), ctx.texCoordMin(2), 2.0/(ctx.texCoordMax(1) - ctx.texCoordMin(1)), 2.0/(ctx.texCoordMax(2) - ctx.texCoordMin(2)));

    % Bind texunit 1 to object coordinates texture:
    glUniform4f(glGetUniformLocation(ctx.trackingRenderShader, 'Viewport'), 0, 0, ctx.silhouetteWidth/2, ctx.silhouetteHeight/2);
    glUseProgram(0);
    
    
    % Shader for update of distribution in sampleBuffer:
    ctx.samplingShader = LoadGLSLProgramFromFiles([shaderpath 'moglFDFSamplingShader'], 1);

    glUseProgram(ctx.samplingShader);

    % Bind texunit 0 to random sample position texture:
    glUniform1i(glGetUniformLocation(ctx.samplingShader, 'SilSamplePositions'), 0);

    % Bind texunit 1 to silhouette image texture:
    glUniform1i(glGetUniformLocation(ctx.samplingShader, 'Silhouette'), 1);

    % Define remapping of texture coordinates into range 0-texResolution --
    % The size of the trackingBuffer:
    glUniform4f(glGetUniformLocation(ctx.samplingShader, 'TextureOffsetBias'), ctx.texCoordMin(1), ctx.texCoordMin(2), ctx.texResolution(1)/(ctx.texCoordMax(1) - ctx.texCoordMin(1)), ctx.texResolution(2)/(ctx.texCoordMax(2) - ctx.texCoordMin(2)));
    glUseProgram(0);
    
    
    % Shader for final creation of foreground dots VBO spec from
    % distribution in sampleBuffer and trackingBuffer:
    ctx.createFGDotsShader = LoadGLSLProgramFromFiles([shaderpath 'moglFDFForegroundDotsRenderShader'], 1);
    glUseProgram(ctx.createFGDotsShader)

    % Bind texunit 0 to Samplebuffer texture:
    glUniform1i(glGetUniformLocation(ctx.createFGDotsShader, 'SampleBuffer'), 0);

    % Bind texunit 1 to object coordinates texture:
    glUniform1i(glGetUniformLocation(ctx.createFGDotsShader, 'GeometryBuffer'), 1);

    % Bind texunit 2 to silhouette texture for last rendered frame:
    glUniform1i(glGetUniformLocation(ctx.createFGDotsShader, 'Silhouette'), 2);
    
    % Assign zThreshold for depths testing of foreground dots before
    % output to handle occlusions correctly:
    glUniform1f(glGetUniformLocation(ctx.createFGDotsShader, 'zThreshold'), ctx.zThreshold);

    % Assign height of final output window + 1 to allow shader to invert
    % y-coordinate of final dots properly to account for difference in
    % y-axis direction of Screen()'s reference frame vs. OpenGL default
    % frame:
    glUniform1f(glGetUniformLocation(ctx.createFGDotsShader, 'ViewportHeight'), ctx.silhouetteHeight + 1);
    
    % Define size of GeometryBuffer -- wrapAround values for interpolated
    % texture lookup coordinates:
    glUniform2f(glGetUniformLocation(ctx.createFGDotsShader, 'texWrapAround'), ctx.texResolution(1), ctx.texResolution(2));

    glUseProgram(0);
    
    % Create gloperator from shader for later use by Screen('TransformTexture'):
    ctx.createFGDotsoperator = CreateGLOperator(ctx.parentWin, [], ctx.createFGDotsShader, 'Create foreground dots.');
    
    % Shader for creation of background dots VBO spec:
    ctx.createBGDotsShader = LoadGLSLProgramFromFiles([shaderpath 'moglFDFBackgroundDotsRenderShader'], 1);
    glUseProgram(ctx.createBGDotsShader)
    % Bind texunit 0 to random sample position texture:
    glUniform1i(glGetUniformLocation(ctx.createBGDotsShader, 'SilSamplePositions'), 0);

    % Bind texunit 1 to silhouette image texture:
    glUniform1i(glGetUniformLocation(ctx.createBGDotsShader, 'Silhouette'), 1);

    % Assign height of final output window + 1 to allow shader to invert
    % y-coordinate of final dots properly to account for difference in
    % y-axis direction of Screen()'s reference frame vs. OpenGL default
    % frame:
    glUniform1f(glGetUniformLocation(ctx.createBGDotsShader, 'ViewportHeight'), ctx.silhouetteHeight + 1);
    
    glUseProgram(0);

    
    % Ok, all PTB managed buffers and shaders loaded and set up.
    % Lets create the VBO that we need to actually render anything in the
    % end. VBO's are not supported yet by PTB's Screen, so we need to
    % switch to our GL context for setup:
    SwitchToGL(ctx.parentWin);
    
    ctx.FGvbo = glGenBuffers(1);
    glBindBuffer(GL.ARRAY_BUFFER, ctx.FGvbo);

    % Calculate size of VBO in bytes: Number of potential foreground dots
    % times 4 components per dot (RGBA == xyzw) times 4 Bytes per float
    % component:
    buffersize = ctx.maxFGDots * 4 * 4;
            
    % Allocate but don't initialize it, ie NULL pointer == 0
    glBufferData(GL.ARRAY_BUFFER, buffersize, 0, GL.STREAM_COPY);
            
    % Done.
    glBindBuffer(GL.ARRAY_BUFFER, 0);
            
    % Setup another VBO for the vertex indices:
    ctx.FGibo = glGenBuffers(1);
    glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, ctx.FGibo);

    % Allocate buffer for number of vertex indices,
    % each taking up 4 Bytes (== sizeof(uint32)) of memory.
    % Initialize immediately with indices and tell
    % OpenGL that this won't change at all during operation
    % (STATIC_DRAW):
    fgdotindices = uint32(0:ctx.maxFGDots-1);
    glBufferData(GL.ELEMENT_ARRAY_BUFFER_ARB, ctx.maxFGDots * 4, fgdotindices, GL.STATIC_DRAW);
    glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, 0);


    ctx.BGvbo = glGenBuffers(1);
    glBindBuffer(GL.ARRAY_BUFFER, ctx.BGvbo);

    % Calculate size of VBO in bytes: Number of potential background dots
    % times 4 components per dot (RGBA == xyzw) times 4 Bytes per float
    % component:
    buffersize = ctx.maxBGDots * 4 * 4;
            
    % Allocate but don't initialize it, ie NULL pointer == 0
    glBufferData(GL.ARRAY_BUFFER, buffersize, 0, GL.STREAM_COPY);
            
    % Done.
    glBindBuffer(GL.ARRAY_BUFFER, 0);
            
    % Setup another VBO for the vertex indices:
    ctx.BGibo = glGenBuffers(1);
    glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, ctx.BGibo);

    % Allocate buffer for number of vertex indices,
    % each taking up 4 Bytes (== sizeof(uint32)) of memory.
    % Initialize immediately with indices and tell
    % OpenGL that this won't change at all during operation
    % (STATIC_DRAW):
    bgdotindices = uint32(0:ctx.maxBGDots-1);
    glBufferData(GL.ELEMENT_ARRAY_BUFFER_ARB, ctx.maxBGDots * 4, bgdotindices, GL.STATIC_DRAW);
    glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, 0);
    
    % Done with VBO setup.
    
    % Restore previous GL context binding:
    RestoreGL;

    % Set batch zero as starting batch:
    ctx.currentBatch = 0;

    % Clear vbosready state:
    ctx.vbosready = 0;
    
    % We're ready for the show!
    initialized = initialized + 1;

    % Init for this 'ctx' context done: Return it to usercode:
    varargout{1} = ctx;
    
    return;
end

% Destroy processing context -- Release all ressources and shaders:
if strcmpi(cmd, 'DestroyContext')
    if nargin < 2
        error('In "DestroyContext": You must provide the "context" to destroy!');
    end

    % Get context object:
    ctx = varargin{1};
    
    BackupGL;
    
    SwitchToGL(ctx.parentWin);
    
    % Delete VBO's:
    glDeleteBuffers(1, ctx.FGibo);
    glDeleteBuffers(1, ctx.FGvbo);
    glDeleteBuffers(1, ctx.BGibo);
    glDeleteBuffers(1, ctx.BGvbo);
    
    SwitchToPTB;
    
    % Close all offscreen windows and their associated textures:
    Screen('Close', [ctx.BGDotsBuffer, ctx.FGDotsBuffer, ctx.trackingBuffer, ctx.silhouetteBuffer, ctx.sampleBuffer]);
    
    % Close our operators:
    Screen('Close', ctx.createFGDotsoperator);
    
    % Delete all shaders:
    glDeleteProgram(ctx.createBGDotsShader);
    glDeleteProgram(ctx.createFGDotsShader);
    glDeleteProgram(ctx.samplingShader);
    glDeleteProgram(ctx.trackingRenderShader);
    glDeleteProgram(ctx.silhouetteRenderShader);

    RestoreGL;
    
    % Shutdown done.
    initialized = max(initialized - 1, 0);
    
    % Return destroyed context:
    ctx = [];
    varargout{1} = ctx;

    return;
end

% Set string to call via feval() to initiate a 3D render cycle for the 3D
% scene/object to be visualized:
if strcmpi(cmd, 'SetRenderCallback')
    if nargin < 3
        error('In "SetRenderCallback": You must provide the "context" and callback string!');
    end

    % Get context object:
    ctx = varargin{1};
    
    % Get the eval string:
    renderCallback = varargin{2};
    if ~ischar(renderCallback)
        error('In "SetRenderCallback": Callback string must be a string, nothing else!');
    end
    
    % Assign:
    ctx.renderCallback = renderCallback;
    
    varargout{1} = ctx;

    return;
end

% Set state of engine to initial setting for a trial:
if strcmpi(cmd, 'ResetState')
    if nargin < 2
        error('In "ResetState": You must provide the "context"!');
    end

    % Get context object:
    ctx = varargin{1};
    
    BackupGL;
        
    % Reset to starting batch zero:
    ctx.currentBatch = 0;

    % Clear out all buffers:
    SwitchToPTB;
    Screen('FillRect', ctx.BGDotsBuffer, [0 0 0 0])
    Screen('FillRect', ctx.FGDotsBuffer, [0 0 0 0])
    Screen('FillRect', ctx.trackingBuffer, [0 0 0 0])
    Screen('FillRect', ctx.silhouetteBuffer, [0 0 0 0])
    Screen('FillRect', ctx.sampleBuffer, [0 0 0 0])

    % Clear vbosready state:
    ctx.vbosready = 0;
    
    RestoreGL;
    
    varargout{1} = ctx;    
    return;
end

% Update cycle, possibly followed by a render operation:
if strcmpi(cmd, 'Update')
    if nargin < 2
        error(sprintf('In "%s": You must provide the "context"!', cmd));
    end

    % Get context object:
    ctx = varargin{1};
    
    BackupGL;
        
    % Switch to OpenGL rendering context to be used for 3D scene rendering,
    % and specifically for our silhouette render buffer:
    SwitchToPTB;
    
    Screen('BeginOpenGL', ctx.silhouetteBuffer);

    % Perform 1st 3D render pass:

    % Need zBuffer occlusion testing for silhouette rendering:
    glEnable(GL.DEPTH_TEST);
    
    % Need cleared buffer, including z buffer:
    glClearColor(0,0,0,0);
    glClear;

    % Bind shader for silhouette rendering:
    if debug~=-1
        % We skip this if debug flag == -1 -- In that case the user wants
        % to see the real rendered image instead of our silhouette
        % encoding.
        glUseProgram(ctx.silhouetteRenderShader);
    end
    
    % Set viewport and scissor to full target window area:
    glViewport(0, 0, ctx.silhouetteWidth, ctx.silhouetteHeight);
    glScissor(0, 0, ctx.silhouetteWidth, ctx.silhouetteHeight);

    % Call the render callback function in workspace of our caller. We did
    % not touch the modelview- or projection matrices, so the projections
    % et al. should be ok...
    evalin('caller', ctx.renderCallback);

    % Don't need depth testing anymore:
    glDisable(GL.DEPTH_TEST);

    Screen('EndOpenGL', ctx.silhouetteBuffer);
    % Silhouette should be ready in silhouette buffer...

    if abs(debug) == 1
        Screen('DrawTexture', ctx.parentWin, ctx.silhouetteBuffer, [], [], [], [], [], debugGain);
    end
    
    % Perform 2nd "pseudo 3D" render pass into trackingBuffer. This will
    % again render the geometry, but with different encoding. A unwrapped
    % texture map will be output, where each pixel corresponds to a surface
    % point on the 3D object (aka texture coordinate). The color of each
    % pixel encodes interpolated screen space (x,y,z) coordinates:
    Screen('BeginOpenGL', ctx.trackingBuffer);
    
    % No depth test here, as fragment depths doesn't encode anything
    % meaningful during this pass:
    glDisable(GL.DEPTH_TEST);
    
    % Bind shader for tracking image rendering:
    glUseProgram(ctx.trackingRenderShader);
    
    % Set viewport and scissor to full trackbuffer window area:
    glViewport(0, 0, ctx.texResolution(1), ctx.texResolution(2));
    glScissor(0, 0, ctx.texResolution(1), ctx.texResolution(2));
    
    % Call the render callback function in workspace of our caller. We did
    % not touch the modelview- or projection matrices, so the projections
    % et al. should be ok...
    evalin('caller', ctx.renderCallback);
    
    % Unbind all shaders:
    glUseProgram(0);

    % Just to make sure it's still off:
    glDisable(GL.DEPTH_TEST);

    % Trackingbuffer should be ready:
    Screen('EndOpenGL', ctx.trackingBuffer);

    if debug == 2
        Screen('DrawTexture', ctx.parentWin, ctx.trackingBuffer, [], [], [], [], [], debugGain);
    end
    
    % We are in Screen()'s rendering context. Do the 2D image processing
    % stuff:

    % Need to attach the silhouette Buffers
    % color buffer texture to texture units 1 and 2: As the texture is part of a
    % color buffer attachment, it is set to nearest neighbour sampling -
    % which is what we want:
    glActiveTexture(GL.TEXTURE1);
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, ctx.silhouetteTexture);
    glActiveTexture(GL.TEXTURE2);
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, ctx.silhouetteTexture);
    glActiveTexture(GL.TEXTURE0);    
    
    % --- BACKGROUND DOTS COMPUTATION ---
    
    % Perform update of background sample buffer with random samples:
    % Compute random sample locations in image via Matlabs/Octaves uniform
    % random number generator:
    randomSamples = rand(ctx.BGsampleLinesPerBatch, ctx.BGsamplesPerLine, 2);
    randomSamples(:,:,1) = randomSamples(:,:,1) * ctx.silhouetteWidth;
    randomSamples(:,:,2) = randomSamples(:,:,2) * ctx.silhouetteHeight;
    sline = ctx.currentBatch * ctx.BGsampleLinesPerBatch + 1;
    eline = sline + ctx.BGsampleLinesPerBatch - 1;
    ctx.BGSampleSet(sline:eline, :, :) = randomSamples;

    % Background batch in background sample buffer updated. Convert whole
    % buffer to texture, with background sampling shader bound:
    BGsampleTex = Screen('MakeTexture', ctx.parentWin, ctx.BGSampleSet, [], [], 2, 0, ctx.createBGDotsShader);

    % Blit to background dots buffer:
    Screen('DrawTexture', ctx.BGDotsBuffer, BGsampleTex, [], [], [], 0);

    % Release texture:
    Screen('Close', BGsampleTex);

    % --- FOREGROUND DOTS COMPUTATION ---
        
    % Perform update of current batch of sampleBuffer:
    
    % Compute random sample locations in image via Matlabs/Octaves uniform
    % random number generator:
    randomSamples = rand(ctx.sampleLinesPerBatch, ctx.samplesPerLine, 2);
    randomSamples(:,:,1) = randomSamples(:,:,1) * ctx.silhouetteWidth;
    randomSamples(:,:,2) = randomSamples(:,:,2) * ctx.silhouetteHeight;

    % Create 32bpc float texture (setting '2') with the random sample
    % locations. If we'd use the texture inside Screen('TransformTexture')
    % we could use textureOrientation setting '3' for isotropic random noise.
    % This settings would save some texture conversion time then. However,
    % as we're only 'DrawTexture'ing and the used blit shaders are position
    % invariant (isotropic), a setting of 0 or 1 also works with no speed
    % loss, as 'DrawTexture' does implicit optimization:
    sampleTex = Screen('MakeTexture', ctx.parentWin, randomSamples, [], [], 2, 0, ctx.samplingShader);
    
    if debug == 3
        Screen('DrawTexture', ctx.parentWin, sampleTex, [], [], [], [], [], debugGain);
    end
    
    % Blit sampleTex into the target batch rectangle of our sampleBuffer,
    % with the sampling shader bound.
    
    % Blit texture at target location into sampleBuffer, offset vertically
    % so the proper batch gets updated. The secondary texture unit provides
    % access to the silhouette image, the shader does bilinear filtering
    % and conversion:
    Screen('DrawTexture', ctx.sampleBuffer, sampleTex, [], OffsetRect(Screen('Rect', sampleTex), 0, double(ctx.currentBatch * ctx.sampleLinesPerBatch)), 0, 0);

    % Release sampleTex for next cycle:
    Screen('Close', sampleTex);

    if debug == 4
        Screen('DrawTexture', ctx.parentWin, ctx.sampleBuffer, [], [], [], [], [], debugGain);
    end
    
    % Our total distribution of foreground samples in sampleBuffer is now
    % up to date. Use stored surface (texcoords) sample locations to read
    % out corresponding locations in trackingBuffer, convert them into
    % vertex location definitions and blit them to the final buffer of
    % foreground dot specs. Screen('TransformTexture') will do the blit and
    % bind sampleBuffer and trackingBuffer as input textures:
    ctx.FGDotsBuffer = Screen('TransformTexture', ctx.sampleBuffer, ctx.createFGDotsoperator, ctx.trackingBuffer, ctx.FGDotsBuffer);
    
    % Ok, the ctx.FGDotsBuffer should contain the encoded set of all
    % foreground dot positions. One can either read this buffer back into a
    % Matlab/Octave matrix for usercode processing, or convert it into a
    % VBO via PBO mechanism, then render it.
    
    % Clear out all intermediate result buffers in preparation of next update cycle:
    Screen('FillRect', ctx.trackingBuffer, [0 0 0 0])
    Screen('FillRect', ctx.silhouetteBuffer, [0 0 0 0])

    if debug == 5
        Screen('DrawTexture', ctx.parentWin, ctx.FGDotsBuffer, [], [], [], [], [], debugGain);
    end
    
    % Clear vbosready state to trigger a refill on next 'Render':
    ctx.vbosready = 0;
    
    RestoreGL;
    
    % Increment batch counter for next update cycle:
    ctx.currentBatch = mod(ctx.currentBatch+1, ctx.dotLifetime);

    % Ready for render: Return updated context:
    varargout{1} = ctx;
    
    return;
end

% Render current result in ctx.FGDotsBuffer into parent window:
if strcmpi(cmd, 'Render')
    if nargin < 2
        error(sprintf('In "%s": You must provide the "context"!', cmd));
    end

    % Get context object:
    ctx = varargin{1};

    if nargin < 3
        targetWin = ctx.parentWin;
    else
        targetWin = varargin{2};
        if ~ismember(Screen('WindowKind', targetWin), [-1, 1])
            error('In "Render": Invalid "targetWindow" handle provided. Not an offscreen or onscreen window!');
        end
    end
    
    if nargin < 4
        drawspec = [1,1];
    else
        drawspec = varargin{3};
    end

    drawFG = drawspec(1);
    drawBG = drawspec(2);

    BackupGL;

    % Can do this in PTB's Screen 2D context, which is more convenient for
    % our 2D drawing operations, as long as we are careful to restore any
    % changed context state:
    SwitchToPTB;

    % Update of VBO's needed?
    if ~ctx.vbosready
        % Yes: Copy content of FGDotsBuffer into VBO, using PBO extension.
        % The 'GetWindowInfo' binds our ctx.FGDotsBuffer FBO so we can
        % glReadPixels() from it:
        Screen('GetWindowInfo', ctx.FGDotsBuffer);

        % There is a bug in the X1000 gfx-card driver on OS/X 10.4.11 which
        % causes glReadPixels() readback values to get clamped to 0-1 range
        % if alpha-blending is enabled. Therefore we need to disable alpha
        % blending during glReadPixels() readback and reenable later if
        % needed:
        alphaenabled = glIsEnabled(GL.BLEND);
        glDisable(GL.BLEND);
        
        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, ctx.FGvbo);        
        glReadPixels(0, 0, ctx.samplesPerLine, ctx.sampleLinesTotal, GL.RGBA, GL.FLOAT, 0);
        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, 0);
        
        Screen('GetWindowInfo', ctx.BGDotsBuffer);
        
        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, ctx.BGvbo);        
        glReadPixels(0, 0, ctx.BGsamplesPerLine, ctx.BGsampleLinesTotal, GL.RGBA, GL.FLOAT, 0);
        glBindBuffer(GL.PIXEL_PACK_BUFFER_ARB, 0);

        % Reenable alpha blending if it was enabled:
        if alphaenabled
            glEnable(GL.BLEND);
        end
        
        % VBO's ready:
        ctx.vbosready = 1;        
    end
    
    % Setup render:

    % The 'GetWindowInfo' binds our ctx.parentWin so we can render to it:
    Screen('GetWindowInfo', targetWin);
        
    % Bind and enable vertex position VBO:
    glEnableClientState(GL.VERTEX_ARRAY);
    
    if drawFG
        % Foreground render:
        glBindBuffer(GL.ARRAY_BUFFER, ctx.FGvbo);
        glVertexPointer(4, GL.FLOAT, 0, 0);

        % Bind vertex index VBO:
        glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, ctx.FGibo);

        % Perform draw operation: All vertices, each triggering render for a
        % single GL.POINT primitive. Colors, sizes, anti-aliasing flags etc.
        % can be set from external code as appropriate. Application of textures
        % or shaders is also possible:
        glDrawRangeElements(GL.POINTS, 0, ctx.maxFGDots-1, ctx.maxFGDots, GL.UNSIGNED_INT, 0);
    end
    
    if drawBG
        % Background render:
        glBindBuffer(GL.ARRAY_BUFFER, ctx.BGvbo);
        glVertexPointer(4, GL.FLOAT, 0, 0);

        % Bind vertex index VBO:
        glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, ctx.BGibo);

        % Perform draw operation: All vertices, each triggering render for a
        % single GL.POINT primitive. Colors, sizes, anti-aliasing flags etc.
        % can be set from external code as appropriate. Application of textures
        % or shaders is also possible:
        glDrawRangeElements(GL.POINTS, 0, ctx.maxBGDots-1, ctx.maxBGDots, GL.UNSIGNED_INT, 0);
    end
    
    % Unbind our VBOs:
    glBindBuffer(GL.ELEMENT_ARRAY_BUFFER_ARB, 0);
    glBindBuffer(GL.ARRAY_BUFFER, 0);

    % Disable vertex array:
    glDisableClientState(GL.VERTEX_ARRAY);
    
    % Render completed. Restore pre-render state:
    RestoreGL;
    
    % Rendering done: Return updated context:
    varargout{1} = ctx;
    
    return;
end

if strcmpi(cmd, 'GetResults')
    if nargin < 2
        error(sprintf('In "%s": You must provide the "context"!', cmd));
    end

    % Get context object:
    ctx = varargin{1};

    BackupGL;
    
    % Readback as "image matrix":

    % Can do this in Screen-Mode:
    SwitchToPTB;
    
    % 'GetWindowInfo' selects ctx.FGDotsBuffer as active FBO, so we can
    % readback from it:
    Screen('GetWindowInfo', ctx.FGDotsBuffer);
    
    % There is a bug in the X1000 gfx-card driver on OS/X 10.4.11 which
    % causes glReadPixels() readback values to get clamped to 0-1 range
    % if alpha-blending is enabled. Therefore we need to disable alpha
    % blending during glReadPixels() readback and reenable later if
    % needed:
    alphaenabled = glIsEnabled(GL.BLEND);
    glDisable(GL.BLEND);

    % Readback to matrix: Cast from float aka single() type to double() type:
    readbackdata = double(glReadPixels(0, 0, ctx.samplesPerLine, ctx.sampleLinesTotal, GL.RGBA, GL.FLOAT));
    
    % Return readback (x,y) dot locations, reshaped into a 2 rows array
    % with one column per (x,y) dot:
    varargout{1} = [ reshape(readbackdata(:,:,1), 1, ctx.samplesPerLine * ctx.sampleLinesTotal) ; reshape(readbackdata(:,:,2), 1, ctx.samplesPerLine * ctx.sampleLinesTotal) ];

    Screen('GetWindowInfo', ctx.BGDotsBuffer);
    
    % Readback to matrix: Cast from float aka single() type to double() type:
    readbackdata = double(glReadPixels(0, 0, ctx.BGsamplesPerLine, ctx.BGsampleLinesTotal, GL.RGBA, GL.FLOAT));
    
    % Return readback (x,y) dot locations, reshaped into a 2 rows array
    % with one column per (x,y) dot:
    varargout{2} = [ reshape(readbackdata(:,:,1), 1, ctx.BGsamplesPerLine * ctx.BGsampleLinesTotal) ; reshape(readbackdata(:,:,2), 1, ctx.BGsamplesPerLine * ctx.BGsampleLinesTotal) ];

    % Reenable alpha blending if it was enabled:
    if alphaenabled
        glEnable(GL.BLEND);
    end
    
    % Readback completed. Restore pre-readback state:
    RestoreGL;
    
    return;
end

if strcmpi(cmd, 'DebugFlag')
    if nargin < 2
        error('Must provide new setting for debug flag!');
    end
    
    debug = varargin{1};
    
    if nargin < 3
        debugGain = [];
    else
        if length(varargin{2})~=4
            error('In "DebugFlag": "debugGain" color modulation gain must be a 4 element [R,G,B,A] modulation color vector!');
        end
        
        debugGain = varargin{2};
    end
    return;
end

error(sprintf('Invalid subcommand ''%s'' specified!', cmd));
return;

% Internal helper functions:
function SwitchToGL(win)

% Switch to our OpenGL context, but keep a backup of original
% drawstate. We do lazy switching if possible:
[currentwin, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

if ~IsOpenGLRendering
    % PTB's context active: Switch to OpenGL rendering for our parent window:
    Screen('BeginOpenGL', win);
else
    % In rendering context. Is it ours? If yes, then there isn't anything
    % to do...
    if currentwin ~= win
        % No, a different windows context is active: First switch to PTB
        % mode, then switch to ours:
        
        % Switch to our parentWin's PTB context:
        Screen('EndOpenGL', currentwin);
        % Switch to our parentWin's GL context:
        Screen('BeginOpenGL', win);
    end
end
return;

function SwitchToPTB

% Switch from our OpenGL context, but keep a backup of original
% drawstate. We do lazy switching if possible:
[currentwin, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

if ~IsOpenGLRendering
    % PTB's context is already active: Nothing to do.
else
    % In rendering context. Switch back to PTB - and to our parentWin:
    Screen('EndOpenGL', currentwin);
end
return;

function BackupGL
global moglFDF_OriginalContext;

if ~isempty(moglFDF_OriginalContext)
    error('BackupGL called twice in succession without intermediate RestoreGL! Ordering inconsistency!');
end

[currentwin, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

if IsOpenGLRendering
    moglFDF_OriginalContext = currentwin;
end
return;

function RestoreGL
global moglFDF_OriginalContext;

[currentwin, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');

if isempty(moglFDF_OriginalContext)
    % PTB was in Screen drawing mode: Switch to that mode, if not active:
    if IsOpenGLRendering
        Screen('EndOpenGL', currentwin);
    end
    return;
end

% Need to restore to GL context if not already active:
if ~IsOpenGLRendering
    Screen('BeginOpenGL', moglFDF_OriginalContext);
else
    % OpenGL context active. Ours? If so -> Nothing to do.
    if currentwin ~= moglFDF_OriginalContext
        % Nope. Need to switch:
        Screen('EndOpenGL', currentwin);
        Screen('BeginOpenGL', moglFDF_OriginalContext);        
    end
end

% Restore to default:
moglFDF_OriginalContext = [];

return;
