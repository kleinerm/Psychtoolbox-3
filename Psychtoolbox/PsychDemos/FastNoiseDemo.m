function FastNoiseDemo(numRects, rectSize, scale, syncToVBL, dontclear)
% FastNoiseDemo([numRects=1][, rectSize=128][, scale=1][, syncToVBL=1][, dontclear=0])
%
% Demonstrates how to generate and draw noise patches on-the-fly in a fast way. Can be
% used to benchmark your system by varying the load. If you like this demo
% then also have a look at FastMaskedNoiseDemo that shows how to
% efficiently draw a masked stimulus by use of alpha-blending.
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
% syncToVBL = 1=Synchronize bufferswaps to retrace. 0=Swap immediately when
% drawing is finished. Value zero is useful for benchmarking the whole
% system, because your measured framerate will not be limited by the
% monitor refresh rate -- Gives you a feeling of how much headroom is left
% in your loop.
%
% dontclear = If set to 1 then the backbuffer is not automatically cleared
% to background color after a flip. Can save up to 1 millisecond on old
% graphics hardware.
%
% Example results on a Intel Pentium-4 3.2 Ghz machine with a NVidia
% GeForce 7800 GTX graphics card, running under M$-Windows XP SP3:
%
% Two patches, 256 by 256 noise pixels each, scaled by any factor between 1
% and 5 yields a redraw rate of 100 Hz.
%
% One patch, 256 by 256 noise pixels, scaled by any factor between 1
% and 5 yields a redraw rate of 196 Hz.
%
% Two patches, 128 by 128 noise pixels each, scaled by any factor between 1
% and 5 yields a redraw rate of 360 - 380 Hz.
% 
% One patch, 128 by 128 noise pixels, scaled by any factor between 1
% and 5 yields a redraw rate of 670 Hz.

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

if nargin < 4 || isempty(syncToVBL)
    syncToVBL = 1; % Synchronize to vertical retrace by default.
end

if syncToVBL > 0
    asyncflag = 0;
else
    asyncflag = 2;
end

if nargin < 5 || isempty(dontclear)
    dontclear = 0; % Clear backbuffer to background color by default after each bufferswap.
end

if dontclear > 0
    % A value of 2 will prevent any change to the backbuffer after a
    % bufferswap. In that case it is your responsibility to take care of
    % that, but you'll might save up to 1 millisecond.
    dontclear = 2;
end

try
    % Find screen with maximal index:
    screenid = max(Screen('Screens'));

    % Open fullscreen onscreen window on that screen. Background color is
    % gray, double buffering is enabled. Return a 'win'dowhandle and a
    % rectangle 'winRect' which defines the size of the window:
    [win, winRect] = Screen('OpenWindow', screenid, 128);
        
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

    % Init framecounter to zero and take initial timestamp:
    count = 0;    
    tstart = GetSecs;

    % Run noise image drawing loop for 1000 frames:
    while count < 1000
        % Generate and draw 'numRects' noise images:
        for i=1:numRects
            % Compute noiseimg noise image matrix with Matlab:
            % Normally distributed noise with mean 128 and stddev. 50, each
            % pixel computed independently:
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

            % After drawing, we can discard the noise texture.
            Screen('Close', tex);
        end
        
        % Done with drawing the noise patches to the backbuffer: Initiate
        % buffer-swap. If 'asyncflag' is zero, buffer swap will be
        % synchronized to vertical retrace. If 'asyncflag' is 2, bufferswap
        % will happen immediately -- Only useful for benchmarking!
        Screen('Flip', win, 0, dontclear, asyncflag);

        % Increase our frame counter:
        count = count + 1;
    end

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
