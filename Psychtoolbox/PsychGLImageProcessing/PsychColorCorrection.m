function varargout = PsychColorCorrection(cmd, varargin)
% varargout = PsychColorCorrection(cmd, varargin)
%
% This function is used to setup and control the built-in color correction 
% and transformation mechanisms of PTB's imaging pipeline. It allows to choose
% among different methods of color correction (for calibrated output of
% stimuli) and to change parameters of the chosen method. This only works
% when the imaging pipeline is enabled and with certain output devices.
% 
% This functions are mostly meant to provide fast color correction, e.g.,
% gamma correction, when PTB is used with special high precision / HDR
% output devices, e.g., CRS Bits++ in Mono++ or Color++ mode, or video
% attenuators. When operating such devices, the standard gamma correction
% mechanisms of your graphics card can't be used, so PTB must do it itself.
%
% With a standard 8bpc framebuffer, you won't need this function for simple
% gamma correction or color correction.
% Instead you'd use the Screen('LoadNormalizedGammaTable') command to
% perform gamma correction with the graphics cards built-in gamma tables.
% Same applies to Bits++ box in Bits++ mode, using the built-in tables of
% the Bits++ device, also controlled via Screen('LoadNormalizedGammaTable').
%
% For more complex correction schemes than a lookup table transform you can
% still use this function, even with a normal 8 bit framebuffer, as long as
% the imaging pipeline is enabled. It's just important to note that you
% don't need to use it for a standard framebuffer in the simple case -- the
% gfx cards gamma tables are more efficient for the simple case.
%
% How to use:
%
% 1. Before opening an onscreen window, you use the following
% PsychImaging() setup call to specify the method of display color correction
% to apply, and the view channel to apply it to (in case there are multiple
% like in many stereo display setups):
%
% PsychImaging('AddTask', whichChannel, 'DisplayColorCorrection', methodname);
% - where whichChannel can be 'LeftView' or 'RightView' for the left- or
% right display output channel/device of a stereo setup, or
% 'FinalFormatting' if you have a single display monoscopic setup or want
% to apply the same color correction to both channels of a stereo setup.
% The parameter 'methodname' is a name string with the name of one of the
% supported methods - See overview below for supported methods.
%
%
% 2. Then, after you've specified all other window parameters via the
% PsychImaging() subcommands, you open the onscreen window via the usual
% win = PsychImaging('OpenWindow', ....); call, as always when you use the
% imaging pipeline. This will open the window and apply the chosen color
% correction method, choosing reasonable default parameters for the method
% at hand.
%
% 3. At any time in your script you can change the operating parameters of
% the chosen color correction method via the PsychColorCorrection()
% subfunctions mentioned below. You'll have to specify the window handle
% for the onscreen window whose parameters should be changed, and - in a
% stereo display setup with separate color correction parameters for each
% of the display channels - the name 'whichChannel' of the display channel
% to change, e.g., 'LeftView'. Changes take effect at next Screen('Flip').
%
%
%
% Supported display color correction methods:
% ===========================================
%
% These are the method names that can be passed as 'methodname' parameter to
% PsychImaging('DisplayColorCorrection', ....., methodname):
%
% 'methodname' is the name string of one of the supported methods:
%
% * 'None': Don't do anything. Fastest, but not safest. 
%
%   If your stimulus contains (by accident) color or luminance values outside
%   the displayable range, the corresponding pixels may show undefined output
%   color -- Likely not what you want. This mode is about 10% faster than
%   'ClampOnly'. This mode is the default if no method is selected, unless
%   you use Bits++ in Mono++ or Color++ mode, where 'ClampOnly' is the
%   default.
%
%
% * 'ClampOnly': Do not apply any color transformation. Default for Bits++
%   in Mono++ or Color++ mode:
%
%   In this mode, values are clamped against the limits set via
%   'SetColorClampingRange' (see below) to always keep them in the
%   requested range set via PsychColorCorrection('SetColorClampingRange'),
%   nothing else.
%
%
% * 'CheckOnly': Do not apply any color transformation.
%
%   In this mode, values are checked against the limits set via
%   PsychColorCorrection('SetColorClampingRange'). Values outside that
%   range are visually coded, so you should be able to see troublesome
%   areas via visual inspection: All values are first clamped, then
%   inverted, in the hope that this creates clearly detectable artifacts in
%   your stimulus. We may change the "marker" method in the future if we
%   happen to find a better visual marker. This mode is meant for debugging
%   your stimulation scripts, not for running them on your subjects!
%
%
% * 'SimpleGamma' : Apply simple power-law gamma correction:
%
%   Simple gamma correction means: Apply a simple power-law to incoming
%   values: outcolor = incolor ^ EncodingGamma. incolor is the uncorrected
%   input color or intensity, outcolor the corrected one, EncodingGamma is
%   encoding gamma value to apply. See PsychColorCorrection('SetEncodingGamma')
%   for how to set the gamma values. After gamma correction, output values
%   are clamped against the range set via PsychColorCorrection('SetColorClampingRange').
%
% * 'LookupTable' : Apply color correction by color table lookup, ie. a CLUT.
%
%   This will allow to pass in a color lookup table of selectable
%   granularity (ie., number of slots) and range, which is later on used to
%   lookup corresponding corrected color values for given framebuffer input
%   values.
%
%
% Supported runtime Subfunctions:
% ===============================
%
% The following routines must be called *after* opening a window for which color
% correction is enabled. They can be called anytime and changed settings
% will apply at the next Screen('Flip'). If your 'window' is actually a
% stereo display window, you may want or need to provide the optional
% 'viewId' parameter to tell PTB which of the two stereo view channels
% shall be changed in its settings. If both stereo views are displayed on
% the same physical display device, this is not needed. If the separate
% views go to separate physical displays, you may need to calibrate them
% separately. Allowable values for viewId are 'AllViews', 'LeftView' and
% 'RightView', corresponding to the 'whichChannel' setting that you used
% when setting up the window with PsychImaging().
%
% PsychColorCorrection('SetColorClampingRange', window, min, max [,viewId]);
% - Set the range of allowable output color or luminance intensity values
% to the interval [min; max] for onscreen window 'window'. Values outside
% that range get either clamped to the 'min'imum or 'max'imum value, or -
% in 'CheckOnly' mode - will be visually marked as out of range. The default
% range is [0.0 ; 1.0] -- The range that your display device can really
% display.
%
%
% PsychColorCorrection('SetEncodingGamma', window, gamma [,viewId]);
% - Set the gamma value to use for gamma correction on window 'window'.
% 'gamma' can be either a single scalar if the same gamma should apply to
% all color channels (or single luminance channel), or it can be a
% three-component [gammaRed, gammaGreen, gammaBlue] vector, if each color
% channel should be gamma corrected with an individual gamma value.
%
% How the value gets applied depends on the chosen method of color
% correction (see above). The simplest method ('SimpleGamma') performs a
% simple power-law mapping of input values to output values: out = in ^ gamma.
% 'in' must be greater than zero, and 'gamma' must be greater than zero,
% otherwise results may be undefined, depending on your graphics hardware.
% However, usually only encoding 'gamma' values in the range of about
% 0.33 - 1.0 are meaningful.
%
% Example: If your monitor has a "decoding gamma" of 1.8, the proper
% setting for 'gamma' would be gamma = 1/1.8. For a decoding gamma of 2.2,
% you'd choose gamma = 1/2.2 ...
%
%
% PsychColorCorrection('SetLookupTable', window, clut [, viewId][, maxinput=1][, scalefactor]);
% - Assign color lookup table 'clut' for use with color correction method
% 'LookupTable'. 'clut' must be a 1 column vector for pure luminance lookup
% tables, or a 3 column matrix for RGB color lookup tables with one column
% per color channel, ie., [1,2,3] = [Red, Green, Blue]. clut must have at
% least 1 row, but usually will have way more than 2 rows, typically almost
% as many rows as n = 2^bpc for a given output device bitdepths bpc. For a
% 10 bit output device, n would be usually 2^10 = 1024 rows for a perfect
% one-to-one mapping. At runtime, color correction will be performed by the
% following formula: Be Rin, Gin, Bin the input red, green and blue color
% components, and Rout, Gout, Bout the final output value for the
% framebuffer. First Rin, Gin and Bin are clamped individually to the range
% 0.0 - 'maxinput' (maxinput is 1.0 by default), scalefactor is chosen by
% default as scalefactor = [number of rows in clut - 1] / maxinput, ie., it
% maps the possible input range 0 - maxinput to the full range of row
% indices 1 - rowcount to cover the full range of entries stored in the
% clut. This is the most reasonable default, but can be changed by the
% optional 'scalefactor' and 'maxinput' arguments.
%
% Then the output color for each component is looked up in the proper slot
% (= row index) of the passed clut:
%
% Rout = clut(Rin * scalefactor,1);
% Gout = clut(Gin * scalefactor,2);
% Bout = clut(Bin * scalefactor,3);
%
% Color values for fractional indices inbetween reference values in the
% clut are interpolated linearly between the two nearest neighbour
% reference values --> linear interpolation.
%
% Finally, Rout, Gout and Bout are clamped to the valid output range as set
% by the function PsychColorCorrection('SetColorClampingRange', ...); by
% default to the range 0.0 - 1.0.
%
%
%
% Internal commands, usually not meant for direct use by pure mortals:
% ====================================================================
%
% All these methods are usually called from within PsychImaging() to do the
% dirty setup work...
%
% Call this *before* opening a window:
%
% PsychColorCorrection('ChooseColorCorrection', methodname);
% - Specify the method to be used for color correction for the next
% onscreen window that will be opened. This needs to be called *before* the
% window is opened, however its usually done automatically at the right
% moment by routines like PsychImaging() or BitsPlusPlus() if you use these
% to open windows.
%
% Called after Screen('OpenWindow') during shader and pipeline setup:
%
% [shader, idstring, configString] = PsychColorCorrection('GetCompiledShaders', window, debuglevel);
% - Compile corresponding shaders for chosen color correction method,
% return shaderhandles and idstring to calling routine. That routine will
% link the returned shaders with other shader code to produce the final
% GLSL program object for color conversion and output formatting. 'shader'
% is the GLSL shader handle, idstring the name string for the shader,
% configString the shader option string for the 'Hookfunction' call, e.g.,
% to bind additional LUT textures, etc.
%
%
% Called after linking and attaching the final processing GLSL program
% objects and slots to the imaging pipelines hook chain(s):
%
% PsychColorCorrection('ApplyPostGLSLLinkSetup', window, viewId);
% - Perform whatever setup work is needed after final GLSL program object
% has been created and attached to imaging pipeline.
%

% History:
% 16.04.2008 Written (MK).
% 18.05.2008 Revised, improved help text, fine-tuning etc. (MK)
% 04.07.2009 Add CLUT based color correction. (MK)

% GL is needed for shader setup and parameter changes:
global GL;
persistent specReady;
persistent icmSpec;
persistent icmDataForHandle;

if isempty(specReady)
    % We default to a setting of 'ClampOnly' if no spec specified. Usercode can
    % override this...
    specReady = 1;
    icmSpec.type = 'ClampOnly';
end

% Child protection:
if nargin < 1
    error('Subcommand specification missing!');
end
        
if isempty(cmd)
    error('Subcommand specification missing!');
end
       
% Subcommand dispatch:

if strcmpi(cmd, 'ChooseColorCorrection')

    if nargin < 2
        error('You must specify a color-correction method!');
    end
    
    % Assign method, that's it...
    icmSpec.type = varargin{1};
    specReady = 1;
    
    return;
end


% Retrieve a (previously specified) collection of precompiled shader
% objects. These implement the subroutines for color processing, according
% to given spec.
if strcmpi(cmd, 'GetCompiledShaders')
    if ~specReady
        error('"GetCompiledShaders" called, but specification of what to compile is unavailable!');
    end
    
    if nargin >= 3
        debuglevel = varargin{2};
    else
        debuglevel = 0;
    end
    
    % Need GL from here on...
    if isempty(GL)
        InitializeMatlabOpenGL([], [], 1);
    end

    % No config string by default:
    icmConfig = '';
    
    switch(icmSpec.type)
        case {'None'}
            % Load pass-through shader:
            icmShaders = LoadShaderFromFile('ICMPassThroughShader.frag.txt', [], debuglevel);
            icmIdString = sprintf('ICM:%s', icmSpec.type);

        case {'ClampedNoName'}
            % Load clamped pass-through shader:
            icmShaders = LoadShaderFromFile('ICMClampedPassThroughShader.frag.txt', [], debuglevel);
            % but set its id string to some special string that is hidden from standard search:
            icmIdString = 'CMIH:ClampedNoName';

        case {'ClampOnly'}
            % Load clamped pass-through shader:
            icmShaders = LoadShaderFromFile('ICMClampedPassThroughShader.frag.txt', [], debuglevel);
            icmIdString = sprintf('ICM:%s', icmSpec.type);

        case {'CheckOnly'}
            % Load checked pass-through shader:
            icmShaders = LoadShaderFromFile('ICMCheckedPassThroughShader.frag.txt', [], debuglevel);
            icmIdString = sprintf('ICM:%s', icmSpec.type);

        % Simple power-law bog-standard gamma correction.
        case {'SimpleGamma'}
            % Load our bog-standard power-law shader:
            icmShaders = LoadShaderFromFile('ICMSimpleGammaCorrectionShader.frag.txt', [], debuglevel);
            icmIdString = sprintf('ICM:%s', icmSpec.type);
            
        % Color correction by CLUT texture lookup table operation:
        case {'LookupTable'}
            % Load our bog-standard power-law shader:
            icmShaders = LoadShaderFromFile('ICMCLUTCorrectionShader.frag.txt', [], debuglevel);
            icmIdString = sprintf('ICM:%s', icmSpec.type);

            % Generate texture handle for fillout later on:
            icmSpec.icmlutid = glGenTextures(1);
            
            % Build config string to bind and use our CLUT texture:
            icmConfig = sprintf('TEXTURERECT2D(2)=%i', icmSpec.icmlutid);            
        otherwise
            error('Unknown type of color correction requested! Internal bug?!?');
    end
    
    
    % Return Vector of shader handles:
    varargout{1} = icmShaders;

    % Return id string:
    varargout{2} = icmIdString;

    % Return shader config string:
    varargout{3} = icmConfig;
    
    return;
end

if strcmpi(cmd, 'ApplyPostGLSLLinkSetup')
    if ~specReady
        error('"GetCompiledShaders" called, but specification of what to compile is unavailable!');
    end
    
    % Need GL from here on...
    if isempty(GL)
        error('No GL struct defined in "ApplyPostGLSLLinkSetup"?!? This is a bug - Check code!!');
    end
    
    if nargin < 3
        error('Must provide window handle to onscreen window as 2nd argument and viewId as 3rd one!');
    end
    
    % Fetch window handle:
    win = varargin{1};
    viewId = varargin{2};
    
    % Retrieve all params for 'win'dow and given icmSpec, bind shader:
    [slot shaderid blittercfg voidptr glsl luttexid] = GetSlotForTypeAndBind(win, icmSpec.type, viewId); %#ok<NASGU>
    
    try
        % Setup initial clamping values to valid range 0.0 - 1.0:
        glUniform2f(glGetUniformLocation(glsl, 'ICMClampToColorRange'), 0.0, 1.0);
        
        switch(icmSpec.type)
            case {'ClampOnly', 'CheckOnly', 'None', 'ClampedNoName'}
                % Nothing to do yet...

            case {'SimpleGamma'}
                % Set default encoding gamma for power-law shader to (1.0, 1.0, 1.0):
                glUniform3f(glGetUniformLocation(glsl, 'ICMEncodingGamma'), 1.0, 1.0, 1.0);
            case {'LookupTable'}
                % Set CLUT texture unit to 2:
                glUniform1i(glGetUniformLocation(glsl, 'ICMCLUT'), 2);
                % Setup everything to a pretty meaningless but safe
                % mapping, which will likely just produce all-white,
                % regardless of input:
                glUniform1f(glGetUniformLocation(glsl, 'ICMPrescale'), 1.0);
                glUniform1f(glGetUniformLocation(glsl, 'ICMMaxInputValue'), 1.0);
                % Note that we won't setup the CLUT texture yet. This is a
                % mandatory step after initial setup of the display. We do
                % store the texture id of the clut texture in a permanent
                % location though:
                icmDataForHandle(win) = icmSpec.icmlutid;
                
            otherwise
                error('Unknown type of color correction requested! Internal bug?!?');
        end
    catch
        % Empty...
        psychrethrow(psychlasterror);
    end
    
    % Unbind shader:
    glUseProgram(0);
    
    return;
end

if strcmpi(cmd, 'SetColorClampingRange')
    
    % Need GL from here on...
    if isempty(GL)
        error('No internal GL struct defined in "SetXXX" routine?!? This is a bug - Check code!!');
    end
    
    if nargin < 2
        error('Must provide window handle to onscreen window as 2nd argument!');
    end

    if nargin < 4
        error('Must provide minimum and maximum allowable value in 3rd and 4th argument!');
    end
    
    % Fetch window handle:
    win = varargin{1};
    
    % Fetch values:
    minc = varargin{2};
    maxc = varargin{3};
    
    if ~isnumeric(minc) || ~isnumeric(maxc)
        error('(min, max) values must be numbers!');
    end
    
    if minc > maxc
        error('Provided minimum value greater than maximum value - This will not work!');
    end

    if nargin < 5
        viewId = [];
    else
        viewId = varargin{4};
    end
    
    % Retrieve all params for 'win'dow and given icmSpec, bind shader. The
    % 'icmSpec' string is empty - This will expand into the general 'ICM:'
    % string, so we use the first slot/shader with the ICM: token.
    icmId = '';
    [slot shaderid blittercfg voidptr glsl luttexid] = GetSlotForTypeAndBind(win, icmId, viewId); %#ok<NASGU>
    
    try
        % Setup clamping values to given range:
        glUniform2f(glGetUniformLocation(glsl, 'ICMClampToColorRange'), minc, maxc);        
    catch
        % Empty...
        psychrethrow(psychlasterror);
    end
    
    % Unbind shader:
    glUseProgram(0);
    
    return;
end

if strcmpi(cmd, 'SetEncodingGamma')
    
    % Need GL from here on...
    if isempty(GL)
        error('No internal GL struct defined in "SetXXX" routine?!? This is a bug - Check code!!');
    end
    
    if nargin < 2
        error('Must provide window handle to onscreen window as 2nd argument!');
    end

    if nargin < 3
        error('Must provide encoding gamma value or vector of 3 encoding gammas for R,G,B in 3rd argument!');
    end
    
    % Fetch window handle:
    win = varargin{1};
    
    % Fetch values:
    gammas = varargin{2};
    
    if ~isnumeric(gammas)
        error('Power law encoding gamma value(s) must be number(s)!');
    end
    
    if length(gammas)~=1 && length(gammas)~=3
        error('Encoding gamma must be a single scalar or a 3 component vector of separate gammas for the Red, Green and Blue color channel!');
    end
    
    if any(min(gammas) < 0) || any(max(gammas) > 1)
        warning(sprintf('At least one of the encoding gamma values %f is outside the sensible range 0.0 - 1.0.\nThis will result in undefined behaviour and is likely not what you want.', gammas)); %#ok<WNTAG,SPWRN>
    end

    if length(gammas) == 1
        % Replicate to all three channels:
        gammas = [gammas, gammas, gammas];
    end
    
    if nargin < 4
        viewId = [];
    else
        viewId = varargin{3};
    end
    
    % Retrieve all params for 'win'dow and given icmSpec, bind shader. The
    % 'icmSpec' string is empty - This will expand into the general 'ICM:'
    % string, so we use the first slot/shader with the ICM: token.
    icmId = '';
    [slot shaderid blittercfg voidptr glsl luttexid] = GetSlotForTypeAndBind(win, icmId, viewId); %#ok<NASGU>
    
    try
        % Set encoding gamma for power-law shader:
        uloc = glGetUniformLocation(glsl, 'ICMEncodingGamma');
        if uloc >= 0
            glUniform3f(uloc, gammas(1), gammas(2), gammas(3));
        else
            error('Tried to set encoding gamma for color correction, but color correction not configured for use of encoding gamma!');
        end
    catch
        % Empty...
        psychrethrow(psychlasterror);
    end
    
    % Unbind shader:
    glUseProgram(0);
    
    return;
end

if strcmpi(cmd, 'SetLookupTable')
    
    % Need GL from here on...
    if isempty(GL)
        error('SetLookupTable: No internal GL struct defined in "SetLookupTable" routine?!? This is a bug - Check code!!');
    end
    
    if nargin < 2
        error('SetLookupTable: Must provide window handle to onscreen window as 2nd argument!');
    end

    if nargin < 3
        error('SetLookupTable: Must provide CLUT matrix for which a color lookup table should be built!');
    end
    
    % Fetch window handle:
    win = varargin{1};
    
    % Fetch clut:
    clut = varargin{2};
    
    if ~isnumeric(clut)
        error('SetLookupTable: CLUT matrix must contain number(s)!');
    end
    
    if size(clut,2)~=1 && size(clut,2)~=3
        error('SetLookupTable: Encoding CLUT must be a one column luminance vector or three column matrix for the Red, Green and Blue color channel!');
    end

    if size(clut,1) < 2
        error('SetLookupTable: Encoding CLUT must have at least 2 rows, or at least 2 elements for a luminance vector!');
    end

    if nargin < 4
        viewId = [];
    else
        viewId = varargin{3};
    end
    
    % Optional max input value provided? Assign most common 1.0 if not:
    if nargin < 5
        ICMMaxInputValue = 1.0;
    else
        ICMMaxInputValue = varargin{4};
    end

    % Optional scaling factor provided? Assign proper scaler for clut size,
    % and max input value if not:
    if nargin < 6
        ICMPrescale = ( size(clut,1) -1 ) / ICMMaxInputValue;
    else
        ICMPrescale = varargin{4};
    end

    % Retrieve all params for 'win'dow and our 'LookupTable' icmSpec, bind shader.
    icmId = 'LookupTable';
    [slot shaderid blittercfg voidptr glsl luttexid] = GetSlotForTypeAndBind(win, icmId, viewId); %#ok<NASGU>
    
    try
        % Setup initial clamping values to valid range 0.0 - maximum in passed CLUT:
        glUniform2f(glGetUniformLocation(glsl, 'ICMClampToColorRange'), 0.0, max(max(clut)));

        % Setup max input value and prescaler:
        glUniform1f(glGetUniformLocation(glsl, 'ICMMaxInputValue'),ICMMaxInputValue);
        glUniform1f(glGetUniformLocation(glsl, 'ICMPrescale'),ICMPrescale);        
    catch
        % Empty...
        psychrethrow(psychlasterror);
    end
    
    % Unbind shader:
    glUseProgram(0);
    
    if isempty(icmDataForHandle) || length(icmDataForHandle) < win || Screen('WindowKind',win) ~= 1
        error('SetLookupTable: Tried to assign clut to a non-onscreen window or one which doesn''t have "LookupTable" based color correction enabled!');
    end
    
    % Convert 'clut' to single(), so it is a float format for OpenGL:
    clut = single(clut);
    
    % Try to encode in highest precision format that the hardware can still
    % filter and process on the fast path:
    winfo = Screen('GetWindowInfo', win);
    if winfo.GLSupportsFilteringUpToBpc >= 32
        % Full 32 bits single precision float:
        internalFormat = GL.LUMINANCE_FLOAT32_APPLE;
        fprintf('PsychColorCorrection: Using a 32 bit float CLUT -> 23 bits effective linear output precision for color correction.\n');
    else
        % No hardware accelerated float32 filtering:
        if (winfo.GLSupportsTexturesUpToBpc >= 16) && ((winfo.BitsPerColorComponent > 8) || (max(max(clut)) > 255) || (min(min(clut)) < 0))
            % We have a greater than 8 bit framebuffer and support for 16
            % bpc float textures. Choose 16 bpc texture anyway to retain precision.
            % We will end up with no linear interpolation or hw
            % acceleration though...
            internalFormat = GL.LUMINANCE_FLOAT16_APPLE;

            if winfo.GLSupportsFilteringUpToBpc >= 16
                % 16 bits half precision float hw accelerated:
                fprintf('PsychColorCorrection: Using a 16 bit float CLUT -> 11 bits effective linear output precision for color correction.\n');
            else
                % No hw acceleration - use anyway:
                fprintf('PsychColorCorrection: Using a 16 bit float CLUT -> 11 bits effective linear output precision for color correction.\n');
                fprintf('PsychColorCorrection: This format can''t be automatically linearly interpolated in hardware, so you may experience either\n');
                fprintf('PsychColorCorrection: a loss in speed, or a loss in precision if your CLUT doesn''t contain enough slots to cover the input range.\n');
            end
        else
            % No support for > 8 bpc textures at all and/or no need for
            % more than 8 bpc precision or range. Choose 8 bpc texture:
            internalFormat = GL.LUMINANCE;
            fprintf('PsychColorCorrection: Using a 8 bit integer CLUT -> 8 bits effective linear output precision for color correction.\n');

            % Plain old 8 bits fixed point:
            if (max(max(clut)) > 255) || (min(min(clut)) < 0)
                % Ohoh, out of range values for integer texture! Try if we
                % can use a float16 texture, even if no linear texture
                % filtering is possible that way. Loss of accuracy is
                % better than completely wrong results.
                fprintf('\nWARNING!PsychColorCorrection: CLUT contains values greater than 255 or negative values which can''t be handled properly on your hardware!!\n');
                fprintf('WARNING!PsychColorCorrection: This will likely cause remapping artifacts in color correction!!\n');
            end
            
            if (winfo.BitsPerColorComponent > 8)
                fprintf('WARNING!PsychColorCorrection: Your hardware can only handle 8 bit precision color correction in outputrange 0-255,\n');                
                fprintf('WARNING!PsychColorCorrection: but your framebuffer is configured for more than 8 bit precision. This may cause\n');                
                fprintf('WARNING!PsychColorCorrection: loss of effective precision in color correction and thereby unwanted artifacts!\n');                
            end
        end
    end
    
    % Bind relevant texture object:
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, icmDataForHandle(win));
    
    % Set filters properly: Want bilinear filtering for linear
    % interpolation inbetween reference values of the clut if clut is of
    % lower resolution than the output resolution of the display device:
    glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_MIN_FILTER, GL.LINEAR);
    glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_MAG_FILTER, GL.LINEAR);

    % Want clamp-to-edge behaviour to saturate at minimum and maximum
    % intensity value, and to make sure that a pure-luminance 1 row clut is
    % properly "replicated" to all three color channels in rgb modes:
    glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_S, GL.CLAMP_TO_EDGE);
    glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_T, GL.CLAMP_TO_EDGE);
    
    % Assign lookuptable data to texture:
    glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, internalFormat, size(clut, 1), size(clut, 2), 0, GL.LUMINANCE, GL.FLOAT, clut);
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

    % Done.
    return;
end


error('Unknown subfunction specified. Typo?!? Read the help.');


function [slot shaderid blittercfg voidptr glsl luttexid] = GetSlotForTypeAndBind(win, icmId, viewId)

    % Fetch slot, and GLSL handle to relevant imaging slot and shader:
    if isempty(viewId)
        viewId = 'AllViews';
    end 
    
    if strcmpi(viewId, 'AllViews') || strcmpi(viewId, 'FinalFormatting')
        chain = 'FinalOutputFormattingBlit';
    end
    
    if strcmpi(viewId, 'LeftView')
        chain = 'StereoLeftCompositingBlit';
    end

    if strcmpi(viewId, 'RightView')
        chain = 'StereoRightCompositingBlit';
    end

    if strcmpi(icmId, 'ClampedNoName')
        icmIdString = sprintf('CMIH:ClampedNoName');
    else
        icmIdString = sprintf('ICM:%s', icmId);
    end
    
    [slot shaderid blittercfg voidptr glsl luttexid] = Screen('HookFunction', win, 'Query', chain, icmIdString);

    % Shader found?
    if slot == -1
        fprintf('Searched chain: %s, searched icmIdString: %s\n', chain, icmIdString);
        error('Could not find shader plugin for color correction inside imaging pipeline for window and view! Is color correction really enabled for this window and view channel?!?');
    end
    
    if glsl == 0
        error('Color correction shader is not operational for unknown reason, maybe a Psychtoolbox bug? Sorry...');
    end
    
    % Bind it:
    glUseProgram(glsl);
return;
