function HDRTest(dotest, meterType, highprecision, screenid, filename)
% HDRTest([dotest=all][, meterType=7][, highprecision=0][, screenid=max][, filename='hdrmeasurements.mat'])
%
% Perform some basic correctness tests and evaluation for HDR display operation,
% using a Colorimeter.
%
% Test if HDR-10 display mode delivers sufficiently correct and precise results
% on a suitable HDR capable display setup and monitor.
%
% This test script requires a supported Photometer or Colorimeter to be connected
% for measurement of luminance and chromaticity, ie. XYZ tristimulus color values.
%
% Supported devices are the ones listed in "help MeasXYZ".
%
% 'dotest' Vector which selects which tests to perform. Defaults to "all tests":
% dotest = [luminance, redprimary, greenprimary, blueprimary], e.g., [1 0 0 0]
% only do luminance test, [0 1 0 0] only do red primary test, [1 1 1 1] do all tests.
%
% 'meterType' id code of the measurement device to use, as listed in "help MeasXYZ".
% Defaults to meterType 7 for the CRS ColorCal2. If you specify a meterType
% 0 then the test script will run through as fast as possible without
% measuring anything - a dry run if you want.
%
% A meterType of -1 requests manual measurement and data entry. The script
% will display a test stimulus, make a beep noise, and expect you to use
% some external manually operated colorimeter to measure, then press a key.
% After keypress a text will ask you to enter the measured luminance in
% nits on the keyboard and press ENTER to finish data entry. Then it will
% advance to the next measurement. The current measurement run can be
% aborted by typing q instead of the nits value. Invalid entry will be
% stored as NaN for "Not a number".
%
% If you don't own a colorimeter supported by CMCheckInit(), but that is
% controllable by Matlab/Octave code, please let us know. You can still
% automate measurement yourself by specifying a meterType of 3 and then
% providing the following functions in your Matlab/Octave function search
% path: CRSColorInit() to initialize your colorimeter, CRSColorClose() to
% shut it down again, and XYZ = CRSColorMeasXYZ() to perform a measurement
% of XYZ tristimulus values. This "hijacks" the builtin support for the CRS
% color toolbox for convenient use of your own colorimeter.
%
% 'highprecision' If set to 1, will request a fp16 16 bpc non-linear
% floating point framebuffer for output, instead of the default setting 0,
% which requests a 10 bpc linear framebuffer. This may be able to raise
% precision of the encoded signal from 10 bit to up to 12 bit. Please note
% that if used on MS-Windows, the test will print false warnings about
% mismatches between shader computed PQ values. That is normal at the
% moment, because on Windows PQ is not used for fp16 mode.
% If set to 2, will request a 16 bpc fixed point framebuffer, which allows for up
% to 16 bpc linear precision, but in reality on early 2021 hardware at most 12 bpc.
% On most operating-systems + driver + gpu combos this 16 bpc mode will fail.
%
% 'screenid' Screen to use. Defaults to maximum screen id.
%
% 'filename' Name of the file where results should be stored. Defaults to file
% 'hdrmeasurements.mat' in the current working directory.
%
% Press the ESCAPE key to abort the test at any time.
%

% History:
% 02-Sep-2020   mk  Written.

global retluminance;
global referenceluminance;

% Check if PTB is properly installed, use cross-platform defaults and normalized
% color range:
PsychDefaultSetup(2);

if nargin < 1 || isempty(dotest)
    dotest = [1, 1, 1, 1];
end

% Default to ColorCal2 if meterType is not specified:
if nargin < 2 || isempty(meterType)
    meterType = 7;
end

if nargin < 3 || isempty(highprecision)
    highprecision = 0;
end

if nargin < 4 || isempty(screenid)
    screenid = max(Screen('Screens'));
end

if nargin < 5 || isempty(filename)
    filename = 'hdrmeasurements.mat';
else
    if ~ischar(filename)
        error('Invalid filename given. Not a string!');
    end
end

if meterType > 0
    % Open the colorimeter, or abort if not possible:
    CMCheckInit(meterType);
end

if meterType == -1
    % Suppress console output for manual data entry:
    ListenChar(2);
end

referenceluminance = [];
skipKbWait = 0;
lasterror('reset'); %#ok<LERR>

% Optionally print gpu hw state:
printgpuhwstate;

try
    % Open an onscreen fullscreen window on a HDR-10 capable monitor.
    % Background color is black, unit for all colors is nits.
    PsychImaging('PrepareConfiguration');
    PsychImaging('AddTask', 'General', 'EnableHDR', 'Nits');

    if highprecision == 1
        PsychImaging('AddTask', 'General', 'EnableNative16BitFloatingPointFramebuffer');
    end

    if highprecision == 2
        PsychImaging('AddTask', 'General', 'EnableNative16BitFramebuffer');
    end

    [win, rect] = PsychImaging('OpenWindow', screenid, 0);
    HideCursor(win);
    Screen('Textsize', win, 16);

    % Optionally print gpu hw state:
    printgpuhwstate;

    % Get displays HDR properties:
    displayhdrprops = PsychHDR('GetHDRProperties', win) %#ok<*NOPRT>
    if ~displayhdrprops.Valid
        % Fallback for macOS: Hard-code reasonable values, what else can we
        % do?
        displayhdrprops.MaxLuminance = 600;
        displayhdrprops.MaxFrameAverageLightLevel = 350;
    end

    maxLuminance = displayhdrprops.MaxLuminance
    maxFrameAverageLightLevel = displayhdrprops.MaxFrameAverageLightLevel; %#ok<NASGU>

    % Compute size of a test patch (filled rectangle) that fills exactly 10% of the
    % monitors display area, so we can test how well the monitor does wrt. peak
    % luminance:
    screenarea10percent = RectWidth(rect) * RectHeight(rect) * 0.10;
    testrectedgelength = floor(sqrt(screenarea10percent));

    % testrect is the proper 10% area rectangle:
    testrect = [0, 0, testrectedgelength, testrectedgelength];
    testrect = CenterRect(testrect, rect);

    if any(dotest(1:4))
        figure;
        DrawChromaticity;
        hold on;
    end

    %% Phase 1: White point and luminance measurement:
    if dotest(1)
        % Step through luminance range 0 - "95% of maxLuminance" nits, sampling in smaller
        % steps at the low end of the luminance range:
        whiteluminance = [0:0.001:0.049, 0.050:0.050:0.950, 1:1:(0.95 * maxLuminance)];
        targetcolors = whiteluminance;

        % Measure 10% area test patch of target luminances 'targetcolors' at display center:
        retluminance = runTestPatchSeries(win, meterType, testrect, targetcolors, skipKbWait);
        skipKbWait = 1;

        try
            % Convert measured values to chromaticity coordinates and luminance:
            xyY = XYZToxyY(retluminance.XYZ);
            ColorGamut(:, 4) = [mean(xyY(1,:)) ; mean(xyY(2,:))];

            % Report and plot measured white point vs. dispplay self reported one:
            fprintf('Reported white-point is at: %f, %f\n', displayhdrprops.ColorGamut(1, 4), displayhdrprops.ColorGamut(2, 4));
            fprintf('Measured white-point is at: %f, %f\n', mean(xyY(1,:)), mean(xyY(2,:)));
            plot(xyY(1,:), xyY(2,:), '+k', mean(xyY(1,:)), mean(xyY(2,:)), 'ok', displayhdrprops.ColorGamut(1, 4), displayhdrprops.ColorGamut(2, 4), '*k');
            title('Chromaticity coordinates of measured samples:');
        catch
        end
    end

    %% Phase 2: Red primary measurement:
    if dotest(2)
        % Step through luminance range 0 - maxLuminance nits, in steps of 1 nit:
        referenceluminance = 10:10:maxLuminance;
        targetcolors = [1 ; 0 ; 0] * referenceluminance;

        % Measure 10% area test patch of target luminances 'targetcolors' at display center:
        retred = runTestPatchSeries(win, meterType, testrect, targetcolors, skipKbWait);
        skipKbWait = 1;

        try
            % Convert measured values to chromaticity coordinates and luminance:
            xyY = XYZToxyY(retred.XYZ);
            ColorGamut(:, 1) = [mean(xyY(1,:)) ; mean(xyY(2,:))];

            % Report and plot measured white point vs. dispplay self reported one:
            fprintf('Reported red-point is at: %f, %f\n', displayhdrprops.ColorGamut(1, 1), displayhdrprops.ColorGamut(2, 1));
            fprintf('Measured red-point is at: %f, %f\n', mean(xyY(1,:)), mean(xyY(2,:)));
            plot(xyY(1,:), xyY(2,:), '+r', mean(xyY(1,:)), mean(xyY(2,:)), 'or', displayhdrprops.ColorGamut(1, 1), displayhdrprops.ColorGamut(2, 1), '*r');
            title('Chromaticity coordinates of measured samples:');
        catch
        end
    end

    %% Phase 3: Green primary measurement:
    if dotest(3)
        % Step through luminance range 0 - maxLuminance nits, in steps of 1 nit:
        referenceluminance = 10:10:maxLuminance;
        targetcolors = [0 ; 1 ; 0] * referenceluminance;

        % Measure 10% area test patch of target luminances 'targetcolors' at display center:
        retgreen = runTestPatchSeries(win, meterType, testrect, targetcolors, skipKbWait);
        skipKbWait = 1;

        try
            % Convert measured values to chromaticity coordinates and luminance:
            xyY = XYZToxyY(retgreen.XYZ);
            ColorGamut(:, 2) = [mean(xyY(1,:)) ; mean(xyY(2,:))];

            % Report and plot measured white point vs. dispplay self reported one:
            fprintf('Reported green-point is at: %f, %f\n', displayhdrprops.ColorGamut(1, 2), displayhdrprops.ColorGamut(2, 2));
            fprintf('Measured green-point is at: %f, %f\n', mean(xyY(1,:)), mean(xyY(2,:)));
            plot(xyY(1,:), xyY(2,:), '+g', mean(xyY(1,:)), mean(xyY(2,:)), 'og', displayhdrprops.ColorGamut(1, 2), displayhdrprops.ColorGamut(2, 2), '*g');
            title('Chromaticity coordinates of measured samples:');
        catch
        end
    end

    %% Phase 4: Blue primary measurement:
    if dotest(4)
        % Step through luminance range 0 - maxLuminance nits, in steps of 1 nit:
        referenceluminance = 10:10:maxLuminance;
        targetcolors = [0 ; 0 ; 1] * referenceluminance;

        % Measure 10% area test patch of target luminances 'targetcolors' at display center:
        retblue = runTestPatchSeries(win, meterType, testrect, targetcolors, skipKbWait);

        try
        % Convert measured values to chromaticity coordinates and luminance:
            xyY = XYZToxyY(retblue.XYZ);
            ColorGamut(:, 3) = [mean(xyY(1,:)) ; mean(xyY(2,:))];

            % Report and plot measured white point vs. dispplay self reported one:
            fprintf('Reported blue-point is at: %f, %f\n', displayhdrprops.ColorGamut(1, 3), displayhdrprops.ColorGamut(2, 3));
            fprintf('Measured blue-point is at: %f, %f\n', mean(xyY(1,:)), mean(xyY(2,:)));
            plot(xyY(1,:), xyY(2,:), '+b', mean(xyY(1,:)), mean(xyY(2,:)), 'ob', displayhdrprops.ColorGamut(1, 3), displayhdrprops.ColorGamut(2, 3), '*b');
            title('Chromaticity coordinates of measured samples:');
        catch
        end
    end

    if any(dotest(2:4))
        if all(dotest(2:4)) && exist('ColorGamut', 'var') && (size(ColorGamut, 1) >= 2) && (size(ColorGamut, 2) >= 3)
            line(ColorGamut(1, [1,2,3,1]), ColorGamut(2, [1,2,3,1]), 'color', 'k');
            line(displayhdrprops.ColorGamut(1, [1,2,3,1]), displayhdrprops.ColorGamut(2, [1,2,3,1]), 'color', 'b');
        end

        hold off;
    end

    % Must wait for keypress for the tests following this line:
    skipKbWait = 0; %#ok<NASGU>

    % Close Screen, clean up:
    sca;
catch me
    % Close Screen, clean up:
    sca;
end

fprintf('Done. Writing results to file %s\n', filename);
save(filename, '-V7');

if meterType == -1
    % Suppress console output for manual data entry:
    ListenChar(0);

    % Disable sound output as used by Beeper():
    Snd('Close');
end

if meterType > 0
    % Close the colorimeter:
    CMClose(meterType);
end

% Close pointless empty chroma plot on manual data entry:
if meterType == -1
    close all;
end

% Some more plotting for test 1, detailed white-point and luminance:
if dotest(1)
    try
        figure;
        whiteluminance = whiteluminance(1:size(retluminance.XYZ, 2));
        plot(whiteluminance, retluminance.XYZ(2,:));
        title('Expected vs. measured luminance in nits:');
        xlabel('Expected luminance [nits]');
        ylabel('Measured luminance [nits]');
    catch
    end

    % The following half-assed algorithm needs a "Viewer discretion is
    % advised" disclaimer...
    fprintf('\n\nCAUTION: The following diagnostic output is based on a currently suboptimal method which\n');
    fprintf('leaves a lot to be desired. Therefore take the automatic assessment with a big grain of salt!\n\n');

    % Check how much "dynamic range" is actually in the measured data. We
    % exclude measurements for target luminance values < 1 nit, as our
    % measurements may not be sensitive enough to resolve properly below that,
    % neither is our binning good enough:
    % Note: This is a bit hacky, better ways should be thought of...
    startind = find(whiteluminance >= 1.0, 1)
    shaderpq10bitval = retluminance.shaderpq10bitval(:, startind:end);
    [~, indices] = unique (shaderpq10bitval', 'rows');
    numlevelsexpected = length(indices)
    levelhisto = hist(retluminance.XYZ(2,:), numlevelsexpected); %#ok<HIST>
    numlevelsmeasured = length(find(levelhisto > 0))
    actualcontentbits = log2(numlevelsmeasured)

    % Check if the display likely does receive real 10 bpc content, as required for
    % HDR-10, ie. no truncation to 8 bpc happens on the gpu, link, or in the displays
    % display controller or matrix:
    if numlevelsexpected > 256 && actualcontentbits < 8
        warning('Measured data contains less than 8 bpc worth of variability. Suspicious...');
    end

    if numlevelsexpected > 256 && actualcontentbits > 8
        fprintf('More than 8 bits of variability detected in measured data. Good, 10 bpc transfer of video to display seems to work.\n');
    end

    % Save again if we made it to here:
    save(filename, '-V7');
end

% Optionally print gpu hw state:
printgpuhwstate;

if exist('me','var')
    rethrow(me);
end

% Auto close empty plots if no colorimeter measurement was done:
if meterType == 0
    close all;
end

% We are done!
end

function ret = runTestPatchSeries(win, meterType, testrect, targetcolors, skipKbWait)
    ret.shaderdiff = [];
    ret.shaderpq10bitval = [];
    ret.shaderpq12bitval = [];
    ret.referencecolors = [];
    ret.XYZ = [];
    ret.trouble = [];

    % Tell monitor what to expect:
    [winw, winh] = Screen('WindowSize', win);
    coverage = (RectWidth(testrect) * RectHeight(testrect)) / (winw * winh);
    % maxCLL intensity in nits of brightest pixel color component over all
    % frames - ie. brightest component in whole test sequence:
    maxCLL = max(targetcolors(:));
    % frame average light levels: For each frame/test patch, the value of
    % the brightest pixel color component in each pixel, averaged over all
    % pixels in a frame. In our case, as all pixels in the test patch have
    % same color and thereby same max component / component value, we just
    % take that maximum for each patch and multiply with how much of the
    % display is covered with the test patch, as the remainder of the
    % screen is all black aka zero:
    FALLs = max(targetcolors, [], 1) * coverage;
    % maxFALL is the maximum over all FALLs from all frames in the sequence:
    maxFALL = max(FALLs);

    msg = sprintf('Coverage %.01f%% : maxCLL = %.02f nits : maxFALL = %.02f nits.\n', 100 * coverage, maxCLL, maxFALL);
    disp(msg);

    % Tell the monitor about maxFALL and maxCLL, clamped to allowed maximum of 65535 nits:
    PsychHDR('HDRMetadata', win, 0, min(maxFALL, 65535), min(maxCLL, 65535));
    Screen('Flip', win);

    if ~skipKbWait
        % Instruct user to set up everything:
        DrawFormattedText(win, sprintf('%sPoint colorimeter at test patch,\nthen press any key to start measurement\n', msg), 'center', 30, [0, 40, 0]);
        Screen('FillRect', win, 40, testrect);
        Screen('FillOval', win, 0, CenterRect([0, 0, 10, 10], testrect));
        Screen('Flip', win);

        % Wait for start signal from user:
        KbStrokeWait(-1);
    else
        WaitSecs(5);
    end

    fprintf('\n\n\nStarting measurement:\n\n');

    for i = 1:size(targetcolors, 2)
        % ESCAPE allows early termination of measurement run:
        [~, ~, keyCode] = KbCheck(-1);
        if keyCode(KbName('ESCAPE'))
            break;
        end

        % Get targetcolor of test patch:
        targetcolor = targetcolors(:, i);

        % Draw test patch:
        Screen('FillRect', win, targetcolor, testrect);
        Screen('Flip', win);

        % Read back and compare Screen()'s shader based EOTF encoding against our
        % Matlab reference implementation in PQ():
        gpupqvalue = squeeze(Screen('GetImage', win, CenterRect([0 0 1 1], testrect), 'backBuffer', 1, 3));
        gpu10bitval = round(gpupqvalue * (2^10 - 1));
        gpu12bitval = round(gpupqvalue * (2^12 - 1));
        [refpqvalue, ref10bitval, ref12bitval] = PQ(single(targetcolor)); %#ok<ASGLU>
        if max(abs(gpu10bitval - ref10bitval)) > 1
            fprintf('WARNING: Mismatch of 10 bpc PQ encoded pixelvalue between PTB shader and reference implementation! %i\n', ...
                    abs(gpu10bitval - ref10bitval));
        end

        ret.shaderdiff(:, end+1) = gpu10bitval - ref10bitval; %#ok<*AGROW>
        ret.shaderpq10bitval(:, end+1) = gpu10bitval;
        ret.shaderpq12bitval(:, end+1) = gpu12bitval;

        if meterType ~= 0
            % Give display(backlight) some time to settle to new steady state:
            WaitSecs(0.5);
        end
 
        % Measure true luminance according to colorimeter:
        ret.referencecolors(:, end+1) = targetcolor;

        if meterType > 0
            [ret.XYZ(:, end+1), ret.trouble(end+1)] = MeasXYZ(meterType);
        else
            switch (meterType)
                case 0 % Fast run-through...
                    [ret.XYZ(:, end+1), ret.trouble(end+1)] = deal([0 ; 0; 0], -1);

                case -1 % Manual measurement and data entry:
                    Beeper;
                    KbStrokeWait(-1);
                    manNits = Ask(win, 'Measured nits + ENTER [q + ENTER to quit run]: ', 200, 0, 'GetChar');
                    % Abort on 'q', otherwise convert entry to manNits
                    % measured luminance value:
                    if ~strcmp(manNits, 'q')
                        manNits = str2double(manNits);
                    else
                        break;
                    end
                    [ret.XYZ(:, end+1), ret.trouble(end+1)] = deal([0 ; manNits ; 0], -1);
            end
        end

        if isscalar(targetcolor)
            targetlum = targetcolor;
        elseif length(find(targetcolor > 0)) == 1
            targetlum = max(targetcolor);
        else
            targetlum = rgb2gray(targetcolor');
        end

        % Display measurement result to top-left corner of screen:
        if isscalar(gpu10bitval)
            msg = sprintf('[PQ-10/12bit = %i/%i] -> Target %.05f nits vs. measured %.05f nits -> Delta %.01f%%. Trouble = %i\n', gpu10bitval, ...
                          gpu12bitval, targetlum, ret.XYZ(2, end), (ret.XYZ(2, end) - targetlum) / targetlum * 100, ret.trouble(end));
        else
            msg = sprintf('[PQ-10/12bit = %i,%i,%i/%i,%i,%i] -> Target %.05f nits vs. measured %.05f nits -> Delta %.01f%%. Trouble = %i\n', gpu10bitval, ...
                          gpu12bitval, targetlum, ret.XYZ(2, end), (ret.XYZ(2, end) - targetlum) / targetlum * 100, ret.trouble(end));
        end
        disp(msg);
        DrawFormattedText(win, msg, 0, 30, 40);
        Screen('Flip', win);

        if meterType ~= 0
            % Rest for a second:
            WaitSecs(1);
        end
    end

    nsamples = min(size(ret.referencecolors, 2), size(ret.XYZ, 2));
    ret.referencecolors = ret.referencecolors(:, 1:nsamples);
    ret.XYZ = ret.XYZ(:, 1:nsamples);
end

function [v, digital10bitval, digital12bitval] = PQ(L)
% ST-2084 PQ "Perceptual Quantizer" inverse EOTF (ie. OETF), from
% https://en.wikipedia.org/wiki/High-dynamic-range_video#Perceptual_Quantizer
%
  L  = L / 10000;
  Lp = L .^ 0.1593017578125;
  f  = (0.8359375 + 18.8515625 .* Lp) ./ (1 + 18.6875 .* Lp);
  v  = f .^ 78.84375;

  % Convert back into 10 bit discrete value:
  digital10bitval = round(v * (2^10 - 1));
  % Convert back into 12 bit discrete value:
  digital12bitval = round(v * (2^12 - 1));
end

function printgpuhwstate
    % Disabled by default:
    if 0
        if IsLinux %#ok<UNRCH>
            % This system() call would print status of gamma lut's on AMD DCN-1 hardware
            % for display engine/ooutput 0. A value of 0 means hw bypass enabled and is
            % good for our purpose. This will prompt for an admin password, so will only
            % work with Octave run from a terminal. Also needs AMD's 'umr' tools installed:
            system ('sudo umr -O bits --read raven1.dcn10.mmCM0_CM_RGAM_CONTROL --read raven1.dcn10.mmCM0_CM_DGAM_CONTROL')
        end
    end
end
