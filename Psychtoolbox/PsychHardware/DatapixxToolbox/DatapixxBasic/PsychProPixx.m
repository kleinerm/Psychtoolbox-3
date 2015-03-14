function varargout = PsychProPixx (cmd, varargin)
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

  if ~isscalar(sourceImage) || ~isnumeric(sourceImage) || Screen('WindowKind', sourceImage) ~= -1
    error('QueueImage: Provided sourceImage is not an open offscreen window or texture!');
  end

  if ~isequal(Screen('Rect', sourceImage), processedRect)
    error('QueueImage: Provided sourceImage does not have expected size!');
  end

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

    sampleCount = mod(sampleCount + 1, 1200);

    % Schedule a flip, do not clear the window after flip:
    if flipmethod == 0
      tvbl(sampleCount + 1) = Screen('Flip', win, tWhen, 2);
    end

    if flipmethod == 1
      tvbl(sampleCount + 1) = Screen('AsyncFlipBegin', win, tWhen, 2);
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
    varargout{1} = 1;
  else
    varargout{1} = 0;
  end

  return;
end

if strcmpi(cmd, 'GetImageBuffer')
  varargout{1} = Screen('OpenOffscreenWindow', win, 0, processedRect);
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

  if (length(varargin) >= 4) && ~isempty(varargin{4})
    % Build offscreen window as target for the input image after geometry correction
    % and potential grayscale conversion:
    processedImage = Screen('OpenOffscreenWindow', win, 0, processedRect);

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

if strcmpi(cmd, 'DisableFastDisplayMode')
  tvbl = diff(tvbl);
  tvbl = tvbl(find(tvbl >= 0 & tvbl < 1));
  plot(1000 * tvbl);
  clear win processedImage gloperator tvbl
  return;
end

error('PsychProPixx: Unknown command %s provided!', cmd);
