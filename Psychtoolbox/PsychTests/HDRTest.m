function HDRTest(meterType, screenid, filename)
% HDRTest([meterType=7][, screenid=max][, filename='hdrmeasurements.mat'])
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
% 'meterType' id code of the measurement device to use, as listed in "help MeasXYZ".
% Defaults to meterType 7 for the CRS ColorCal2.
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

global XYZ;
global referenceluminance;

% Check if PTB is properly installed, use cross-platform defaults and normalized
% color range:
PsychDefaultSetup(2);
escape = KbName('ESCAPE');

% Default to ColorCal2 if meterType is not specified:
if nargin < 1 || isempty(meterType)
    meterType = 7;
end

if nargin < 2 || isempty(screenid)
    screenid = max(Screen('Screens'));
end

if nargin < 3 || isempty(filename)
    filename = 'hdrmeasurements.mat';
else
    if ~ischar(filename)
        error('Invalid filename given. Not a string!');
    end
end

% Open the colorimeter, or abort if not possible:
CMCheckInit(meterType);

XYZ = [];
referenceluminance = [];
shaderdiff = [];
shaderpq10bitval =[];

lasterror('reset'); %#ok<LERR>

% Optionally print gpu hw state:
printgpuhwstate;

try
    % Open an onscreen fullscreen window on a HDR-10 capable monitor.
    % Background color is black, unit for all colors is nits.
    PsychImaging('PrepareConfiguration');
    PsychImaging('AddTask', 'General', 'EnableHDR', 'Nits');
    [win, rect] = PsychImaging('OpenWindow', screenid, 0);
    HideCursor(win);
    Screen('Textsize', win, 16);

    % Optionally print gpu hw state:
    printgpuhwstate;

    displayhdrprops = PsychHDR('HDRMetadata', win) %#ok<*NOPRT>
    maxLuminance = displayhdrprops.MaxLuminance
    maxFrameAverageLightLevel = displayhdrprops.MaxFrameAverageLightLevel

    % Compute size of a test patch (filled rectangle) that fills exactly 10% of the
    % monitors display area:
    screenarea10percent = RectWidth(rect) * RectHeight(rect) * 0.10;
    testrectedgelength = floor(sqrt(screenarea10percent));

    % testrect is the proper 10% area rectangle:
    testrect = [0, 0, testrectedgelength, testrectedgelength];

    % Instruct user to set up everything:
    DrawFormattedText(win, 'Point colorimeter at test patch,\nthen press any key to start measurement\n', 'center', 30, [0, maxFrameAverageLightLevel/3, 0]);
    Screen('FillRect', win, maxFrameAverageLightLevel/3, CenterRect(testrect, rect));
    Screen('Flip', win);

    % Wait for start signal from user:
    KbStrokeWait(-1);
    fprintf('\n\n\nStarting measurement:\n\n');

    for targetluminance = 0:1:maxLuminance
        % ESCAPE allows early termination of measurement run:
        [~, ~, keyCode] = KbCheck(-1);
        if keyCode(escape)
            break;
        end

        % Draw 10% test patch of target luminance 'targetluminance' at display center
        % and show it:
        Screen('FillRect', win, targetluminance, CenterRect(testrect, rect));
        Screen('Flip', win);

        % Read back and compare Screen()'s shader based EOTF encoding against our
        % Matlab reference implementation in PQ():
        gpupqvalue = Screen('GetImage', win, CenterRect([0 0 1 1], rect), 'backBuffer', 1, 1);
        gpu10bitval = floor(gpupqvalue * (2^10 - 1));
        [refpqvalue, ref10bitval] = PQ(single(targetluminance)); %#ok<ASGLU>
        if abs(gpu10bitval - ref10bitval) > 1
            fprintf('WARNING: Mismatch of PQ encoded pixelvalue between PTB shader and reference implementation! %i vs. %i -> %i\n', ...
                    gpu10bitval, ref10bitval, gpu10bitval - ref10bitval);
        end
        shaderdiff(end+1) = gpu10bitval - ref10bitval; %#ok<*AGROW>
        shaderpq10bitval(end+1) = gpu10bitval;

        % Give display(backlight) some time to settle to new steady state:
        WaitSecs(0.5);

        % Measure true luminance according to colorimeter:
        referenceluminance(end+1) = targetluminance;
        [XYZ(:, end+1), trouble] = MeasXYZ(meterType);

        % Display measurement result to top-left corner of screen:
        msg = sprintf('[PQ-10bit = %i] -> Target %.05f nits vs. measured %.05f nits -> Delta %.01f%%. Trouble = %i\n', gpu10bitval, ...
                      targetluminance, XYZ(2, end), (XYZ(2, end) - targetluminance) / targetluminance * 100, trouble);
        disp(msg);
        DrawFormattedText(win, msg, 0, 30, 40);
        Screen('Flip', win);

        % Rest for a second:
        WaitSecs(1);
    end

    % Close Screen, clean up:
    sca;
catch me
    % Close Screen, clean up:
    sca;
end

nsamples = min(length(referenceluminance), size(XYZ, 2));
referenceluminance = referenceluminance(1:nsamples);
XYZ = XYZ(:, 1:nsamples);

fprintf('Done. Writing results to file %s\n', filename);
save(filename, '-V7');

% Close the colorimeter:
CMClose(meterType);

try
    plot(referenceluminance, XYZ(2,:));
    title('Expected vs. measured luminance in nits:');
    xlabel('Expected luminance [nits]');
    ylabel('Measured luminance [nits]');
catch
end

if exist('me','var')
    rethrow(me);
end

% Optionally print gpu hw state:
printgpuhwstate;

% Check how much "dynamic range" is actually in the measured data:
numlevelsexpected = length(unique(shaderpq10bitval))
levelhisto = hist(XYZ(2,:), numlevelsexpected);
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

function [v, digital10bitval] = PQ(L)
% ST-2084 PQ "Perceptual Quantizer" inverse EOTF (ie. OETF), from
% https://en.wikipedia.org/wiki/High-dynamic-range_video#Perceptual_Quantizer
%
  L  = L / 10000;
  Lp = L .^ 0.1593017578125;
  f  = (0.8359375 + 18.8515625 .* Lp) ./ (1 + 18.6875 .* Lp);
  v  = f .^ 78.84375;

  % Convert back into 10 bit discrete value:
  digital10bitval = floor(v * (2^10 - 1));
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
