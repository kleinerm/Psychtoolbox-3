function VRRFixedRateSwitchingTest(test, n, hwmeasurement, testImage, saveplots, screenNumber)
% VRRFixedRateSwitchingTest([test='upstep'][, n=2000][, hwmeasurement=0][, testImage][, saveplots=0][, screenNumber=max])
%
% Test accuracy of Fixed refresh rate (FRR) stimulation, while rapid refresh rate
% switching is facilitated by use of VRR functionality, aka "FreeSync", "G-Sync",
% or "DisplayPort Adaptive Sync".
%
% FRR switching is useful for paradigms where you want to have fine-grained control
% over video refresh rate, e.g., to play back movies or show animations at a specific
% framerate, and you want very stable timing, but you only occassionally need to
% switch framerates. FRR switching is fast, it only takes one or at most two video
% refresh cycles. However, during those 1-2 cycles, the display may blank or flicker.
% You may want to use FRR switching between consecutive trials to avoid flicker.
%
% Normal VRR modes allow equally fine-grained timing control and perfectly seamless
% changes of frame duration on a frame-to-frame basis. However, the current
% implementation may be a bit more noisy in terms of timing stability.
%
% The test exercises fast FRR switching on a suitable system. Currently only AMD
% graphics cards under Linux X11 with DisplayPort connected VRR display monitors
% are supported, no other graphics cards, no HDMI connected displays, no other
% displays. You may need to run PsychLinuxConfiguration(), make sure that the
% deep color config file gets installed or updated by it, and reboot your machine
% once to make this work on many Linux kernels.
%
% It submits OpenGL bufferswaps / flips of varying 'delay' between successive
% flips and measures and plots how well the hw can follow the requested timing.
% When appropriate, the fixed refresh rate of the display is switched via rapid
% switching, to achieve a frame interval as close as possible - or identical - to
% the requested delay. This works especially well within the VRR range of your
% display, typically between 48 Hz and the maximum refresh rate of your display.
%
% Usage:
%
% All parameters are optional.
%
% The 'test' parameter selects the test pattern:
%
% 'upstep'    Stepwise increasing duration every 60 flips.
%
% 'downstep'  Stepwise decreasing duration every 60 flips.
%
% 'const'     Run at some constant frame duration.
%
%
% 'n' Number of flips / trials to run. 2000 by default.
%
%
% 'hwmeasurement' Use external measurement hardware to get timing ground-truth:
%
% 0 = Off [Default]. This display a static 'testImage' to allow to check how much
%     the VRR display flickers under different stimulation timing regimes.
%
% 1 = VPixx DataPixx or similar VPixx device.
%
% 2 = VideoSwitcher + RTBox TTL pulse input port. Works with RTBox, but also with
%     emulated RTBox of the CRS Bits#. Takes TTL pulse input from the VideoSwitcher.
%     Selecting this will execute a enable/disable sequence for the VideoSwitcher
%     at start and end of a measurement session.
%
% 3 = CRS Bits# via a loopback cable from trigger out to trigger in BNC port.
%     Careful: Uses T-Lock for signalling/triggering at stimulus onset and therefore
%     a rather deficient (mis)design. This works as long as low framerate compensation
%     does not get triggered, otherwise the reference timestamps from the CRS hardware
%     will be completely useless and bogus trash!
%
% 4 = Like 2 -- RtBox pulse input, but for use with a ColorCal2 or photo-diode that
%     sends a TTL pulse to the RtBox / Bits# BNC trigger input instead of VideoSwitcher.
%
% 5 = Measure via a supported photo-diode via PsychPhotodiode().
%
% 6 = Produce light-flash pattern to drive external photo-diode. Don't record yourself.
%
%
% 'testImage' Either the name of an image file, or a numeric m x n or m x n x 3
% matric with color values. The image read from the image file, or given image
% matrix, will be displayed covering the full window in hwmeasurement == 0 mode.
% If the parameter is omitted, one of Psychtoolbox default demo images will be
% displayed. The purpose of this static image display is to test how much the
% display device flickers under different VRR stimulation timings.
%
%
% 'saveplots' Should plots with results be saved to filesystem? Defaults to
% 0 for 'No', 1 = 'Yes'.
%
%
% 'screenNumber' Number of screen to test on. Maximum X-Screen by default.
%
% You can abort the test earlier by pressing the ESC key.
%
% The main plot figure will plot actual measured delay between successive flips
% (blue) against requested optimal duration (red). It will also show the difference
% in green and median error in red at the bottom of the graph. Median/Average error
% and standard deviation is printed to the console last. The plot is also saved
% to the current working directory as PDF file.
%

% History:
% 23-May-2025  mk  Written. Derived from VRRTest.

if ~IsLinux || IsWayland
    fprintf('This functionality is only supported on Linux X11.\n');
    return;
end

% Upstep pattern by default:
if nargin < 1 || isempty(test)
    test = 'upstep';
end

% 2000 flips / samples by default:
if nargin < 2 || isempty(n)
    n = 2000;
end

% Don't use external measurement hardware for ground-truth by default:
if nargin < 3 || isempty(hwmeasurement)
    hwmeasurement = 0;
end

if nargin < 4 || isempty(testImage)
    % Use default demo images, if no special image was provided.
    testImage = [ PsychtoolboxRoot 'PsychDemos' filesep 'konijntjes1024x768.jpg'];
end

if nargin < 5 || isempty(saveplots)
    saveplots = 0;
end

% Use screen with highest number by default:
if nargin < 6 || isempty(screenNumber)
    screenNumber = [];
end

if ischar(testImage)
    % Read image from image file and extend it with 200 lines of neutral gray:
    testImage = double(imread(testImage)) / 255;
elseif ~isnumeric(testImage)
    error('testImage must be the name of an image file or a color matrix.');
end

try
    % Standard settings and key mappings. ESCape key aborts early, before
    % n samples are done:
    PsychDefaultSetup(2);
    RestrictKeysForKbCheck(KbName('ESCAPE'));

    % Max X-Screen selected by default:
    if isempty(screenNumber)
        screenNumber = max(Screen('Screens'));
    end

    % Enforce some debug output with RandR modeline of current mode:
    oldverbo = Screen('Preference','Verbosity', 5);
    Screen('FrameRate', screenNumber);
    Screen('Preference','Verbosity', oldverbo);

    % Switch to FIFO realtime-priority and memory locking to reduce timing jitter
    % and interruptions caused by other applications and the operating system itself:
    Priority(MaxPriority(screenNumber));

    % Prepare window configuration for this script:
    PsychImaging('PrepareConfiguration')

    if hwmeasurement == 1
        % Use DataPixx hardware for external timestamping to get ground-truth
        % about true display timing for correctness tests.
        PsychImaging('AddTask', 'General', 'UseDataPixx');
    end

    if hwmeasurement == 2
        % Switch Videoswitcher into high precision luminance + trigger mode:
        PsychVideoSwitcher('SwitchMode', screenNumber, 1);
    end

    if hwmeasurement == 3
        % Enable T-Lock generation for Bits#
        PsychImaging('AddTask', 'General', 'EnableBits++Bits++Output');
    end

    % Open double-buffered fullscreen (kms-pageflipped) window with black background:
    [w, rect] = PsychImaging('OpenWindow', screenNumber, 0);
    HideCursor(w);
    [width, height] = Screen('Windowsize', w);

    % Get window info struct about onscreen fullscreen window:
    winfo = Screen('GetWindowInfo', w);

    % In hwmeasurement 0 mode - no measurement - display a texture with testImage,
    % as a means to check for visual artifacts of flicker:
    if hwmeasurement == 0
        tex = Screen('MakeTexture', w, testImage);
    end

    if hwmeasurement == 2 || hwmeasurement == 3 || hwmeasurement == 4
        IOPort('CloseAll');
        rtbox = PsychRTBox('Open');
        rtboxport = PsychRTBox('BoxInfo', rtbox);
        rtboxport = rtboxport.handle;
        PsychRTBox('Stop', rtbox);
        PsychRTBox('Clear', rtbox);
        PsychRTBox('Disable', rtbox, 'all');
        PsychRTBox('Enable', rtbox, 'pulse');
    end

    if hwmeasurement == 3
        % Hold extra reference for the emulated RTBox:
        BitsPlusPlus('OpenBits#');

        % Trigger pulse of 0.1 msecs (One 100 usecs minimum unit) on TriggerOut port (bit 15):
        tLockTriggerData = zeros(1, 248);
        tLockTriggerData(1, 1:10) = ones(1,1) * 2^15;
        BitsPlusPlus('DIOCommand', w, n, 2^15, tLockTriggerData, 0);
    end

    if hwmeasurement == 5
        % Activate photo-diode timestamping driver:
        InitializePsychSound(1);
        pdiode = PsychPhotodiode('Open');

        % Perform calibration of optimal photo-diode trigger level:
        triggerlevel = PsychPhotodiode('CalibrateTriggerLevel', pdiode, w) %#ok<NASGU,NOPRT>
    end

    % Get minimal frame duration 'ifi' from calibration loop:
    originalMaxHz = Screen('FrameRate', screenNumber, 1);
    [ifi, nvalid, stddev ] = Screen('GetFlipInterval', w);
    fprintf('Measured refresh interval, as reported by "GetFlipInterval" is %2.5f ms. (nsamples = %i, stddev = %2.5f ms)\n', ifi*1000, nvalid, stddev*1000);

    % Init data-collection arrays for collection of n samples:
    ts=zeros(1,n);
    beampos=ts;
    missest=ts;
    flipfin=ts;
    dpixxdelay=ts;
    td=ts;
    so=ts;
    sodpixx = ts;
    boxTime = ts;

    valids = false(1,n);
    meascount = 0;

    % Perform some initial Flip to get us in sync with vblank:
    % tvbl is the timestamp when post-flip scanout started, as returned by
    % kernel/OpenML sync control.
    tvbl = Screen('Flip', w);
    tStart = tvbl;
    tdeadline = tvbl;

    % How much to subsample hw measurements to not overload non-VPixx ts methods:
    hwstepping = 4;

    % minFlipDelta == minimum frame duration in seconds == nominal reported max fps:
    minFlipDelta = 1 / originalMaxHz;

    % minVRR == minimum refresh rate in VRR range of typical consumer VRR display monitors:
    % Note: Some can go down to 30 Hz, so 48 Hz is a conservative setting for those who can't.
    minVRR = 48;
    maxFlipDelta = 1 / minVRR;
    durRange = maxFlipDelta - minFlipDelta;

    % Keeps track of current refresh rate induced IFI:
    olddelay = nan;

    % Test-loop: Collects n samples.
    for i=1:n
        % Update simulation time for sine wave etc.:
        t = i / n * 1;

        % Select frame timing test function:
        switch (test)
            case {'upstep'}
                % Stepwise increase of frame duration, 2 msecs every 60 flips: Goes below min VRR.
                delay = minFlipDelta + 2 * floor(i / 60) / 1000;
            case {'downstep'}
                % Stepwise increase of frame duration, 2 msecs every 60 flips: Goes below min VRR.
                delay = minFlipDelta + 2 * floor((n - i) / 60) / 1000;
            case {'const', 'constant'}
                % Constant frame duration. Hard-code as needed:
                delay = minFlipDelta * 1.5;

            % The following test patterns are here for reference, but will not work well at all,
            % a total flickerfest! See VRRTest.m for reference on how to do these better via full
            % fine grained VRR scheduling.
            case {'sine'}
                % Sine-wave pattern of changing frame duration: Goes below min VRR.
                delay = minFlipDelta + durRange * (sin(2 * pi * t) + 1) / 2;
            case {'random', 'rand'}
                % Randomized within VRR range:
                delay = minFlipDelta + (1 / minVRR - minFlipDelta) * rand();
            case {'maxrandom', 'maxrand'}
                % Randomized beyond VRR range:
                delay = minFlipDelta + durRange * rand();
            case {'upsweep'}
                % Linear increase of frame duration: Goes below min VRR.
                delay = minFlipDelta + durRange * i / n;
            case {'downsweep'}
                % Linear decrease of frame duration: Goes below min VRR.
                delay = minFlipDelta + durRange * (n - i) / n;
        end

        % If computed delay differs from old one, request a refresh rate switch
        % of the display to adapt optimally to allow to get equal or close to the
        % requested new delay:
        if delay ~= olddelay
            % Do clever trickery for delay outside display supported VRR range:
            if delay > maxFlipDelta
                for wf = 2:100
                    if delay / wf <= maxFlipDelta && delay / wf >= minFlipDelta
                        break;
                    end
                end

                reqHz = 1 / (delay / wf);
            else
                % Simple standard case within supported VRR range:
                reqHz = 1 / delay;
            end

            actualHz = Screen('ConfigureDisplay', 'FineGrainedSwitchRefreshRate', screenNumber, 0, reqHz);
            ifi = Screen('GetFlipInterval', w);
            olddelay = delay;
        end

        % Compute absolute deadline for next swapbuffers/flip request, relative
        % to previous completed flip:
        tdeadline = tvbl + (round(delay / ifi) - 0.5) * ifi;

        % Store requested delay for i'th trial in td:
        td(i) = delay;

        if hwmeasurement == 0
            Screen('DrawTexture', w, tex, [], rect);
        end

        % Draw some simple stim for next frame of animation: A rectangle that moves over the screen.
        pos = mod(i, width-100);
        Screen('FillRect', w, [1 0 1], [pos height-100 pos+100 height]);

        if hwmeasurement == 1
            % VPixx device: Ask for a Datapixx onset timestamp for next 'Flip':
            PsychDataPixx('LogOnsetTimestamps', 1);
            valids(i) = 1;
        end

        if (hwmeasurement == 2 || hwmeasurement == 3 || hwmeasurement == 4) && i == 1
            PsychRTBox('Start', rtbox, 0);
        end

        if hwmeasurement == 2 || hwmeasurement == 3 || hwmeasurement == 4
            %PsychRTBox('EngageTRTrigger', rtbox);
        end

        if hwmeasurement == 2 && mod(i, hwstepping) == 0
            % VideoSwitcher + RtBox: Draw green bar at top of screen to trigger VideoSwitcher:
            Screen('FillRect', w, [0 1 0], [0 0 width 5]);
            valids(i) = 1;
        end

        if hwmeasurement == 3 && mod(i, hwstepping) == 0
            % Bits# T-Lock loopback: TODO -- Only every 2nd frame, like others...
            valids(i) = 1;
        end

        if (hwmeasurement == 4 || hwmeasurement == 5 || hwmeasurement == 6) && mod(i, hwstepping) == 0
            % Photosensor + RtBox: Draw white rectangle at top of screen to trigger photo sensor / ColorCal2 etc.:
            yshift = height / 5 * 0;
            Screen('FillRect', w, 1, [0 yshift width yshift+height/5]);
            Screen('DrawText', w, sprintf('+ %d msecs_______', round((yshift+height/5/2) / height * ifi * 1000)), width - 300, yshift+height/5/2);
            valids(i) = 1;

            if hwmeasurement == 5
                diodestart = PsychPhotodiode('Start', pdiode); %#ok<NASGU>
            end
        end

        % Request flip at time tdeadline. Return the driver reported timestamp
        % when post-flip scanout starts in tvbl and so(i).
        % The rasterbeam-position (scanline) when the measurement was taken is
        % returned in beampos(i), or -1 on unsupported hw/sw combo.
        % The time when flip returned to Octave/Matlab is returned in flipfin(i).
        [tvbl, so(i), flipfin(i), missest(i), beampos(i)] = Screen('Flip', w, tdeadline);
        if hwmeasurement == 1
            % Ask for a Datapixx onset timestamp from last 'Flip':
            [boxTime(i), sodpixx(i)] = PsychDataPixx('GetLastOnsetTimestamp');
            dpixxdelay(i) = GetSecs;
        end

        if (hwmeasurement == 5) && valids(i)
            % Ask for photo-diode onset timestamp from last 'Flip':
            meascount = meascount + 1;
            tPhoto = PsychPhotodiode('WaitSignal', pdiode);
            if ~isempty(tPhoto)
                sodpixx(meascount) = tPhoto;
            else
                sodpixx(meascount) = NaN;
            end

            boxTime(meascount) = sodpixx(meascount);
        end

        % Record timestamp [again - redundant on Linux] for later use:
        ts(i) = tvbl;

        % Give user a chance to abort the test anytime by pressing ESC key:
        if KbCheck
            break;
        end

        if hwmeasurement == 2 || hwmeasurement == 3 || hwmeasurement == 4
            %[nw, tpost] = IOPort('Write', rtboxport, 'f', 1);
        end
    end

    % calculate clock skew corrected Datapixx onset timestamps
    if hwmeasurement == 1
        sodpixx = PsychDataPixx('BoxsecsToGetsecs', boxTime);
    end

    if hwmeasurement == 2 || hwmeasurement == 3 || hwmeasurement == 4
        Screen('Flip', w);
        Screen('Flip', w);
        WaitSecs(0.5);
        PsychRTBox('Stop', rtbox);
        [~, evt, rtinboxsecs] = PsychRTBox('GetSecs', rtbox);
        dpixxdelay = so; % Fake a zero delay for RTBox.

        % Filter out "almost duplicates" closer than 1 ifi together:
        rtboxsecs = rtinboxsecs(1);
        for kk=2:length(rtinboxsecs)
            if rtinboxsecs(kk) - rtinboxsecs(kk-1) > (hwstepping - 0.15) * ifi
                rtboxsecs(end+1) = rtinboxsecs(kk);
            end
        end

        fprintf('Raw hw samples %i vs. filtered samples %i.\n', length(rtinboxsecs), length(rtboxsecs));

        boxTime = rtboxsecs;
        % Calibrated remapping of rtboxsecs to GetSecs secs in sodpixx:
        sodpixx = PsychRTBox('BoxsecsToGetsecs', rtbox, rtboxsecs);

        if hwmeasurement ~= 3
            PsychRTBox('Close', rtbox);
        else
            % Drop our ref for the simulated RTBox of Bits#
            BitsPlusPlus('Close');
            clear PsychRTBox;
        end
    end

    if hwmeasurement == 5
        PsychPhotodiode('Close', pdiode);
    end

    % Normal scheduling:
    Priority(0);

    % Reset keys:
    RestrictKeysForKbCheck([]);

    % Restore original refresh rate on display:
    actualFinalHz = Screen('ConfigureDisplay', 'FineGrainedSwitchRefreshRate', screenNumber, 0, originalMaxHz)

    % Close window and clean up:
    sca;

    if hwmeasurement == 2
        % Switch Videoswitcher back to standard RGB passthrough:
        PsychVideoSwitcher('SwitchMode', screenNumber, 1);
    end

    % Restrict to actual number of collected samples:
    n = i;

    % These data is available for all n flips/trials:
    ts = ts(1:n);
    so = so(1:n);
    flipfin = flipfin(1:n);
    missest = missest(1:n);
    beampos = beampos(1:n);
    td = td(1:n);
    dpixxdelay = dpixxdelay(1:n);

    %sodpixx = sodpixx(valids);
    %boxTime = boxTime(valids);

    % Plot all our measurement results:

    % Figure 1 shows time deltas between successive flips in milliseconds:
    % Blue = Actual measured/kernel reported flip deltas:
    figure;
    hold on;
    deltaT = (ts(2:n) - ts(1:n-1)) * 1000;
    plot(deltaT);
    ni = minFlipDelta;
    if minFlipDelta < 1
        ni = 1;
    end

    % Red = Requested flip deltas - Ideal result:
    td = td(2:end) * 1000;
    plot(td, 'r');

    err = deltaT - td;

    % Green = Error between requested and actual frame duration for each flip:
    plot(err, 'g');

    % Find presents that are more than 1 msec too early - ie. definite failures.
    realbadones = find((err) < -1);
    if length(realbadones) > 0
        warning('WARNING: Multiple presents too early!! %i items.\n', length(realbadones));
    end

    % Red = Median error over whole run:
    plot(ones(1,n) * median(err), 'r');
    fprintf('Avg diff target vs. true: %f msecs. stddev %f msecs. Median diff %f msecs. Too early: %i\n', ...
            mean(err), std(err), median(err), length(realbadones));
    fprintf('Without 1st two samples : %f msecs. stddev %f msecs. Median diff %f msecs. Too early: %i\n', ...
            mean(err(3:end)), std(err(3:end)), median(err(3:end)), length(realbadones));
    title('Delta between successive Flips in milliseconds [red=requested, blue=actual]:');
    text(0,-10, 'Difference (green), median error (horizontal red):');
    text(0,-30, sprintf('Avg diff target vs. true: %f msecs. stddev %f msecs. Median diff %f msecs. Too early: %i\n', ...
                        mean(err), std(err), median(err), length(realbadones)));
    xlabel('Number');
    ylabel('msecs');

    grid on;
    hold off;

    if saveplots
        % Save the plot as PDF file into current working directory:
        fname = sprintf('VRRFixedRateSwitchingTestResult_%s.pdf', test);
        print(fname, '-dpdf');
        fprintf('Plot saved to working directory as %s\n', fname);
    end

    figure;
    hist(err, 100);
    title('Histogram of difference between requested and actual frame duration [msecs]');

    % Figure 2 shows the recorded beam positions if hw and sw setup allowed that:
    if winfo.VBLEndline ~= -1
        figure;
        plot(beampos);
        title('Rasterbeam position when timestamp was taken (in scanlines):');
    end

    % If Datapixx hardware measurement was used, plot difference between kernel
    % reported flip times and ground-truth from external measurement hardware:
    if hwmeasurement
        figure;
        grid on;

        % sov is the subset of so samples for which we have valid hw measurements:
        sov = so(valids);

        if length(sodpixx) ~= length(sov)
            cutoffn = min(length(sodpixx), length(sov));
            if hwmeasurement ~= 5
                fprintf('Warning: Missing data %i vs. %i, only using first %i.\n', length(sodpixx), length(sov), cutoffn);
            end

            sodpixx = sodpixx(1:cutoffn);
            sov = sov(1:cutoffn);
        end

        plot((sov - sodpixx) * 1000);
        title('Time delta in msecs onset according to Flip - onset according to measurement hw:');
        fprintf('Average discrepancy between Flip timestamping and hardware is %f msecs, stddev = %f msecs, Median %f msecs.\n', ...
                mean((sov - sodpixx) * 1000), std((sov - sodpixx) * 1000), median((sov - sodpixx) * 1000));

        % Hw timestamping delay only valid/meaningfull for DataPixx hwmeasurement == 1, otherwise skip:
        if (length(dpixxdelay) == length(sov)) && any(dpixxdelay - sov)
            figure;
            plot((dpixxdelay - sov) * 1000);
            title('Time delta between stimulus onset and return of hardware timestamping in milliseconds:');
        end

        if hwmeasurement == 2 || hwmeasurement == 4 || hwmeasurement == 5
            figure;
            grid on;
            plot(1000 * diff(sodpixx));
            title('Delta between successive Flips in milliseconds according to measurement hw:');
        end
    end

    if ~IsGUI
        fprintf('Press enter to close plot and exit.\n');
        pause;
    end
catch %#ok<*CTCH>
    Priority(0);
    RestrictKeysForKbCheck([]);

    if exist('originalMaxHz', 'var')
        % Restore original refresh rate on display:
        actualFinalHz = Screen('ConfigureDisplay', 'FineGrainedSwitchRefreshRate', screenNumber, 0, originalMaxHz)
    end

    sca;
    psychrethrow(psychlasterror);
end

return
