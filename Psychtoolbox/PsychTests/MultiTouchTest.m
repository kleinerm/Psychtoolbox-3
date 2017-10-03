function MultiTouchTest
% MultiTouchTest - A minimal test for multi-touch touchscreens.
%
% Run it. Pressing any key will stop it.
% Touch the screen and watch the nice colorful happy blobs
% sprinkle to life :)
%

% History:
% 01-Oct-2017 mk  Written.

  % Setup useful PTB defaults:
  PsychDefaultSetup(2);

  % Get first touchscreen or touchpad (only tested with touchscreen):
  dev = min(GetTouchDeviceIndices);

  % Open a default onscreen window with black background color and
  % 0-1 color range:
  [w, rect] = PsychImaging('OpenWindow', 0, 0);
  baseSize = RectWidth(rect) / 20;

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
          % touch-point. Associate a blob 1 to it:
          id = 1;
          % Select it as a immediately released/dying blob:
          blobcol{id}.mul = 0.999;
          blobcol{id}.col = rand(3, 1);
          blobcol{id}.x = evt.mappedX;
          blobcol{id}.y = evt.mappedY;

          foo = evt
        end

        if evt.Type == 2
          % New touch point -> New blob!
          blobcol{id}.col = rand(3, 1);
          blobcol{id}.mul = 1.0;
          blobcol{id}.x = evt.mappedX;
          blobcol{id}.y = evt.mappedY;
        end

        if evt.Type == 3
          % Moving touch point -> Moving blob!
          blobcol{id}.x = evt.mappedX;
          blobcol{id}.y = evt.mappedY;
        end

        if evt.Type == 4
          % Touch released - finger taken off the screen -> Dying blob!
          blobcol{id}.mul = 0.999;
          blobcol{id}.x = evt.mappedX;
          blobcol{id}.y = evt.mappedY;
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
            % An active touch. Print its unique touch id:
            Screen('DrawText', w, num2str(i), blobcol{i}.x, blobcol{i}.y, [1 1 0]);
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
    sca;
  catch
    TouchQueueRelease(dev);
    sca;
    psychrethrow(psychlasterror);
  end
end
