function ARToolkitDemo(multiMarker)
% Use ARToolkit to track and visualize 3D objects in live-video.
%
% Usage: ARToolkitDemo([multiMarker = 0])
%
% Minimalistic demo on how to capture video data and use ARToolkit to
% detect and track the rigid position and orientation of markers, then
% visualize corresponding 3D objects on top of video via OpenGL.
%
% Parameters:
%
% multiMarker = If set to 0 (default), track single markers. Otherwise
% track so called multi markers or composite markers for more robustness
% against occlusion of single markers.
%

% History:
% 19.04.2009  mk  First prototype written.

% Running on PTB-3? Hopefully...
AssertOpenGL;

% Size of rendered objects in millimeters:
sizeMM = 50;

try
    % No sync tests for this simple demo:
    olsync = Screen('Preference', 'SkipSyncTests', 2);
    
    InitializeMatlabOpenGL;
    
    % Open onscreen window for diplay:
    screenid = max(Screen('Screens'));
    
    % Size of window: Default to fullscreen:
    roi = CenterRect([0 0 640 480], Screen('Rect',screenid));
    
    % Use imaging pipeline to flip image horizontally to avoid confusing
    % the viewer due to mirror view:
    PsychImaging('PrepareConfiguration');
    PsychImaging('AddTask', 'AllViews', 'FlipHorizontal');
    win = PsychImaging('OpenWindow', screenid, 0, roi);

    % Open videocapture device: For now we use engine 2, the ARVideo
    % engine, to simplify compatibility issues...
    Screen('Preference', 'DefaultVideocaptureEngine', 2);
    grabber = Screen('OpenVideoCapture', win, 0, [0 0 640 480], 4);

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
    ardata = [ PsychtoolboxRoot() 'PsychDemos/ARToolkitDemoData/' ];
    [imgbuffer, projectionMatrix, debugimagebuffer] = PsychCV('ARInitialize', [ardata 'camera_para.dat'], w, h, channels); %#ok<NASGU>

    % Single markers by default:
    if nargin < 1
        multiMarker = 0;
    end
    
    % Load our (multi-)marker(s):
    % Need to cd() into marker directory, as some paths inside the marker
    % config files are coded relative to working directory :-(
    olddir = pwd;
    try
        if multiMarker
            cd([ ardata 'multi/' ]);
            marker = PsychCV('ARLoadMarker', 'marker.dat', 1);
        else
            cd(ardata);
            marker(1) = PsychCV('ARLoadMarker', 'patt.hiro', 0);
            marker(2) = PsychCV('ARLoadMarker', 'patt.kanji', 0);
        end
    catch
    end
    cd(olddir);
    
    % Setup OpenGL 3D rendering for our simple test:
    Screen('BeginOpenGL', win);

    % Enable lighting and depth-test:
    glColor3f(1,1,0);
    glEnable(GL.LIGHTING);
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
    
    % OpenGL 3D setup done:
    Screen('EndOpenGL', win);
        
    % Init segmentation gain to 50% of avg image intensity:
    gain = 0.5;
    SetMouse(gain * Screen('WindowSize', win)/2, 100);
    
    count = 0;
    t=GetSecs;
    KbReleaseWait;

    % Run tracking and viz loop until keypress:
    while ~KbCheck 
        % Update image segmentation gain from horizontal mouse position:
        gain = 2 * GetMouse / Screen('WindowSize', win);
        
        % Retrieve next captured image. The 'waitforImage=2' waits for a
        % new image, but disables texture creation, so 'dummy' is actually
        % an empty handle. The 'specialMode'=4 flag requests video data to
        % be put into the PsychCV() memory buffer referenced by
        % 'imgbuffer'. 'imgIntensity' returns the average image intensity -
        % Useful for computing an adaptive image segmentation threshold:
        [dummy pts nrdropped imgIntensity] = Screen('GetCapturedImage', win, grabber, 2, [], 4, imgbuffer);
                
        % Define dynamic threshold for image segmentation:
        thresh = max(min(imgIntensity * gain, 254), 1);

        %tic;
        
        % ARToolkit processing:
        detectedMarkers = PsychCV('ARDetectMarkers', [], thresh);

        %cpudur = toc * 1000
        
        % Same can be done via PsychCV('ARRenderImage'); !
        % Disadvantage is that then we don't have control over the image
        % rendering. Advantage is that AR takes care of texture formats
        % etc.
        %
        % Retrieve image from imgbuffer and show it: We mark image buffer
        % content as upside down, and provide the low-level texture data
        % encoding for OS/X to get proper colors:
        %         tex = Screen('SetOpenGLTextureFromMemPointer', win, tex, debugimagebuffer, w, h, channels, 1, [], GL.RGBA8, GL.BGRA, GL.UNSIGNED_INT_8_8_8_8);
        %         Screen('DrawTexture', win, tex);
        
        
        % Visualize detected objects in 3D mode:
        Screen('BeginOpenGL', win);

        % Clear z-buffer for proper occlusion handling:
        glClear(GL.DEPTH_BUFFER_BIT);
         
        % Render current video image as backdrop:
        PsychCV('ARRenderImage');
        
        % For each candidate marker do...
        for i=1:length(detectedMarkers)
            % i'th candidate detected reliably?
            if detectedMarkers(i).MatchError < realmax
                % Yes: Draw 3D object corresponding to i'th pattern:
                
                % Setup rigid positio and orientation via 4x4 xform matrix:
                glLoadMatrixd(detectedMarkers(i).ModelViewMatrix);
                
                % Switch drawn object by markerId:
                switch detectedMarkers(i).Id
                    case marker(1),
                        glutWireTeapot(sizeMM);
                    case marker(2),
                        glutSolidCube(sizeMM);
                end;                
            end
        end
        
        % Done with objects:
        Screen('EndOpenGL', win);

        % Show updated image at next retrace:
        Screen('Flip', win);
        
        % Next tracking loop iteration...
        count = count + 1;
    end;
    
    telapsed = GetSecs - t;
    
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
