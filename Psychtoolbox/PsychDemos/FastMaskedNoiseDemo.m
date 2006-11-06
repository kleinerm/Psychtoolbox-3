function FastMaskedNoiseDemo(numRects, rectSize, scale)
% FastMaskedNoiseDemo([numRects=1][, rectSize=128][, scale=1])
%
% Demonstrates how to generate and draw noise patches on-the-fly in a fast
% way. The patches are shown through circular apertures by the use of
% alpha-blending.
%
% numRects = Number of random patches to generate and draw per frame.
%
% rectSize = Size of the generated random noise image: rectSize by rectSize
%            pixels. This is also the size of the Psychtoolbox noise
%            texture.
%
% scale = Scalefactor to apply to texture during drawing: E.g. if you'd set
% scale = 2, then each noise pixel would be replicated to draw an image
% that is twice the width and height of the input noise image. In this
% demo, a nearest neighbour filter is applied, i.e., pixels are just
% replicated, not bilinearly filtered -- Important to preserve statistical
% independence of the random pixel values!
%
% If you play around with the parameters and compare performance to the
% FastNoiseDemo, you will notice the following:
%
% - Scaling the stimulus to a bigger size is nearly free on modern graphics
% hardware, so you can generate low-resolution noise stimuli that still
% fill a huge fraction of your display area if you want.
%
% - Drawing the aperture is nearly free, i.e., this demo runs nearly as
% fast as the FastNoiseDemo without masking. This is because modern
% gfx-hardware is highly optimized for texture drawing and alpha blending.
% The aperture textures are cached in fast onboard VRAM memory to speed up
% drawing them.
%
% - The drawing speed is mostly limited by how fast Matlab can compute new
% random dot number matrices, not by properties of the stimulus images.
%

% History:
% 4.11.2006 Written (MK).

% Abort script if it isn't executed on Psychtoolbox-3:
AssertOpenGL;

% Assign default values for all unspecified input parameters:

if nargin < 1 || isempty(numRects)
    numRects = 1; % Draw one noise patch by default.
end

if nargin < 2 || isempty(rectSize)
    rectSize = 128; % Default patch size is 128 by 128 noisels.
end

if nargin < 3 || isempty(scale)
    scale = 1; % Don't up- or downscale patch by default.
end

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

    % ArrangeRects creates 'numRects' copies of 'objRect', all nicely
    % arranged / distributed in our window of size 'winRect':
    dstRect = ArrangeRects(numRects, objRect, winRect);

    % Now we rescale all rects: They are scaled in size by a factor 'scale':
    for i=1:numRects
        % Compute center position [xc,yc] of the i'th rectangle:
        [xc, yc] = RectCenter(dstRect(i,:));
        % Create a new rectange, centered at the same position, but 'scale'
        % times the size of our pixel noise matrix 'objRect':
        dstRect(i,:)=CenterRectOnPoint(objRect * scale, xc, yc);
    end

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

    % Perform initial Flip and sync us to the retrace:
    vbl = Screen('Flip', win);

    % Recort time of start of presentation loop:
    tstart = vbl;

    % Run noise image drawing loop for 10 seconds.
    while vbl < (tstart + 10)
        % Generate and draw 'numRects' noise images:
        for i=1:numRects
            % Compute 'noiseimg' noise image matrix with Matlab:
            % Normally distributed noise with mean 128 and stddev. 50, each
            % pixel computed independently with a size of rectSize x
            % rectSize noise pixels:
            noiseimg=(50*randn(rectSize, rectSize) + 128);

            % Convert it to a texture 'tex':
            tex=Screen('MakeTexture', win, noiseimg);

            % Draw the texture into the screen location defined by the
            % destination rectangle 'dstRect(i,:)'. If dstRect is bigger
            % than our noise image 'noiseimg', PTB will automatically
            % up-scale the noise image. We set the 'filterMode' flag for
            % drawing of the noise image to zero: This way the bilinear
            % filter gets disabled and replaced by standard nearest
            % neighbour filtering. This is important to preserve the
            % statistical independence of the noise pixels in the noise
            % texture! The default bilinear filtering would introduce local
            % correlations when scaling is applied:
            Screen('DrawTexture', win, tex, [], dstRect(i,:), [], 0);

            % Overdraw the rectangular noise image with our special
            % aperture image. The noise image will shine through in areas
            % of the aperture image where its alpha value is zero (i.e.
            % transparent):
            Screen('DrawTexture', win, aperture, [], dstRect(i,:), [], 0);
            
            % After drawing, we can discard the noise texture.
            Screen('Close', tex);
        end % Next noise image...
        
        % Done with drawing the noise patches to the backbuffer, our
        % stimulus is ready.:

        % If you uncomment the following line, it will print out the
        % elapsed computation time from last bufferswap to now. It is an
        % indication of how much time was really spent by Matlab and the
        % graphics hardware in order to create the final stimulus. This
        % command has the side effect of slightly reducing overall speed of
        % the graphics pipeline, that's why it is commented out by default.

        % elapsed = Screen('DrawingFinished', win, 0, 1)
        
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
        vbl = Screen('Flip', win, vbl + 0.5*ifi);

        % Increase our frame counter:
        count = count + 1;
    end % Next stimulus frame...

    % We're done: Output average framerate:
    telapsed = GetSecs - tstart
    updaterate = count / telapsed
    
    % Done. Close Screen, release all ressouces:
    Screen('CloseAll');
catch
    % Our usual error handler: Close screen and then...
    Screen('CloseAll');
    % ... rethrow the error.
    psychrethrow(psychlasterror);
end
