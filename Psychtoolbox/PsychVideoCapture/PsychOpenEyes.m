function rc = PsychOpenEyes(cmd, handle, varargin)
% result = PsychOpenEyes(cmd, handle [, arg1, arg2, ...]);
%
% PsychOpenEyes controls Psychtoolboxs built-in computer vision based eye
% tracker, based on the free-software open-source "OpenEyes" toolkit.
%
% Syntax: result = PsychOpenEyes(command, handle, optional arguments...);
%
% 'command' is a subcommand, similar to the subcommands in Screen() and
% other PTB functions.
%
% 'handle' A numeric handle to the tracker connection. Currently you should
% just pass zero (0) as handle.
%
% 'arg1, arg2, ...' Optional arguments for different subcommands.
%
%
% Subcommand and their syntax/meaning:
%
% PsychOpenEyes('OpenTracker')

persistent EyeImageMemBuffer;
persistent EyeOutImageMemBuffer;
persistent SceneImageMemBuffer;
persistent ThresholdImageMemBuffer;
persistent EllipseImageMemBuffer;
persistent videoptr;
persistent win;
persistent ownwin;
persistent starttime;
persistent fps;
persistent eyeWidth;
persistent eyeHeight;
persistent eyeChannels;
persistent sceneWidth;
persistent sceneHeight;

if nargin < 1
    cmd = [];
end

if isempty(cmd)
    error('PsychOpenEyes: Subcommand missing! Must set one.');
end

if nargin < 2
    handle = [];
end

if isempty(handle)
    error('PsychOpenEyes: Tracker connection handle missing! Must set one.');
end

% Subcommand dispatch:

if strcmp(cmd, 'GetGazePosition')
    % Perform a single eye tracking cycle:
    
    if nargin >=4 && ~isempty(varargin{1})
        pupilx = varargin{1};
        pupily = varargin{2};
        mode = 3;
    else
        pupilx = [];
        pupily = [];
        mode = 0;
    end
    
    % Wait for next captured video frame, then fill the trackers in-memory
    % buffer with the captured data. Do not return a texture:
    waitflag = 2;
    specialflag = 4;
    [eyeTex, cts] = Screen('GetCapturedImage', win, videoptr, waitflag, [], specialflag, EyeImageMemBuffer);
    
    % Ok, the tracker should have the eye image in its internal buffer.
    % Process it:
    EyeResult = PsychCV('OpenEyesTrackEyePosition', handle, mode, pupilx, pupily);
    
    % EyeResult hopefully contains tracking result.
    EyeResult.Delay = EyeResult.Timestamp - cts;
    
    % Assign return argument and you're done:
    rc = EyeResult;
    return;
end

if strcmp(cmd, 'SetDynamicConstraints')
    if nargin < 6
        error('Must set all dynamic constraints!!! (minDist, maxDist, minArea, maxArea)');
    end
    
    mode = 5;
    minDist = varargin{1};
    maxDist = varargin{2};
    minArea = varargin{3};
    maxArea = varargin{4};
    EyeResult = PsychCV('OpenEyesTrackEyePosition', handle, mode, minDist, maxDist, minArea, maxArea);

    % Assign return argument and you're done:
    rc = EyeResult;
    return;
end

if strcmp(cmd, 'SetReferencePoint')
    if nargin < 4
        error('Must provide reference point position (rx, ry).');
    end
    
    mode = 6;
    PsychCV('OpenEyesTrackEyePosition', handle, mode, varargin{1}, varargin{2});

    rc = 0;
    return;
end

if strcmp(cmd, 'GetTrackerTexture');
    % Texture handle for a feedback texture requested: These contain image
    % data processed by the OpenEyes eyetracker - either result
    % visualizations or debug images.
    
    % Choose between different images:
    if nargin < 3 || isempty(varargin{1})
        % Return eye-image by default:
        texmemptr = EyeImageMemBuffer;
    else
        switch(varargin{1})
            case 0,
                % Return processed eye image:
                texmemptr = EyeOutImageMemBuffer;
                width = eyeWidth;
                height = eyeHeight;
                depth = 1;
            case 1,
                % Return scene image:
                texmemptr = SceneImageMemBuffer;
                width = sceneWidth;
                height = sceneHeight;
                depth = 3;
            case 2,
                % Return threshold image:
                texmemptr = ThresholdImageMemBuffer;
                width = eyeWidth;
                height = eyeHeight;
                depth = 1;
            case 3,
                % Return ellipse image:
                texmemptr = EllipseImageMemBuffer;
                width = eyeWidth;
                height = eyeHeight;
                depth = 3;
            case 4,
                texmemptr = EyeImageMemBuffer;
                width = eyeWidth;
                height = eyeHeight;
                depth = eyeChannels;
                
            otherwise
                error('Invalid feedback texture type requested - Valid between zero and 3');
        end
    end
    
    % Build PTB Texture from content of the memory buffer pointed to by
    % texmemptr. This is a PsychCV - internal memory buffer:
    upsidedown = 1;
    tex = Screen('SetOpenGLTextureFromMemPointer', win, [], texmemptr, width, height, depth, upsidedown);

    rc = tex;
    return;
end

if strcmp(cmd, 'CalibrateMapping')

    % This is a 9-Point Grid calibration. Iterate through all nine points,
    % show each point onscreen, wait for keypress, then acquire eye+scene
    % sample and store it in internal calibration:
    
    % Delete old (previous) calibration:
    PsychCV('OpenEyesTrackEyePosition', handle, 2);

    % Double flip to get a neutral background:
    Screen('Flip', win);
    Screen('Flip', win);
    
    winrect = Screen('Rect', win);
    tscale = 0.8;
    trect = ScaleRect(winrect, tscale, tscale);
    trect = CenterRect(trect, winrect);
    xoff = trect(RectLeft);
    yoff = trect(RectTop);
    xinc = RectWidth(trect) / 2;
    yinc = RectHeight(trect) / 2;
    
    for xp = 1:3
        for yp = 1:3
            % Draw fixation target:
            tx = (xp - 1) * xinc + xoff;
            ty = (yp - 1) * yinc + yoff;
            Screen('FillOval', win, [255 255 0], CenterRectOnPoint([0 0 10 10], tx, ty));
            Screen('DrawText', win, 'FIXATE ME AND PRESS SPACE KEY!', tx + 5, ty + 5, [0 255 0]);
            Screen('Flip', win);
            while KbCheck; end;
            KbWait;
            
            % Wait for next captured video frame, then fill the trackers in-memory
            % buffer with the captured data. Do not return a texture:
            waitflag = 2;
            specialflag = 4;
            [eyeTex, cts] = Screen('GetCapturedImage', win, videoptr, waitflag, [], specialflag, EyeImageMemBuffer);
            [eyeTex, cts] = Screen('GetCapturedImage', win, videoptr, waitflag, [], specialflag, EyeImageMemBuffer);
            [eyeTex, cts] = Screen('GetCapturedImage', win, videoptr, waitflag, [], specialflag, EyeImageMemBuffer);

            % Ok, the tracker should have the eye image in its internal buffer.
            % Process it:
            eyepos = PsychCV('OpenEyesTrackEyePosition', handle, 0);
            
            pupx(xp, yp) = eyepos.PupilX;
            pupy(xp, yp) = eyepos.PupilY;
            
            % Submit scene point position and add as calibration point:
            PsychCV('OpenEyesTrackEyePosition', handle, 4, tx, ty); 
        end
    end
    
    % Activate calibration:
    PsychCV('OpenEyesTrackEyePosition', handle, 1);
    
    % Calibration sequence finished. Clear screen:
    Screen('Flip', win);

    for xp = 1:3
        for yp = 1:3
            tx = (xp - 1) * xinc + xoff;
            ty = (yp - 1) * yinc + yoff;
            Screen('FillOval', win, [255 255 0], CenterRectOnPoint([0 0 10 10], tx, ty));
            Screen('DrawText', win, sprintf('Scene %i, %i', xp, yp), tx + 5, ty + 5, [0 255 0]);
            
            tx = pupx(xp, yp);
            ty = pupy(xp, yp);
            
            Screen('FillOval', win, [255 255 0], CenterRectOnPoint([0 0 10 10], tx, ty));
            Screen('DrawText', win, sprintf('Pupil %i, %i', xp, yp), tx + 5, ty + 5, [0 255 0]);
            
        end
    end
    
    Screen('Flip', win);
    while KbCheck; end
    KbWait;
    
    Screen('Flip', win);
    
    rc = 1;
    return;
end

if strcmp(cmd, 'TrackerParameters')
    % Query or setup/change of tracker operating parameters requested:
    
    if nargin < 3 || isempty(varargin{1})
        % Pure query:
        [rc.pupilEdgeThresh, rc.rays, rc.minCand, rc.corneaWinSize, rc.edgeThresh, rc.gaussWidth, rc.eccentricity, rc.initialAngleSpread, rc.fanoutAngle1, rc.fanoutAngle2, rc.featuresPerRay, rc.specialFlags] = PsychCV('OpenEyesParameters', handle);
    else
        % Query old, set new:
        [rc.pupilEdgeThresh, rc.rays, rc.minCand, rc.corneaWinSize, rc.edgeThresh, rc.gaussWidth, rc.eccentricity, rc.initialAngleSpread, rc.fanoutAngle1, rc.fanoutAngle2, rc.featuresPerRay, rc.specialFlags] = PsychCV('OpenEyesParameters', handle, varargin{:});
    end
    
    return;
end

if strcmp(cmd, 'OpenTracker')
    % Open a tracker connection, initialize tracker:
    
    % Create a unique handle:
    handle = 1;
    
    % Do we need to open our own dummy window?
    if nargin < 3 || isempty(varargin{1})
        % Open our own dummy window:
        win = Screen('OpenWindow', 0, 0, [0 0 4 4]);
        ownwin = 1;
    else
        win = varargin{1};
        ownwin = 0;
    end
    
    % Open the PTB video capture engine:
    eyeCam = 0;
    captureEngineType = [];
    captureRateFPS = 30;
    logfilename = [];
    eyeROI = [];
    eyeChannels = 1;
    
    videoptr = Screen('OpenVideoCapture', win, eyeCam, eyeROI, eyeChannels, [], [], [], [], captureEngineType);

    % Start capture engine and grab first frame:
    fps = Screen('StartVideoCapture', videoptr, captureRateFPS, 1);
    [eyeTex, starttime]=Screen('GetCapturedImage', win, videoptr, 1);
    
    % Get rectangle, so we know the eye image size for sure:
    eyeROI = Screen('Rect', eyeTex);
    eyeWidth = RectWidth(eyeROI);
    eyeHeight = RectHeight(eyeROI);
    sceneWidth = [];
    sceneHeight = [];
    
    % Query the real number of image channels we've got:
    eyeChannels = Screen('PixelSize', eyeTex) / 8;
    
    % Release eyeTex:
    Screen('Close', eyeTex);

    % Call low-level init and retrieve all relevant memory buffer pointers:
    [EyeOutImageMemBuffer, EyeImageMemBuffer, SceneImageMemBuffer, ThresholdImageMemBuffer, EllipseImageMemBuffer] = PsychCV('OpenEyesInitialize', handle, eyeChannels, eyeWidth, eyeHeight, sceneWidth, sceneHeight, logfilename);

    % Tracker should be online now...
    
    rc = videoptr;
    return;
end

if strcmp(cmd, 'CloseTracker')
    
    % Stop capture engine and release it:
    droppedframes = Screen('StopVideoCapture', videoptr);
    Screen('CloseVideoCapture', videoptr);
    
    % Close window if it was created by us:
    if ownwin
        Screen('Close', win);
    end
    
    win = [];
    ownwin = 0;

    % Release and shutdown OpenEyes:
    PsychCV('OpenEyesShutdown', handle);
    
    % Done.
    rc = droppedframes;

    return;
end

error('Invalid/Unknown sub command specified! Read "help PsychOpenEyes" for valid commands.');
