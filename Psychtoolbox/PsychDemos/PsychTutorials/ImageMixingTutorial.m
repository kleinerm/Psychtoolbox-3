function ImageMixingTutorial(mode, ms, myimgfile)
% ImageMixingTutorial([mode=1][, ms=200][, myimgfile])
%
% ImageMixingTutorial shows how to use a combination of alpha blending,
% offscreen windows and some basic image processing shaders to mix two
% images together, using a "mix weight mask" (aka alpha mask) which itself
% is dynamically updated via Screen() drawing commands like DrawTexture,
% DrawTexture with shaders, FillRect etc. This allows for interesting
% new gaze contingent displays or dynamically changing binocular rivalry
% stimuli.
%
% The basic working principle:
%
% 1. An offscreen window is created which stores the alpha blend mask
%    with per-pixel mixing weights. ("masktex" in the code).
%
% 2. The offscreen window stores the mix weights in its *luminance* channel,
%    (which is the same as the red channel for technical reasons). This way,
%    grayscale "luminance" values (luminance == red == green == blue) directly
%    encode "mixing weights". As we use a normalized 0-1 color range in this
%    demo ("PsychDefaultSetup(2)"), a grayscale value from 0 - 1 (aka from
%    black to white) directly corresponds to a mix weight from 0 - 1. This
%    allows us to use standard Screen() 2D drawing commands as usual to draw
%    a mix weight mask as a grayscale image into the offscreen window without
%    any deeper knowledge or thought about alpha blending. We can use all
%    drawing commands to quickly and dynamically update or redraw the grayscale
%    image in the offscreen window to create a dynamically changing mix weight
%    mask.
%
% 3. A shader is used to convert the grayscale image in the offscreen window
%    into a alpha mask and draw that alpha mask into the framebuffer of the
%    onscreen window, thereby setting the alpha channel of the onscreen window
%    to the desired mix weight mask for mixing the actual stimulus images.
%
% 4. Alpha blending is used to draw the two target stimulus images, mixing
%    them together according to the alpha channel created in step 3 from the
%    grayscale weight mask dynamically created in step 2.
%
% 5. The final mixed stimulus, e.g., a binocular rivalry stimulus, is shown
%    to the subject, rinse wash, repeat with step 2.
%
% This demo shows how to use normalized color ranges from 0 - 1 as a more
% natural representation of such alpha mix weights. It shows how to use the
% 'WeightedColorComponentSum' shader to both morph up to 4 masks together into
% one weight mask, and as an alternate use, how to move the content of the
% red channel of a window (== luminance/grayscale channel in a grayscale image)
% into the alpha channel, allowing to implement step 3 above. It also uses
% alpha blending in combination with a separate offscreen window in a non-usual
% way to allow to logically separate the process of creating/updating a mix weight
% mask from the process of actually applying that mask to a pair of stimulus images.
% This approach is not neccessary for simple gaze-contingent displays or rivalry
% stimuli (cfe. GazeContingentDemo / GazeContingentTutorial / BubbleDemo for simpler
% approaches). It is beneficial for stimuli which require complex mix masks, or
% complex dynamically updated mix masks, as it allows to implement an approach that
% reduces implementation complexity and is more natural or easier on the brain of
% the implementer of the stimulus, with less potential for coding errors or confusion
% about side effects of alpha blending.
%
% The tutorial allows you to switch between different stages of the processing
% involved in this approach and see their effects "live", by use of different
% keys on the keyboard, and to draw a dynamic mask via use of the mousecursor
% as a paint brush. It also shows some automatically running use of procedural
% shaders, texture animation and other Screen drawing primitives.
%
% This tutorial is powerful in its potential use cases, but requires significant
% customization for specific paradigms, and a good and careful reading of the code.
%
% For a much more simple demo and application of the technique, have a look at
% the SimpleImageMixingDemo.m, written and contributed by Natalia Zaretskaya.
% ___________________________________________________________________

% HISTORY
% 11-Nov-2014 mk   Written.

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

% Basepath to our own demo images:
basepath = [ PsychtoolboxRoot 'PsychDemos' filesep ];

% Use default demo images, if no special image was provided.
if nargin < 3
    myimgfile= [basepath 'konijntjes1024x768.jpg'];
end

myblurimgfile= [basepath 'konijntjes1024x768blur.jpg'];
mygrayimgfile= [basepath 'konijntjes1024x768gray.jpg'];

try
    % Set background color to black aka zero intensity:
    backgroundcolor = 0.0;

    % Get the list of screens and choose the one with the highest screen number.
    screenNumber=max(Screen('Screens'));

    % Open a double buffered fullscreen window. Use PsychImaging(), so the
    % normalized 0.0 - 1.0 color format is used for drawing, instead of the
    % old 0 - 255 range:
    [w, wRect] = PsychImaging('OpenWindow', screenNumber, backgroundcolor);

    % Open an offscreen window the same size as the onscreen window. We use
    % this to define the alpha/mixing weight channel used to later mix
    % two images together:
    masktex = Screen('OpenOffscreenWindow', w, [0 0 0 0]);
    DrawFormattedText(masktex, 'Draw something into the mask with the mouse!', 'center', 0, [1 1 1 1]);
    Screen('TextSize', masktex, 256);

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
            secondimdata = imdata;
            % Periphery contains grayscale-version:
            firstimdata = imdatagray;
        case 2
            % Fovea contains original image data:
            secondimdata = imdata;
            % Periphery contains blurred-version:
            firstimdata = imdatablur;
        case 3
            % Fovea contains color-inverted image data:
            secondimdata(:,:,:) = 255 - imdata(:,:,:);
            % Periphery contains original data:
            firstimdata = imdata;
        case 4
            % Test-case: One shouldn't see any foveated region on the
            % screen - this is a basic correctness test for blending.
            secondimdata = imdata;
            firstimdata = imdata;
        case 5
            secondimdata = imdata;
            firstimdata = imread([basepath 'PsychExampleExperiments/OldNewRecognition/stims/stim3.jpg']);
        otherwise
            % Unknown mode! We force abortion:
            fprintf('Invalid mode provided!');
            abortthisbeast
    end

    % Build texture for first image:
    firstImage=Screen('MakeTexture', w, firstimdata);

    % Build texture for second image:
    secondImage=Screen('MakeTexture', w, secondimdata);

    % We create a two layers Luminance + Alpha matrix for use as "gaussian" transparency
    % (or mixing weights) brush: Layer 1 (Luminance) is filled with luminance
    % value 1.0 aka white - the ones() function does this nicely for us, by
    % first filling both layers with 1.0:
    [x,y] = meshgrid(-ms:ms, -ms:ms);
    maskblob = ones(2*ms+1, 2*ms+1, 2);

    % Layer 2 (Transparency aka Alpha) is now filled/overwritten with a gaussian
    % transparency/mixing mask.
    xsd = ms / 2.2;
    ysd = ms / 2.2;
    maskblob(:,:,2) = exp(-((x / xsd).^2) - ((y / ysd).^2));

    % Copy alpha to luminance, just for visualization of the "mask brush" later on:
    maskblob(:,:,1) = maskblob(:,:,2);

    % Build a single transparency mask texture:
    gaussbrush = Screen('MakeTexture', w, maskblob);

    % Do initial flip to show blank screen:
    Screen('Flip', w);

    % The mouse-cursor position will define drawing-position. Set cursor
    % initially to center of screen, but do hide it from view:
    [a,b] = RectCenter(wRect);
    SetMouse(a,b,screenNumber);

    % Wait until all keys on keyboard are released:
    KbReleaseWait;

    % Show first image:
    Screen('DrawTexture', w, firstImage);
    Screen('TextSize', w, 24);
    DrawFormattedText(w, 'Step1: Create first texture:\nPress a key to continue\n', 0, 0, 1, 50);
    Screen('Flip', w);

    % Wait for mouseclick:
    KbStrokeWait;

    % Show second image:
    Screen('DrawTexture', w, secondImage);
    Screen('TextSize', w, 24);
    DrawFormattedText(w, 'Step2: Create second texture:\nPress a key to continue\n', 0, 0, 1, 50);
    Screen('Flip', w);
    KbStrokeWait;

    coverage = 0.25;
    mode = 0;
    brushtype = 0;
    startAngle = 0;
    imgRect = CenterRect(Screen('Rect', firstImage), wRect);
    cRect = OffsetRect([0 0 300 300], imgRect(RectLeft), imgRect(RectTop));

    % Build a procedural sine grating texture for a grating with a support of
    % 300 x 300 pixels and a RGBA color offset of 0.5:
    gratingtex = CreateProceduralSineGrating(w, 300, 300, [0.5 0.5 0.5 0.5]);
    gRect = OffsetRect([0 0 300 300], imgRect(RectLeft), imgRect(RectTop) + 300);
    phase = 0;

    % Create a shader that allows to combine the up to four input channels
    % of a texture into a weighted linear combination, using 'DrawTexture's
    % modulateColor parameter to specify the weights. This is used for
    % morphing between up to four alpha-masks, stored in the morphedAlphaTexture.
    minimorphshader = CreateSinglePassImageProcessingShader(w, 'WeightedColorComponentSum');
    
    % Create a texture with the alpha masks we want to morph between.
    %
    % We only create a one channel "luminance" texture which contains a gauss blob.
    % In the following use of this texture we'd like to "morph" between an all-zero layer,
    % an all-one layer and the "maskblob" gaussian shape layer. However, here we can optimize
    % a bit: We don't need an all-zero layer, because we get that implicitely, as any zero
    % value multiplied by any weight will always result in zero ( 0 * x == 0 for any x).
    % A "single layer" luminance texture will store our gaussian blob shape. Now any
    % "single layer luminance texture" automatically gets its alpha channel initialized to a
    % layer of all ones, ie., it implicitely carries around an alpha channel (the 4th channel)
    % which is filled with ones. In practice morphTex will have layers 1-3 (red, green, blue)
    % filled with the luminance values from morphTargets, and layer 4 filled
    % with 1's, so we can morph between the maskblob shape and a "constant one shape" simply by
    % drawing with the minimorphshader attached and modulateColor set to [w, 0, 0, 1-w] with w
    % moving between 0.0 and 1.0. We can morph between an "constant zero shape" and the morphTargets
    % shape by modulateColor set to [w, 0, 0, 0] with w moving between 0.0 and 1.0.
    % For this to work we need to use the minimorphshader during texture drawing. For convenience
    % we already attach the minimorphshader to the texture here, for later use:
    ysd = ms / 1.5;
    xsd = ysd;
    morphTargets = 100 * exp( -((x / xsd).^2) - ((y / ysd).^2) );
    morphTex = Screen('MakeTexture', w, morphTargets, [], [], 2, [], minimorphshader);
    mRect = []; %OffsetRect(Screen('Rect', morphTex), imgRect(RectLeft), imgRect(RectTop) + 600);

    blobtex = CreateProceduralGaussBlob(w, 300, 300, [], 1);

    ESCAPE = KbName('ESCAPE');
    SPACE = KbName('space');
    LeftArrow = KbName('LeftArrow');

    while 1
        % Query current mouse cursor position:
        [mx, my, buttons]=GetMouse;

        % Query keyboard:
        [pressed secs keycode] = KbCheck;
        if pressed
            KbReleaseWait;

            % ESC exits demo.
            if keycode(ESCAPE)
                break;
            end

            % Left Cursor key switches "drawing tool" for mask:
            if keycode(LeftArrow)
                brushtype = mod(brushtype + 1, 3);
            end

            % Space key switches display mode: mask, intermediate steps, final stim:
            if keycode(SPACE)
                mode = mod(mode + 1, 4);
            end
        end

        % --------------- Update / Draw into alpha "mixing" mask texture: -------------------

        % Compute position and size of destinationrect for gaussian brush
        % texture:
        dRect = CenterRectOnPoint(Screen('Rect', gaussbrush), mx, my);

        % Any buttons pressed for drawing/erasing?
        if any(buttons)
            % Yes! Draw into alpha mask image:

            % Which mouse button, if any?
            if any(buttons(2:end))
                % 2nd, 3rd, ... button: Erase by overpainting with zero alpha:
                cfactor = 0;
                Screen('Blendfunction', masktex, GL_ONE, GL_ZERO);
            else
                % First or none. Draw and accumulate with positive alpha:
                cfactor = 1;
                Screen('Blendfunction', masktex, GL_ONE, GL_ONE);
            end

            % Which drawing tool?
            switch brushtype
                case 0,
                  % Gaussian blob texture:
                  Screen('DrawTexture', masktex, gaussbrush, [], dRect, [], [], [], cfactor * [coverage coverage coverage coverage]);
                case 1,
                  % Oval:
                  Screen('FillOval', masktex, cfactor * [coverage coverage coverage coverage], CenterRectOnPoint([0 0 40 40], mx, my));
                case 2,
                  % Text can only be drawn, not erased, so do nothing in "erase mode":
                  if cfactor > 0
                      % Paint:
                      Screen('DrawText', masktex, 'Hello!', mx, my, [1 1 1 1]);
                  end
            end
        end

        % Some useless animations, just to show we can...
        Screen('Blendfunction', masktex, GL_ONE, GL_ZERO);

        Screen('FillRect', masktex, [0 0 0 0], cRect);
        startAngle = mod(startAngle + 2, 360);
        Screen('FillArc', masktex, [1 1 1 1], cRect, startAngle, 20);
        
        % Another useless animation - a procedural sine grating:
        amplitude = 0.5;
        freq = 5/360;
        angle = 0;
        Screen('DrawTexture', masktex, gratingtex, [], gRect, angle, [], [], [], [], [], [phase, freq, amplitude, 0]);
        
        % And another one - a mask morphing from all-zero to a gauss blob to all-one and back:
        if 0
            % From 0 -> Blob -> 1 -> Blob -> 0
            morphValue = sin(((phase / 10) - 90) / 360 * 2 * pi) + 1;
            if morphValue < 1
                weights = [morphValue, 0, 0, 0];
            else
                eweight = morphValue - 1;
                weights = [1 - eweight, 0, 0, eweight];
            end
            Screen('DrawTexture', masktex, morphTex, [], mRect, [], [], [], weights);
        else
            % From 0 -> Superblob -> 0
            morphValue = (sin(((phase / 10) - 90) / 360 * 2 * pi) + 1) / 2;
            weights = [morphValue, 0, 0, 0];
%            Screen('DrawTexture', masktex, morphTex, [], mRect, [], [], [], weights);
        end

        if 1
            % Use shader to draw blob with controllable amplitude and standard deviation:
            % Parameter vector [amplitude, stddev, aspect, 0]:
            morphValue = (sin(((phase / 10) - 90) / 360 * 2 * pi) + 1) / 2;
            Screen('DrawTexture', masktex, blobtex, [], [], [], [], [], [], [], kPsychDontDoRotation, [morphValue * 10, 100, 1.0, 0]);
            %Screen('DrawTexture', masktex, blobtex, [], [], [], [], [], [], [], kPsychDontDoRotation, [1, 200 * morphValue, 1.0, 0]);
        end
        
        phase = phase + 4;

        % --------------- Update actual stimulus, using the alpha "mixing" mask texture: -------------------

        % Step 1: Draw the alpha-mask into the backbuffer.
        if mode > 0
            % Actual use of masktex to define transition/mix:

            % First clear framebuffer to backgroundcolor, not using
            % alpha blending (== GL_ONE, GL_ZERO). Enable all channels
            % for writing [1 1 1 1], so everything gets cleared to good
            % starting values:
            Screen('BlendFunction', w, GL_ONE, GL_ZERO, [1 1 1 1]);
            Screen('FillRect', w, backgroundcolor);

            % Then keep alpha blending disabled and draw the mask
            % texture, but *only* into the alpha channel. Don't touch
            % the RGB color channels but use the channel mask via
            % [R G B A] = [0 0 0 1] to only enable the alpha-channel
            % for drawing into it. Use of modulateColor = [1 0 0 0] and
            % the minimorphshader causes the red channel to be copied into
            % the alpha channel. As red == luminance this means the grayscale
            % luminance value of masktext directly defines the final mask weights.
            Screen('BlendFunction', w, GL_ONE, GL_ZERO, [0 0 0 1]);
            Screen('DrawTexture', w, masktex, [], [], [], [], [], [1 0 0 0], minimorphshader);
        else
            % Visualize the alpha/mask masktex itself to explain
            % the concept - alpha values of 1 will show as white,
            % values of zero as black, intermediates as gray levels:
            Screen('BlendFunction', w, GL_ONE, GL_ZERO, [1 1 1 1]);
            Screen('DrawTexture', w, masktex, [], [], [], [], [], [1 0 0 0], minimorphshader);
        end

        % Step 2: Draw first image. It is only/increasingly drawn where
        % the alpha-value in the backbuffer is 1.0 or close, leaving
        % the foveated area (low or zero alpha values) alone:
        % This is done by weighting each color value of each pixel
        % with the corresponding alpha-value in the backbuffer
        % (GL_DST_ALPHA). Disable alpha channel writes via [1 1 1 0], so
        % alpha mask stays untouched and only RGB color channels are
        % affected:
        if mode == 1 || mode == 3
            Screen('BlendFunction', w, GL_DST_ALPHA, GL_ZERO, [1 1 1 0]);
            Screen('DrawTexture', w, firstImage);
        end

        % Step 3: Draw second image, but only/increasingly where the
        % alpha-value in the backbuffer is zero or low: This is
        % done by weighting each color value with one minus the
        % corresponding alpha-value in the backbuffer
        % (GL_ONE_MINUS_DST_ALPHA).
        if mode == 2 || mode == 3
            Screen('BlendFunction', w, GL_ONE_MINUS_DST_ALPHA, GL_ONE, [1 1 1 0]);
            Screen('DrawTexture', w, secondImage);
        end

        % Draw some text with explanation of the different steps:
        switch(mode)
            case 0,
                txt = 'Step3: Draw into alpha mask texture around mouse position:\nThis shows the alpha mask texture used for mixing of the images (white = 1.0 alpha weight, black = 0.0 alpha weight)';
            case 1,
                txt = 'Step4: Draw first texture, but weight each incoming source color pixel by the alpha value stored in the framebuffers alpha channel';
            case 2,
                txt = 'Step5: Draw second texture, but weight each incoming source color pixel by 1 minus the alpha value stored in the framebuffers alpha channel';
            case 3,
                txt = 'Perform alpha weighted compositing (all previous steps together):\n1. Draw alpha weight mask according to mouse position,\n2. Overdraw with alpha-weighted first texture,\n3. Overdraw with 1-alpha weighted second texture.';
        end
        txt = [txt '\nPress the SPACE key to continue to next step.\nPress Cursor left key to switch drawing tool.\nPress ESCAPE to exit demo.\n'];
        txt = [txt 'Press left mouse button to draw mask, other mouse button to erase mask'];
        DrawFormattedText(w, txt, 0, 0, [1 0 0], 60);

        % Show final result on screen. The 'Flip' also clears the drawing
        % surface back to black background color and a zero alpha value:
        Screen('Flip', w);
    end

    % Display full image a last time, just for fun...
    Screen('BlendFunction', w, GL_ONE, GL_ZERO);
    Screen('DrawTexture', w, secondImage);
    Screen('Flip', w);

    Screen('CloseAll');
    fprintf('End of ImageMixingTutorial. Bye!\n\n');
    return;
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    ShowCursor;
    Priority(0);
    psychrethrow(psychlasterror);
end %try..catch..
