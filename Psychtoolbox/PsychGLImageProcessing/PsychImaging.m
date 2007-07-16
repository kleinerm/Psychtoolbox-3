function [rc winRect] = PsychImaging(cmd, varargin)
% rc = PsychImaging(subcommand [,arg1][,arg2][....,argn]) - Control common
% functions of the Psychtoolbox GPU image processing pipeline.
%
% This function allows you to setup and control various aspects and common
% functions of the Psychtoolbox image processing pipeline in a simple way.
% Various standard scenarious can be conveniently set up with this routine,
% e.g., geometric transformations of your stimulus image, various types of
% display correction, ...
%
% If you want to perform less common, unusual or simply not yet supported tasks
% with the pipeline, use the low-level Screen('HookFunction', ...)
% interface instead and have a peek in the M-File code for the
% PsychImaging.m file to learn about the low-level interface.
% See "help PsychGLImageprocessing" for more info.
%
%
% Subcommands and their meaning:
%
% PsychImaging('PrepareConfiguration');
% - Prepare setup of imaging pipeline for onscreen window.
% This is the first step in the sequence of configuration steps.
%
%
% PsychImaging('AddTask', whichChannel, whichTask [,param1]...);
% - Add a specific task or processing requirement to the list of actions
% to be performed by the pipeline for the currently selected onscreen
% window. 'whichChannel' is a string with the name of the channel to
% configure:
%
% 'LeftView' applies the action to the processing channel
% for the left-eye view of a stereo configuration. 'RightView' applies the
% action to the right-eye view of a stereo configuration. 'AllViews' applies
% the action to both, left- and right eye view channels of a stereo
% configuration or to the single monoscopic channel of a mono display
% configuration. Other options are 'Compositor', 'FinalFormatting' and
% 'Finalizer' for special purpose channels.
%
% 'whichTask' contains the name string of one of the supported
% actions:
%
% * 'RestrictProcessing' Restrict stimulus processing to a specific subarea
%   of the screen. If your visual stimulus only covers a subarea of the
%   display screen you can restrict PTB's output processing to that
%   subarea. This may save some computation time to allow for higher
%   display redraw rates.
%
%   Syntax: PsychImaging('AddTask', whichChannel, 'RestrictProcessing', ROI);
%   ROI is a rectangle defining the area to process ROI = [left top right bottom];
%   E.g., ROI = [400 400 800 800] would only create output pixels in the
%   screen area with top-left corner (400,400) and bottom-right corner
%   (800, 800).
%
% * 'FlipHorizontal' and 'FlipVertical' flip your output images
%   horizontally (left- and right interchanged) or vertically (upside down).
%
% * 'GeometryCorrection' Apply some geometric warping operation during
%   rendering of the final stimulus image to correct for geometric
%   distortion of your physical display device. You need to measure the
%   geometric distortion of your display with a suitable calibration
%   procedure, then compute an inverse warp transformation to undo this
%   distortion, then provide that transformation to this function.
%
%   Syntax: PsychImaging('AddTask', whichChannel, 'GeometryCorrection');
%
% * More actions will be supported in the future. If you can think of an
%   action of common interest not yet supported by this framework, please
%   file a feature request on our Wiki (Mainpage -> Feature Requests).
%
%
% imagingMode = PsychImaging('FinalizeConfiguration');
% - Finish the configuration phase for this window. This will compute an
% optimal configuration for all stages of the pipeline, but won't apply it
% yet. You'll have to call Screen('OpenWindow', windowPtr, ......,
% imagingMode, ...); with the returned 'imagingMode' + any other options
% you'd like to have for your window. After that, you'll have to call
% PsychImaging('PostConfiguration') to really apply and setup all your
% configuration settings. If you don't have unusual needs, you can simplify
% these steps by simply calling PsychImaging('OpenImagingWindow', ....);
% with the same parameters that you'd pass to Screen('OpenWindow', ....);
% PsychImaging will perform all necessary steps to upon return, you'll have
% your window properly configured.
%
%
% PsychImaging('PostConfiguration', windowPtr);
% - To be called after opening the onscreen window 'windowPtr'.
% Performs all the setup work to be done after the window was created.
%
%
% [windowPtr, windowRect] = PsychImaging('OpenWindow', screenid,
% [backgroundcolor], ....);
% - Finish the setup phase for imaging pipeline, create a suitable onscreen
% window and perform all remaining configuration steps.
%
%

% History:
% 3.6.2007 Written. (MK)

persistent configphase_active;
persistent reqs;

if isempty(configphase_active)
    configphase_active = 0;
end

if nargin < 1 || isempty(cmd)
    error('You did not provide any subcommand to execute!');
end

rc = [];
winRect = [];

if strcmp(cmd, 'PrepareConfiguration')
    % Prepare new configuration:
    if configphase_active
        error('Tried to prepare a new configuration phase, but you did not finalize the previous phase yet!');
    end
    
    configphase_active = 1;
    clear reqs;
    reqs = [];
    
    rc = 0;
    return;
end

if strcmp(cmd, 'AddTask')
    if nargin < 3 || isempty(varargin{1}) || isempty(varargin{2})
        error('Parameters missing: Need at least "whichChannel" and "whichTask"!');
    end
    
    if configphase_active ~= 1
        error('Call PsychImaging(''PrepareConfiguration''); first to prepare the configuration phase!');
    end
        
    % Store requirement in our cell array of requirements. We need to
    % extend each requirement vector to some number of max elements, so all
    % rows in the cell array have the same length:
    x = varargin;
    maxreqarg = 10;
    if length(x) < maxreqarg
        for i=length(x)+1:maxreqarg
            x{i}='';
        end
    end
    reqs = [reqs ; x];

    rc = 0;
    return;
end

if strcmp(cmd, 'FinalizeConfiguration')
    if configphase_active ~= 1
        error('You tried to finalize configuration, but no configuration in progress!');
    end

    if isempty(reqs)
        error('You tried to FinalizeConfiguration, but you did not specify any requirements or tasks!');
    end

    configphase_active = 2;
    
    % Compute correct imagingMode - Settings for current configuration and
    % return it:
    rc = FinalizeConfiguration(reqs);
    return;
end

if strcmp(cmd, 'PostConfiguration')
    if configphase_active ~= 2
        error('Tried to call PostConfiguration without calling FinalizeConfiguration before!');
    end
    
    if nargin < 2 || isempty(varargin{1}) || Screen('WindowKind', varargin{1})~=1
        error('No "windowPtr" or invalid "windowPtr" or non-onscreen window handle provided!');
    end

    rc = PostConfiguration(reqs, varargin{1});

    configphase_active = 0;
    return;
end
    
if strcmp(cmd, 'OpenWindow')
    if configphase_active ~= 1
        error('You tried to OpenImagingWindow, but didn''t specify any imaging configuration!');
    end

    if nargin < 2
        error('You must supply at least a "screenId" for the screen on which the window should be opened');
    end
    
    % Final config phase:
    configphase_active = 2;
    
    screenid = varargin{1};
    
    if nargin < 3 || isempty(varargin{2})
        clearcolor = [];
    else
        clearcolor = varargin{2};
    end
    
    if nargin < 4 || isempty(varargin{3})
        winRect = [];
    else
        winRect = varargin{3};
    end
    
    % Ignore pixelSize:
    pixelSize = [];
    
    % Override numbuffers -- always 2:
    numbuffers = 2;
    
    % Compute correct imagingMode - Settings for current configuration and
    % return it:
    [imagingMode needStereoMode] = FinalizeConfiguration(reqs);
    
    if nargin < 7 || isempty(varargin{6})
        stereomode = 0;
    else
        stereomode = varargin{6};
    end
    
    % Override stereomode derived from requirements?
    if needStereoMode ~= -1
        if needStereoMode == -2 && stereomode == 0
            % Stereo operation needed, but not set up by usercode:
            error('Your requirements demand a stereo presentation mode, but you didn''t specify one!');
        else
            if (needStereoMode > -1) && (stereomode ~= needStereoMode)
                warning('Your provided "stereomode" conflicts with required stereomode for imaging pipeline. Overriden...');
                stereomode = needStereoMode;
            end
        end
    end
    
    if nargin < 8 || isempty(varargin{7})
        multiSample = 0;
    else
        multiSample = varargin{7};
    end
    
    if nargin < 9 || isempty(varargin{8})
        imagingovm = 0;
    else
        imagingovm = varargin{8};
    end
    
    imagingMode = mor(imagingMode, imagingovm);
    
    % Open onscreen window with proper imagingMode and stereomode set up:
    if nargin > 10
        [win, winRect] = Screen('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode, varargin{9:end});
    else
        [win, winRect] = Screen('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode);
    end
        
    % Perform double-flip, so both back- and frontbuffer get initialized to
    % background color:
    Screen('Flip', win);
    Screen('Flip', win);
    
    % Window open. Perform imaging pipe postconfiguration:
    rc = PostConfiguration(reqs, win);

    rc = win;
    
    % Done.
    configphase_active = 0;

    return;
end

% Catch all for unknown commands:
error('Unknown subcommand specified! Read "help PsychImaging" for usage info.');

end

% Internal helper routines:

% FinalizeConfiguration consolidates the current set of requirements and
% derives the needed stereoMode settings and imagingMode setting to pass to
% Screen('OpenWindow') for pipeline preconfiguration.
function [imagingMode stereoMode] = FinalizeConfiguration(reqs)

% Set imagingMode to minimum: Pipeline enabled...
imagingMode = kPsychNeedFastBackingStore;

% Set stereoMode to don't care:
stereoMode = -1;

if ~isempty(find(strcmp(reqs, 'LeftView'))) || ~isempty(find(strcmp(reqs, 'RightView')))
    % Specific eye channel requested: Need a stereo display mode.
    stereoMode = -2;
    
    % Also need image processing stage, because only it can provide
    % separate processing for both eyes:
    imagingMode = mor(imagingMode, kPsychNeedImageProcessing);
else
    % Not a single eye specific command requested: Check if there's any
    % other spec that would require the image processing stage:
    % Any command that applies to 'AllViews' naturally needs the image
    % processing:
    if ~isempty(find(strcmp(reqs, 'AllViews')))
        imagingMode = mor(imagingMode, kPsychNeedImageProcessing);
    end
end

% Image processing stage needed?
if imagingMode & kPsychNeedImageProcessing
    % Yes. How many commands per chain?
    nrslots = max(length(find(strcmp(reqs, 'LeftView'))), length(find(strcmp(reqs, 'RightView'))));
    nrslots = max(nrslots, length(find(strcmp(reqs, 'AllViews'))));
    
    % More than one slot per chain? Otherwise we use the default
    % single-pass chain:
    if nrslots > 1
        % More than two slots per chain?
        if nrslots > 2
            % Need full blown multistage chain:
            imagingMode = mor(imagingMode, kPsychNeedMultiPass);
        else
            % Only two slots. More efficient dual-pass chain is sufficient:
            imagingMode = mor(imagingMode, kPsychNeedDualPass);
        end
    end
end

% Final output formatting stage needed?
if ~isempty(find(strcmp(reqs, 'FinalFormatting')))
    imagingMode = mor(imagingMode, kPsychNeedOutputConversion);
end

return;
end      % Of FinalizeConfiguration subroutine.

% PostConfiguration is called after the onscreen window is open: Performs
% actual pipeline setup of the hook chains:
function rc = PostConfiguration(reqs, win)

global GL;

if isempty(GL)
    % Perform minimal OpenGL init, so we can call OpenGL commands and use
    % GL constants. We do not activate a full 3D rendering context:
    InitializeMatlabOpenGL([], [], 1);
end

% Number of used slots in left- and right processing chain:
leftcount = 0;
rightcount = 0;
outputcount = 0;

% Flags for horizontal/vertical flip operations:
leftUDFlip = 0;
rightUDFlip = 0;
leftLRFlip = 0;
rightLRFlip = 0;

% Stereomode?
winfo = Screen('GetWindowInfo', win);

% --- First action in pipe is a horizontal- or vertical flip, if any ---

% Any flip horizontal requested?
floc = find(strcmp(reqs, 'FlipHorizontal'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            if strcmp(reqs{row, 1}, 'LeftView') || strcmp(reqs{row, 1}, 'AllViews')
                % LeftView horizontal flip.
                leftLRFlip = 1;
            end

            if strcmp(reqs{row, 1}, 'RightView') || strcmp(reqs{row, 1}, 'AllViews')
                % LeftView horizontal flip.
                rightLRFlip = 1;
            end
        end
    end
end

% Any flip vertical requested?
floc = find(strcmp(reqs, 'FlipVertical'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            if strcmp(reqs{row, 1}, 'LeftView') || strcmp(reqs{row, 1}, 'AllViews')
                % LeftView vertical flip.
                leftUDFlip = 1;
            end

            if strcmp(reqs{row, 1}, 'RightView') || strcmp(reqs{row, 1}, 'AllViews')
                % LeftView vertical flip.
                rightUDFlip = 1;
            end
        end
    end
end

% Left channel flipping needed?
if leftLRFlip || leftUDFlip
    % Yes.
    sx = 1;
    ox = 0;
    sy = 1;
    oy = 0;

    if leftLRFlip
        sx = -1;
        ox = RectWidth(Screen('Rect', win));
    end

    if leftUDFlip
        sy = -1;
        oy = RectHeight(Screen('Rect', win));
    end

    % Enable left imaging chain:
    Screen('HookFunction', win, 'Enable', 'StereoLeftCompositingBlit');
    % Append blitter for LR/UD flip:
    Screen('HookFunction', win, 'AppendBuiltin', 'StereoLeftCompositingBlit', 'Builtin:IdentityBlit', sprintf('Offset:%i:%i:Scaling:%f:%f', ox, oy, sx, sy));
    leftcount = leftcount + 1;
end

if winfo.StereoMode > 0
    % Stereomode enabled: Need to possibly handle right channel as
    % well. In mono-mode there would be only a left channel...

    % Right channel flipping needed?
    if rightLRFlip || rightUDFlip
        % Yes.
        sx = 1;
        ox = 0;
        sy = 1;
        oy = 0;

        if rightLRFlip
            sx = -1;
            ox = RectWidth(Screen('Rect', win));
        end

        if rightUDFlip
            sy = -1;
            oy = RectHeight(Screen('Rect', win));
        end

        % Enable right imaging chain:
        Screen('HookFunction', win, 'Enable', 'StereoRightCompositingBlit');
        % Append blitter for LR/UD flip:
        Screen('HookFunction', win, 'AppendBuiltin', 'StereoRightCompositingBlit', 'Builtin:IdentityBlit', sprintf('Offset:%i:%i:Scaling:%f:%f', ox, oy, sx, sy));
        rightcount = rightcount + 1;
    end
end

% --- End of the flipping stuff ---

% --- Geometry correction via warped blit ---
floc = find(strcmp(reqs, 'GeometryCorrection'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            % Extract first parameter:
            calibmatrix = reqs{row, 3};

            % Is it a display list handle?
            if ndims(calibmatrix)==2 && length(calibmatrix)==1
                % One single value: This must be a display list handle.
                gld = double(calibmatrix);
                if ~glIsList(gld)
                    % Game over:
                    Screen('CloseAll');
                    error('PsychImaging: Passed a handle to ''GeometryCorrection'' which is not a valid OpenGL display list!');
                end
            else
                % Its hopefully the filename of a calibration file:

                % Build OpenGL display list which contains the warp-mapping:
                [w, h] = Screen('WindowSize', win);
                subdivision = 100;
                xnum = 2;
                ynum = 2;
                gld = glGenLists(1);
                glNewList(gld, GL.COMPILE);
                frompts = zeros(2, 2, 2);
                frompts(:, 1, 1) = [0 h];
                frompts(:, 1, 2) = [w h];
                frompts(:, 2, 2) = [w 0];
                frompts(:, 2, 1) = [0 0];

                topts = zeros(3, ynum, xnum);
                % Top-Left:
                topts(:, 1, 1) = [0 0 0];
                % Top-Middle:
                topts(:, 1, 2) = [w/2 0 0];
                % Top-Right
                topts(:, 1, 3) = [w 0 0];
                % Bottom-Right:
                topts(:, 2, 3) = [w h 0];
                % Bottom-Middle:
                topts(:, 2, 2) = [w/2 h 0];
                % Bottom-Left:
                topts(:, 2, 1) = [0 h 0];

                glMap2d(GL.MAP2_VERTEX_3, 0, 1, 3, size(topts,2), 0, 1, 3*size(topts,2), size(topts,3), topts);
                glMap2d(GL.MAP2_TEXTURE_COORD_2, 0, 1, 2, size(frompts,2), 0, 1, 2*size(frompts,2), size(frompts,3), frompts);
                glEnable(GL.MAP2_VERTEX_3);
                glEnable(GL.MAP2_TEXTURE_COORD_2);
                glMapGrid2d(subdivision, 0, 1, subdivision, 0, 1);
                glEvalMesh2(GL.FILL, 0, subdivision, 0, subdivision);
                glDisable(GL.MAP2_VERTEX_3);
                glDisable(GL.MAP2_TEXTURE_COORD_2);
                glEndList;
            end

            % Ok, 'gld' should contain a valid OpenGL display list for
            % geometry correction. Attach proper blitter to proper chain:
            
            if strcmp(reqs{row, 1}, 'LeftView') || strcmp(reqs{row, 1}, 'AllViews')
                % Need to setup left view warp:
                if leftcount > 0
                    % Need a bufferflip command:
                    Screen('HookFunction', win, 'AppendBuiltin', 'StereoLeftCompositingBlit', 'Builtin:FlipFBOs', '');
                end
                Screen('HookFunction', win, 'AppendBuiltin', 'StereoLeftCompositingBlit', 'Builtin:IdentityBlit', sprintf('Blitter:DisplayListBlit:Handle:%i:Bilinear', gld));
                Screen('HookFunction', win, 'Enable', 'StereoLeftCompositingBlit');
                leftcount = leftcount + 1;
            end

            if strcmp(reqs{row, 1}, 'RightView') || (strcmp(reqs{row, 1}, 'AllViews') && winfo.StereoMode > 0)
                % Need to setup right view warp:
                if rightcount > 0
                    % Need a bufferflip command:
                    Screen('HookFunction', win, 'AppendBuiltin', 'StereoRightCompositingBlit', 'Builtin:FlipFBOs', '');
                end
                Screen('HookFunction', win, 'AppendBuiltin', 'StereoRightCompositingBlit', 'Builtin:IdentityBlit', sprintf('Blitter:DisplayListBlit:Handle:%i:Bilinear', gld));
                Screen('HookFunction', win, 'Enable', 'StereoRightCompositingBlit');
                rightcount = rightcount + 1;
            end
            
            if strcmp(reqs{row, 1}, 'FinalFormatting')
                % Need to setup final formatting warp:
                if outputcount > 0
                    % Need a bufferflip command:
                    Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');
                end
                Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:IdentityBlit', sprintf('Blitter:DisplayListBlit:Handle:%i:Bilinear', gld));
                Screen('HookFunction', win, 'Enable', 'FinalOutputFormattingBlit');
                outputcount = outputcount + 1;
            end            
        end
    end
end
% --- End of geometry correction via warped blit ---

% --- Restriction of processing area ROI requested? ---

% This should be at the end of setup, so we can reliably prepend the
% command to each chain to guarantee that restriction applies to all
% processing:
floc = find(strcmp(reqs, 'RestrictProcessing'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            % Extract scissor rectangle:
            scissorrect = reqs{row, 3};
            if size(scissorrect,1)~=1 || size(scissorrect,2)~=4
                error('Task "RestrictProcessing" in channel %s expects a 1-by-4 ROI rectangle to define the ROI, e.g, [left top right bottom]!', reqs{row,1});
            end

            ox = scissorrect(RectLeft);
            oy = scissorrect(RectTop);
            w  = RectWidth(scissorrect);
            h  = RectHeight(scissorrect);
            
            if strcmp(reqs{row, 1}, 'LeftView') || strcmp(reqs{row, 1}, 'AllViews')
                % Need to restrict left view processing:
                Screen('HookFunction', win, 'PrependBuiltin', 'StereoLeftCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
            end

            if strcmp(reqs{row, 1}, 'RightView') || strcmp(reqs{row, 1}, 'AllViews')
                % Need to restrict right view processing:
                Screen('HookFunction', win, 'PrependBuiltin', 'StereoRightCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
            end
            
            if (strcmp(reqs{row, 1}, 'AllViews') || strcmp(reqs{row, 1}, 'Compositor')) && winfo.StereoMode > 5
                % Needed to restrict both views processing and a
                % compositing mode is active. If both views are restricted
                % in their output area then it makes sense to restrict the
                % compositor to the same area. We also restrict the
                % compositor if that was requested.
                Screen('HookFunction', win, 'PrependBuiltin', 'StereoCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
            end

            if strcmp(reqs{row, 1}, 'FinalFormatting')
                % Need to restrict final formatting blit processing:
                Screen('HookFunction', win, 'PrependBuiltin', 'FinalOutputFormattingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
            end
            
        end
    end
end

% --- End of Restriction of processing area ROI ---

% Return reqs array, for whatever reason...
rc = reqs;
return;

end % Of PostConfiguration subroutine.
