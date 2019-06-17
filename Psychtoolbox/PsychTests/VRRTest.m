function VRRTest(test, n, maxFlipDelta, hwmeasurement, screenNumber)
% VRRTest([test='sine'][, n=2000][, maxFlipDelta=0.2][, hwmeasurement=0][, screenNumber=max])
%
% Test accuracy of VRR aka "FreeSync" aka "Adaptive Sync" mode on Linux 5.2.
%
% CAUTION: This script is highly experimental, subject to change in the near
%          future!
%
% The test exercises VRR on a system with Linux 5.2+, Mesa 19.0+, latest
% amdgpu-ddx on a FreeSync capable AMD gpu and display combo.
%
% It submits OpenGL bufferswaps / flips of varying 'delay' between successive
% flips and measures and plots how well the hw can follow the requested timing.
%
% Usage:
%
% All parameters are optional.
%
% The 'test' parameter selects the test pattern:
% 'sine' Sine wave, smoothly changing. Exceeds VRR range to test low framerate
% compensation (lfc). 'sine' is the default pattern if parameter is omitted.
%
% 'random' Randomized from flip to flip, within VRR range.
% 'upsweep' linear increasing duration.
% 'downsweep' linear decreasing duration.
% 'upstep' Stepwise increasing duration every 60 flips.
% 'downstep' Stepwise decreasing duration every 60 flips.
% 'const' Run at some constant frame duration.
%
% 'n' Number of flips / trials to run. 2000 by default.
%
% 'maxFlipDelta' Maximum frame duration in seconds. 0.2 secs = 200 msecs by default.
%
% 'hwmeasurement' Use external measurement hardware to get timing ground-truth.
% 0 = Off [Default]
% 1 = VPixx DataPixx or similar
% 2 = PsychRTBox pulse input port. Works with RTBox, but also with emulated RTBox
%     of the CRS Bits#. Takes TTL pulse input from external device, e.g., from a
%     photometer with flash detection -> trigger output, or the VideoSwitcher.
%     Selecting this will execute a enable/disable sequence for the VideoSwitcher
%     at start and end of a measurement session.
% 3 = CRS Bits# via a loopback cable from trigger out to trigger in BNC port.
%     Careful: Uses T-Lock for signalling/triggering at stimulus onset and therefore
%     a rather deficient (mis)design. This works as long as low framerate compensation
%     does not get triggered, otherwise the reference timestamps from the CRS hardware
%     will be completely useless and bogus trash!
%
% 'screenNumber' Number of X-Screen to test on. Maximum X-Screen by default.
%
% You can abort the test earlier by pressing the ESC key.
%
% The main plot figure will plot actual measured delay between successive flips
% (blue) against requested optimal duration (red). It will also show the difference
% in green and median error in red at the bottom of the graph. Median/Average error
% and standard deviation is printed to the console last. The plot is also saved
% to the current working directory as PDF file.
%
%
% Preliminary results with a patch-set targeted at Linux 5.2:
%
% Testing on an upcoming Linux 5.2 kernel showed good behaviour on tested AMD
% DCE-8 (Sea Islands), DCE-11 (Polaris) and DCN-1 (Raven) gpu's within the VRR
% range of the monitor. Also pretty good behaviour on DCN-1 for flip rates below
% the minimum VRR rate by use of low framerate compensation. lfc performance was
% mixed on the DCE-8 and DCE-11 gpu's. For 'upsweep', 'upstep' and some 'const'
% duration it was pretty reasonable or as good as DCN-1. For 'downsweep' and
% 'downstep' and some 'const' durations, lfc didn't do a great job, with actual
% frame durations often deviating by dozens of msecs from requested ones.
%

% History:
% ??-Jan-2019  mk  Written.

% Sine-Wave pattern by default:
if nargin < 1 || isempty(test)
    test = 'sine';
end

% 2000 flips / samples by default:
if nargin < 2 || isempty(n)
    n = 2000;
end

% Max tested frame duration is 0.2 secs by default:
if nargin < 3 || isempty(maxFlipDelta)
    maxFlipDelta = 0.2;
end

% Don't use external measurement hardware for ground-truth by default:
if nargin < 4 || isempty(hwmeasurement)
    hwmeasurement = 0;
end

% Use X-Screen with highest number by default:
if nargin < 5
    screenNumber = [];
end

% Disable OpenML swap scheduling, as vblank count based scheduling is
% meaningless in VRR mode. Instead simply wait for target presentation
% time to be reached, then glXSwapBuffers():
Screen('Preference','ConserveVRAM', 524288);

try
    % Standard settings and key mappings. ESCape key aborts early, before
    % n samples are done:
    PsychDefaultSetup(1);
    RestrictKeysForKbCheck(KbName('ESCAPE'));

    % Max X-Screen selected by default:
    if isempty(screenNumber)
        screenNumber = max(Screen('Screens'));
    end

    % Enforce some debug output with RandR modeline of current mode:
    oldverbo = Screen('Preference','Verbosity', 5);
    Screen('FrameRate', screenNumber);
    Screen('Preference','Verbosity', oldverbo);

    % Query height of screen for our simple animation:
    [~, screenheight] = Screen('Windowsize', screenNumber);

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
    w = PsychImaging('OpenWindow', screenNumber, 0);
    HideCursor(w);
    [width, height] = Screen('Windowsize', w);

    % Get window info struct about onscreen fullscreen window:
    winfo = Screen('GetWindowInfo', w);

    if hwmeasurement == 2 || hwmeasurement == 3
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

    % Switch to FIFO realtime-priority and memory locking to reduce timing jitter
    % and interruptions caused by other applications and the operating system itself:
    Priority(MaxPriority(w));

    % Get minimal frame duration 'ifi' from calibration loop:
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
    tSecondary = ts;
    sodpixx = ts;
    boxTime = ts;

    % Perform some initial Flip to get us in sync with vblank:
    % tvbl is the timestamp (system gettimeofday time in seconds)
    % when post-flip scanout started, as returned by kernel/OpenML sync control.
    tvbl = Screen('Flip', w);
    tStart = tvbl;
    tdeadline = tvbl;

    % minFlipDelta == minimum frame duration in seconds == measured max fps:
    minFlipDelta = ifi;

    % minVRR == minimum refresh rate in VRR range:
    minVRR = 33;

    durRange = maxFlipDelta - minFlipDelta;

    % Phase of sine wave for sine-wave frame timing pattern:
    phase = 0 %0.25;

    % Test-loop: Collects n samples.
    for i=1:n
        % Update simulation time for sine wave etc.:
        t = i / n * 1;

        % Select frame timing test function:
        switch (test)
            case {'sine'}
                % Sine-wave pattern of changing frame duration: Goes below min VRR.
                delay = minFlipDelta + durRange * (sin(2 * pi * (t + phase)) + 1) / 2;
            case {'random', 'rand'}
                % Randomized within VRR range:
                delay = minFlipDelta + (1 / minVRR - minFlipDelta) * rand();
            case {'upsweep'}
                % Linear increase of frame duration: Goes below min VRR.
                delay = minFlipDelta + durRange * i / n;
            case {'downsweep'}
                % Linear decrease of frame duration: Goes below min VRR.
                delay = minFlipDelta + durRange * (n - i) / n;
            case {'upstep'}
                % Stepwise increase of frame duration, 2 msecs every 60 flips: Goes below min VRR.
                delay = minFlipDelta + 2 * floor(i / 60) / 1000;
            case {'downstep'}
                % Stepwise increase of frame duration, 2 msecs every 60 flips: Goes below min VRR.
                delay = minFlipDelta + 2 * floor((n - i) / 60) / 1000;
            case {'const', 'constant'}
                % Constant frame duration. Hard-code as needed:
                delay = minFlipDelta * 1.5;
        end

        % Compute absolute deadline for next swapbuffers/flip request, relative
        % to previous completed flip, with some constant offset removed to compensate
        % for average scheduling delay Flip -> Mesa -> X-Server -> kms driver:
        %tdeadline = tvbl + delay - (1.863093)/ 1000; % Polaris 11 machine
        %tdeadline = tvbl + delay - (1.853770)/ 1000; % Bob / Sea Islands machine
        tdeadline = tvbl + delay - (1.6909) / 1000;  % Raven / DCN-1 machine

        % Store requested delay for i'th trial in td:
        td(i) = delay;

        if hwmeasurement == 1
            % Ask for a Datapixx onset timestamp for next 'Flip':
            PsychDataPixx('LogOnsetTimestamps', 1);
        end

        if (hwmeasurement == 2 || hwmeasurement == 3) && i == 1
            PsychRTBox('Start', rtbox, 0);
        end

        if hwmeasurement == 2 || hwmeasurement == 3
            %PsychRTBox('EngageTRTrigger', rtbox);
        end

        if hwmeasurement == 2
            % Draw green bar at top of screen:
            Screen('FillRect', w, [0 255 0], [0 0 width 5]);
        end

        % Request flip/OpenGL glXSwapBuffers at time tdeadline.
        % Return the driver reported timestamp when post-flip scanout starts in
        % tvbl and so(i).
        % The rasterbeam-position (scanline) when the measurement was taken is
        % returned in beampos(i), or -1 on unsupported hw/sw combo.
        % The time when flip returned to Octave is returned in flipfin(i).
        [tvbl, so(i), flipfin(i), missest(i), beampos(i)] = Screen('Flip', w, tdeadline);
        if hwmeasurement == 1
            % Ask for a Datapixx onset timestamp from last 'Flip':
            [boxTime(i), sodpixx(i)] = PsychDataPixx('GetLastOnsetTimestamp'); %#ok<ASGLU>
            dpixxdelay(i) = GetSecs;
        end

        % Record timestamp [again - redundant on Linux] for later use:
        ts(i) = tvbl;

        % fprintf('pflip completed - flip ts = %f msecs.\n', 1000 * (winfo.RawSwapTimeOfFlip - tvbl));

        % Draw some simple stim for next frame of animation: A rectangle that moves over the screen.
        pos = mod(i, screenheight);
        Screen('FillRect', w, [255 0 255], [pos+20 pos+20 pos+400 pos+400]);

        % Give user a chance to abort the test anytime by pressing ESC key:
        if KbCheck
            break;
        end

        if hwmeasurement == 2 || hwmeasurement == 3
            %[nw, tpost] = IOPort('Write', rtboxport, 'f', 1);
        end

    end

    % calculate clock skew corrected Datapixx onset timestamps
    if hwmeasurement == 1
        sodpixx = PsychDataPixx('BoxsecsToGetsecs', boxTime);
    end

    if hwmeasurement == 2 || hwmeasurement == 3
        Screen('Flip', w);
        Screen('Flip', w);
        WaitSecs(0.5);
        PsychRTBox('Stop', rtbox);
        [tmpsecs, evt, rtboxsecs] = PsychRTBox('GetSecs', rtbox);
        dpixxdelay = so; % Fake a zero delay for RTBox.
        boxTime(1:length(rtboxsecs)) = rtboxsecs;
        % Calibrated remapping of rtboxsecs to GetSecs secs in sodpixx:
        sodpixx(1:length(tmpsecs)) = PsychRTBox('BoxsecsToGetsecs', rtbox, rtboxsecs);

        if hwmeasurement ~= 3
            PsychRTBox('Close', rtbox);
        else
            % Drop our ref for the simulated RTBox of Bits#
            BitsPlusPlus('Close');
            clear PsychRTBox;
        end
    end

    % Normal scheduling:
    Priority(0);

    % Reset keys
    RestrictKeysForKbCheck([]);

    % Close window and clean up:
    sca;

    if hwmeasurement == 2
        % Switch Videoswitcher back to standard RGB passthrough:
        PsychVideoSwitcher('SwitchMode', screenNumber, 1);
    end

    % Restrict to actual number of collected samples:
    n = i;
    ts = ts(1:n);
    so = so(1:n);
    flipfin = flipfin(1:n);
    missest = missest(1:n);
    beampos = beampos(1:n);
    td = td(1:n);
    dpixxdelay = dpixxdelay(1:n);
    tSecondary = tSecondary(1:n);
    sodpixx = sodpixx(1:n);
    boxTime = boxTime(1:n);

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

    % Green = Error between requested and actual frame duration for each flip:
    plot(deltaT - td, 'g');

    % Red = Median error over whole run:
    plot(ones(1,n) * median(deltaT - td), 'r');
    fprintf('Avg diff target vs. true: %f msecs. stddev %f msecs. Median diff %f msecs.\n', ...
            mean(deltaT - td), std(deltaT - td), median(deltaT - td));
    title('Delta between successive Flips in milliseconds [red=requested, blue=actual]:');
    text(0,-10, 'Difference (green), median error (horizontal red):');
    text(0,-30, sprintf('Avg diff target vs. true: %f msecs. stddev %f msecs. Median diff %f msecs.\n', ...
                        mean(deltaT - td), std(deltaT - td), median(deltaT - td)));
    grid on;
    hold off;

    % Save the plot as PDF file into current working directory:
    fname = sprintf('VRRTestResult_%s.pdf', test);
    print(fname, '-dpdf');
    fprintf('Plot saved to working directory as %s\n', fname);

    figure;
    hist(deltaT - td, 100);
    title('Histogram of difference between requested and actual frame duration [msecs]');

    % Figure 2 shows the recorded beam positions if hw and sw setup allowed that:
    if winfo.VBLEndline ~= -1
        figure;
        plot(beampos);
        title('Rasterbeam position when timestamp was taken (in scanlines):');
    end

    % Optionally plot execution/scheduling delay from predicted start of post-flip
    % scanout and return of control to this script - a measure of general scheduling
    % delay:
    if false
        if isequal(ts, so)
            % Same info in vbltime and stimulus onset time. Only
            % do one plot and label it in a less confusing manner:
            figure;
            plot((flipfin - so)*1000);

            if IsLinux && (Screen('Preference', 'VBLTimestampingmode') == 4)
                % Linux mode 4: X11/OpenML or Wayland presentation feedback, "so" is stimulus onset:
                title('Time delta between stimulus onset and return of Flip in milliseconds:');
            elseif (IsLinux || IsOSX) && ismember(Screen('Preference', 'VBLTimestampingmode'), [1, 3]) && (winfo.VBLCount > 0)
                % Linux or OSX, vbl timestamping requested and apparently working. "so" is vblank time:
                title('Time delta between start of VBL and return of Flip in milliseconds:');
            else
                % Windows or other os'es without working high precision timestamping: "so" is raw timestamp:
                title('Time delta between return from swap completion and return of Flip in milliseconds:');
            end
        else
            % Figure 3 shows difference in ms between finish of Flip and estimated
            % start of VBL time:
            figure
            plot((flipfin - ts)*1000);
            title('Time delta between start of VBL and return of Flip in milliseconds:');

            % Figure 4 shows difference in ms between finish of Flip and estimated
            % stimulus-onset:
            figure
            plot((flipfin - so)*1000);
            title('Time delta between stimulus onset and return of Flip in milliseconds:');
        end
    end

    % If Datapixx hardware measurement was used, plot difference between kernel
    % reported flip times and ground-truth from external measurement hardware:
    if hwmeasurement
        figure;
        grid on;

        if length(sodpixx) ~= length(so)
            cutoffn = min(length(sodpixx), length(so));
            fprintf('Warning: Missing data %i vs. %i, only using first %i.\n', length(sodpixx), length(so), cutoffn);
            sodpixx = sodpixx(1:cutoffn);
            so = so(1:cutoffn);
        end

        plot((so - sodpixx) * 1000);
        title('Time delta in msecs onset according to Flip - onset according to measurement hw:');
        fprintf('Average discrepancy between Flip timestamping and hardware is %f msecs, stddev = %f msecs.\n', mean((so - sodpixx) * 1000), std((so - sodpixx) * 1000));

        if (length(dpixxdelay) == length(so)) && any(dpixxdelay - so)
            figure;
            plot((dpixxdelay - so) * 1000);
            title('Time delta between stimulus onset and return of hardware timestamping in milliseconds:');
        end

        if hwmeasurement == 2
            sodpixx = sodpixx(1:min(find(sodpixx == 0))-1);
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
    sca;
    psychrethrow(psychlasterror);
end

return
