function varargout = PsychProPixx(cmd, varargin)
% PsychProPixx() - Drive fast display modes of ProPixx projectors.
%
% This function is an EXPERIMENTAL PROTOTYPE. It is subject
% to backwards incompatible change, or maybe even complete
% removal from future Psychtoolbox versions without warning!
% If you intend to use it in your own experiments, better make
% a private copy of the function file, or be prepared to rewrite
% your experiment code after Psychtoolbox updates.
%
% This function provides a preliminary method to setup Psychtoolbox
% for the fast display modes of the ProPixx projector and to use those
% modes in a convenient and relatively efficient fashion.
%
% See PropixxImageUndistortionThrowaway.m for a preliminary demo
% on how to use this function.
%
%
% Subcommands and their meaning:
%
% PsychProPixx('SetupFastDisplayMode', window, rate [, flipmethod=0][, localcalibration=none][, dogpumeasure=0]);
% -- Setup for Propixx fast display mode. 'window' is the handle of
% the onscreen window which displays on the ProPixx. 'rate' is the
% desired update rate, 4 for 4-fold rate (= 120 * 4 = 480 Hz), or
% 12 for 12-fold rate (= 120 * 12 = 1440 Hz). 'flipmethod' method
% of flipping the stimulus images onto the screen. 0 = Standard
% Screen('Flip'), 1 = Screen('AsyncFlipBegin') async flips,
% 2 = Optimized non-blocking flips with later retrieval of timestamps
% via PsychProPixx('GetTimingSamples'). Method 2 is the most efficient
% method, but it is only supported on Linux with the open-source graphics
% drivers. Method 1 is likely the second most efficient method and should
% work on all operating systems, but returned timestamps need some
% understanding of the rules for async flips. Method 0 is the most easy
% to use on any system, but also likely the least efficient one.
% 'localcalibration' Specify a calibration file for display geometry correction
% via a local method. This is usually slower than use of a global calibration,
% so better use that and leave this parameter [] empty.
% 'dogpumeasure' If set to 1 then the actual processing time per frame
% on the graphics card (gpu) will be measured with high precision if the
% gpu + graphics driver supports this. This is useful for benchmarking and
% tuning of code. Results are returned in the global variable 'gpudur' and
% optionally plotted via PsychProPixx('DisableFastDisplayMode', 1);
%
%
% image = PsychProPixx('GetImageBuffer');
% -- Get an offscreen window suitable for drawing a stimulus into it,
% and then passing it to PsychProPixx('QueueImage', image);
%
%
% presenting = PsychProPixx('QueueImage', image [, tWhen]);
% - Queue a 'image' for presentation. 'image' must be a texture
% or OffScreenWindow of proper size, ie., half the width and height
% of the onscreen window. The most easy way to get a matching
% offscreen window for drawing into and presentation is to use
% image = PsychProPixx('GetImageBuffer').
%
% Once a sufficient number of images are queued, the final
% image for driving the ProPixx projector is created and presented,
% either at the next video refresh, or at the optional target time
% 'tWhen' - which is passed to the Screen('Flip') command. When
% this happens, the 'presenting' flag will return as either 1 in offline
% timestamping mode (flipmethod 2), or as the Flip timestamp of
% image onset for the whole composite image, otherwise it will return
% as 0 if an image has just been queued for presentation.
%
%
% samples = PsychProPixx('GetTimingSamples');
% -- Returns 'Flip' timestamps in the vector 'samples'. Depending
% on flipmode, either all samples of all presentations are returned
% (for flipmode 2 on Linux), or up to the most recent 1200 samples
% are returned (for flipmode 0 or 1 on all operating systems).
%
%
% PsychProPixx('DisableFastDisplayMode' [, plottimestamps]);
% -- Disable Propixx mode, reset internal variables etc. If the optional
% 'plottimestamps' flag is set to 1 then plots are generated which show
% the collected durations of frame presentations, and potentially
% collected timestamps of gpu rendertime if benchmarking was enabled.
%

%
% History:
% 14-Mar-2015  mk  Initial incomplete prototype for testing.

persistent win
persistent rate
persistent hw
persistent hh
persistent phaseid
persistent quadrants
persistent dstColorMask
persistent processedImage
persistent processedRect
persistent gloperator
persistent sampleCount
persistent tvbl
persistent flipmethod
persistent gpumeasure
persistent dogpumeasure
global gpudur

if nargin < 1
  % TODO: Use @PsychProPixx function to create global calibration?
  % Doable, but is it worth the effort?
  %  varargout{1} = warpstruct;
  %  varargout{2} = filterMode;
  return;
end

if strcmpi(cmd, 'QueueImage')
  sourceImage = varargin{1};

  if (1 ~= numel(sourceImage)) || ~isnumeric(sourceImage) || Screen('WindowKind', sourceImage) ~= -1
    error('QueueImage: Provided sourceImage is not an open offscreen window or texture!');
  end

  % This sucks away performance on Octave, as isequal is awfully slow:
  %if ~isequal(Screen('Rect', sourceImage), processedRect)
  %  error('QueueImage: Provided sourceImage does not have expected size!');
  %end

  if length(varargin) >= 2
    tWhen = varargin{2};
  else
    tWhen = [];
  end

  % Needs processing, e.g., geometry correction or grayscale conversion?
  if ~isempty(gloperator) && ~isempty(processedImage)
    % Process sourceImage via gloperator, storing result to internal processedImage buffer:
    processedImage = Screen('TransformTexture', sourceImage, gloperator, [], processedImage);
    outImage = processedImage;
  else
    % No: Just pass it through:
    outImage = sourceImage;
  end

  % Define destination quadrant:
  dstRect = quadrants(mod(phaseid, 4) + 1, :);

  % Define destination color channel in 12x mode:
  if rate == 12
    Screen('Blendfunction', win, [], [], dstColorMask(mod(phaseid, 12) + 1, :));
  end

  % Blit outImage to target quadrant and channel, disable filtering as outImage
  % already has matching size for 1-to-1 blit:
  Screen('DrawTexture', win, outImage, [], dstRect, [], 0);

  % Update phaseid:
  phaseid = phaseid + 1;

  % Output image for projector complete?
  if phaseid == rate
    % Yes. Do the output thing:
    phaseid = 0;

    % Restore colormask to normal:
    Screen('Blendfunction', win, [], [], [1 1 1 1]);

    sampleCount = mod(sampleCount + 1, 1200);

    % Schedule a flip, do not clear the window after flip:
    if flipmethod == 0
      tvbl(sampleCount + 1) = Screen('Flip', win, tWhen, 2);
      tstamp = tvbl(sampleCount + 1);
    end

    if flipmethod == 1
      tvbl(sampleCount + 1) = Screen('AsyncFlipBegin', win, tWhen, 2);
      tstamp = tvbl(sampleCount + 1);
    end

    if flipmethod == 2
      % A sync flip, but non-blocking, without timestamping.
      % Instead we use swap completion logging to collect all
      % timestamps at the end of the session. Linux only feature...
      Screen('Flip', win, tWhen, 2, 1);
      tstamp = 1;
    end

    if dogpumeasure
        % Result of GPU time measurement expected?
        if gpumeasure
            % Retrieve results from GPU load measurement:
            % Need to poll, as this is asynchronous and non-blocking,
            % so may return a zero time value at first invocation(s),
            % depending on how deep the rendering pipeline is:
            while 1
                winfo = Screen('GetWindowInfo', win);
                if winfo.GPULastFrameRenderTime > 0
                    break;
                end
            end

            % Store it:
            gpudur(sampleCount + 1) = winfo.GPULastFrameRenderTime;
            gpumeasure = 0;
        end

        % Start GPU timer: gpumeasure will be true if this
        % is actually supported and will return valid results:
        gpumeasure = Screen('GetWindowInfo', win, 5);
    end

    % Signal flip either needed or already scheduled:
    varargout{1} = tstamp;
  else
    varargout{1} = 0;
  end

  return;
end

if strcmpi(cmd, 'GetImageBuffer')
  varargout{1} = Screen('OpenOffscreenWindow', win, 0, processedRect, [], 32);
  return;
end

if strcmpi(cmd, 'SetupFastDisplayMode')
  win = varargin{1};
  if ~isscalar(win) || ~isnumeric(win) || Screen('WindowKind', win) ~= 1
    error('SetupFastDisplayMode: Provided windowHandle is not an open onscreen window!');
  end

  rate = varargin{2};
  if ~isscalar(rate) || ~isnumeric(rate) || ~ismember(rate, [4, 12])
    error('SetupFastDisplayMode: Provided rate value is not 4 or 12 fold!');
  end

  [hw, hh] = Screen('WindowSize', win);
  hw = round(hw / 2);
  hh = round(hh / 2);
  processedRect = [0, 0, hw, hh];
  quadrants(1, :) = OffsetRect(processedRect, 0, 0);
  quadrants(2, :) = OffsetRect(processedRect, hw, 0);
  quadrants(3, :) = OffsetRect(processedRect, 0, hh);
  quadrants(4, :) = OffsetRect(processedRect, hw, hh);
  
  dstColorMask(1,:) = [1 0 0 0];
  dstColorMask(2,:) = [1 0 0 0];
  dstColorMask(3,:) = [1 0 0 0];
  dstColorMask(4,:) = [1 0 0 0];

  dstColorMask(5,:) = [0 1 0 0];
  dstColorMask(6,:) = [0 1 0 0];
  dstColorMask(7,:) = [0 1 0 0];
  dstColorMask(8,:) = [0 1 0 0];

  dstColorMask(9,:) = [0 0 1 0];
  dstColorMask(10,:) = [0 0 1 0];
  dstColorMask(11,:) = [0 0 1 0];
  dstColorMask(12,:) = [0 0 1 0];

  phaseid = 0;
  sampleCount = 0;
  gpumeasure = 0;

  if rate == 4
    Screen('Blendfunction', win, [], [], [1 1 1 1]);
  end

  if (length(varargin) >= 3) && ~isempty(varargin{3})
    flipmethod = varargin{3};
  else
    flipmethod = 0;
  end

  if flipmethod == 2
     Screen('GetFlipInfo', win, 1);
  end

  if (length(varargin) >= 4) && ~isempty(varargin{4})
    % Build offscreen window as target for the input image after geometry correction
    % and potential grayscale conversion:
    processedImage = Screen('OpenOffscreenWindow', win, 0, processedRect, [], 32);

    calib = varargin{4};
    
    % calib can be a calibration structure, calibration file name, or gloperator.
    % If it isn't a ready made gloperator yet, assume it is calibration info and
    % build/setup a proper gloperator to implement the calibration:
    if ~isscalar(calib) || ~isnumeric(calib) || Screen('WindowKind', calib) ~= 4
      % Create an empty image processing operator for onscreen window 'w':
      gloperator = CreateGLOperator(win);
      AddImageUndistortionToGLOperator(gloperator, processedImage, calib, 0, 73, 73);
    else
      gloperator = calib;
    end
  end

  if (length(varargin) >= 5) && ~isempty(varargin{5})
    dogpumeasure = varargin{5};
  else
    dogpumeasure = 0;
  end

  return;
end

if strcmpi(cmd, 'GetTimingSamples')
  if flipmethod == 2
    tvbl = [];

    while 1
      flipinfo = Screen('GetFlipInfo', win, 3);
      if isempty(flipinfo)
        break;
      end

      tvbl(end+1) = flipinfo.OnsetTime;
      if ~strcmp(flipinfo.SwapType, 'Pageflip')
        disp(flipinfo);
      end
    end
  end

  varargout{1} = tvbl;

  return;
end

if strcmpi(cmd, 'DisableFastDisplayMode')
  if length(varargin) >= 1 && ~isempty(varargin{1}) && varargin{1}
    tvbl = diff(tvbl);
    tvbl = tvbl(find(tvbl >= 0 & tvbl < 1));
    figure;
    plot(1000 * tvbl);
    title('Delta between successive presentations [msecs]:')

    if gpumeasure && length(gpudur) > 0
      figure;
      plot(1000 * gpudur);
      title('Processing time needed on GPU per frame [msecs]:')
    end
  end

  tvbl = [];
  win = [];
  processedImage = [];
  gloperator = [];
  return;
end

error('PsychProPixx: Unknown command %s provided!', cmd);
