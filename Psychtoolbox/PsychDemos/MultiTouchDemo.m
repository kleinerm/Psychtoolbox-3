function MultiTouchDemo(dev)
% MultiTouchDemo([dev]) - A basic demo for multi-touch touchscreens.
%
% Run it. Pressing any key will stop it.
% Touch the screen and watch the nice colorful happy blobs
% sprinkle to life :)
%
% The demo will try to use the first available touchscreen, or if
% there isn't any touchscreen, the first available touchpad. You
% can also select a specific touch device by passing in its 'dev'
% device handle. Use of touchpads usually needs special configuration.
% See "help TouchInput" for more info.
%
% This demo currently only works on Linux + X11 display system,
% not on Linux + Wayland, not on other operating systems.
%

% History:
% 01-Oct-2017 mk  Written.

  % Setup useful PTB defaults:
  PsychDefaultSetup(2);

  if nargin < 1
    dev = [];
  end

  % If no user-specified 'dev' was given, try to auto-select:
  if isempty(dev)
    % Get first touchscreen:
    dev = min(GetTouchDeviceIndices([], 1));
  end

  if isempty(dev)
    % Get first touchpad:
    dev = min(GetTouchDeviceIndices([], 0));
  end

  if isempty(dev) || ~ismember(dev, GetTouchDeviceIndices)
    fprintf('No touch input device found, or invalid dev given. Bye.\n');
    return;
  else
    info = GetTouchDeviceInfo(dev);
    valInfos = info.valuatorInfos;
    disp(info);
  end

  % Open a default onscreen window with black background color and
  % 0-1 color range:
  [w, rect] = PsychImaging('OpenWindow', 0, 0);
  baseSize = RectWidth(rect) / 20;

  HideCursor(w);

  try
    % Create and start touch queue for window and device:
    TouchQueueCreate(w, dev);
    TouchQueueStart(dev);

    % Wait for the go!
    KbReleaseWait;

    % blobcol tracks active touch points - and dying ones:
    blobcol = {};

    % Main loop: Run until keypress:
    while ~KbCheck
      % Process all currently pending touch events:
      while TouchEventAvail(dev)
        % Process next touch event 'evt':
        evt = TouchEventGet(dev, w);

        % Touch blob id - Unique in the session at least as
        % long as the finger stays on the screen:
        id = evt.Keycode;

        if evt.Type == 1 || evt.Type == 0
          % Not really a touch point, but movement of the
          % simulated mouse cursor, driven by the primary
          % touch-point. Or a button event on the touch device.
          % Associate a blob 1 to it:
          id = 1;
          % Select it as a immediately released/dying blob:
          blobcol{id}.mul = 0.999;
          blobcol{id}.col = rand(3, 1);
          blobcol{id}.x = evt.mappedX;
          blobcol{id}.y = evt.mappedY;
        end

        if evt.Type == 2
          % New touch point -> New blob!
          blobcol{id}.col = rand(3, 1);
          blobcol{id}.mul = 1.0;
          blobcol{id}.x = evt.mappedX;
          blobcol{id}.y = evt.mappedY;
          blobcol{id}.t = evt.Time;
          blobcol{id}.dt = 0;
        end

        if evt.Type == 3
          % Moving touch point -> Moving blob!
          blobcol{id}.x = evt.mappedX;
          blobcol{id}.y = evt.mappedY;
          blobcol{id}.dt = (evt.Time - blobcol{id}.t) * 1000;
          blobcol{id}.t = evt.Time;
           disp(GetTouchValuators(evt.Valuators, valInfos));
        end

        if evt.Type == 4
          % Touch released - finger taken off the screen -> Dying blob!
          blobcol{id}.mul = 0.999;
          blobcol{id}.x = evt.mappedX;
          blobcol{id}.y = evt.mappedY;
        end

        if evt.Type == 5
          % Lost touch data for some reason:
          fprintf('Ooops - Sequence data loss!\n');
        end
      end

      % Now that all touches for this iteration are processed, repaint screen
      % with all live blobs at their new positions, and fade out the dying/orphaned
      % blobs:
      for i=1:length(blobcol)
        if ~isempty(blobcol{i}) && blobcol{i}.mul > 0.1
          % Draw it: .mul defines size of the blob:
          Screen('DrawDots', w, [blobcol{i}.x ; blobcol{i}.y], blobcol{i}.mul * baseSize, blobcol{i}.col);

          if blobcol{i}.mul < 1
            % An orphaned blob with no finger touching anymore, so slowly fade it out:
            blobcol{i}.mul = blobcol{i}.mul * 0.95;
          else
            % An active touch. Print its unique touch id and dT timestamp delta between updates in msecs:
            Screen('DrawText', w, num2str(i), blobcol{i}.x, blobcol{i}.y, [1 1 0]);
            Screen('DrawText', w, num2str(blobcol{i}.dt), blobcol{i}.x, blobcol{i}.y - 100, [1 1 0]);
          end
        else
          % Below threshold: Kill the blob:
          blobcol{i} = [];
        end
      end

      % Done repainting - Show it:
      Screen('Flip', w);

      % Next touch processing -> redraw -> flip cycle:
    end

    TouchQueueStop(dev);
    TouchQueueRelease(dev);
    ShowCursor(w);
    sca;
  catch
    TouchQueueRelease(dev);
    sca;
    psychrethrow(psychlasterror);
  end
end
