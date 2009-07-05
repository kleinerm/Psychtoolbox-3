function VideoTextureExtractionDemo(lightson, objtype, multiMarker)
% Use ARToolkit to track and visualize 3D objects in live-video.
%
% Usage: ARToolkitDemo([lightson = 0][, objtype = 3][, multiMarker = 2])
%
% Minimalistic demo on how to capture video data and use ARToolkit to
% detect and track the rigid position and orientation of markers, then
% visualize corresponding 3D objects on top of video via OpenGL.
%
% Parameters:
%
% multiMarker = If set to 2 (default), track single markers. If set to 1,
% track so called multi markers or composite markers for more robustness
% against occlusion of single markers. A setting of 3 (=1+2) should track
% both types of markers simultaneously, but this doesn't seem to work well
% yet.
%

% History:
% 19.04.2009  mk  First prototype written.

global GL;
global sizeMM;

% Running on PTB-3? Hopefully...
AssertOpenGL;

% Size of rendered objects in millimeters:
sizeMM = 40; %#ok<NASGU>

if nargin < 1
    lightson = [];
end

if isempty(lightson)
    lightson = 0;
end

if nargin < 2
    objtype = [];
end

if isempty(objtype)
    objtype = 3;
end

try
    % No sync tests for this simple demo:
    olsync = Screen('Preference', 'SkipSyncTests', 2);
    
    InitializeMatlabOpenGL;
    
    % Open onscreen window for diplay:
    screenid = max(Screen('Screens'));
    
    % Size of window: Default to fullscreen:
    if objtype ~= 3
        roi = CenterRect(1.5 * [0 0 640 480], Screen('Rect',screenid));
    else
        roi = [];
    end
    
    % Use imaging pipeline to flip image horizontally to avoid confusing
    % the viewer due to mirror view:
    PsychImaging('PrepareConfiguration');
    PsychImaging('AddTask', 'General', 'UseFastOffscreenWindows');
    PsychImaging('AddTask', 'AllViews', 'FlipHorizontal');
    win = PsychImaging('OpenWindow', screenid, 0, roi);

    % Open videocapture device: For now we use engine 2, the ARVideo
    % engine, to simplify compatibility issues...
    imgFormat = [];
    engineId = 0;
    
    switch engineId
        case 2,
            grabber = Screen('OpenVideoCapture', win, 0, [], 5, [], [], [], [], engineId);
        case 0,
            devid = PsychGetCamIdForSpec('Eye');
            grabber = Screen('OpenVideoCapture', win, devid, [], 1, [], [], [], [], engineId);
%            imgFormat = 4;
        case 1,
            grabber = Screen('OpenVideoCapture', win, 0, [0 0 640 480], [], [], [], [], [], engineId);
        otherwise
            error('Unknown video capture engineId');
    end
    
    % Start capture with requested 30 fps:
    Screen('StartVideoCapture', grabber, 30, 1);

    % Get a single texture that we can query for the image format:
    tex = Screen('GetCapturedImage', win, grabber);
    [w, h] = Screen('WindowSize', tex) %#ok<NOPRT>
    channels = Screen('PixelSize', tex) / 8 %#ok<NOPRT>
    
    % Release texture after query:
    Screen('Close', tex);
    tex = []; %#ok<NASGU>
    
    % Debug level for tracker:
    PsychCV('Verbosity', 3);
    
    % Initialize ARToolkit: Load camera calibration data, assign image size
    % and properties for raw video input images:
    ardata = [ PsychtoolboxRoot 'PsychDemos/ARToolkitDemoData/' ];
    PsychCV('ARRenderSettings', [], [], 2000);
    
    % ON OS/X:
    %
    % imgFormat == 4 fuer Quicktime 0,4 channel or ARVideo 0,4 channel.
    % imgFormat == [] or 7 fuer ARVideo 5 channels (aka 4 channels swizzled).
    % imgFormat == [] or 1 fuer Quicktime or ARVideo 3 channels.
    % imgFormat == [] or 6 fuer Quicktime 1,2 channels.
    % imgFormat == [] fuer Firewire 0,1,2,3,4 channels.
    [imgbuffer, projectionMatrix, debugimagebuffer] = PsychCV('ARInitialize', [ardata 'camera_para.dat'], w, h, channels, imgFormat); %#ok<NASGU>
    
    % [templateMatchingInColor, imageProcessingFullSized, imageProcessingIdeal, trackingWithPCA] = PsychCV('ARTrackerSettings')

    % Single markers by default:
    if nargin < 3
        multiMarker = 2;
    end
    
    % Load our (multi-)marker(s):
    % Need to cd() into marker directory, as some paths inside the marker
    % config files are coded relative to working directory :-(
    olddir = pwd;
    try
        marker = [];
                
        if bitand(multiMarker, 2)
            cd(ardata);
%            marker(end+1) = PsychCV('ARLoadMarker', 'patt.sample2', 0, 40);
%            marker(end+1) = PsychCV('ARLoadMarker', 'patt.hiro', 0);
            marker(end+1) = PsychCV('ARLoadMarker', 'patt.kanji', 0, 40);
        end
        
        if bitand(multiMarker, 1)
            cd([ ardata 'multi/' ]);
            marker(end+1) = PsychCV('ARLoadMarker', 'marker.dat', 1); %#ok<NASGU>
        end        
    catch
    end
    cd(olddir);

    if objtype == 3
        % Setup our 3D dense OpenGL head model for mirroring -- Our "Avatar":
        MFHeadmodel('Initialize', win);

        % Enable lighting model for head rendering:
        MFHeadmodel('SetLighting', 0);

        % Load OBJ's and initialize morph & render engine:
        CGHeadModelFile = 'CGHeadModels/TwentySevenCGKeyExpressions/TwentySevenCGKeyExpressions.mat';
        ExpressionsCount = MFHeadmodel('LoadModels', sprintf('%s%s', MFData, CGHeadModelFile)); %#ok<NASGU>
        
        % Compute nonrigid facial expression morph for this frame:
        morphWeights = 1;
        MFHeadmodel('MorphHead', morphWeights);
        
        texprops = MFHeadmodel('GetModelTextureProperties') %#ok<NOPRT>
        texResolution = texprops.texResolution;
        texSize = max(texResolution);
        texCoordMin = texprops.texCoordMin;
        texCoordMax = texprops.texCoordMax;
        texSize = 512;
        texScale = texSize / max(texResolution);
        texCoordMin = [0, 0];
        texCoordMax = [texSize, texSize];
        texResolution = [texSize, texSize];
        
    else
        texSize = 512;
        texCoordMin = [0, 0];
        texCoordMax = [texSize, texSize];
        texResolution = [texSize, texSize];
    end
    
    % zThreshold may need tweaking for depth range of object:
    zThreshold = 0.000001;
    callbackEvalString = 'glCallList(gld);';
    
    context = moglExtractTexture('CreateContext', win, [0 0 w h], texCoordMin, texCoordMax, texResolution, zThreshold);
    context = moglExtractTexture('SetRenderCallback', context, callbackEvalString);
    
    % Setup OpenGL 3D rendering for our simple test:
    Screen('BeginOpenGL', win);

    % Enable lighting and depth-test:
    glColor3f(1,1,1);
    if lightson
        glEnable(GL.LIGHTING);
    end
    glEnable(GL.LIGHT0);
    glEnable(GL.DEPTH_TEST);

    % Set projection matrix to one provided by ARToolkit:
    glMatrixMode(GL.PROJECTION);
    glLoadIdentity;
    glLoadMatrixd(projectionMatrix);
    
    % Init modelview matrix to identity: Will be overwritten, reloaded on
    % each object draw operation by modelview matrix from ARToolkit:
    glMatrixMode(GL.MODELVIEW);
    glLoadIdentity;

    % Position lightsource:
    glLightfv(GL.LIGHT0,GL.POSITION,[ 1 2 3 0 ]);

    % Define background clearcolor etc.:
    glClearColor(0,0,0,0);
    glClear;

    % Absolutely crucial that alpha blending is "off"!
    glDisable(GL.BLEND);

    % Build display list with static texture extraction 3D object:
    gld = glGenLists(1);
    glNewList(gld, GL.COMPILE);
    texmin = 0.00;
    texmax = 1.00;
    glPushMatrix;

    switch(objtype)
        case 1,
            glTranslated(-sizeMM/2, -sizeMM/2, 0);
            glTranslated(0, -sizeMM, 0);
            glBegin(GL.QUADS);
            glTexCoord2f(texmin, texmin);
            glVertex2f(0.0, 0.0);
            glTexCoord2f(texmin, texmax);
            glVertex2f(0.0, 1.0 * sizeMM);
            glTexCoord2f(texmax, texmax);
            glVertex2f(1.0 * sizeMM, 1.0 * sizeMM);
            glTexCoord2f(texmax, texmin);
            glVertex2f(1.0 * sizeMM, 0.0);
            glEnd;

        case 2,
            glTranslated(-sizeMM/2, -sizeMM/2, 0);
            mysphere = gluNewQuadric;
            gluQuadricTexture(mysphere, GL.TRUE);
            glMatrixMode(GL.TEXTURE);
            glPushMatrix;
            glLoadIdentity;
            glScaled(texSize, texSize, 1);
            gluSphere(mysphere, sizeMM, 100, 100);
            glPopMatrix;
            glMatrixMode(GL.MODELVIEW);
            gluDeleteQuadric(mysphere);

        case 3,
            glTranslated(0, 1*sizeMM, 0);
            glRotated(180, 0, 0, 1);
            glMatrixMode(GL.TEXTURE);
            glPushMatrix;
            glLoadIdentity;
            glScaled(texScale, texScale, 1);
            moglmorpher('render');
            glPopMatrix;
            glMatrixMode(GL.MODELVIEW);
    end

    glPopMatrix;
    glEndList;

    % OpenGL 3D setup done:
    Screen('EndOpenGL', win);
    
    % Shutdown head renderer: We can release the MFHeadmodels ressources
    % here already, because we only need 1 static "neutral expression"
    % shape, which we stored conveniently into a GL display list above :-)
    MFHeadmodel('Shutdown');
    
    % Init segmentation gain to 50% of avg image intensity:
    gain = 0.5;
    SetMouse(gain * Screen('WindowSize', win)/2, 100);
    
    count = 0;
    t=GetSecs;
    KbReleaseWait;

    inImage = [];
    texBuffer = [];
    doExtract = 0;
    buttons = [0 0 0];
    
    % Run tracking and viz loop until keypress:
    
    while ~buttons(2) 
        % Update image segmentation gain from horizontal mouse position:
        [xm, ym, buttons] = GetMouse;
        gain = 2 * xm / Screen('WindowSize', win);
        
        if xm == 0 && ym == 0
            break;
        end
        
        if buttons(1) && ~oldbuttons
            doExtract = 1 - doExtract;
        end

        oldbuttons = buttons(1);
        
        % Retrieve next captured image. The 'waitforImage=2' waits for a
        % new image, but disables texture creation, so 'dummy' is actually
        % an empty handle. The 'specialMode'=4 flag requests video data to
        % be put into the PsychCV() memory buffer referenced by
        % 'imgbuffer'. 'imgIntensity' returns the average image intensity -
        % Useful for computing an adaptive image segmentation threshold:
        [inImage pts nrdropped imgIntensity] = Screen('GetCapturedImage', win, grabber, 1, inImage, 4, imgbuffer);
                
        % Define dynamic threshold for image segmentation:
        thresh = max(min(imgIntensity * gain, 254), 1);

        % ARToolkit processing:
        detectedMarkers = PsychCV('ARDetectMarkers', [], thresh);
        
        % Visualize detected objects in 3D mode:
        Screen('BeginOpenGL', win);

        % Clear z-buffer for proper occlusion handling:
        glClear(GL.DEPTH_BUFFER_BIT);
         
        % Render current video image as backdrop:
        PsychCV('ARRenderImage');
        
        % For each candidate marker do...
        for i=1:length(detectedMarkers)
            % i'th candidate detected reliably?
            merr = detectedMarkers(i).MatchError;

            if merr < realmax
                % Yes: Draw 3D object corresponding to i'th pattern:
                mpos = detectedMarkers(i).ModelViewMatrix(1:3, 4);

                statusTxt = sprintf('POS X = %04f  Y = %04f  Z = %04f', mpos(1), mpos(2), mpos(3));
                DrawFormattedText(win, statusTxt, 'center', 0, [255 0 0], [], 1);

                % Setup rigid position and orientation via 4x4 xform matrix:
                glLoadMatrixd(detectedMarkers(i).ModelViewMatrix);

                if doExtract                    
                    % Perform texture extraction pass from input video image:
                    [texBuffer, texId, texTarget] = moglExtractTexture('Extract', context, inImage, 0);
                else
                    % Perform regular render pass:
                    if isempty(texBuffer)
                        glCallList(gld);
                    end
                end
                
                % Rerender:
                if ~isempty(texBuffer)
                    if objtype ~= 3
                        glTranslatef(2.2*sizeMM, 0, 0);
                    end
                    glEnable(texTarget);
                    glBindTexture(texTarget, texId);
                    glCallList(gld);
                    glBindTexture(texTarget, 0);
                    glDisable(texTarget);
                end                
            end
        end
        
        % Done with objects:
        Screen('EndOpenGL', win);

        if doExtract && ~isempty(texBuffer)
            % Draw extracted texture:
%            Screen('DrawTexture', win, texBuffer);
        end

%         if ~isempty(texBuffer)
%             % Close previously extracted texture:
%             Screen('Close', texBuffer);
%             texBuffer = [];
%         end

        % Show updated image at next retrace:
        Screen('Flip', win, 0, 0, 1);
        
        % Next tracking loop iteration...
        count = count + 1;
    end;
    
    telapsed = GetSecs - t;
    
    moglExtractTexture('DestroyContext', context);
    
    % Shutdown head renderer:
    MFHeadmodel('Shutdown');
    
    % Stop capture, close engine and onscreen window:
    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);
    Screen('CloseAll');

    % Shutdown tracker:
    PsychCV('ARShutdown');

    % Stats...
    avgfps = count / telapsed %#ok<NOPRT,NASGU>
catch
   Screen('CloseAll');
   PsychCV('ARShutdown');
end;

% Restore sync settings:
Screen('Preference', 'SkipSyncTests', olsync);

return
end

function MCalib = computeRigidXForm(MTarget, MHandle) 
    TTarget = MTarget(1:3, 4);
    RTarget = MTarget(1:3, 1:3);
    
    % Tinverse is the 4x4 translation matrix with the negated
    % (inverted) T ranslation vector of the tracking target:
    Tinverse = diag([1 1 1 1]);
    Tinverse(1:3, 4) = -TTarget;

    % Rinverse is the transpose (==inverse) of the tracking targets
    % rotation matrix RTarget, extended to a 4x4 matrix:
    Rinverse = diag([1 1 1 1]);
    Rinverse(1:3, 1:3) = transpose(RTarget);

    % Build final inverse transformation:
    MTargetInverse = Rinverse * Tinverse;
    
    % Build combined forward transformation:
    MCalib = MTargetInverse * MHandle;

    return;
end
