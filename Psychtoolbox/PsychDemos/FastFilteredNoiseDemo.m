function FastFilteredNoiseDemo(validate, filtertype, rectSize, kwidth, scale, syncToVBL, dontclear)
% FastFilteredNoiseDemo([validate=1][, filtertype=1][, rectSize=128][, kwidth=5][, scale=1][, syncToVBL=1][, dontclear=0])
%
% Demonstrates how to generate, filter and draw noise patches on-the-fly 
% in a fast way by use of GLSL fragment shaders.
% Use it to benchmark your system by varying the load. If you like this demo
% then also have a look at FastMaskedNoiseDemo that shows how to
% efficiently draw a masked stimulus by use of alpha-blending.
%
% filtertype = Type of filter to apply, see switch statement below for
% supported filters. Zero selects no filtering, 1 is Gaussian blur.
%
% rectSize = Size of the generated random noise image: rectSize by rectSize
%            pixels. This is also the size of the Psychtoolbox noise
%            texture.
%
% kwidth = For filters which support a varying kernel size, the kernel
% size. Will create a kwidth x kwidth convolution kernel.
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
numRects = 1;

if nargin < 1 || isempty(validate)
    validate = 1; % Perform validation of the filter result by default:
end

if nargin < 2 || isempty(filtertype)
    filtertype = 1; % Gaussian blur by default.
end

if nargin < 3 || isempty(rectSize)
    rectSize = 128; % Default patch size is 128 by 128 noisels.
end

if nargin < 4 || isempty(kwidth)
    kwidth = 5; % Kernel width is 5 x 5 by default.
end

if nargin < 5 || isempty(scale)
    scale = 1; % Don't up- or downscale patch by default.
end

if nargin < 6 || isempty(syncToVBL)
    syncToVBL = 1; % Synchronize to vertical retrace by default.
end

if syncToVBL > 0
    asyncflag = 0;
else
    asyncflag = 2;
end

if nargin < 7 || isempty(dontclear)
    dontclear = 0; % Clear backbuffer to background color by default after each bufferswap.
end

if dontclear > 0
    % A value of 2 will prevent any change to the backbuffer after a
    % bufferswap. In that case it is your responsibility to take care of
    % that, but you'll might save up to 1 millisecond.
    dontclear = 2;
end

%try
    % Find screen with maximal index:
    screenid = max(Screen('Screens'));

    % Initialize OpenGL. We need it for the image processing:
    InitializeMatlabOpenGL([], [], 1);
    
    % Open fullscreen onscreen window on that screen. Background color is
    % gray, double buffering is enabled. Return a 'win'dowhandle and a
    % rectangle 'winRect' which defines the size of the window:
    [win, winRect] = Screen('OpenWindow', screenid, 128, [],[],[],[],[],kPsychNeedFastBackingStore);
    
    % Build a filter kernel:
    stddev = kwidth / 2;

    switch(filtertype)
        case 0
            kernel = [1];
        case 1
            kernel = fspecial('gaussian', kwidth, stddev);
        case 2
            kernel = fspecial('prewitt');
        case 3
            kernel = fspecial('sobel');
        case 4
            kernel = fspecial('laplacian');
        case 5
            kernel = fspecial('gaussian', kwidth, stddev);
            kernel1 = fspecial('gaussian', [kwidth, 1], stddev);
            kernel2 = fspecial('gaussian', [1, kwidth], stddev);
        case 6
            kernel = randn(kwidth, kwidth);
        case 7
            kernel = ones(kwidth, kwidth)*1;
            for i=1:length(kernel)*length(kernel)
%                kernel(i) = 1 - 2*mod(i,2);
            end
    end

stype = 2;
channels = 1;

    if filtertype > 0
        % Build shader from kernel:
%        shader = EXPCreateStatic2DConvolutionShader(kernel, channels, 1, 1, stype);
        convoperator = CreateGLOperator(win, kPsychNeed32BPCFloat);
        if filtertype~=5
            Add2DConvolutionToGLOperator(convoperator, kernel, [], channels, 1, 4, stype);
        else
            Add2DSeparableConvolutionToGLOperator(convoperator, kernel1, kernel2, [], channels, 1, 4, stype);
        end
        
%        Add2DConvolutionToGLOperator(convoperator, kernel, [], channels, 1, 4, stype);
%        Add2DConvolutionToGLOperator(convoperator, kernel, [], channels, 1, 4, stype);
    end

    glFinish;
    
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
    glFinish;
    tstart = GetSecs;
    endtime = tstart + 5;
    xtex = 0;
    
    % Run noise image drawing loop for 20 seconds.
    while GetSecs < endtime
        % Increase our frame counter:
        count = count + 1;

        % Generate and draw 'numRects' noise images:
        for i=1:numRects
            % Compute noiseimg noise image matrix with Matlab:
            % Normally distributed noise with mean 128 and stddev. 50, each
            % pixel computed independently:
            noiseimg=(50*randn(rectSize, rectSize) + 128);
            %noiseimg=ones(rectSize, rectSize)*255;
            %noiseimg=imread([PsychtoolboxRoot '/PsychDemos/konijntjes1024x768gray.jpg']);
            if validate
                noiseimg=uint8(noiseimg);
                noiseimg=double(noiseimg);
            end
            
            % Convert it to a texture 'tex':
            tex=Screen('MakeTexture', win, noiseimg,[],[],0);
            
            % Draw the texture into the screen location defined by the
            % destination rectangle 'dstRect(i,:)'. If dstRect is bigger
            % than our noise image 'noiseimg', PTB will automatically
            % up-scale the noise image. We set the 'filterMode' flag for
            % drawing of the noise image to zero: This way the bilinear
            % filter gets disabled and replaced by standard nearest
            % neighbour filtering. This is important to preserve the
            % statistical independence of the noise pixels in the noise
            % texture! The default bilinear filtering would introduce local
            % correlations:
            if validate
                glFinish;
                tic
                % Apply filter to texture:
                xtex = Screen('TransformTexture', tex, convoperator, [], xtex);
                Screen('DrawTexture', win, xtex, [], dstRect(i,:), [], 0);
                %Screen('DrawTexture', win, tex, [], dstRect(i,:), [], 0, [], [], shader);
                glFinish;
                gput(count) = toc;
            else                
                % Apply filter to texture:
                xtex = Screen('TransformTexture', tex, convoperator, [], xtex);
                Screen('DrawTexture', win, xtex, [], dstRect(i,:), [], 0);
            end
            
            if validate
                % Compute same convolution on CPU:
                %noiseimg = single(noiseimg);
                tic
                if filtertype ~=5
                    ref = conv2(noiseimg, single(kernel), 'same');
                else
                    ref = conv2(single(kernel1), single(kernel2), noiseimg, 'same');
                end
                cput(count) = toc;
                %ref = uint8(0.5 + ref);
                %ref = single(ref);
            end
            
            % After drawing, we can discard the noise texture.
            Screen('Close', tex);
        end
        
        % Done with drawing the noise patches to the backbuffer: Initiate
        % buffer-swap. If 'asyncflag' is zero, buffer swap will be
        % synchronized to vertical retrace. If 'asyncflag' is 2, bufferswap
        % will happen immediately -- Only useful for benchmarking!
        Screen('Flip', win, 0, dontclear, asyncflag);
        
        if validate
            %gpu = (Screen('GetImage', win, dstRect(1,:)));
            gpu = Screen('GetImage', xtex, [], [], 1, 1) * 255;
            blah = class(gpu)
            bluh = class(ref)
            m0=size(gpu)
            m1=max(max(gpu))
            m2=min(min(gpu))
            m3=max(max(ref))
            m4=min(min(ref))
            
            difference = gpu(:,:,1) - ref;
            difference = abs(difference(length(kernel):end-length(kernel), length(kernel):end-length(kernel)));
            maxdiff = max(max(difference))
        end        
    end

    % We're done: Output average framerate:
    glFinish;
    telapsed = GetSecs - tstart
    updaterate = count / telapsed
    if validate
        avgspeedup=mean(cput(2:end)) / mean(gput(2:end))
        close all;
        imagesc(difference);
        figure;
        imagesc(ref);
        figure;
        imagesc(gpu);
    end

    % Done. Close Screen, release all ressouces:
    Screen('CloseAll');
%catch
    % Our usual error handler: Close screen and then...
%    Screen('CloseAll');
    % ... rethrow the error.
%    psychrethrow(psychlasterror);
%end
