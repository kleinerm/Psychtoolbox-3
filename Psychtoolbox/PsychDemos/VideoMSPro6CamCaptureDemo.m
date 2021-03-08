function VideoMSPro6CamCaptureDemo(exposure, fullscreen, fullsize, moviename)
% Demonstrate simple use of built-in front video camera of MS Surface Pro 6
%
% VideoMSPro6CamCaptureDemo([exposure=80][, fullscreen=0][, fullsize=0][, moviename])
%
% VideoMSPro6CamCaptureDemo initializes the MS Surface Pro 6 front camera,
% then shows its video image in a Psychtoolbox window.
%
% A press of the ESCape key ends the demo.
%
% Optional parameters:
%
% 'fullscreen' If set to non-zero value, the image is displayed in a
% fullscreen window, as usual, otherwise a normal GUI window is used.
%
% 'fullsize' If set to 1, the cameras image is scaled up to full screen
% resolution, ie. so it fills the maximum amount of display area, but
% preserving the original aspect ratio.
%
% 'moviename' Name string for selection of filename of a target movie file
% to which video should be recorded. Defaults to none,ie., no video
% recording.
%

% History:
% 08-Mar-2021  mk  Written. Derived from VideoDVCamCaptureDemo.m

PsychDefaultSetup(1);

if nargin < 1 || isempty(exposure)
    exposure = 80;
end

if nargin < 2 || isempty(fullscreen)
    fullscreen = 0;
end

if nargin < 3 || isempty(fullsize)
    fullsize = 0;
end

if nargin < 4
    moviename = [];
end

% Microsoft Surface Pro 6 front camera sensor resolution:
roi = [0 0 1280 720];

% Auto-Select depth and pixelformat:
depth = [];

% Special gst-launch style capture spec:
capturebinspec = 'libcamerasrc ! videoflip video-direction=vert';

% Signal to Screen() that spec string should be used. This via special deviceId -9:
deviceId = -9;

% No preroll - it would hang!
recordingflags = 8;

screenid=max(Screen('Screens'));

try
    if fullscreen < 1
        win = Screen('OpenWindow', screenid, 0, roi);
    else
        win = Screen('OpenWindow', screenid, 0);
    end

    % Initial flip to a blank screen:
    Screen('Flip',win);

    % Set text size for info text to 24 pixels.
    Screen('TextSize', win, 24);

    % Assign capturebinspec as gst-launch style capture bin spec for use as video source:
    Screen('SetVideoCaptureParameter', -1, sprintf('SetNextCaptureBinSpec=%s', capturebinspec));

    % Open camera: We always use engineId 3 for GStreamer, as only GStreamer supports this camera:
    grabber = Screen('OpenVideoCapture', win, deviceId, roi, depth, [], [], moviename, recordingflags, 3);

    % We refrain from setting target fps here. Instead we use the
    % "do what you think is right" 'realmax' joker:
    Screen('StartVideoCapture', grabber, realmax, 1);

    % Workarounds needed as of 8-Mar-2021:
    %
    % Manually override exposure time of camera, *after* StartVideoCapture, as
    % libcamerasrc always resets exposure to maximum of about 1.5 seconds at start:
    system(['v4l2-ctl -d /dev/v4l-subdev4 --set-ctrl exposure=' num2str(exposure)]);

    % Do not flip image into upright position in camera, instead we do it in
    % software via GStreamer. Reason: The flipping in the camera hw screws up
    % color filtering and we get a pink image instead of proper colors:
    system('v4l2-ctl -d /dev/v4l-subdev4 --set-ctrl vertical_flip=0');

    dstRect = [];
    oldpts = 0;
    count = 0;
    t = GetSecs;

    % Run for at most 600 seconds or until ESCAPE keypress:
    while (GetSecs - t) < 600
        [~, ~, keys] = KbCheck(-1);
        if keys(KbName('ESCAPE'))
            break;
        end

        [tex, pts] = Screen('GetCapturedImage', win, grabber, 1);
        if tex > 0
            % Perform first-time setup of transformations, if needed:
            if fullsize && (count == 0)
                texrect = Screen('Rect', tex);
                winrect = Screen('Rect', win);
                sf = min([RectWidth(winrect) / RectWidth(texrect) , RectHeight(winrect) / RectHeight(texrect)]);
                dstRect = CenterRect(ScaleRect(texrect, sf, sf) , winrect);
            end

            % Draw new texture from framegrabber.
            Screen('DrawTexture', win, tex, [], dstRect);

            % Print pts:
            Screen('DrawText', win, sprintf('%.4f', pts), 0, 0, 255);
            if count > 0
                % Compute delta:
                delta = (pts - oldpts) * 1000;
                oldpts = pts;
                Screen('DrawText', win, sprintf('%.4f', delta), 0, 20, 255);
            end

            % Show it.
            Screen('Flip', win);
            Screen('Close', tex);
        end

        count = count + 1;
    end

    telapsed = GetSecs - t;

    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);

    sca;

    avgfps = count / telapsed;
    fprintf('Average fps: %f\n', avgfps);
catch
    sca;
end
