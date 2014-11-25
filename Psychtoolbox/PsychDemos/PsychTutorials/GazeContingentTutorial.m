function GazeContingentTutorial(mode, ms, myimgfile)
%
% ___________________________________________________________________
%
% Demo implementation of a generic gaze-contingent display.
% We take one input image and create - via image processing - two images
% out of it: An image to show at the screen location were the subject
% fixates (According to the eye-tracker). A second image to show in the
% peripery of the subjects field of view. These two images are blended into
% each other via a gaussian weight mask (an aperture). The mask is centered
% at the center of gaze and allows for a smooth transition between the two
% images.
%
% This illustrates an application of OpenGL Alpha blending by compositing
% two images based on a spatial gaussian weight mask. Compositing is done
% by the graphics hardware.
%
% See also: PsychDemos, MovieDemo, DriftDemo

% HISTORY
%
% mm/dd/yy 
% 
%  7/23/05    mk      Derived it from Frans Cornelissens AlphaImageDemoOSX.
% 11/19/06    dhb     Remove OSX from name.
% 11/11/14    mk      Clean up, modernize, use 0-1 color range.

% Use new-style color specifications in normalized range 0.0 - 1.0:
PsychDefaultSetup(2);

% Setup default mode to color vs. gray.
if nargin < 1
    mode = 1;
end

% Setup default aperture size to 2*200 x 2*200 pixels.
if nargin < 2
    ms=200;
end

basepath = [ PsychtoolboxRoot 'PsychDemos' filesep ];

% Use default demo images, if no special image was provided.
if nargin < 3
    myimgfile= [basepath 'konijntjes1024x768.jpg'];
end

myblurimgfile= [basepath 'konijntjes1024x768blur.jpg'];
mygrayimgfile= [basepath 'konijntjes1024x768gray.jpg'];

try
    fprintf('GazeContingentDemo (%s)\n', datestr(now));
    fprintf('Press a key or click on mouse to stop demo.\n');

    % Set background color to black aka zero intensity:
    backgroundcolor = 0.0;


    % Get the list of screens and choose the one with the highest screen number.
    screenNumber=max(Screen('Screens'));

    % Open a double buffered fullscreen window. Use PsychImaging(), so the
    % normalized 0.0 - 1.0 color format is used for drawing, instead of the
    % old 0 - 255 range:
    [w, wRect] = PsychImaging('OpenWindow', screenNumber, backgroundcolor);

    % Load image file:
    fprintf('Using image ''%s''\n', myimgfile);
    imdata=imread(myimgfile);
    imdatablur=imread(myblurimgfile);
    imdatagray=imread(mygrayimgfile);

    % Crop image if it is larger then screen size. There's no image scaling
    % in maketexture:
    [iy, ix, id]=size(imdata);
    [wW, wH]=WindowSize(w);

    if ix>wW || iy>wH
        disp('Image size exceeds screen size');
        disp('Image will be cropped');
    end

    if ix>wW
        cl=round((ix-wW)/2);
        cr=(ix-wW)-cl;
    else
        cl=0;
        cr=0;
    end
    if iy>wH
        ct=round((iy-wH)/2);
        cb=(iy-wH)-ct;
    else
        ct=0;
        cb=0;
    end

    % imdataXXX is the cropped version of the images.
    imdata=imdata(1+ct:iy-cb, 1+cl:ix-cr,:);
    imdatablur=imdatablur(1+ct:iy-cb, 1+cl:ix-cr,:);
    imdatagray=imdatagray(1+ct:iy-cb, 1+cl:ix-cr,:);

    % Compute image for foveated region and periphery:
    switch (mode)
        case 1
            % Mode 1:
            % Fovea contains original image data:
            foveaimdata = imdata;
            % Periphery contains grayscale-version:
            peripheryimdata = imdatagray;
        case 2
            % Fovea contains original image data:
            foveaimdata = imdata;
            % Periphery contains blurred-version:
            peripheryimdata = imdatablur;
        case 3
            % Fovea contains color-inverted image data:
            foveaimdata(:,:,:) = 255 - imdata(:,:,:);
            % Periphery contains original data:
            peripheryimdata = imdata;             
        case 4
            % Test-case: One shouldn't see any foveated region on the
            % screen - this is a basic correctness test for blending.
            foveaimdata = imdata;
            peripheryimdata = imdata;             
        otherwise
            % Unknown mode! We force abortion:
            fprintf('Invalid mode provided!');
            abortthisbeast
    end

    % Build texture for foveated region:
    foveatex=Screen('MakeTexture', w, foveaimdata);
    tRect=Screen('Rect', foveatex);

    % Build texture for peripheral (non-foveated) region:
    nonfoveatex=Screen('MakeTexture', w, peripheryimdata);
    [ctRect, dx, dy]=CenterRect(tRect, wRect);

    % We create a two layers Luminance + Alpha matrix for use as transparency
    % (or mixing weights) mask: Layer 1 (Luminance) is filled with luminance
    % value 1.0 aka white - the ones() function does this nicely for us, by
    % first filling both layers with 1.0:
    [x,y] = meshgrid(-ms:ms, -ms:ms);
    maskblob = ones(2*ms+1, 2*ms+1, 2);

    % Layer 2 (Transparency aka Alpha) is now filled/overwritten with a gaussian
    % transparency/mixing mask.
    xsd = ms / 2.2;
    ysd = ms / 2.2;
    maskblob(:,:,2) = 1 - exp(-((x / xsd).^2) - ((y / ysd).^2));

    % Build a single transparency mask texture:
    masktex = Screen('MakeTexture', w, maskblob);
    mRect = Screen('Rect', masktex);

    fprintf('Size image texture: %d x %d\n', RectWidth(tRect), RectHeight(tRect));
    fprintf('Size  mask texture: %d x %d\n', RectWidth(mRect), RectHeight(mRect));

    % Do initial flip to show blank screen:
    Screen('Flip', w);

    % The mouse-cursor position will define gaze-position (center of
    % fixation) to simulate (x,y) input from an eyetracker. Set cursor
    % initially to center of screen, but do hide it from view:
    [a,b] = RectCenter(wRect);
    SetMouse(a,b,screenNumber);
    HideCursor;
    buttons = 0;

    priorityLevel=MaxPriority(w);
    Priority(priorityLevel);

    % Wait until all keys on keyboard are released:
    KbReleaseWait;

    mxold=0;
    myold=0;

    % Show periphery image:
    Screen('DrawTexture', w, nonfoveatex);
    Screen('TextSize', w, 24);
    DrawFormattedText(w, 'Step1: Create Non-foveated (periphery) texture:\nPress mouse button to continue\n', 0, 0, 1, 50);
    Screen('Flip', w);

    % Wait for mouseclick:
    GetClicks;

    % Show fovea image:
    Screen('DrawTexture', w, foveatex);
    Screen('TextSize', w, 24);
    DrawFormattedText(w, 'Step2: Create Fovea texture:\nPress mouse button to continue\n', 0, 0, 1, 50);
    Screen('Flip', w);
    GetClicks;

    mode = 0;

    % Infinite display loop: Whenever "gaze position" changes, we update
    % the display accordingly. Loop aborts on keyboard press or mouse
    % click:
    while 1
        % Query current mouse cursor position (our "pseudo-eyetracker") -
        % (mx,my) is our gaze position.
        [mx, my, buttons]=GetMouse;

        if any(buttons)
            while any(buttons)
                [mx, my, buttons]=GetMouse; %(w);
            end

            mode = mode + 1;
            mxold = -1;

            if mode == 4
                break;
            end
        end

        % We only redraw if gazepos. has changed:
        if (mx~=mxold || my~=myold)
            % Compute position and size of source- and destinationrect and clip them:
            myrect=[mx-ms my-ms mx+ms+1 my+ms+1];
            dRect = ClipRect(myrect,ctRect);
            sRect=OffsetRect(dRect, -dx, -dy);

            % Valid destination rectangle?
            if ~IsEmptyRect(dRect)
                % Yes! Draw image for current frame:

                % Step 1: Draw the alpha-mask into the backbuffer. It
                % defines the aperture for foveation: The center of gaze
                % has zero alpha value. Alpha values increase with distance from
                % center of gaze according to a gaussian function and
                % approach 1.0 at the border of the aperture...
                if mode > 0
                    % Actual use of masktex to define transitions/mix:

                    % First clear framebuffer to backgroundcolor, not using
                    % alpha blending (== GL_ONE, GL_ZERO), enable all channels
                    % for writing [1 1 1 1], so everything gets cleared to good
                    % starting values:
                    Screen('BlendFunction', w, GL_ONE, GL_ZERO, [1 1 1 1]);
                    Screen('FillRect', w, backgroundcolor);

                    % Then keep alpha blending disabled and draw the mask
                    % texture, but *only* into the alpha channel. Don't touch
                    % the RGB color channels but use the channel mask
                    % [R G B A] = [0 0 0 1] to only enable the alpha-channel
                    % for drawing into it:
                    Screen('BlendFunction', w, GL_ONE, GL_ZERO, [0 0 0 1]);
                    Screen('DrawTexture', w, masktex, [], myrect);
                else
                    % Visualize the alpha/mask channel of the
                    % framebuffer and the new masktex itself to explain
                    % the concept - alpha values of 1 will show as white,
                    % values of zero as black, intermediates as gray levels:
                    Screen('BlendFunction', w, GL_SRC_ALPHA, GL_ZERO);
                    Screen('FillRect', w, 1);
                    Screen('DrawTexture', w, masktex, [], myrect);
                end

                % Step 2: Draw peripheral image. It is only/increasingly drawn where
                % the alpha-value in the backbuffer is 1.0 or close, leaving
                % the foveated area (low or zero alpha values) alone:
                % This is done by weighting each color value of each pixel
                % with the corresponding alpha-value in the backbuffer
                % (GL_DST_ALPHA). Disable alpha channel writes via [1 1 1 0], so
                % alpha mask stays untouched and only RGB color channels are
                % affected:
                if mode == 1 || mode == 3
                    Screen('BlendFunction', w, GL_DST_ALPHA, GL_ZERO, [1 1 1 0]);
                    Screen('DrawTexture', w, nonfoveatex, [], ctRect);
                end

                % Step 3: Draw foveated image, but only/increasingly where the
                % alpha-value in the backbuffer is zero or low: This is
                % done by weighting each color value with one minus the
                % corresponding alpha-value in the backbuffer
                % (GL_ONE_MINUS_DST_ALPHA).
                if mode == 2 || mode == 3
                    Screen('BlendFunction', w, GL_ONE_MINUS_DST_ALPHA, GL_ONE, [1 1 1 0]);
                    Screen('DrawTexture', w, foveatex, sRect, dRect);
                end

                % Draw some text with explanation of the different steps:
                switch(mode)
                    case 0,
                        txt = 'Draw gaussian aperture mask texture around center of fixation (aka mouse position):\nThis shows the alpha mask channel of the framebuffer used for mixing of the images (white = 1.0 alpha weight, black = 0.0 alpha weight)';
                    case 1,
                        txt = 'Draw periphery texture, but weight each incoming source color pixel by the alpha value stored in the framebuffers alpha channel';
                    case 2,
                        txt = 'Draw fovea texture, but weight each incoming source color pixel by 1 minus the alpha value stored in the framebuffers alpha channel';
                    case 3,
                        txt = 'Perform alpha weighted compositing (all previous steps together):\n1. Draw alpha weight mask according to mouse position,\n2. Overdraw with alpha-weighted periphery texture,\n3. Overdraw with 1-alpha weighted fovea texture.';
                end
                txt = [txt '\nMouse click for next step.'];
                DrawFormattedText(w, txt, 0, 0, [1 0 0], 60);

                % Show final result on screen. The 'Flip' also clears the drawing
                % surface back to black background color and a zero alpha value:
                Screen('Flip', w);
            end
        end

        % Keep track of last gaze position:
        mxold=mx;
        myold=my;

        % We wait 1 ms each loop-iteration so that we
        % don't overload the system in realtime-priority:
        WaitSecs('YieldSecs', 0.001);

        % Abort demo on keypress our mouse-click:
        if KbCheck || find(buttons)
            break;
        end
    end

    % Display full image a last time, just for fun...
    Screen('BlendFunction', w, GL_ONE, GL_ZERO);
    Screen('DrawTexture', w, foveatex);
    Screen('Flip', w);

    % The same command which closes onscreen and offscreen windows also
    % closes textures.
    Screen('CloseAll');
    ShowCursor;
    Priority(0);
    fprintf('End of GazeContingentDemo. Bye!\n\n');
    return;
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    ShowCursor;
    Priority(0);
    psychrethrow(psychlasterror);
end %try..catch..
