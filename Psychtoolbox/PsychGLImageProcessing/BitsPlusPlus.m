function [win, winRect] = BitsPlusPlus(cmd, arg, dummy, varargin)
% BitsPlusPlus(cmd [, arg1][, arg2]) -- Psychtoolbox interface to
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
% cmd - The command that BitsPlusPlus should execute. cmd can be any of
% the following:
%
% Open a window on the Bits++ display as with Screen('OpenWindow', ...),
% perform all initialization:
%
% [win, winRect] = BitsPlusPlus('OpenWindow', screenid, ...);
%
% This will execute Screen('OpenWindow') with all proper parameters,
% followed by Bits++ init routines. It is a completely sufficient
% drop in replacement for Screen('OpenWindow'), accepting and returning
% exactly the same arguments that Screen() would do, adjusting all
% parameters to the constraints of the Bits++, if necessary.
%

debuglevel = 1;

if strcmp(cmd, 'OpenWindowBits++')
    error('Not yet implemented :(');
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
    if (imagingmode & kPsychNeed16BPCFloat) || (imagingmode & kPsychNeed16BPCFixed)
        % User specified override: Use it.
        ourspec = 0;
    else
        % No user specified accuracy. We play safe and choose the highest
        % one:
        ourspec = kPsychNeed32BPCFloat;
    end
    
    % Imagingmode must at least include the following:
    imagingmode = mor(imagingmode, kPsychNeedFastBackingStore, kPsychNeedOutputConversion, ourspec);

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
        Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', 'Color++ output formatting shader for CRS Bits++', shader);
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

    % Restore old graphics preferences:
    Screen('Preference', 'Enable3DGraphics', ogl);

    % Ready!
    return;
end

error('BitsPlusPlus: Unknown subcommand provided. Read "help BitsPlusPlus".');
