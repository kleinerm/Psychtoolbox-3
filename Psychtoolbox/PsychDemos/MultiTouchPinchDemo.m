function MultiTouchPinchDemo(dev, screenId, debug)
% MultiTouchPinchDemo([dev][, screenId=max][, debug=0]) - A basic demo for multi-touch touchscreens.
%
% Run it. Pressing the ESCape key, or comleting ten demo trials will stop it.
%
% This shows implementation of simple code for "two finger pinch" detection.
% The demo draws a line segment at random location and expects the user to put
% to finger on the touch screen to "squeeze" the line segment between their finger.
% It will complete a trial if both fingers are moved close enough and the line
% segment is between both fingers.
%
% Motivated by the line-intersection task from:
% https://psychtoolbox.discourse.group/t/help-with-multitouch/4953
%
% The demo will try to use the first available touchscreen, or if
% there isn't any touchscreen, the first available touchpad. You
% can also select a specific touch device by passing in its 'dev'
% device handle. Use of touchpads usually needs special configuration.
% See "help TouchInput" for more info.
%
% You can select a specific screen to display on - usually the screenId
% of the touch screen display surface - with the optional 'screenId' parameter,
% or it will select the default maximum screenId if omitted.
%
% This demo currently works on Linux + X11 display system, not on Linux + Wayland.
% It also works on MS-Windows 10 and later.
%
% For background info on capabilities and setup see "help TouchInput".
%

% History:
% 27-Apr-2023 mk  Written. Derived from MultiTouchMinimalDemo.

% Setup useful PTB defaults:
PsychDefaultSetup(2);

if nargin < 1
    dev = [];
end

if nargin < 2 || isempty(screenId)
    screenId = max(Screen('Screens'));
end

if nargin < 3 || isempty(debug)
    debug = 0;
end

% If no user-specified 'dev' was given, try to auto-select:
if isempty(dev)
    % Get first touchscreen:
    dev = min(GetTouchDeviceIndices([], 1));
end

if isempty(dev)
    % Get first touchpad if no touchscreen found:
    dev = min(GetTouchDeviceIndices([], 0));
end

if isempty(dev) || ~ismember(dev, GetTouchDeviceIndices)
    fprintf('No touch input device found, or invalid dev given. Bye.\n');
    return;
else
    fprintf('Touch device properties:\n');
    info = GetTouchDeviceInfo(dev);
    disp(info);
end

% Open a default onscreen window with black background color and 0-1 color range:
[w, rect] = PsychImaging('OpenWindow', screenId, 0);
HideCursor(w);

% Get maximum supported dot diameter for smooth dots:
[~, maxSmoothPointSize] = Screen('DrawDots', w);

% Select good diameter for touch point blobs, but no more than what 'DrawDots' supports:
baseSize = min(RectWidth(rect) / 20, maxSmoothPointSize);

try
    % Only ESCape allows to exit the demo:
    RestrictKeysForKbCheck(KbName('ESCAPE'));

    % Create and start touch queue for window and device:
    TouchQueueCreate(w, dev);
    TouchQueueStart(dev);
    
    % Wait for the go!
    KbReleaseWait;
    
    % Ten trials:
    for trial=1:10
        % touch tracks active touch points:
        touch = {};
        touches = [];
        intersectPoint = [];

        % Select randomly located line segment:
        l1 = [rand() * 200 + 200, rand() * 800 + 200];
        l2 = [rand() * 800 + 200, rand() * 800 + 200];

        % Loop for this trial: Run until keypress or line bisected.
        while ~KbCheck && (isempty(intersectPoint) || ~isempty(touches))
            % Process all currently pending touch events:
            while TouchEventAvail(dev)
                % Process next touch event 'evt':
                evt = TouchEventGet(dev, w);
                
                if evt.Type < 2
                    % Not a touch point, but something else:
                    continue;
                end
                
                % Touch blob id - Unique in the session:
                id = evt.Keycode;
                
                % In the list of known touches? Give idx for referencing it:
                idx = find(touches == id);
                
                % New touch point and not yet both two wanted touches registered?
                if evt.Type == 2 && length(touches) < 2 && isempty(idx)
                    % Add to end of list:
                    touches(end+1) = id;
                    idx = length(touches);
                    
                    touch{idx}.col = rand(3, 1);
                    touch{idx}.x = evt.MappedX;
                    touch{idx}.y = evt.MappedY;
                    touch{idx}.t = evt.Time;
                end
                
                % Moving known touch point?
                if evt.Type == 3 && ~isempty(idx)
                    touch{idx}.x = evt.MappedX;
                    touch{idx}.y = evt.MappedY;
                    touch{idx}.t = evt.Time;
                end
                
                % Known touch released?
                if evt.Type == 4 && ~isempty(idx)
                    % Remove from list:
                    touches = touches(touches ~= id);
                    
                    % Remove from tracking array, rearrange items if needed:
                    if idx == 1 && length(touch) > 1
                        touch{1} = touch{2};
                    end
                    touch{2} = [];
                end
            end
            
            % Draw all known active touch points:
            for i = 1:length(touches)
                Screen('DrawDots', w, [touch{i}.x, touch{i}.y], 3 * baseSize, touch{i}.col, [], 1, 1);
            end

            % Visualize "cut line" between the two fingers:
            Screen('DrawLine', w, [0 1 0], l1(1), l1(2), l2(1), l2(2), 2);

            % Exactly two touches as wanted?
            if length(touches) == 2
                % Find and draw mid-point, compute distance between fingers:
                t1 = [touch{1}.x, touch{1}.y];
                t2 = [touch{2}.x, touch{2}.y];
                
                distance = norm(t1 - t2);
                midpoint = mean([t1; t2]);
                
                % Show mid-point and connecting line between fingers, also print
                % distance between fingers in pixels:
                Screen('DrawDots', w, [midpoint(1), midpoint(2)], 1 * baseSize, [0 1 0], [], 1, 1);
                Screen('DrawLine', w, [1 0 0], t1(1), t1(2), t2(1), t2(2), 2);
                Screen('DrawText', w, num2str(distance), midpoint(1), midpoint(2), [1 1 0]);

                % Pinched tight enough?
                if distance < 600 && isempty(intersectPoint)
                    % The following code snippet for line segment intersection is
                    % derived from copy-pasted bits of the linexline.m function from:
                    % https://github.com/preethamam/Line2Line-IntersectionPoint
                    %
                    % The code is is licensed under the BSD 2-Clause License, and
                    % Copyright (c) 2022, Preetham Manjunatha.
                    % SPDX license identifier "BSD-2"
                    % See: https://spdx.org/licenses/BSD-2-Clause.html

                    x1=t1(1);
                    x2=t2(1);
                    x3=l1(1);
                    x4=l2(1);
                    y1=t1(2);
                    y2=t2(2);
                    y3=l1(2);
                    y4=l2(2);

                    % Find line intersection parameters:
                    u = ((x1-x3)*(y1-y2) - (y1-y3)*(x1-x2)) / ((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
                    t = ((x1-x3)*(y3-y4) - (y1-y3)*(x3-x4)) / ((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
                    
                    % Intersection actually on the lines?
                    if (u >= 0 && u <= 1.0) && (t >= 0 && t <= 1.0)
                        % Yes. Compute and assign intersection point:
                        xi = ((x3 + u * (x4-x3)) + (x1 + t * (x2-x1))) / 2; 
                        yi = ((y3 + u * (y4-y3)) + (y1 + t * (y2-y1))) / 2;
                        intersectPoint = [xi,yi];
                    end
                end
            end

            % Valid intersection point?
            if ~isempty(intersectPoint)
                % Show it on line, instruct subject to release touches:
                Screen('DrawDots', w, intersectPoint, 1 * baseSize, [0 1 1], [], 1, 1);
                DrawFormattedText(w, 'Well done! Lift fingers for next trial!', 'center', 'center', 1);

                % TODO: Store timestamps, distance, intersectPoint, other info
                % with results for this trial:
            else
                DrawFormattedText(w, 'Pinch the line between two fingers!', 'center', 'center', 1);
            end

            % Done repainting - Show it:
            Screen('Flip', w);
            
            % This little bit here will provoke stimulus onset timing failures on
            % MS Windows at debug level 1 for debugging if something is not quite right.
            if debug && IsWin
                WaitSecs(0.025);
                [~, ~, ~, visualtimingmaybesane] = GetMouse(w)
            end
            
            % Next touch processing -> redraw -> flip cycle:
        end

        % End of trial.
        % TODO store data...

        % Next trial:
    end

    TouchQueueStop(dev);
    TouchQueueRelease(dev);
    RestrictKeysForKbCheck([]);
    ShowCursor(w);
    sca;
catch
    TouchQueueRelease(dev);
    RestrictKeysForKbCheck([]);
    sca;
    psychrethrow(psychlasterror);
end

