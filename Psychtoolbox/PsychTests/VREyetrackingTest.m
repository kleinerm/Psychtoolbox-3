function VREyetrackingTest(stereoscopic, render3D, testEye, modulateBackgroundBrightness, text)
% VREyetrackingTest([stereoscopic=0][, render3D=0][, testEye=0][, modulateBackgroundBrightness=0][, text])
%
% A test for eye gaze tracking in VR HMD's via PsychVRHMD supported methods.
% Tested with HTC Vive Pro Eye binocular eyetracker.
%
% Parameters:
%
% 'stereoscopic' if set to 1, configures the HMD as a stereoscopic display.
% A default setting of 0 configures it as a monoscopic display with both eyes
% seeing the same stimulus.
%
% 'render3D' if set to 1, use OpenGL 3D rendering of target with use of
% perspective projection, using the driver provided MODELVIEW and
% PROJECTION matrices.
%
% 'testEye' which eye gaze source to test and plot further. Defaults to 0.
% Choose 1 for left-eye in binocular setup, or mono-eye in monocular setup.
% Choose 2 for right eye in binocular setup, 3 for cyclops eye in binocular setup.
%
% 'modulateBackgroundBrightness' if set greater than zero, change brightness
% of background over time, to see if that affects gaze tracking, and for
% suitable gaze trackers if it correlates with measured/reported pupil diameter.
%
% 'text' An optional text string that can be displayed on the screen.

% History:
% 31-May-2023  mk Written, derived from VRHMDDemo.

global gazeX; %#ok<*GVMIS>
global gazeY;
global tx;
global ty;

% Setup unified keymapping and unit color range:
PsychDefaultSetup(2);

close all;

if nargin < 1 || isempty(stereoscopic)
    stereoscopic = 0;
end

if nargin < 2 || isempty(render3D)
    render3D = 0;
end

if nargin < 3 || isempty(testEye)
    testEye = 0;
end

if nargin < 4 || isempty(modulateBackgroundBrightness)
    modulateBackgroundBrightness = 0;
end

if nargin < 5
    text = '';
end

% Needed for 3D rendering, if any:
InitializeMatlabOpenGL;

% Select screen with highest id as debug output display:
screenid = max(Screen('Screens'));

% Open our fullscreen onscreen window with black background clear color:
PsychImaging('PrepareConfiguration');

% Build final task requirements: PerEyeFOV
basicReqs = 'Use2DViewsWhen3DStopped NoTimingSupport TimestampingSupport Eyetracking';

if render3D
    hmd = PsychVRHMD('AutoSetupHMD', 'Tracked3DVR', basicReqs);
    stereoscopic = 1;
else
    if ~stereoscopic
        % Setup the HMD to act as a regular "monoscopic" display monitor
        % by displaying the same image to both eyes:
        hmd = PsychVRHMD('AutoSetupHMD', 'Monoscopic', basicReqs);
    else
        % Setup for stereoscopic presentation:
        hmd = PsychVRHMD('AutoSetupHMD', 'Stereoscopic', basicReqs);
    end
end

if isempty(hmd)
    fprintf('No support for VR, so can not run this test. Bye.\n');
    return;
end

% Background color is 50% gray:
bgColor = 0.5;

[win, rect] = PsychImaging('OpenWindow', screenid, bgColor);
hmdinfo = PsychVRHMD('GetInfo', hmd);

% Disable gaze tracking test if unsupported by setup:
if ~hmdinfo.eyeTrackingSupported
    fprintf('No support for eyetracking in XR device, so can not run this test. Bye.\n');
    sca;
    return;
end

Screen('TextStyle', win, 1);
Screen('TextSize', win, 100);

tOnset = [];
tSample = [];
tNow = [];
gazeX = [];
gazeY = [];
gazeOpen = [];
gazePupil = [];
gazeSensor = [];
gazeDir = [];
tx = [];
ty = [];
cnt = 1;

[cx, cy] = RectCenter(rect);
radius = 200;

ipd = 0.065;
%PsychVRHMD('View2DParameters', hmd, 0, [-ipd/2, 0.040, -1]);
%PsychVRHMD('View2DParameters', hmd, 1, [+ipd/2, 0.040, -1]);

% Retrieve and set camera projection matrix for optimal rendering on the HMD:
[projMatrix{1}, projMatrix{2}] = PsychVRHMD('GetStaticRenderParameters', hmd);

while ~KbCheck
    % Get current eyetracking state and possibly head tracking (in render3D case):
    state = PsychVRHMD('PrepareRender', hmd, [], render3D + 4);
    tNow(cnt) = GetSecs; %#ok<*AGROW>

    % Query and store latest gaze samples:
    for i = 1:length(state.gazeStatus)
        % fprintf('Eye %i: status %i pos = %i %i\n', i, state.gazeStatus(i), state.gazePos{i}(1), state.gazePos{i}(2));
        if state.gazeStatus(i) >= 3
            [gazeX(i, cnt), gazeY(i, cnt), gazeOpen(i, cnt), gazePupil(i, cnt)] = deal(state.gazePos{i}(1), state.gazePos{i}(2), state.gazeEyeOpening(i), state.gazeEyePupilDiameter(i));
            gazeDir(i, cnt) = state.gazeRayLocal{i}.gazeD(1);
        else
            [gazeX(i, cnt), gazeY(i, cnt), gazeOpen(i, cnt), gazePupil(i, cnt)] = deal(NaN, NaN, state.gazeEyeOpening(i), state.gazeEyePupilDiameter(i));
            gazeDir(i, cnt) = NaN;
        end
    end

    if isfield(state.gazeRaw(1), 'sensor2D')
        gazeSensor(1, cnt) = state.gazeRaw(1).sensor2D(1);
        gazeSensor(2, cnt) = state.gazeRaw(1).sensor2D(2);
        gazeSensor(3, cnt) = state.gazeRaw(2).sensor2D(1);
        gazeSensor(4, cnt) = state.gazeRaw(2).sensor2D(2);
    end

    % If gaze sample time is in the past, ie. it is a previously measured gaze
    % position, then take a screenshot of the ROI of fixation here of the
    % formerly presented stimulus image, and before rendering a new future
    % to-be-presented stimulus image:
    if testEye > 0 && ~isnan(gazeX(testEye, cnt)) && state.gazeTime(1) <= tNow(cnt)
        if testEye ~= 2
            Screen('SelectStereoDrawBuffer', win, 0);
        else
            Screen('SelectStereoDrawBuffer', win, 1);
        end

        roi = ClipRect(CenterRectOnPointd([0 0 150 150], gazeX(testEye, cnt), gazeY(testEye, cnt)), rect);
        if ~IsEmptyRect(roi)
            gazeImage{cnt} = Screen('GetImage', win, roi, 'drawBuffer');
        end
    end

    if render3D
        cx = 0;
        cy = 0;
        radius = 1;
    end

    angle = mod(GetSecs, 10) * 36;
    tx(cnt) = cx + radius * cos(angle / 360 * 2 * pi);
    ty(cnt) = cy + radius * sin(angle / 360 * 2 * pi);

    % Render one view for each eye in stereoscopic mode, one common view in mono:
    for eye = 0:stereoscopic
        Screen('SelectStereoDrawBuffer', win, eye);

        if modulateBackgroundBrightness
            Screen('FillRect', win, mod(GetSecs / 10, 1) * modulateBackgroundBrightness);
        else
            Screen('FillRect', win, bgColor);
        end

        if render3D
            Screen('BeginOpenGL', win, 1);

            glMatrixMode(GL.PROJECTION);
            glLoadMatrixd(projMatrix{eye+1});

            % Setup camera position and orientation for this eyes view:
            glMatrixMode(GL.MODELVIEW);
            modelView = state.modelView{eye + 1};
            %modelView = diag([1 1 1 1]);
            glLoadMatrixd(modelView);
            zplane = -3;
            glPointSize(10);
            glBegin(GL.POINTS);
            glColor3dv([mod(GetSecs, 1) 0 0]);
            glVertex3d(cx, cy, zplane);
            glColor3dv([0 1 1]);
            glVertex3d(tx(cnt), ty(cnt), zplane);
            glEnd;

            Screen('EndOpenGL', win);
        else
            Screen('FillOval', win, [mod(GetSecs, 1) 0 0], CenterRect([0 0 3 3], rect));
            Screen('DrawDots', win, [tx(cnt) ; ty(cnt)], 10, [0,1,1], [], 1);
        end

        if ~isempty(text)
            DrawFormattedText(win, text, 'center', 'center', [1, 0, 0]);
        end
    end

    % If gaze sample time is in the future, ie. it is a predicted gaze
    % position, then take a screenshot of the ROI of fixation here after
    % rendering the future to-be-presented stimulus image:
    if testEye > 0 && ~isnan(gazeX(testEye, cnt)) && state.gazeTime(1) > tNow(cnt)
        if testEye ~= 2
            Screen('SelectStereoDrawBuffer', win, 0);
        else
            Screen('SelectStereoDrawBuffer', win, 1);
        end

        roi = ClipRect(CenterRectOnPointd([0 0 150 150], gazeX(testEye, cnt), gazeY(testEye, cnt)), rect);
        if ~IsEmptyRect(roi)
            gazeImage{cnt} = Screen('GetImage', win, roi, 'drawBuffer');
        end
    end

    % Display new image asap, but don't clear draw buffer after flip, so we
    % can screenshot it in the next iteration:
    tOnset(cnt) = Screen('Flip', win, [], 1);
    tSample(cnt) = state.gazeTime(1);
    cnt = cnt + 1;
end

% Close XR display device:
sca;

fps = 1 / mean(diff(tOnset)) %#ok<NOPRT>

% Data analysis and visualization:
if 1
    figure;
    plot(1:length(tOnset), [tOnset ; tNow; tSample ; tOnset - tSample ; tNow - tSample]);
    legend('StimOnset', 'NowTime', 'GazeTime', 'StimOnset vs GazeTime', 'Now vs GazeTime');
    title('Absolute timestamps and relative timestamp delta for stim onset vs. gaze sample time');

    figure;
    hold on;
    for i = 1:size(gazeX, 1)
        plot(1:length(tOnset), [gazeOpen(i, :); gazePupil(i, :)]);
    end
    hold off;
    title('Top: Pupil size (mm), Bottom: Eye opening (normalized)');
end

figure
hold on;
for i = 1:size(gazeX, 1)
    scatter(gazeX(i,:), gazeY(i,:));
    idxs = find(~isnan(gazeX(i,:)));
    avgpos{i} = round(mean([gazeX(i,idxs); gazeY(i,idxs)]')); %#ok<UDIM>
end
scatter(tx, ty);
scatter(cx, cy);

if size(gazeX, 1) == 3
    legend('Lefteye', 'Righteye', 'Cyclopseye', 'Target', 'Fixation');
elseif size(gazeX, 1) == 2
    legend('Lefteye', 'Righteye', 'Target', 'Fixation');
else
    legend('Cyclopseye', 'Target', 'Fixation');
end
hold off;
title('2D target/fix position vs. left/right/cyclops tracked gaze');

if 0
    figure; %#ok<UNRCH>
    KbReleaseWait;
    for i=1:2:size(gazeX, 2)
        hold on;
        scatter(gazeX(testEye,1:i), gazeY(testEye,1:i));
        scatter(tx(1:i), ty(1:i));
        hold off;
        drawnow;
        if KbCheck
            break;
        end
    end
end

if testEye > 0 && exist('gazeImage', 'var')
    oldskip = Screen('Preference', 'SkipSyncTests', 2);
    [wfig, figrect] = Screen('OpenWindow', 0, [], [0 0 155, 155]);
    Screen('Preference', 'SkipSyncTests', oldskip);
    KbReleaseWait;
    vbl = Screen('Flip', wfig);
    for i=1:length(gazeImage)
        if numel(gazeImage{i}) > 0
            tex = Screen('MakeTexture', wfig, gazeImage{i});
            Screen('DrawTexture', wfig, tex);
            Screen('FillOval', wfig, 0, CenterRect([0 0 5 5], figrect));
            Screen('Close', tex);
            vbl = Screen('Flip', wfig, vbl + 1 / fps);
        end
        if KbCheck
            break;
        end
    end

    if ~render3D
        figure;
        idxs = find(~isnan(gazeX(testEye,:)));
        idxs = idxs(2:end);
        dx = gazeX(testEye, idxs) - tx(idxs-1);
        dy = gazeY(testEye, idxs) - ty(idxs-1);
        dd = sqrt(dx.^2 + dy.^2);
        plot(idxs, [dx ; dy ; dd]);
        legend('x-error', 'y-error', 'abs error');
        title('Error gazepos vs. moving target in pixels');
    end
end

if ~isempty(gazeSensor)
    figure;
    plot(1:length(gazeSensor), [gazeSensor(1,:); gazeSensor(2,:); gazeSensor(3,:); gazeSensor(4,:)]);
    legend('x-SensorL', 'y-SensorL','x-SensorR', 'y-SensorR');
    title('Normalized sensor coordinates of pupil:')
end

if ~isempty(gazeDir)
    figure;
    if size(gazeDir, 1) == 3
        plot(1:length(gazeDir), [gazeDir(1,:); gazeDir(2,:); gazeDir(3,:)]);
        legend('L', 'R','C');
    else
        plot(gazeDir(1,:));
        legend('Cyclops/Mono');
    end
    title('gazeRayLocal component:');
end

sca;

% Display means:
disp(avgpos);
