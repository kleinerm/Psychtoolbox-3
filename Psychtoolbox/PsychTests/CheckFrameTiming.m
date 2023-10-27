function [times,badOut,misses]=CheckFrameTiming(rectSize, scale, dur, thresh, runPriority, useWhen, useDrawingFinished)
% [times, badOut, misses]=CheckFrameTiming([rectSize=128][, scale=1][, dur=30][, thresh=0.05][, runPriority=MaxPriority('KbCheck')][, useWhen=0][,useDrawingFinished=1])
%
% based on FastMaskedNoiseDemo, CheckFrameTiming collects and displays
% timing info
%
% rectSize = Size of the generated random noise image: rectSize by rectSize
%            pixels. This is also the size of the Psychtoolbox noise
%            texture.
%
% scale = Scalefactor to apply to texture during drawing: E.g. if you'd set
%         scale = 2, then each noise pixel would be replicated to draw an image
%         that is twice the width and height of the input noise image. In this
%         demo, a nearest neighbour filter is applied, i.e., pixels are just
%         replicated, not bilinearly filtered -- Important to preserve statistical
%         independence of the random pixel values!
%
% dur   = seconds to display
%
% thresh = percent deviation to report as a miss (specify 5% as 0.05)
%
% runPriority = the priority at which to run
%
% useWhen = 0 for no when parameter sent to Screen('Flip') (seems to give
%           better timing.  any other value sends lastVbl + .5*ifi
%
% useDrawingFinished = 0 to use Screen('DrawingFinished'). mario's comment
%                      suggets it may slow thigns down, but edf thinks it is supposed to speed
%                      things up and only printing its return value would slow things down.
%
%return vals:
%times is a vector of inter-vbl-times as measured by Screen('Flip')
%badOut is 3 x N, with one column for each frame whose time deviates by more than thresh % from the ifi
%   row 1 is the frame number
%   row 2 is the measured frame time
%   row 3 is the % deviation from ifi
%misses are the frame numbers that Screen('Flip') noticed it missed

% History:
% 4.11.2006 Written (MK).
% 7.11.2006 edf modified from FastMaskedNoiseDemo

% Abort script if it isn't executed on Psychtoolbox-3:
AssertOpenGL;

% Assign default values for all unspecified input parameters:

if nargin < 1 || isempty(rectSize)
    rectSize = 128; % Default patch size is 128 by 128 noisels.
end

if nargin < 2 || isempty(scale)
    scale = 1; % Don't up- or downscale patch by default.
end

if nargin < 3 || isempty(dur)
    dur = 30; % play for 30 sec by default
end

if nargin < 4 || isempty(thresh)
    thresh = .05; % report ifi's off by 5% or more by default
end

if nargin < 5 || isempty(runPriority)
    runPriority=MaxPriority('KbCheck') %run at top priority that allows keyboard interaction by default
end

if nargin < 6 || isempty(useWhen)
    useWhen=0; %don't use when by default
end

if nargin < 7 || isempty(useDrawingFinished)
    useDrawingFinished=1; % use DrawingFinished by default
end

oldPriority=Priority();
try
    % Find screen with maximal index:
    screenid = max(Screen('Screens'));

    % Open fullscreen onscreen window on that screen. Background color is
    % gray, double buffering is enabled. Return a 'win'dowhandle and a
    % rectangle 'winRect' which defines the size of the window.
    [win, winRect] = Screen('OpenWindow', screenid, 128);

    % Query monitor flip interval. We need it to properly time our
    % display loop:
    ifi = Screen('GetFlipInterval', win);

    % Compute destination rectangle locations for the random noise patches:

    % 'objRect' is a rectangle of the size 'rectSize' by 'rectSize' pixels of
    % our Matlab noise image matrix:
    objRect = SetRect(0,0, rectSize, rectSize);
    dstRect = ArrangeRects(1, objRect, winRect);

    % Now we rescale
    % Compute center position [xc,yc] of therectangle:
    [xc, yc] = RectCenter(dstRect(1,:));
    % Create a new rectange, centered at the same position, but 'scale'
    % times the size of our pixel noise matrix 'objRect':
    dstRect(1,:)=CenterRectOnPoint(objRect * scale, xc, yc);

    % Build a nice aperture texture: Offscreen windows can be used as
    % textures as well, so we open an Offscreen window of exactly the same
    % size 'objRect' as our noise textures, with a gray default background.
    % This way, we can use the standard Screen drawing commands to 'draw'
    % our aperture:
    aperture=Screen('OpenOffscreenwindow', win, 128, objRect);

    % First we clear out the alpha channel of the aperture disk to zero -
    % In this area the noise stimulus will shine through:
    Screen('FillOval', aperture, [255 255 255 0], objRect);

    % Then we draw a nice black border around the disk, now with maximum
    % alpha, ie, opaque:
    Screen('FrameOval', aperture, [0 0 0 255], objRect);

    % Draw a little green opaque fixation spot into it:
    Screen('FillOval', aperture, [0 255 0 255], CenterRect(SetRect(0,0,10,10),objRect));

    % Now just for the fun of it a bit of text in red, with a randomly
    % selected alpha value between 50% transparent and fully transparent.
    Screen('TextSize', aperture, 24);
    Screen('TextStyle', aperture, 1);
    DrawFormattedText(aperture, 'Subliminal\n\nMessage', 'center', 'center', [255 0 0 (255 * 0.5 * rand)]);

    % Enable alpha blending: This makes sure that the alpha channel
    % (transparency channel) of our 'aperture' texture is used properly:
    % It means: Whenever a new pixel is drawn to the framebuffer, then the
    % new color of the framebuffer should be a weighted average of its old
    % color value dstcolor and the current drawing srccolor. The alpha
    % value srcalpha of the drawing color is used as weight:
    % srcalpha = alpha value / 255, so alpha 0 -> srcalpha 0.0, alpha 255
    % -> srcalpha 1.0.
    %
    % newcolor = srccolor * srcalpha + dstcolor * (1-srcalpha).
    %
    % This blending mode will allow to draw partially opaque shapes or
    % texture images, where the opacity is controlled by the alpha value of
    % the current pen color (shape drawing) or of the alpha channel of
    % textures. Alpha blending can be configured in many ways. See e.g.,
    % help PsychAlphaBlending, or help GL_SRC_ALPHA for further
    % information.
    Screen('BlendFunction', win, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    % Init framecounter to zero.
    count = 0;

    %allocate vars outside realtime loop
    lastVbl=0;
    noiseimg=(ones(rectSize));
    times = -1*ones(1,ceil(2*(dur/ifi)));
    misses = 0*times;
    tex=0;
    vbl=0;
    sos=0;
    fts=0;
    missed=0;
    bps=0;

    Priority(runPriority);

    % Perform initial Flip and sync us to the retrace:
    vbl = Screen('Flip', win);

    % Recort time of start of presentation loop:
    tstart = vbl;

    % Run noise image drawing loop for dur seconds.
    while vbl < (tstart + dur)
        lastVbl=vbl;
        % Generate and draw noise image:

        % Compute 'noiseimg' noise image matrix with Matlab:
        % Normally distributed noise with mean 128 and stddev. 50, each
        % pixel computed independently with a size of rectSize x
        % rectSize noise pixels:
        noiseimg=(50*randn(rectSize, rectSize) + 128);

        % Convert it to a texture 'tex':
        tex=Screen('MakeTexture', win, noiseimg);

        % Draw the texture into the screen location
        % If dstRect is bigger
        % than our noise image 'noiseimg', PTB will automatically
        % up-scale the noise image. We set the 'filterMode' flag for
        % drawing of the noise image to zero: This way the bilinear
        % filter gets disabled and replaced by standard nearest
        % neighbour filtering. This is important to preserve the
        % statistical independence of the noise pixels in the noise
        % texture! The default bilinear filtering would introduce local
        % correlations when scaling is applied:
        Screen('DrawTexture', win, tex, [], dstRect(1,:), [], 0);

        % Overdraw the rectangular noise image with our special
        % aperture image. The noise image will shine through in areas
        % of the aperture image where its alpha value is zero (i.e.
        % transparent):
        Screen('DrawTexture', win, aperture, [], dstRect(1,:), [], 0);

        % After drawing, we can discard the noise texture.
        Screen('Close', tex);


        % Done with drawing the noise patches to the backbuffer, our
        % stimulus is ready.:

        % from mario:
        % This
        % command has the side effect of slightly reducing overall speed of
        % the graphics pipeline, that's why it is commented out by default.
        %
        % from edf: i thought this command helped performance? isn't it
        % just printing out the return value that is slow?

        if useDrawingFinished
            Screen('DrawingFinished', win, 0);
        end

        % Initiate buffer-swap.
        %
        % Here we specifically ask 'Flip' to swap at the next retrace after
        % time 'vbl + 0.5*ifi' == The deadline is exactly one monitor refresh
        % interval after the last time we updated our stimulus. Providing
        % Flip with a explicit deadline allows the internal skipped frame
        % detection to work more reliably, because then it has a simple
        % "model" of what correct timing would be, instead of needing to
        % read our mind (i.e. make an educated but possibly wrong guess)
        % of what we wanted to have. PTB will also try to optimize its
        % internal operations in order to meet that deadline.
        % As usual, the returned vbl value will contain a good estimate of
        % when the bufferswap in sync with retrace really happened. vbl
        % provides the baseline for future invocations of flip.

        if useWhen
            [vbl sos fts missed bps] =  Screen('Flip', win);
        else
            %edf: better timing with no when parameter
            [vbl sos fts missed bps] =  Screen('Flip', win, vbl + 0.5*ifi);
        end

        % Increase our frame counter:
        count = count + 1;

        times(count)=vbl-lastVbl;

        if missed>0
            misses(count)=1;
        end


    end % Next stimulus frame...

    %compute and plot timing

    times=times(times>-1);
    pcts = abs(times-ifi)/ifi;
    badtimes=find(pcts > thresh);
    misses=find(misses);
    badOut=[badtimes;times(badtimes);pcts(badtimes)];

    subplot(3,1,1)
    hist(times(pcts>=1),200);
    ylabel('count')
    xlabel('seconds')
    subplot(3,1,2)
    hist(times(pcts>thresh & pcts<=2),200);
    ylabel('count')
    xlabel('seconds')
    subplot(3,1,3)
    hist(times(setdiff(1:length(times),badtimes)),200);
    ylabel('count')
    xlabel('seconds')

    figure
    plot(times)
    ylabel('seconds')
    xlabel('frame num')

    % We're done: Output average framerate:
    telapsed = GetSecs - tstart
    updaterate = count / telapsed

    % Done. Close Screen, release all ressouces:
    Screen('CloseAll');
    Priority(oldPriority);
catch
    % Our usual error handler: Close screen and then...
    Screen('CloseAll');
    Priority(oldPriority);
    % ... rethrow the error.
    psychrethrow(psychlasterror);
end
