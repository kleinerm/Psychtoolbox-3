function [win, winRect] = BitsPlusPlus(cmd, arg, dummy, varargin)
% BitsPlusPlus(cmd [, arg1][, arg2, ...]) -- Psychtoolbox interface to
% Cambridge Research Systems Bits++ box for high precision stimulus
% output to analog displays via 14 bit video converters.
%
% This function is used to set up and interface with the Bits++ box of
% CRS. It is a Matlab wrapper around lower level GLSL Psychtoolbox
% functions. This function depends on graphics hardware that supports the
% Psychtoolbox imaging pipeline and framebuffers with more than 14 bit
% precision, i.e., 16 bpc fixed point framebuffers or floating point
% framebuffers. Have a look at the Psychtoolbox Wiki where you can find a
% list of graphics cards with the neccessary features.
%
% This function supersedes the old Matlab based Bits++ Toolbox, which
% essentially provides the same functionality on any graphics card, but is
% harder to use, much slower and not fully integrated into PTB, ie, you
% can't take full advantage of PTB's advanced drawing and image processing
% functions when using the old Bits++ toolbox.
%
% cmd - The command that BitsPlusPlus should execute. cmd can be any of
% the following:
%
%
% Load a linear identity mapping CLUT into Bits++ while running in Bits++
% mode:
%
% BitsPlusPlus('LoadIdentityClut', window);
%
% Will schedule update to an identity clut. Next invocation of
% Screen('Flip', window); will actually upload the identity clut into
% Bits++.
%
%
% Schedule a Bits++ DIO command for execution on next Screen('Flip'):
%
% BitsPlusPlus('DIOCommand', window, repetitions, mask, data, command [, xpos, ypos]);
%
% This will draw the proper T-Lock control codes at positions (xpos, ypos)
% for execution of the Bits++ DIO commands (mask, data, command).
%
% You can specify multiple codes at once: If mask, data, command, xpos and
% ypos are matrices or vectors with 'num' rows, then each of the 'num' rows
% defines one T-Lock code. If mask, command, xpos and ypos are scalars and
% data is a one row vector, then only the corresponding T-Lock line is
% drawn.
%
% For each DIO command:
% 'mask' must be a 8 bit integer value, 'command' must be a 8 bit integer
% value, whereas 'data' must be a a 248 element row vector of bytes. 
%
% Consult your Bits++ manual for explanation of the meaning of the values.
%
% xpos and ypos are optional: By default, the T-Lock code is drawn into the
% 3rd pixel row of the output image, so it can't collide with a potential
% T-Lock code for CLUT updates.
%
% The DIO command will become effective during the next flip command. The
% T-Lock code is drawn during 'repetitions' successive invocations of
% Screen('Flip'). If you set 'repetitions' to -1, then the code will be
% drawn until you stop it via a call to BitsPlusPlus('DIOCommandReset', window);
%
%
% Disable use of the DIO T-Lock code blitting:
%
% BitsPlusPlus('DIOCommandReset', window);
% Stops blitting of T-Lock command codes immediately. If you want to use
% them again, you have to respecify codes via the BitsPlusPlus('DIOCommand',...);
%
%
%
%
% Open a full-screen window on the Bits++ display as with
% Screen('OpenWindow', ...), perform all initialization:
%
% The following commands will execute Screen('OpenWindow') with all proper
% parameters, followed by Bits++ init routines. They are completely sufficient
% drop in replacements for Screen('OpenWindow'), accepting and returning
% exactly the same arguments that Screen() would do, adjusting all
% parameters to the constraints of the Bits++, if necessary.
%
% Activate Bits++ mode:
%
% [win, winRect] = BitsPlusPlus('OpenWindowBits++', screenid, ...);
%
% This will open an onscreen window on Bits++ screen 'screenid' with a
% standard 8 bits per color channel framebuffer. The gamma table of your
% graphics hardware will be loaded with an identity gamma table, so the
% T-Lock system of Bits++ works and Bits++ can accept commmands embedded
% into the stimulus images. Psychtoolbox will automatically embed a T-Lock
% control line into the top line of the display screen, which encodes the
% 256 entry, 14 bit per color channel CLUT to use for Bits++ display mode.
% You can change the Bits++ CLUT at any time via the standard PTB
% Screen('LoadNormalizedGammaTable', win, newclut, 2); command. The
% 'newclut' will get uploaded to the Bits++ at the next invocation of
% Screen('Flip') to allow updates of the CLUT synchronous to stimulus
% updates. 'newclut' has to be a 256 rows by 3 columns matrix with values
% in range 0.0 - 1.0: 0.0 is mapped to clut color value 0, 1.0 is mapped to
% the highest Bits++ output color value 16383.
%
% This mode works on any OpenGL graphics hardware.
%
%
% Activate Mono++ mode:
%
% [win, winRect] = BitsPlusPlus('OpenWindowMono++', screenid, ...);
%
% This will open an onscreen window on Bits++ screen 'screenid' for display
% of pure luminance (grayscale) images. The framebuffer has a resolution of
% 32 bit floating point precision by default: This means that pixel luminance
% values have to be specified as floating point numbers between 0.0 and
% 1.0. 0.0 maps to black (Output intensity 0 on Bits++ device). 1.0 maps to
% white (Maximum output intensity 16383 on Bits++ device). The intensity
% range between 0.0 and 1.0 is internally represented and processed by
% Psychtoolbox with a resolution 23 bits, i.e. over 8 million levels of
% luminance. The Bits++ can resolve this range to 14 bits, ie. 16384 levels
% of luminance during display. This mode is not compatible with the use of
% any gamma- or clut- tables. Both the graphics hardwares gamma table and
% the Bits++ internal clut are set to an identity mapping while this mode
% is active. Please read the notes below the Color++ section for graphics
% hardware requirements and other useful tips for use of Bits++.
%
%
% Activate Color++ mode:
%
% [win, winRect] = BitsPlusPlus('OpenWindowColor++', screenid, ...);
%
% This will open an onscreen window on Bits++ screen 'screenid' for display
% of 14 bit per color component 42bpp color images. The framebuffer has
% a resolution of 32 bit floating point precision for each color component
% by default: This means that (Red, Green, Blue) color pixel component
% values have to be specified as floating point numbers between 0.0 and
% 1.0. 0.0 maps to minimum output intensity on Bits++ device for a channel.
% 1.0 maps to maximum output intensity 16383 on Bits++ device for a channel.
% The color intensity range between 0.0 and 1.0 is internally represented and
% processed by Psychtoolbox with an effective resolution of about 23 bits,
% i.e. over 8 million levels of color per color channel. The Bits++ can resolve
% this range to 14 bits, ie. 16384 levels of color during display. This mode
% is not compatible with the use of any gamma- or clut- tables. Both the graphics
% hardwares gamma table and the Bits++ internal clut are set to an identity
% mapping while this mode is active. Please read the notes below for graphics
% hardware requirements and other useful tips for use of Bits++.
%
%
% Notes for both Mono++ and Color++ mode:
%
% In Mono++ and Color++ mode, PTB expects color values in the range 0.0 to
% 1.0 instead of the (otherwise usual) range 0 to 255. The range 0.0-1.0
% is a more natural fit for high dynamic range/precision output devices than
% the 0-255 range with its strong ties to 8 bpc output devices. 0-1 is also
% the "natural" native color range of OpenGL, so colors in this range can
% be handled by the graphics hardware at a higher speed. You can change the
% mapping of input colors to output intensities by use of the command
% Screen('ColorRange') (see its online help for usage), but in the interest
% of uniform code and to avoid possible side effects with some graphics
% hardware, we strongly recommend using the default 0.0-1.0 color range.
%
% You can still pass standard 8bpc (256 color/intensity levels) color/luminance
% textures to PTB via standard use of Screen('MakeTexture') - the hardware
% will convert such 8bpc images to OpenGL's native color range, as well as
% any images delivered by the Quicktime movie playback engine or the video
% capture engine. If you want to provide high dynamic range, high color
% depths images, please specify them as Matlab double matrices to
% Screen('MakeTexture') and set the optional flag 'floatprecision' to 1 or
% 2, i.e., hdrtex = Screen('MakeTexture', win, myHDRImage, [], [], 2);
%
% Psychtoolbox will represent such images with an internal precision of 10
% bits + 1 bit sign if you choose the 'floatprecision' flag to be 1. If you
% choose a 'floatprecision' flag of 2, PTB will represent the images with
% an internal precision of 23 bits + 1 bit sign. You can provide negative
% color values as well, e.g., -0.5. If you wonder what the use of this
% might be, have a careful look at the tutorial script...
% 'AdditiveBlendingForLinearSuperpositionTutorial.m'
% ... for an example of extremely fast drawing of luminance gratings with
% controllable size, orientation and contrast and correct linear superposition:
% 
% By default, PTB will use a 32-bit floating point framebuffer for your
% drawings, ie. the precision is way higher than needed for any high
% dynamic range/resolution display device in existence. The downside of this
% super-precision is that alpha-blending is not supported in this mode, unless
% you employ an NVidia Geforce 8000 series graphics card. If you need
% alpha-blending on older/other hardware then specify the optional flag
% 'kPsychNeed16BPCFloat' for the 'imagingmode' argument. This will reduce
% effective accuracy of the framebuffer to 10 bit precision, but allow for
% fast alpha-blending. 10 Bit precision are 4 bits less than the 14 bits
% that Bits++ can provide, but it will be possible to use the extra 14-10 =
% 4 bits for gamma correction of the display by employing a gamma
% correction shader.
%
% Graphics hardware requirements: Mono++ and Color++ mode require use of the
% Psychtoolbox imaging pipeline and floating point framebuffers. The
% minimum requirements are ATI Radeon X1000 series or NVidia Geforce-6800
% series and later graphics hardware. We currently recommend NVidia
% Geforce-8000 series hardware for best results. However, this functions
% have been successfully tested on ATI Radeon X1600 and NVidia Geforce-7800
% hardware as well.
%
% All Bits++ modes supported by this function should work Plug & Play,
% requiring no changes to your stimulus code other than mentioned here to
% take full advantage of all functionality of Psychtoolbox just as with standard
% 8 bpc displays at the higher 14 bpc quality of Bits++. If you find this
% not to be the case then it's either an omission in our documentation
% or a bug - Please report it.

% History:
% 22.04.2007 Written (MK).

global GL;

% Flag for validation: If not set to one, then this routine will check if
% proper operation of Bitsplusplus with GPU imaging has been verified.
persistent validated;

% Encoded T-Lock display list handle for driving Bits++ DIO:
persistent tlockhandle;
% Counter of pending T-Lock display list blits: Zero == Disabled.
persistent blitTLockCode;

if nargin < 1
    error('You must specify a command in argument "cmd"!');
end

win = [];
winRect = [];

if cmd == 1
    % Fast callback path for PTB imaging pipeline. We got called from the
    % finalizer blit chain of the imaging pipeline, asking us to perform
    % some post-processing on the final framebuffer image, immediately
    % before bufferswap.
    %
    % Currently, the only supported operation is drawing of a DIO T-Lock
    % code into the framebuffer, for control of the DIO pins of the Bits++
    % box. The T-Lock code has been generated already by a call to
    % 'DIOCommand'. We just have to blit that "Code Image" to the
    % framebuffer. We can't use Screen() commands here as we are called
    % from inside Screen -- Screen is not reentrant!
    %
    % We only blit if there is something to blit. Then we reset to nothing
    % to blit:
    if blitTLockCode ~= 0
        glCallList(tlockhandle);
        blitTLockCode = blitTLockCode - 1;
    end
    
    return;
end

% Default debuglevel for output during initialization:
debuglevel = 1;

if isempty(validated)
    validated = 0;
    tlockhandle = 0;
    blitTLockCode = 0;
end

if strcmp(cmd, 'DIOCommand')

    % Reset to safe default:
    blitTLockCode = 0;
    
    if nargin < 2 || isempty(arg)
        error('window handle for Bits++ onscreen window missing!');
    end
    
    if nargin < 3 || isempty(dummy)
        error('Number of repetitions for DIO command missing!');
    end

    if nargin < 6
        error('DIOCommand must have the parameters "Mask", "Command" and "Data"!');
    end

    mask = varargin{1};
    data = varargin{2};
    command = varargin{3};

    % Create or recreate our display list:
    glNewList(tlockhandle, GL.COMPILE);
    
    for i=1:size(mask, 1)
        % Process i'th row of command sequence:

        % Generate DIO T-Lock image as Matlab matrix:
        tlockdata = BitsPlusDIO2Matrix(mask(i), data(i,:), command(i));

        % Convert from Matlab matrix to OpenGL pixel format:
        encodedDIOdata = uint8(zeros(3, 508));
        % Pack 3 separate RGB planes into rows 1,2,3. As Matlabs data format is
        % column major order, this will end up as tightly packed pixel array in
        % format RGBRGBRGB.... just as glDrawPixels likes it.
        encodedDIOdata(1,:) = tlockdata(1,:,1);
        encodedDIOdata(2,:) = tlockdata(1,:,2);
        encodedDIOdata(3,:) = tlockdata(1,:,3);

        if nargin >= 7
            % Optional x, y blit position provided:
            xDIO = varargin{4};
            yDIO = varargin{5};
            xDIO = xDIO(i);
            yDIO = yDIO(i);
            
            if yDIO < 1
                yDIO = 1;
            end
        else
            % Set default position: 3rd scanline of display, so we don't get
            % into the way of a possible CLUT T-Lock code:
            xDIO = 0;
            yDIO = 3;
        end
        
        % Add command sequence for this T-Lock code to display list:
        glRasterPos2i(xDIO, yDIO);
        glDrawPixels(508, 1, GL.RGB, GL.UNSIGNED_BYTE, encodedDIOdata);
    end
    
    % Finish display list;
    glEndList;
        
    % Assign number of repetitions:
    blitTLockCode = dummy;

    % Done. Return.
    return;

end

if strcmp(cmd, 'DIOCommandReset')
    % Dummy error check: arg will be used in later revisions...
    if nargin < 2 || isempty(arg)
        error('window handle for Bits++ onscreen window missing!');
    end
    
    % Disable T-Lock blitting:
    blitTLockCode = 0;
    return;
end

if strcmp(cmd, 'ForceUnvalidatedRun')
    % Enforce use of this routine without verification of correct function
    % of the imaging pipeline. This is used by the correctness test itself
    % in order to be able to run the validation.
    validated = 1;
    return;
end

if strcmp(cmd, 'StoreValidation')
    % Enforce use of this routine without verification of correct function
    % of the imaging pipeline. This is used by the correctness test itself
    % in order to be able to run the validation.
    ValidateBitsPlusImaging(arg, 1);
    return;
end

if strcmp(cmd, 'LoadIdentityClut')
    % Load an identity CLUT into Bits++ at next Screen('Flip'). This is
    % just a little convenience wrapper around 'LoadNormalizedGammaTable':
    % Restore Bits++ Identity CLUT so it can be used as normal display:
    if nargin < 2 || isempty(arg)
        error('window handle for Bits++ onscreen window missing!');
    end
    linear_lut =  repmat(linspace(0, 1, 256)', 1, 3);
    Screen('LoadNormalizedGammaTable', arg, linear_lut, 2);
    return;
end

if strcmp(cmd, 'OpenWindowBits++')
    % Execute the Screen('OpenWindow') command with proper flags, followed
    % by our own Initialization. Return values of 'OpenWindow'.
    %
    % This will set up the Bits++ mode of Bits++
        
    % Assign screen index:
    if nargin < 2 || isempty(arg) || ~isa(arg, 'double')
        error('BitsPlusPlus: "OpenWindow..." called without valid screen handle.');
    end
    screenid = arg;

    % Assign optional clear color:
    if nargin < 3
        clearcolor = [];
    else
        clearcolor = dummy;
    end

    % windowRect is always full screen -- Anything else would make the
    % Bits++ display fail.
    winRect = [];

    % pixelSize is also fixed to 32 bit RGBA8 framebuffer:
    pixelSize = 32;

    % Same for double-buffering:
    numbuffers = 2;

    % stereomode we take...
    if nargin >= 7
        stereomode = varargin{4};
    else
        stereomode = [];
    end

    % multiSample gets overriden for now... Would probably interfere
    % with Bits++ display controller:
    multiSample = 0;

    % Open the window, pass all parameters (partially modified or overriden), return Screen's return values:
    if nargin >= 9
        [win, winRect] = Screen('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, varargin{6:end});
    else
        [win, winRect] = Screen('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample);
    end

    % Ok, if we reach this point then we've got a proper onscreen
    % window on the Bits++. Let's reassign our arguments and continue with
    % the init sequence:
    arg = win;

    % First load the graphics hardwares gamma table with an identity mapping,
    % so it doesn't interfere with Bits++ -- Functions from Bits++ toolbox.
    LoadIdentityClut(win);

    % Now enable finalizer hook chains and load them with the special Bits++
    % command for T-Lock based Bits++ internal CLUT updates:
    Screen('HookFunction', win, 'PrependBuiltin', 'LeftFinalizerBlitChain', 'Builtin:RenderClutBits++', '');
    Screen('HookFunction', win, 'Enable', 'LeftFinalizerBlitChain');

    if (~isempty(stereomode) && stereomode == 1)
        % This is only needed on quad-buffered stereo contexts. Enable CLUT
        % updates via T-Lock on right stereo buffer as well:
        Screen('HookFunction', win, 'PrependBuiltin', 'RightFinalizerBlitChain', 'Builtin:RenderClutBits++', '');
        Screen('HookFunction', win, 'Enable', 'RightFinalizerBlitChain');
    end

    % We need the GL for DIO T-Lock setup:
    if isempty(GL)
        % Load & Initalize constants and moglcore, but don't set the 3D gfx
        % flag for Screen():
        InitializeMatlabOpenGL([], [], 1);
    end;

    % Setup finalizer callback for DIO T-Lock updates:
    tlockhandle = SetupDIOFinalizer(win, stereomode);

    % Load an identity CLUT into the Bits++ to start with:
    linear_lut =  repmat(linspace(0, 1, 256)', 1, 3);
    Screen('LoadNormalizedGammaTable', win, linear_lut, 2);
    
    % Check validation:
    if ~validated
        % MK: Actually, don't! Validation code doesn't check/validate
        % anything in Bits++ mode, so this is pointless... Leave it here
        % for documentation.
        % ValidateBitsPlusImaging(win, 0);
    end
    
    % Reset validation flag after first run:
    validated = 0;
    
    % Ready!
    return;
end

if strcmp(cmd, 'OpenWindowMono++') || strcmp(cmd, 'OpenWindowColor++')
    % Execute the Screen('OpenWindow') command with proper flags, followed
    % by our own Initialization. Return values of 'OpenWindow'.
    %
    % This will set up the Mono++ or Color++ mode of Bits++
    
    % Assign screen index:
    if nargin < 2 || isempty(arg) || ~isa(arg, 'double')
        error('BitsPlusPlus: "OpenWindow..." called without valid screen handle.');
    end
    screenid = arg;

    % Assign optional clear color:
    if nargin < 3
        clearcolor = [];
    else
        clearcolor = dummy;
    end

    % windowRect is always full screen -- Anything else would make the
    % Bits++ display fail.
    winRect = [];

    % pixelSize is also fixed to 32 bit RGBA8 framebuffer:
    pixelSize = 32;

    % Same for double-buffering:
    numbuffers = 2;

    % stereomode we take...
    if nargin >= 7
        stereomode = varargin{4};
    else
        stereomode = [];
    end

    % multiSample gets overriden for now... Would probably interfere
    % with Bits++ display controller:
    multiSample = 0;

    % Imaging mode we take - and combine it with our own requirements:
    if nargin >= 9
        imagingmode = varargin{6};
    else
        imagingmode = 0;
    end

    % For imagingmode we need at least fast backingstore, the output
    % formatter enabled and some high precision color buffer. We default to
    % 32 bpc floating point, the only safe choice accross different graphics cards
    % from different vendors, but the users imagingmode setting is free to
    % override this with a 16 bpc fixed buffer. 16 bpc float works as well
    % but can't use the full Bits++ color range at full precision.
    if bitand(imagingmode, kPsychNeed16BPCFloat) || bitand(imagingmode, kPsychNeed16BPCFixed)
        % User specified override: Use it.
        ourspec = 0;
    else
        % No user specified accuracy. We play safe and choose the highest
        % one:
        ourspec = kPsychNeed32BPCFloat;
    end
    
    % Imagingmode must at least include the following:
    imagingmode = mor(imagingmode, kPsychNeedFastBackingStore, kPsychNeedOutputConversion, ourspec);

    if strcmp(cmd, 'OpenWindowColor++')
        % In Color++ mode we only have half the effective horizontal
        % resolution. Tell PTB to take this into account for all relevant
        % calculations:
        imagingmode = mor(imagingmode, kPsychNeedHalfWidthWindow);
    end
    
    % Open the window, pass all parameters (partially modified or overriden), return Screen's return values:
    if nargin > 9
        [win, winRect] = Screen('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingmode, varargin{7:end});
    else
        [win, winRect] = Screen('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingmode);
    end

    % Ok, if we reach this point then we've got a proper onscreen
    % window on the Bits++. Let's reassign our arguments and continue with
    % the init sequence:
    arg = win;

    % First load the graphics hardwares gamma table with an identity mapping,
    % so it doesn't interfere with Bits++ -- Function from Bits++ toolbox.
    LoadIdentityClut(win);

    % Set color range to 0.0 - 1.0: This makes more sense than the normal
    % 0-255 values. Try to disable color clamping. This may fail and
    % produce a PTB warning, but if it succeeds then we're better off for
    % the 2D drawing commands...
    Screen('ColorRange', win, 1, 0);

    % Backup current gfx-settings, so we can restore them after
    % modifications: The LoadGLSLProgramFromFiles() routine enables this
    % implicitely. This is unwanted in case we are in pure 2D mode, so we
    % need to undo it below...
    ogl = Screen('Preference', 'Enable3DGraphics');
    
    % Operate in Mono++ mode or Color++ mode?
    if strcmp(cmd, 'OpenWindowMono++')
        % Setup for Mono++ mode:

        % Load Bits++ Mono++ formatting shader:
        shader = LoadGLSLProgramFromFiles('Bits++_Mono++_FormattingShader', debuglevel);

        % Now enable output formatter hook chain and load them with the special Bits++
        % Mono++ data formatting shader: We append the shader because it
        % absolutely must be the last shader to execute in that chain!
        Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', 'Mono++ output formatting shader for CRS Bits++', shader);
    else
        % Setup for Color++ mode:

        % Load Bits++ Color++ formatting shader:
        shader = LoadGLSLProgramFromFiles('Bits++_Color++_FormattingShader', debuglevel);

        % Now enable output formatter hook chain and load them with the special Bits++
        % Color++ data formatting shader: We append the shader because it
        % absolutely must be the last shader to execute in that chain!
        % We apply a scaling of 2.0 in horizontal direction for the output
        % blit, to take the fact into account that the internal window
        % buffers only have half display width.
        Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', 'Color++ output formatting shader for CRS Bits++', shader, 'Scaling:2.0:1.0');
    end

    % Setup shaders image source as the first texture unit, this is by
    % definition of how the imaging pipe works. Don't think really needed,
    % as this is the default, but its good practice to not rely on such
    % things...
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, 'Image'), 0);
    glUseProgram(0);

    % Enable framebuffer output formatter: From this point on, all visual
    % output will be reformatted to Bits++ framebuffer format at each
    % invokation of Screen('DrawingFinished') or Screen('Flip'), whatever
    % comes first.
    Screen('HookFunction', win, 'Enable', 'FinalOutputFormattingBlit');

    % Setup finalizer callback for DIO T-Lock updates:
    tlockhandle = SetupDIOFinalizer(win, stereomode);
    
    % Restore old graphics preferences:
    Screen('Preference', 'Enable3DGraphics', ogl);

    % Check validation:
    if ~validated
        ValidateBitsPlusImaging(win, 0);
    end

    % Reset validation flag after first run:
    validated = 0;

    % Ready!
    return;
end

error('BitsPlusPlus: Unknown subcommand provided. Read "help BitsPlusPlus".');
end

% Helper function: Check if system already validated for current settings:
function ValidateBitsPlusImaging(win, writefile)
    
    % Compute fingerprint of this system configuration:
    validated = 0;
    global GL;
    
    screenid = Screen('WindowScreenNumber', win);
    [w, h] = Screen('WindowSize', screenid);
    d = Screen('PixelSize', win);
    v = Screen('Version');
    v = v.version;
    gfxconfig = [ glGetString(GL.VENDOR) ':' glGetString(GL.RENDERER) ':' glGetString(GL.VERSION) ];
    gfxconfig = sprintf('%s : Screen %i : Resolution %i x %i x %i : ScreenVersion = %s', gfxconfig, screenid, w, h, d, v);
    
    if ~writefile
        % Check if a validation file exists and if it contains this
        % configuration:
        fid = fopen([PsychtoolboxRoot 'ptbbitsplusplusvalidationfile.txt'], 'r');
        if fid~=-1
            while ~feof(fid)
                vconf = fgetl(fid);
                if strcmp(vconf, gfxconfig)
                    validated = 1;
                    break;
                end
            end
            fclose(fid)
        end

        if ~validated
            fprintf('\n\n------------------------------------------------------------------------------------------------------------------\n')
            fprintf('\n\nThis specific configuration of graphics hardware, graphics driver and Psychtoolbox version has not yet been tested\n');
            fprintf('for correct working with Bits++ for the given display screen, screen resolution and color depths.\n\n');
            fprintf('Please run the test script "BitsPlusImagingPipelineTest(%i);" once, so this configuration can be verified.\n', Screen('WindowScreenNumber', win));
            fprintf('After that test script suceeded, re-run your experiment script.\nThanks.\n');
            fprintf('\n');
            fprintf('Configuration to verify: %s\n', gfxconfig);
            
            Screen('CloseAll'); ShowCursor; Priority(0);
            
            error('Configuration not yet verified. Please do it now.');
        end
    end
    
    if writefile
        % Append current configuration to file to mark it as verified:
        [fid msg]= fopen([PsychtoolboxRoot 'ptbbitsplusplusvalidationfile.txt'], 'a');
        if fid == -1
            sca;
            errtxt = sprintf('Could not write validation file %s to filesystem [%s].', [PsychtoolboxRoot 'ptbbitsplusplusvalidationfile.txt'], msg);
            error(errtxt);
        end

        % Append line:
        fprintf(fid, [gfxconfig '\n']);
        fclose(fid);
    end
end

% Helper function for setup of finalizer blit chains in all modes. Sets up
% callback into our file for T-Lock drawing etc...
function displist = SetupDIOFinalizer(win, stereomode)

    % Generate unique display list handle for later use:
    displist = glGenLists(1);
    
    % Now enable finalizer hook chains and load them with the special Bits++
    % command for T-Lock based Bits++ DIO updates:
    Screen('HookFunction', win, 'PrependMFunction', 'LeftFinalizerBlitChain', 'Render T-Lock DIO data callback', 'BitsPlusPlus(1);');
    Screen('HookFunction', win, 'Enable', 'LeftFinalizerBlitChain');

    if (~isempty(stereomode) && stereomode == 1)
        % This is only needed on quad-buffered stereo contexts.
        Screen('HookFunction', win, 'PrependMFunction', 'RightFinalizerBlitChain', 'Render T-Lock DIO data callback',  'BitsPlusPlus(1);');
        Screen('HookFunction', win, 'Enable', 'RightFinalizerBlitChain');
    end

end
