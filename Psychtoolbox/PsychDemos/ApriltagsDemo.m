function ApriltagsDemo(camId, colordepth, tagSize, tagFamily, targetMarkers)
% ApriltagsDemo([camId][, colordepth=1][, tagSize=0.017][, tagFamily='tag16h5'][, targetMarkers])
%
% Use ApriltagsDemo to track April tag markers and visualize them in live-video.
%
% Minimalistic demo on how to capture video data and use PsychCV's builtin
% apriltag marker tracking to detect and track the rigid position and orientation
% of such fiducial markers, then visualize corresponding 3D objects on top of the
% live video of the markers via OpenGL.
%
% Note that this demo can only act as a starting point. There are too many parameters
% which need to be set for your specific use case, and we can't pass all of them via
% function parameters. Read the code, think about it, hack it accordingly.
%
% Optional parameters:
%
% camId = Id of the camera to use, [] selects default cam.
% colordepth = Video capture mode: 1 = Mono/Gray, 3 = RGB color, 4 = RGBA color.
% tagSize = Size of April tag (edge length) in meters. Needed for 3D pose estimation.
% tagFamily = April tag family to use.
% targetMarkers = List of marker id's to track and return. Default is [] for "all markers".
%
%
% Press any key to finish the demo.
%

% History:
% 15.03.2024  mk  Derived from ARToolkitDemo.

% Setup defaults and normalized color range:
PsychDefaultSetup(2);

if ~IsLinux
    fprintf('Sorry, this demo is currently only supported on Linux.\n');
    return;
end

if nargin < 1 || isempty(camId)
    % Use default camera, unless if running on machine "touchy", then use cam 2:
    camId = [];
    computer = Screen('Computer');
    if strcmp(computer.machineName, 'touchy')
        camId = 2;
    end
end

% Use 1 channel mono/grayscale video for highest efficiency by default:
if nargin < 2 || isempty(colordepth)
    colordepth = 1;
end

if nargin < 3 || isempty(tagSize)
    tagSize = 0.017; % April tag size in meters. Default to 1.7 cm.
end

% Need to have some default tagFamily:
if nargin < 4 || isempty(tagFamily)
    tagFamily = 'tag16h5';
end

% Try to detect all markers by default:
if nargin < 5 || isempty(targetMarkers)
    targetMarkers = [];
end

try
    % No sync tests for this simple demo:
    olsync = Screen('Preference', 'SkipSyncTests', 2);

    % Setup OpenGL for rendering:
    InitializeMatlabOpenGL;

    % Open onscreen window for diplay:
    screenid = max(Screen('Screens'));

    PsychImaging('PrepareConfiguration');
    % Flip image horizontally to avoid confusing the viewer due to mirror view:
    % PsychImaging('AddTask', 'AllViews', 'FlipHorizontal');
    [win, winrect] = PsychImaging('OpenWindow', screenid, 0);

    % Open videocapture device camId, at default resolution, return 1 channel grayscale images:
    grabber = Screen('OpenVideoCapture', win, camId, [], colordepth);

    % Start video capture at maximum possible framerate:
    Screen('StartVideoCapture', grabber, realmax, 1);

    % Get a single texture that we can query for the image format:
    vidtex = Screen('GetCapturedImage', win, grabber);
    [w, h] = Screen('WindowSize', vidtex)
    channels = Screen('PixelSize', vidtex) / 8

    % Compute scaling factor for upscaling all content to fill window:
    [winw, winh] = Screen('WindowSize', win);
    winscale = [winw / w, winh / h];

    % Debug level for tracker:
    PsychCV('Verbosity', 3);

    % Initialize Apriltags:

    % Camera intrinsics. Should come from calibration, but here we just make stuff up:
    cx = w / 2;
    cy = h / 2;
    fx = w * 0.555;
    fy = w * 0.555;
    camIntrinsics = [ cx, cy, fx, fy ];

    % View frustum for 3D rendering near and far distance, 0.01 - 5 meters:
    zmin = 0.01;
    zmax = 5;

    % Setup parameters for 3D marker 6-DoF pose reconstruction, and retrieve the
    % GL_PROJECTION_MATRIX to use for rendering tags superimposed onto the video:
    projectionMatrix = PsychCV('April3DSettings', camIntrinsics, tagSize, zmin, zmax);

    % Assign April tag family, input image size, and properties for raw video input
    % images, get the imgbuffer memory buffer handle where the Psychtoolbox video
    % capture engine should store camera images as input for marker detection:
    imgbuffer = PsychCV('AprilInitialize', tagFamily, w, h, channels);

    % Request multi-threaded operation with 8 threads, instead of the default 1 thread single-threaded:
    PsychCV('AprilSettings', 8);

    % Tunable parameters for 2D tag marker detection:
    % [nrThreads, imageDecimation, quadSigma, refineEdges, decodeSharpening] = PsychCV('AprilSettings', 1, 2, 0.5, 1, 1)

    % Some markers to visualize:
    marker = [9, 11, 23];

    % Size of rendered objects in meters:
    sizeM = 0.01;

    % Setup OpenGL 3D rendering for our simple test:
    Screen('BeginOpenGL', win);

    % Set projection matrix to the one provided by apriltags:
    glMatrixMode(GL.PROJECTION);
    glLoadMatrixd(projectionMatrix);

    % Init modelview matrix to identity: Will be overwritten on each
    % object draw operation by the modelview matrix from apriltag:
    glMatrixMode(GL.MODELVIEW);
    glLoadIdentity;

    % Position the light source:
    glEnable(GL.LIGHTING);
    glEnable(GL.LIGHT0);
    glLightfv(GL.LIGHT0, GL.POSITION, [ 0, 0, 1, 0 ]);

    % Change the color - or better: The light reflection properties of the
    % material - of the following objects to greenish. We only change ambient and
    % diffuse reflection properties. The color for specular reflection is left
    % to its default of "white":
    glMaterialfv(GL.FRONT_AND_BACK,GL.AMBIENT, [ 0.0 0.6 0.0 1 ]);
    glMaterialfv(GL.FRONT_AND_BACK,GL.DIFFUSE, [ 0.0 0.6 0.0 1 ]);

    % Set proper viewport:
    glViewport(0, 0, w * winscale(1), h * winscale(2));

    % Define background clearcolor:
    glClearColor(0,0,0,0);

    % Enable depth test for depth correct 3D rendering:
    glEnable(GL.DEPTH_TEST);

    % OpenGL 3D setup done:
    Screen('EndOpenGL', win);

    % Init stats:
    count = 0;
    tstart = GetSecs;
    KbReleaseWait;

    % Run tracking and viz loop until keypress:
    while ~KbCheck
        % Retrieve next captured image in vidtex: The 'specialMode'= 4 flag
        % requests video data to be put into the PsychCV() memory buffer
        % referenced by 'imgbuffer':
        vidtex = Screen('GetCapturedImage', win, grabber, 1, vidtex, 4, imgbuffer);

        % Draw captured video image as backdrop:
        Screen('DrawTexture', win, vidtex, [], ScaleRect(Screen('Rect', vidtex), winscale(1), winscale(2)));

        % Clear depth buffer before rendering 3D objects for this frame:
        Screen('BeginOpenGL', win);
        glClear(GL.DEPTH_BUFFER_BIT);
        Screen('EndOpenGL', win);

        %fprintf('\n=================================\n');

        % Detect apriltags in captured image:
        detectedMarkers = PsychCV('AprilDetectMarkers', targetMarkers);

        % For each candidate marker do...
        j = 0;
        for i = 1:length(detectedMarkers)
            % i'th candidate detected reliably?
            score = detectedMarkers(i).MatchQuality;
            hamming = detectedMarkers(i).HammingErrorBits;

            if score > 100 && hamming <= 2
                % Yes. Get info about it:
                j = j + 1;
                id = detectedMarkers(i).Id;

                % Visualize (x,y) 2D image position of marker center with a green dot:
                xy = detectedMarkers(i).Center2D;
                Screen('DrawDots', win, xy .* winscale', 10, [0 1 0], [], 1);

                % Visualize (x,y) 2D image position of marker corners as yellow dots,
                % and boundaries as red lines:
                xyv = detectedMarkers(i).Corners2D;
                Screen('FramePoly', win, [1 0 0], xyv' .* winscale, 2);
                Screen('DrawDots', win, xyv .* winscale', 10, [1 1 0], [], 1);

                % 3D pose and pose reconstruction error:
                poseError = detectedMarkers(i).PoseError;

                % Pose as OpenGL GL_MODELVIEW_MATRIX compatible 4x4 matrix:
                OpenGLPoseMatrix = detectedMarkers(i).ModelViewMatrix;

                % 3D position of the markers center in apriltag natural reference frame.
                % Extract mpos as the 3 component translation vector from the pose matrix:
                mpos = detectedMarkers(i).TransformMatrix(1:3, 4);
                %fprintf('Id: %i  Hamming %i  Score %f  PosError [mm]: %f\n', id, hamming, score, poseError * 1e3);

                % Draw some text next to each markers center, with marker id and estimated 'mpos'
                % 3D center coordinates in meters, relative to the optical center of the camera,
                % with x-axis to the right, y-axis down, and z-axis facing away from the camera,
                % ie. along its optical axis, ie. in the viewing direction:
                statusTxt = sprintf('%i: %04f : %04f : %04f', id, mpos(1), mpos(2), mpos(3));
                Screen('DrawText', win, statusTxt, xy(1) * winscale(1) + 10, xy(2) * winscale(2), [1 0 1], [], 0);

                % Visualize detected marker in 3D as well, via OpenGL 3D rendering:
                Screen('BeginOpenGL', win);

                % Setup rigid position and orientation of marker via the OpenGLPoseMatrix:
                glLoadMatrixd(OpenGLPoseMatrix);

                % Visualize location and orientation of tag, by drawing some reference frame on it:
                % Tag in x-y plane, z-axis comes out of tag like a surface normal vector.
                % Draw little reference frame in 3D, ie. x/y/z axis as red, green, blue lines:
                glDisable(GL.LIGHTING);

                glBegin(GL.LINES);
                glColor3d(1, 0, 0);
                glVertex3d(0, 0, 0);
                glVertex3d(sizeM, 0, 0);

                glColor3d(0, 1, 0);
                glVertex3d(0, 0, 0);
                glVertex3d(0, sizeM, 0);

                glColor3d(0, 0, 1);
                glVertex3d(0, 0, 0);
                glVertex3d(0, 0, sizeM);
                glEnd;

                % For the fun of it, also draw some 3D objects "glued" onto the
                % tags denoted by the vector 'marker':
                glEnable(GL.LIGHTING);

                % Switch drawn 3D objects by markerId:
                glTranslatef(0, 0, sizeM/2)
                switch j
                    case 1,
                        glutSolidTeapot(sizeM);
                    case 2,
                        glutSolidCube(sizeM);
                    case 3,
                        glutSolidSphere(sizeM, 50, 50);
                end

                % Done with this markers 3D drawing:
                Screen('EndOpenGL', win);
            end
        end

        % Show updated image at next retrace:
        Screen('Flip', win);

        % Next tracking loop iteration...
        count = count + 1;
    end

    % Stats...
    telapsed = GetSecs - tstart;
    fprintf('Average fps: %f\n', count / telapsed);

    % Stop capture, close engine and onscreen window:
    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);
    sca;

    % Shutdown tracker:
    PsychCV('AprilShutdown');
catch
   sca;
   PsychCV('AprilShutdown');
end

% Restore sync settings:
Screen('Preference', 'SkipSyncTests', olsync);
