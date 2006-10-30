function BrightSideHDR(cmd, arg, dummy)
% BrightSideHDR(cmd [, arg1][, arg2]) -- Psychtoolbox interface to
% BrightSide Technologies High Dynamic Range display device.
%
% This function is used to set up and interface with the High Dynamic Range
% display of BrightSide Technologies. It is a Matlab wrapper around lower
% level GLSL Psychtoolbox functions and the low-level BrightSideCore.dll
% MEX-File. You need to have Brightsides DLL's (and the display device)
% installed on your machine for this to work. Without the DLL's this will
% simply crash.
%
% cmd - The command that BrightSideHDR should execute. cmd can be any of
% the following:
%
% BrightSideHDR('Initialize', win [, dummy]); -- Initialize the BrightSide libraries
% for HDR output into onscreen window 'win'. Onscreen window 'win' must have been
% opened before on a screen which corresponds to the attached High Dynamic Range
% display device. If you set 'dummy' to 1, then we are in emulation mode,
% i.e., we work without invocation of the mex file and without a real HDR
% display.
%
% BrightSideHDR('BeginDrawing'); -- Mark start of drawing operations into
% the high dynamic range backbuffer. After this command you can issue
% standard Screen or mogl OpenGL commands to draw into the high resolution
% framebuffer.
%
% BrightSideHDR('EndDrawing'); -- Mark end of drawing operations. This will
% convert the HDR image content in the HDR backbuffer into the special data
% format needed by the HDR display hardware. After execution of this
% command you can simply call the usual Screen('Flip', win, ...) command to
% trigger display of the HDR image on the HDR display.
%
% BrightSideHDR('Shutdown'); -- Shut down the HDR device and core library,
% switch PTB back to standard LDR output.

% History:
% 10/30/2006 Initial prototype implementation. (MK)

global GL;
persistent windowPtr;
persistent winwidth;
persistent winheight;
persistent hdrfbo;
persistent hdrtexid;
persistent online;
persistent inhdrdrawmode;
persistent dummymode;

% Cold start: Setup our variables to safe defaults.
if isempty(online)
    online = 0;
    inhdrdrawmode = 0;
    dummymode = 0;
end

if nargin < 1 || isempty(cmd)
    error('BrightSideHDR: Missing command specification.');
end

% Command dispatcher:
if strcmp(cmd, 'Initialize')
    % Initialization command:

    % Child protection:
    if online
        error('BrightSideHDR: "Initialize" called on a HDR display already initialized.');
    end

    if nargin < 2 || isempty(arg) || ~isa(arg, 'double')
        error('BrightSideHDR: "Initialize" called without valid HDR onscreen window handle.');
    end

    if nargin < 3 || isempty(dummy)
        dummymode = 0;
    else
        dummymode = dummy;
    end
    
    % OpenGL mode of Psychtoolbox already initialized?
    if isempty(GL)
        error('BrightSideHDR: OpenGL is offline! You need to call "InitializeMatlabOpenGL" before calling BrightSide functions.');
    end

    % Another bit of child protection:
    AssertGLSL;

    % Step 1: Make sure we got a window handle for a valid onscreen window:
    if isempty(find(Screen('Windows')==arg)) || Screen('WindowKind', arg)~=1
        error('BrightSideHDR: "Initialize" called with something else than a valid onscreen window handle.');
    end
    
    % Valid handle, assign it and query its dimensions, aka the dimensions
    % of our HDR framebuffer:
    windowPtr = arg;
    [winwidth, winheight] = Screen('WindowSize', windowPtr);
    
    % Step 1: Create a floating point precision RGBA framebuffer object
    % with a RGBA32FLOAT color buffer attached. We don't attach depth
    % buffers or stencil buffer yet. This fbo will be the rendertarget for
    % HDR drawing and its color buffer texture will be the input to the HDR
    % core library for the HDR blit operation:
    [hdrfbo, hdrtexid] = moglCreateFBO(winwidth, winheight);
    
	% BrightSide::DCGI.Initialize( "../", "DR-37P-beta.xml" );
	% //when display is called, the texture in tex will be rendered to the back buffer
	% BrightSide::DCGI.SetInputOutput(g_texture_tobe_displayed, 0);

    % We have our framebuffer. Pass the location of the HDR config file and
    % the texture handle of the framebuffers HDR colorbuffer texture to the
    % core library and initialize it:
    if ~dummymode
        % Initiate loading, linking and initialization of the core:
        BrightSideCore(-1);
        % Initialize the libraries and display device:
        BrightSideCore(0, [fileparts(which('BrightSideCore')) '/BSRuntimeLibs/'], 'DR-37P-beta.xml', hdrtexid, 0);
    end
    
    % Reset draw mode:
    inhdrdrawmode = 0;

    % We are online:
    online = 1;
    return;
end

if strcmp(cmd, 'Shutdown')
    % Shutdown command:

    % Child protection:
    if ~online
        warning('BrightSideHDR: "Shutdown" command called, although display is already offline.');
        return;
    end
    
    % Shutdown BrightSide core and display:
    if ~dummymode
        BrightSideCore(1);
    end

    % Destroy our HDR framebuffer and its textures:
    moglDeleteFBO(hdrfbo);

    % Reset to preinit state:
    hdrfbo = 0;
    hdrtexid = 0;
    windowPtr = 0;
    dummymode = 0;
    inhdrdrawmode = 0;

    % Shutdown complete, we're offline:
    online = 0;    
    return;
end

if strcmp(cmd, 'BeginDrawing')
    % Drawing preparation command:
    if ~online
        error('BrightSideHDR: "BeginDrawing" command called, although display is offline.');
    end

    if inhdrdrawmode
        error('BrightSideHDR: "BeginDrawing" called, although already in HDR drawing mode.');
    end
    
    % Bind our HDR framebuffer as rendertarget. This will unbind its color
    % buffer texture hdrtexid automatically and setup the viewport,
    % projection and modelview matrices for orthonormal drawing
    % automatically:
    moglChooseFBO(hdrfbo);

    % Ready:
    inhdrdrawmode = 1;
    return;
end

if strcmp(cmd, 'EndDrawing')
    % Preflip command:
    if ~online
        error('BrightSideHDR: "EndDrawing" command called, although display is offline.');
    end

    if ~inhdrdrawmode
        error('BrightSideHDR: "EndDrawing" called without previous call to "BeginDrawing".');
    end

    % Unbind our HDR framebuffer as rendertarget, so its color buffer
    % texture can be used for blitting by the core library:
    moglChooseFBO(0);
    
    % Setup special projection matrices for the BrightSide core lib:
    glMatrixMode(GL.PROJECTION);
    glPushMatrix;
%    glLoadIdentity;
% MK: This causes an OpenGL error:	gluOrtho2D(0, 0, winwidth, winheight); % This seems to be a quite odd coordinate system.
	glMatrixMode (GL.MODELVIEW);
    glPushMatrix;
%	glLoadIdentity;

    % Perform HDR-->LDR conversion:
    if ~dummymode
        % Call the BrightSide core library to do the conversion:
        BrightSideCore(2);
    else
        % Dummy mode: We do it ourselves.
        moglBlitTexture(hdrtexid);
    end

    % Restore normal matrices:
    glMatrixMode(GL.PROJECTION);
    glPopMatrix;
	glMatrixMode (GL.MODELVIEW);
    glPopMatrix;
    
    % Ready:
    inhdrdrawmode = 0;
    return;
end

% Unknown command.
error('BrightSideHDR: Unknown subcommand specified! Type ''help BrightSideHDR'' for usage info.');
return
