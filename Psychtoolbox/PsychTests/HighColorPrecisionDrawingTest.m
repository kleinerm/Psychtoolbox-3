function HighColorPrecisionDrawingTest(testconfig, maxdepth, testblocks)
% HighColorPrecisionDrawingTest([testconfig][, maxdepth][, testblocks])
%
% Test for numeric drawing precision of your graphics card (GPU). Exercises
% a number of tests, where some 2D drawing primitive(s) is drawn in some
% well defined color, then the content of the framebuffer is read back and
% compared against expected results computed via Matlab code in double
% precision. The Matlab code emulates the expected behaviour of an ideal
% GPU that works at a precision higher than that of any real GPU.
% Difference between exptected and actual results is calculated for each
% tested pixel location and the maximum error is stored. Each test case
% plots that maximum error to the Matlab window. From the maximum error
% value, we also derive the maximum bitdepths of an output device for which
% the error would be negligible, ie., the difference would not show up in
% any measurable way because the deviation is too small to have any effect
% on the display device.
%
% The actual results of the tests depends on a number of conditions like
% tested precision range, selected precision of the framebuffer,
% alpha-blending mode (if any), texture filtering mode (if textures are
% used as drawing primitive), mode of operation of PTB, operating system
% etc. For that reason you can specify the exact test conditions via a
% number of arguments to this function -- Displayed test results will only
% be valid for that exact configuration, so if you want to evaluate
% suitability of your hardware+OS combo for a given type of visual
% stimulus, make sure you choose a test configuration to closely matches
% the one used in your stimulus presentation script.
%
% The test itself is currently in a BETA stage! While many of the test
% cases seem to work reliably on a variety of tested hardware, there may be
% (untested) hardware+OS combos for which the tests display effective
% precision numbers that are lower than the ones really achieved by your
% hardware -- the test is too pessimistic. So USE WITH CARE, DON'T TRUST
% THE TEST BLINDLY and APPLY COMMON SENSE when looking at the results.
%
% Optional parameters and their meaning:
%
% 'testconfig' is a vector that defines the framebuffer and PTB
% configuration to use. All elements have defaults:
%
% Colorclamping (aka high-precision vertex colors vs. standard vertex colors)
% -1 / 0 / 1 = Unclamped high-res via shader / Unclamped high-res / Clamped.
%              Default is 0 == Let PTB auto-select opmode with highest
%              precision. A setting of -1 overrides PTB's choice to always
%              use an internal implementation -- If auto-detection works
%              perfectly this should not give better results than mode 0,
%              but no automatic is perfect, so it doesn't hurt to test that
%              mode. 1 is "low-precision, clamped" mode: It shouldn't ever
%              give better results than -1 or 0 and is normally only used
%              for standard 8 bit precision output of standard stimuli
%              where bit-accurate output doesnt' matter too much.
%
% Framebuffer: (aka bit-depth and format of framebuffer)
% 0 / 1 / 2 / 3/ 4 = 8 bpc fixed, 16bpc float, 32bpc float, 32bpc float
%              if possible while alpha-blending enabled 16bpc otherwise,
%              16bpc fixed point (on ATI hardware only).
%
%              Precision with which the framebuffer operates: 8 bpc fixed
%              is a standard 8 bits per precision 256 levels framebuffer.
%              16bpc float allows for an effective 10-11 bit precision,
%              32bpc float allows for an effective 23 bit precision, 16 bpc
%              fixed is only supported on ATI hardware and allows for an
%              effective 16 bit precision, but without alpha-blending
%              support.
%
%              Selected precision is a accuracy vs. speed & functionality
%              tradeoff. Higher resolution means higher output precision
%              and higher precision for calculation of intermediate
%              results. However it means also more memory usage, slower
%              processing and drawing speed and - on some hardware - it
%              means that alpha-blending and anti-aliasing doesn't work or
%              works only very slowly. Therefore you need to select a mode
%              that is good enough for your purpose. Direct3D 10 compliant
%              hardware from NVidia and ATI (Geforce 8000 and later, Radeon
%              HD 2000 and later) is supposed to have no relevant
%              limitations wrt. to functionality or precision anymore -- It
%              can carry out all operations including alpha-blending etc.
%              at highest precision (32 bpc float). If you happen to have
%              such hardware then the only reason to choose less than
%              maximum precision is speed -- Lower precision is still
%              processed faster.
%
%              Please also note that the attainable precision of all the
%              test cases in this script is of course limited by the
%              precision of the framebuffer. E.g., if you chose a 16bpc
%              float framebuffer, none of the tests will be able to attain
%              more than about 10-11 bits of precision.
%
%
% Textures: (aka texture precision)
% 0 / 1 / 2 = 8 bpc fixed, 16bpc float, 32bpc float.
%
%              Precision with which textures are represented -- and
%              ultimately drawn. Same explanations apply as with
%              'Framebuffer'. However, there is no limitation in
%              functionality associated with high texture precision: Should
%              the hardware have some limitations, PTB will work-around
%              them. Higher precision textures still incur higher storage
%              requirements and lower drawing speeds though, so don't use
%              higher precision than you really need!
%
% Samplers: (aka texture sampling method)
% 0 / 1 / 2 = Hardware / PTB-Auto / PTB-Shaders.
%
%              Method employed for texture drawing: PTB-Auto is the
%              preferred choice -- Let PTB auto-select best method for
%              given texture precision and other requirements. However you
%              can manually override to always use PTB-Shaders (built-in
%              workarounds for less capable hardware) or Hardware
%              implementation of your GPU -- usually faster, but maybe less
%              precise.
%
% Filters: (aka texture filtering method)
% 0 / 1     = Nearest-Neighbour / Bilinear filtering.
%
%             Method of filtering texture pixels before drawing:
%             Nearest-Neighbour just uses pixels as they are -- blocky or
%             aliased appearance if you draw rotated textures, textures
%             where the 'srcRect' and 'dstRect' parameters in
%             Screen('DrawTexture') don't exactly match in size, and no way
%             of "scrolling" or positioning textures with subpixel
%             accuracy. However, also no loss in precision due to filtering
%             artifacts caused by low precision filtering hardware.
%             'Bilinear filtering' always provides perfectly anti-aliased
%             and smooth looking textures due to use of bilinear filtering,
%             but may introduce a slight loss of precision if your hardware
%             doesn't sample accurately. See DriftTexturePrecisionTest for
%             an extra test of filter accuracy of your GPU.
%
%
% 'maxdepth' parameter: Choose the maximum precision for which the
% test-cases test your hardware. Defaults to 16 bits and is restricted to
% about 18 bits by the current implementation of this test script. 16 bits
% is chosen because there aren't any display devices with more than 14 bit
% output precision available on the market, so 16 bits is "good enough" for
% most purposes. Plese note that even if your GPU is able to provide much
% more than 'maxdepth' bits of precision, the test won't detect that -- it
% will only test up to 'maxdepth' bits of precision!
%
%
% 'testblocks' parameter: A vector of tests to carry out. By default all
% tests are carried out and each single test is interruptible by holding
% down the left mouse button for a while. However this may take quite long
% -- dozens of minutes, maybe even over an hour! For that reason you can
% specify your own 'testblocks' vector to only run a subset of all test
% cases and save some time.
%
% The following test cases are currently implemented:
%
% 1  = Test precision of clearing of the framebuffer to selected
%      'clearcolor'. 'clearcolor' is set in Screen('OpenWindow') or
%      PsychImaging('OpenWindow') etc. or via Screen('FillRect', window,
%      clearcolor). Framebuffer clearing is performed after each
%      Screen('Flip') or Screen('FillRect', window, clearcolor) command and
%      this test tests precision of that operation.
%
% 2  = Test precision of Screen 2D drawing commands like FillRect,
%      FrameRect, FillOval, FrameOval, DrawLine etc.
%
% 3  = Test precision of Screen 2D batch-drawing commands, ie. commands
%      that allow to draw multiple primitives per command, e.g., DrawDots,
%      DrawLines and the batch versions of FillRect, FrameRect, FillOval
%      etc.
%      Also tests precision of the 'DrawTexture' command and of the
%      built-in gamma correction mechanism of PTB when used with the PTB
%      imaging pipeline, e.g., in Mono++ or Color++ mode of a CRS Bits++
%      box.
%
% 4  = Test precision of texture drawing commands when the special
%      'globalAlpha' or 'modulateColor' arguments are used to modulate the
%      textures pixel values during drawing -- for example for contrast
%      selection.
%
% 5  = Test of precision of alpha-blending: Does use of the alpha-blending
%      function via Screen('BlendFunction') introduce any loss of numeric
%      stimulus precision - and if so, how much?
%
%      This testcase 5 is incomplete and under development!
%
%

% History:
% 04/20/08  Written (MK).
% 10/22/10  Refined to account for small differences between GPU's (MK).

global win;

close all;

% Octave's new plotting backend 'fltk' interferes with Screen(),
% due to internal use of OpenGL. Problem is it changes the
% bound OpenGL rendering context behind our back and we
% don't protect ourselves against this yet. Switch plotting backend
% to good'ol gnuplot to work around this issue until we fix it properly
% inside Screen():
if IsOctave && exist('graphics_toolkit')
    graphics_toolkit ('gnuplot');
end

if nargin < 1 || isempty(testconfig)
    % Empty 'testconfig' or missing: Do all tests.
    testconfig = [0 2 2 1 1]
end

if length(testconfig) ~=5
    error('testconfig vector must have 5 elements!');
end

if nargin < 2 || isempty(maxdepth)
    maxdepth = 16;
end

if maxdepth > 24
    maxdepth = 24;
    fprintf('Max bit depths value clamped to 24 bits -- More bits are not meaningful on current hardware...\n');
end

if nargin < 3 || isempty(testblocks)
    testblocks = [1, 2, 3, 4, 5];
end
fprintf('Executing the following tests: %i.\n', testblocks);

screenid = max(Screen('Screens'));

ColorClamping = testconfig(1)
Framebuffer = testconfig(2)
Textures = testconfig(3)
Samplers = testconfig(4)
Filters = testconfig(5)

if Framebuffer == 0
    fbdef = 'UseVirtualFramebuffer';
end

if Framebuffer == 1
    fbdef = 'FloatingPoint16Bit';
end

if Framebuffer == 2
    fbdef = 'FloatingPoint32Bit';
end

if Framebuffer == 3
    fbdef = 'FloatingPoint32BitIfPossible';
end

if Framebuffer == 4
    fbdef = 'FixedPoint16Bit';
end

fprintf('Selected framebuffer mode: %s\n', fbdef);
resstring = '';

% Disable sync tests for this script:
oldsync = Screen('Preference', 'SkipSyncTests', 2);
Screen('Preference', 'Verbosity', 4);

% Generate testvector of all color values to test. We test the full
% intensity range from 0.0 (black) to 1.0 (white) divided into 2^maxdepth steps
% -- the finest reasonable number of steps for any display device.
% We start with value 1 and decrement to 0, because the largest errors are
% expected at high values (due to characteristics of IEEE floating point
% format), so if user exits test early, he will get useable results:
testcolors = 1 - linspace(0,1,2^maxdepth);

if ismember(1, testblocks)
    PsychImaging('PrepareConfiguration')
    PsychImaging('AddTask', 'General', fbdef);

    % Open window with black background color:
    [win rect] = PsychImaging('OpenWindow', screenid, 0, [0 0 25 25], [], [], [], []);

    % Set color clamping (and precision) for standard 2D draw commands:
    Screen('ColorRange', win, 1, ColorClamping);

    Screen('Flip', win);

    drawncolors = zeros(1, length(testcolors));
    i=1;

    % Test 1: Precision of backbuffer clears during Screen('Flip'):
    for tc = testcolors
        [xm ym buttons] = GetMouse;
        if buttons(1)
            break;
        end

        % Set clear color: Incidentally this also fills the backbuffer with the
        % cleared color:
        Screen('FillRect', win, tc);

        % Overdraw a smallish 20 x 20 patch in top-left corner with something
        % else, just to make sure the 'FillRect' above doesn't bleed through:
        Screen('FillRect', win, rand, [0 0 20 20]);

        % Flip the buffers - this will fill the backbuffer with 'clear' color
        % after flip: We don't sync to retrace to speed things up a bit...
        Screen('Flip', win, 0, 0, 2);

        % Readback drawbuffer, top-left 10x10 area, with float precision, only
        % the red/luminance channel:
        patch = Screen('GetImage', win, [0 0 10 10], 'drawBuffer', 1, 1);

        % Store result:
        drawncolors(i) = patch(5,5);
        i=i+1;

        if mod(i, 1000)==0
            fprintf('At %i th testvalue of %i...\n', i, 2^maxdepth);
            beep; drawnow;
        end
    end

    Screen('CloseAll');

    % Test done.
    deltacolors = single(testcolors(1:i-1)) - drawncolors(1:i-1);
    minv = min(abs(deltacolors));
    maxv = max(abs(deltacolors));
    goodbits = floor(-(log2(maxv))) - 1;
    if goodbits < 0
        goodbits = 0;
    end

    if goodbits <= maxdepth
        resstring = [resstring sprintf('Clearbuffer test: Mindiff.: %1.17f, Maxdiff.: %1.17f --> Accurate to %i bits out of max tested bitdepths %i.\n', minv, maxv, goodbits, maxdepth)];
    else
        resstring = [resstring sprintf('Clearbuffer test: Mindiff.: %1.17f, Maxdiff.: %1.17f --> Accurate to full tested bitdepth range of %i bits.\n', minv, maxv, maxdepth)];
    end

    plot(deltacolors);
    drawnow;
    
end % Of Test 1.

if ismember(2, testblocks)
    % Test 2: Precision of non-batched Screen 2D drawing commands.
    % This tests how well assignment and interpolation of vertex
    % colors across primitives works - or how well the generic 'varying'
    % interpolators work in case that our own shader based solution is active:
    PsychImaging('PrepareConfiguration')
    PsychImaging('AddTask', 'General', fbdef);

    % Open window with black background color:
    [win rect] = PsychImaging('OpenWindow', screenid, 0, [0 0 250 250], [], [], [], []);

    % Set color clamping (and precision) for standard 2D draw commands:
    Screen('ColorRange', win, 1, ColorClamping);

    Screen('Flip', win);

    drawncolors = zeros(10, length(testcolors));
    i=1;

    for tc = testcolors
        [xm ym buttons] = GetMouse;
        if buttons(1)
            break;
        end

        % FillRect test: Top-left 10x10 patch:
        Screen('FillRect', win, tc, [0 0 10 10]);

        % FrameRect test:
        Screen('FrameRect', win, tc, [0 0 15 15], 2);

        % FillOval test:
        Screen('FillOval', win, tc, [20 0 30 10]);

        % FrameOval test:
        Screen('FrameOval', win, tc, [20 20 35 41], 4);
        
        % FillArc test: Don't need extra tests for FrameArc or DrawArc as internal codepath
        % in Screen is nearly identical...
        Screen('FillArc', win, tc, [40 0 55 21], 0, 320);
        
        % DrawLine test:
        Screen('DrawLine', win, tc, 0, 24, 10, 24, 2);

        % FramePoly test:
        Screen('FramePoly', win, tc, [0 28 ; 5 28 ; 10 28], 2);
        
        % FillPoly test:
        Screen('FillPoly', win, tc, [0 32 ; 10 32 ; 5 35 ]);
        
        % glPoint test: 
        Screen('glPoint', win, tc, 58, 5, 3);
        
        % gluDisk test:
        Screen('gluDisk', win, tc, 58, 15, 3);
        
        % Flip the buffers - We don't sync to retrace to speed things up a
        % bit. We also don't clear the drawbuffer, as we're overwriting it in
        % next loop iteration at the same location anyway -- saves some time.
        % N.B.: Technically we don't need to flip at all, as we're reading from
        % the 'drawBuffer' anyway which is unaffected by flips.
        if mod (i, 1000) == 0
            % Ok, only do it every 1000th trial to visualize...
            Screen('Flip', win, 0, 2, 2);
        end
        
        % Readback drawbuffer with float precision, only
        % the red/luminance channel:
        patch = Screen('GetImage', win, [0 0 60 40], 'drawBuffer', 1, 1);

        % Store result: FillRect
        drawncolors(1,i) = patch(5,5);

        % Store result: FrameRect
        drawncolors(2,i) = patch(15,10);

        % Store result: FillOval
        drawncolors(3,i) = patch(5,25);

        % Store result: FrameOval
        drawncolors(4,i) = patch(30, 35-1);

        % Store result: FillArc
        drawncolors(5,i) = patch(10, 50-1);

        % DrawLine:
        drawncolors(6,i) = patch(24, 5);
        
        % FramePoly:
        drawncolors(7,i) = patch(28, 5);

        % FillPoly:
        drawncolors(8,i) = patch(33, 5);

        % glPoint:
        drawncolors(9,i) = patch(5, 58);

        % gluDisk:
        drawncolors(10,i) = patch(15, 58);

        if mod(i, 1000)==0
            fprintf('At %i th testvalue of %i...\n', i, 2^maxdepth);
            beep; drawnow;
        end

        i=i+1;
    end

    Screen('CloseAll');

    % Test done.
    primname = {'FillRect', 'FrameRect', 'FillOval', 'FrameOval', 'FillArc', 'DrawLine', 'FramePoly', 'FillPoly', 'glPoint', 'gluDisk'};

    for j=1:size(drawncolors, 1)
        deltacolors = single(testcolors(1:i-1)) - drawncolors(j, 1:i-1);
        minv = min(abs(deltacolors));
        maxv = max(abs(deltacolors));
        goodbits = floor(-(log2(maxv))) - 1;
        if goodbits < 0
            goodbits = 0;
        end

        testname = char(primname{j});

        if goodbits <= maxdepth
            resstring = [resstring sprintf('2D drawing test: %s : Mindiff.: %1.17f, Maxdiff.: %1.17f --> Accurate to %i bits out of max tested bitdepths %i.\n', testname, minv, maxv, goodbits, maxdepth)];
        else
            resstring = [resstring sprintf('2D drawing test: %s : Mindiff.: %1.17f, Maxdiff.: %1.17f --> Accurate to full tested bitdepth range of %i bits.\n', testname, minv, maxv, maxdepth)];
        end

        plot(deltacolors);
        drawnow;
    end
end % Test 2.

if ismember(3, testblocks)
    % Test 3: Precision of Screen 2D batch drawing commands.
    % This tests how well assignment and interpolation of vertex
    % colors across primitives works - or how well the generic 'varying'
    % interpolators work in case that our own shader based solution is
    % active. Batch drawing commands use a different code-path inside
    % Screen for efficient submission of many primitives and colors values,
    % that's why we need to test them extra.
    PsychImaging('PrepareConfiguration')
    PsychImaging('AddTask', 'General', fbdef);

    % Open window with black background color:
    [win rect] = PsychImaging('OpenWindow', screenid, 0, [0 0 514 514], [], [], [], []);

    % Set color clamping (and precision) for standard 2D draw commands:
    Screen('ColorRange', win, 1, ColorClamping);

    Screen('Flip', win);

    % Convert testcolors to matrix of RGBA quadruples -- Most batch drawing
    % commands only accept RGB or RGBA, not pure Luminance:
    rgbacolors = [ repmat(testcolors, 3, 1) ; ones(1, length(testcolors)) ];

    % Compute corresponding 'xy' matrix of output positions:
    [outx , outy] = meshgrid(0:2^floor(maxdepth/2)-1, 0:2^floor(maxdepth/2)-1);
    
    % Build 2-row matrix of (x,y) pixel positions:
    xy = [outx(:)' ; outy(:)'];
        
    % Compute reference patch:
    refpatch = reshape(rgbacolors(1,:), length(outy), length(outx));

    % Readout region of framebuffer:
    fbrect = [0, 0, max(xy(1,:))+1, max(xy(2,:))+1];

    % DrawDots test: All pixels in a rectangular block in top-left corner,
    % each with a different color:
    dyOffset = 0;
    Screen('DrawDots', win, xy, 1, rgbacolors, [0,dyOffset], 0);
    testname = 'DrawDots';

    % Evaluate and log:
    [resstring, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect);
    if goodbits == 0
        fprintf ('DrawDots result is nonsense. Retrying with a slight twist...\n');
        % resstring = resstring(1:findstr(resstring, 'DrawDots')-1);
        resstring = '';
        dyOffset = 1;
        Screen('DrawDots', win, xy, 1, rgbacolors, [0,dyOffset], 0);
        testname = 'DrawDots';

        % Evaluate and log:
        [resstring, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect);
    end
    
    % Visualize and clear buffer back to zero aka black:
    Screen('Flip', win, 0, 0, 2);
    
    % DrawLines test: All pixels in a rectangular block in top-left corner,
    % each with a different color:

    % Need to replicate each column in 'xy', as consecutive columns define
    % a pair of start- and endpoints for a single line-segment. Actually,
    % we need to add a horizontal x offset of 1 to the end-point, as using
    % exactly the same start- and endpoint would create a line of zero
    % length -- ie. no line at all.
    lxy(:, 1:2:(2*size(xy,2))-1) = xy;    
    lxy(:, 2:2:(2*size(xy,2))-0) = xy + repmat([1;0], 1, size(xy, 2));

    % Same replication for color values:
    cxy(:, 1:2:(2*size(xy,2))-1) = rgbacolors;    
    cxy(:, 2:2:(2*size(xy,2))-0) = rgbacolors;
    
    % We draw the lines without line-smoothing (=0), as it only works with
    % alpha-blending and we do not want to use alpha-blending in this test
    % block:
    Screen('DrawLines', win, lxy, 1, cxy, [], 0);
    testname = 'DrawLines';

    % Evaluate and log:
    [resstring2, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect);
    if goodbits == 0
        fprintf ('DrawLines result is nonsense. Retrying with a slight twist...\n');
        Screen('DrawLines', win, lxy, 1, cxy, [0,1], 0);
        testname = 'DrawLines';

        % Evaluate and log:
        [resstring2, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect);
    end
    resstring = resstring2;
    
    % Visualize and clear buffer back to zero aka black:
    Screen('Flip', win, 0, 0, 2);

    % FillRect test:
    fxy = [xy ; xy + repmat([1;1], 1, size(xy, 2))];
    Screen('FillRect', win, rgbacolors, fxy);
    testname = 'FillRect';

    % Evaluate and log:
    [resstring, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect);

    % Visualize and clear buffer back to zero aka black:
    Screen('Flip', win, 0, 0, 2);

    % FrameRect test:
    fxy = [xy ; xy + repmat([1;1], 1, size(xy, 2))];
    Screen('FrameRect', win, rgbacolors, fxy);
    testname = 'FrameRect';

    % Evaluate and log:
    [resstring, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect);

    % Visualize and clear buffer back to zero aka black:
    Screen('Flip', win, 0, 0, 2);
    
    % FillOval test:
    foxy = [xy ; xy + repmat([2;2], 1, size(xy, 2))];
    Screen('FillOval', win, rgbacolors, foxy);
    testname = 'FillOval';

    % Evaluate and log:
    [resstring, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect);

    % Visualize and clear buffer back to zero aka black:
    Screen('Flip', win, 0, 0, 2);
        
    % DrawTexture test: Need only 1 texture draw to test all values
    % simultaneously:
    foxy = [xy ; xy + repmat([2;2], 1, size(xy, 2))];
    teximg = refpatch;
    if Textures <=0
        % Integer texture instead of float texture: Now expected range of
        % values is 0-255 instead of 0.0 - 1.0. Need to rescale:
        teximg = uint8(refpatch * 255);
    end
    
    tex = Screen('MakeTexture', win, teximg, [], [], Textures);
    Screen('DrawTexture', win, tex, [], fbrect, [], Filters);
    Screen('Close', tex);
    testname = 'DrawTexture';

    % Evaluate and log:
    [resstring, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect);

    % Visualize and clear buffer back to zero aka black:
    Screen('Flip', win, 0, 0, 2);
    
    % Test of gamma correction shader: This is the
    % 'SimpleGamma' shader used by the imaging
    % pipeline, setup by PsychColorCorrection():
    gamma = 1/2.374238462047320;
    gammaShader = LoadGLSLProgramFromFiles({'GammaCorrectionShader.frag.txt' , 'ICMSimpleGammaCorrectionShader.frag.txt'});
    glUseProgram(gammaShader);
    glUniform3f(glGetUniformLocation(gammaShader, 'ICMEncodingGamma'), gamma, gamma, gamma);
    % Default min and max luminance is 0.0 to 1.0, therefore reciprocal 1/range is also 1.0:
    glUniform3f(glGetUniformLocation(gammaShader, 'ICMMinInLuminance'), 0.0, 0.0, 0.0);
    glUniform3f(glGetUniformLocation(gammaShader, 'ICMMaxInLuminance'), 1.0, 1.0, 1.0);
    glUniform3f(glGetUniformLocation(gammaShader, 'ICMReciprocalLuminanceRange'), 1.0, 1.0, 1.0);
    % Default gain to postmultiply is 1.0:
    glUniform3f(glGetUniformLocation(gammaShader, 'ICMOutputGain'), 1.0, 1.0, 1.0);
    % Default bias to is 0.0:
    glUniform3f(glGetUniformLocation(gammaShader, 'ICMOutputBias'), 0.0, 0.0, 0.0);

    glUniform2f(glGetUniformLocation(gammaShader, 'ICMClampToColorRange'), 0.0, 1.0);
    glUseProgram(0);
    
    teximg = refpatch;
    if Textures <=0
        % Integer texture instead of float texture: Now expected range of
        % values is 0-255 instead of 0.0 - 1.0. Need to rescale:
        teximg = uint8(refpatch * 255);
    end
    
    tex = Screen('MakeTexture', win, teximg, [], [], Textures);
    Screen('DrawTexture', win, tex, [], fbrect, [], Filters, [], [], gammaShader);
    Screen('Close', tex);
    testname = 'GammaCorrection';

    % Evaluate and log:
    gammapatch = refpatch .^ gamma;
    [resstring, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, gammapatch, fbrect);

    % Visualize and clear buffer back to zero aka black:
    Screen('Flip', win, 0, 0, 2);
    
    Screen('CloseAll');

end % Test 3.

if ismember(4, testblocks)
    % Test 4: Precision of texture drawing commands.
    PsychImaging('PrepareConfiguration')
    PsychImaging('AddTask', 'General', fbdef);

    % Open window with black background color:
    [win rect] = PsychImaging('OpenWindow', screenid, 0, [0 0 514 514], [], [], [], []);

    % Set color clamping (and precision) for standard 2D draw commands. In
    % our case here, this affects the precision and clamping of the
    % 'modulateColor' and 'globalAlpha' parameters of 'DrawTexture'
    % commands:
    Screen('ColorRange', win, 1, ColorClamping);

    Screen('Flip', win);

    % Convert testcolors to matrix of RGBA quadruples -- Most batch drawing
    % commands only accept RGB or RGBA, not pure Luminance:
    rgbacolors = [ repmat(testcolors, 3, 1) ; ones(1, length(testcolors)) ];

    % Compute corresponding 'xy' matrix of output positions:
    [outx , outy] = meshgrid(0:2^floor(maxdepth/2)-1, 0:2^floor(maxdepth/2)-1);
    
    % Build 2-row matrix of (x,y) pixel positions:
    xy = [outx(:)' ; outy(:)'];
        
    % Compute color patch:
    colpatch = reshape(rgbacolors(1,:), length(outy), length(outx));
    colpatch = [colpatch , -colpatch];
    
    
    % Readout region of framebuffer:
    fbrect = [0, 0, max(xy(1,:))+1, max(xy(2,:))+1];

    % DrawTexture test: Need only 1 texture draw to test all values inside
    % the texture simultaneously:
    foxy = [xy ; xy + repmat([2;2], 1, size(xy, 2))];
    teximg = colpatch;
    if Textures <=0
        % Integer texture instead of float texture: Now expected range of
        % values is 0-255 instead of 0.0 - 1.0. Need to rescale:
        teximg = uint8(colpatch * 255);
    end
    
    tex = Screen('MakeTexture', win, teximg, [], [], Textures);
    fbrect = Screen('Rect', tex)

    % Now we test modulation of drawn texture pixels with the
    % 'modulateColor' argument:
    
    i=0;
    mingoodbits = inf;
    % Step through range 1 down to zero, in 1/1000th decrements:
    for mc = 1.0:-0.001:-1.0
        i=i+1;

        if mod(i, 100)==0
            beep; drawnow;
            [blah bloh buttons] = GetMouse(win);
            if any(buttons)
                break;
            end
        end

        % DrawTexture, modulateColor == [mc mc mc 1] modulated:
        Screen('DrawTexture', win, tex, [], fbrect, [], Filters, [], mc);
        
        % While the GPU does its thing, we compute the Matlab reference
        % patch:
        refpatch = colpatch * mc;
        
        testname = 'DrawTexture-modulateColor';

        % Evaluate and log:
        [dummy, minv, maxv(i), goodbits] = comparePatches([], testname, maxdepth, refpatch, fbrect);
        mingoodbits = min([mingoodbits, goodbits]);
        
        % Visualize and clear buffer back to zero aka black:
        Screen('Flip', win, 0, 0, 2);        
    end
    
    resstring = [resstring sprintf('%s : Maxdiff.: %1.17f --> Accurate to at least %i bits.\n', testname, max(maxv), mingoodbits)];
    
    Screen('Close', tex);
        
    Screen('CloseAll');
end % Test 4.

if ismember(5, testblocks)
    % Test 5: Precision of alpha blending commands.
    PsychImaging('PrepareConfiguration')
    PsychImaging('AddTask', 'General', fbdef);

    % Open window with black background color:
    [win rect] = PsychImaging('OpenWindow', screenid, [0 0 0 0], [0 0 514 514], [], [], [], []);

    % Set color clamping (and precision) for standard 2D draw commands. In
    % our case here, this affects the precision and clamping of the
    % 'modulateColor' and 'globalAlpha' parameters of 'DrawTexture'
    % commands:
    Screen('ColorRange', win, 1, ColorClamping);

    Screen('Flip', win);

    % Enable alpha blending in additive mode: Let's see how precise
    % addition is carried out:
    Screen('Blendfunction', win, GL_ONE, GL_ONE);
    
    % Convert testcolors to matrix of RGBA quadruples -- Most batch drawing
    % commands only accept RGB or RGBA, not pure Luminance:
    rgbacolors = [ repmat(testcolors, 3, 1) ; ones(1, length(testcolors)) ];

    % Compute corresponding 'xy' matrix of output positions:
    [outx , outy] = meshgrid(0:2^floor(maxdepth/2)-1, 0:2^floor(maxdepth/2)-1);
    
    % Build 2-row matrix of (x,y) pixel positions:
    xy = [outx(:)' ; outy(:)'];
        
    % Compute color patch:
    colpatch = reshape(rgbacolors(1,:), length(outy), length(outx));
    colpatch = [colpatch , -colpatch];
    
    
    % Readout region of framebuffer:
    fbrect = [0, 0, max(xy(1,:))+1, max(xy(2,:))+1];

    % DrawTexture test: Need only 1 texture draw to test all values inside
    % the texture simultaneously:
    foxy = [xy ; xy + repmat([2;2], 1, size(xy, 2))];
    teximg = colpatch;
    if Textures <=0
        % Integer texture instead of float texture: Now expected range of
        % values is 0-255 instead of 0.0 - 1.0. Need to rescale:
        teximg = uint8(colpatch * 255);
    end
    
    tex = Screen('MakeTexture', win, teximg, [], [], Textures);
    fbrect = Screen('Rect', tex)

    % Now we test modulation of drawn texture pixels with the
    % 'modulateColor' argument:
    nroverdraws = 2;
    alpha=1;
    i=0;
    mingoodbits = inf;
    % Step through range 1 down to zero, in 1/1000th decrements:
    for mc = 1.0:-0.001:-1.0
        i=i+1;

        if mod(i, 100)==0
            beep; drawnow;
            [blah bloh buttons] = GetMouse(win);
            if any(buttons)
                break;
            end
        end

        % DrawTexture, modulateColor == [mc mc mc 1] modulated:
        for odc=1:nroverdraws
            Screen('DrawTexture', win, tex, [], fbrect, [], Filters, [], [mc mc mc alpha]);
        end
        
        % While the GPU does its thing, we compute the Matlab reference
        % patch:
        
        refpatch = zeros(size(colpatch));
        
        for odc=1:nroverdraws
            % MK: This is needed for ATI X1600 under Tiger to emulate the known color clamping bug: refpatch = max(0, min(1, (refpatch + colpatch * mc * alpha)));
            refpatch = (refpatch + colpatch * mc * alpha);
        end
        
        testname = 'DrawTexture-modulateColor&Blend1+1';

        % Evaluate and log:
        [dummy, minv, maxv(i), goodbits] = comparePatches([], testname, maxdepth, refpatch, fbrect);
        mingoodbits = min([mingoodbits, goodbits]);
        
        % Visualize and clear buffer back to zero aka black:
        Screen('Flip', win, 0, 0, 2);        
    end
    
    resstring = [resstring sprintf('%s : Maxdiff.: %1.17f --> Accurate to at least %i bits with %i overdraws.\n', testname, max(maxv), mingoodbits, nroverdraws)];
    
    Screen('Close', tex);
        
    Screen('CloseAll');
end % Test 5.



fprintf('\n\nTest summary:\n');
fprintf('-------------\n\n');
fprintf('%s\n\n', resstring);

% Restore synctest settings:
Screen('Preference', 'SkipSyncTests', oldsync);
Screen('Preference', 'Verbosity', 3);

end

function [resstring, minv, maxv, goodbits] = comparePatches(resstring, testname, maxdepth, refpatch, fbrect)
    global win;

    % Readback drawbuffer with float precision, only the red/luminance channel:
    patch = Screen('GetImage', win, fbrect, 'drawBuffer', 1, 1);
        
    deltacolors = single(refpatch) - single(patch);
    imagesc(deltacolors);

    minv = min(min(abs(deltacolors)));
    maxv = max(max(abs(deltacolors)));
    goodbits = floor(-(log2(maxv))) - 1;
    if goodbits < 0
        goodbits = 0;
    end

    if goodbits <= maxdepth
        resstring = [resstring sprintf('2D drawing test: %s : Mindiff.: %1.17f, Maxdiff.: %1.17f --> Accurate to %i bits out of max tested bitdepths %i.\n', testname, minv, maxv, goodbits, maxdepth)];
    else
        resstring = [resstring sprintf('2D drawing test: %s : Mindiff.: %1.17f, Maxdiff.: %1.17f --> Accurate to full tested bitdepth range of %i bits.\n', testname, minv, maxv, maxdepth)];
    end
end
