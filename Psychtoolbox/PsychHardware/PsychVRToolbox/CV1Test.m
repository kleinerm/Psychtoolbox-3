function res = CV1Test(waitframes, useRTbox)
% res = CV1Test([waitframes=90][, useRTbox=0]) - A timing test script for HMDs by use of a photometer.
%
% Needs the RTBox, and a photo-diode or such, e.g., a ColorCal-II,
% connected to the TTL trigger input of a RTBox or CRS Bits#.
%
% While measured timestamps/timing on OculusVR-1 via PsychOculusVR1 is catastrophic,
% and bad on all proprietary OpenXR runtimes on Windows (OculusVR, SteamVR) and Linux
% (SteamVR), as well as with standard Monado, we get close to perfect timestamps with
% our "metrics enhanced" Monado on Linux + Mesa Vulkan drivers with timestamping support,
% as tested with both Oculus Rift CV-1 and HTC Vive Pro Eye on AMD Raven Ridge apu with
% radv + timing extension and Monado metrics mode. Errors are sub-millisecond wrt. to
% testing with a ColorCal2 and also with a Videoswitcher in simulated HMD mode.
%

if nargin < 2 || isempty(useRTbox)
    useRTbox = 0;
end

if nargin < 1 || isempty(waitframes)
    waitframes = 90;
end

% Setup unified keymapping and unit color range:
PsychDefaultSetup(2);

% Select screen with highest id as Oculus output display:
screenid = max(Screen('Screens'));
% Open our fullscreen onscreen window with black background clear color:
PsychImaging('PrepareConfiguration');
% Setup the HMD to act as a regular "monoscopic" display monitor
% by displaying the same image to both eyes. We need reliable timing and
% timestamping support for this test script:
hmd = PsychVRHMD('AutoSetupHMD', 'Monoscopic', 'TimingPrecisionIsCritical TimingSupport TimestampingSupport');
if isempty(hmd)
    error('No supported XR device found. Game over!');
end

win = PsychImaging('OpenWindow', screenid, [0 0 0]);
ifi = Screen('GetFlipInterval', win)

hmdinfo = PsychVRHMD('GetInfo', hmd)

% Render one view for each eye in stereoscopic mode, in an animation loop:
res.getsecs = [];
res.blackDelayMsecs = [];
res.vbl = [];
res.failFlag = [];
res.tBase = [];
res.measuredTime = [];

if useRTbox
    rtbox = PsychRTBox('Open'); %, 'COM5');

    % Query and print all box settings inside the returned struct 'boxinfo':
    res.boxinfo = PsychRTBox('BoxInfo', rtbox);
    disp(res.boxinfo);

    % Enable photo-diode and TTL trigger input of box, and only those:
    PsychRTBox('Disable', rtbox, 'all');
    %PsychRTBox('Enable', rtbox, 'pulse');
    WaitSecs(1);

    % Clear receive buffers to start clean:
    PsychRTBox('Stop', rtbox);
    PsychRTBox('Clear', rtbox);
    PsychRTBox('Start', rtbox);

    if ~IsWin
        % Hack: Enable async background reads to speedup box operations:
        IOPort ('ConfigureSerialport', res.boxinfo.handle, 'BlockingBackgroundRead=1 StartBackgroundRead=1');
    end
end

if 0 % ~useRTbox
    isBad = 0;
    KbReleaseWait;
    de = waitframes * ifi
    for pass=0:1
        tBase = Screen('Flip', win);
        tactual = tBase;
        t1 = GetSecs;
        for i=1:10
            Screen('FillRect', win, 0);
            DrawFormattedText(win, num2str(i), 'center', 'center', 1);
            tic;
            if pass == 0
                dt = i * de;
                tWhen = tBase + dt;
            else
                tWhen = tactual + de;
            end

            tactual = Screen('Flip', win, tWhen);
            fprintf('After flip delay %f secs : Frame %i reported %f vs. requested %f. Delta %f msecs: ', toc, i, tactual, tWhen, 1000 * (tactual - tWhen));
            if abs(tactual - tWhen) > 1.2 * ifi
                fprintf('BAD!');
                isBad = isBad + 1;
            end
            fprintf('\n');

            if KbCheck
                break;
            end
        end

        t2 = GetSecs;
        fps = i / (t2 - t1)
        WaitSecs(1);
    end
    %KbStrokeWait;
    sca;

    if isBad > 0
        fprintf('\nBAD timing in %i trials.\n', isBad);
    else
        fprintf('\nALL GOOD.\n');
    end

    return;
end

Screen('FillRect', win, 0);
tBase = Screen('Flip', win);

while ~KbCheck
    if useRTbox
        PsychRTBox('Clear', rtbox);
        %PsychRTBox('EngageLightTrigger', rtbox);
        PsychRTBox('EngagePulseTrigger', rtbox);
    end
    Screen('FillRect', win, 1);
    % Draw VideoSwitcher horizontal trigger line:
    Screen('DrawLine', win, [255 255 255], 0, 1, 1000, 1, 5);

    res.tBase(end+1) = tBase;
    res.vbl(end+1) = Screen('Flip', win, tBase + waitframes * ifi);
    Screen('FillRect', win, 0);
    tBase = Screen('Flip', win);
    res.blackDelayMsecs(end+1) = 1000 * (tBase - res.vbl(end));
    res.getsecs(end+1) = GetSecs;

    % Measure real onset time:
    if useRTbox
        % Fetch sample immediately to preserve correspondence:
        [time, event, mytstamp] = PsychRTBox('GetSecs', rtbox);
        if isempty(mytstamp)
            % Failed within expected time window. This probably due to
            % tearing artifacts or GPU malfunction. Mark it as "tearing"
            % and retry for 1 full more video refresh:
            res.failFlag(end+1) = 1;
            [time, event, mytstamp] = PsychRTBox('GetSecs', rtbox);
            if isempty(mytstamp)
                % Ok, this is fucked up. No way to recover :-(
                res.failFlag(end) = 2;
                res.measuredTime(end+1) = nan;
            else
                % Got something:
                res.measuredTime(end+1) = min(mytstamp);
            end
        else
            % Success!
            res.failFlag(end+1) = 0;
            %foo = mytstamp
            %bar = time
            %baz = event
            res.measuredTime(end+1) = min(mytstamp);
        end

        % Only online-print for large deltas between frames, to not
        % throttle stuff on that:
        if ~isempty(time) && waitframes > 30
            fprintf('DT Flip %f msecs. Box uncorrected %f msecs. Range %f msecs.\n', 1000 * (res.vbl(end) - res.tBase(end)), 1000 * (min(time) - res.tBase(end)), 1000 * range(time));
        end
    else
        fprintf('DT Flip %f msecs.\n', 1000 * (res.vbl(end) - res.tBase(end)));
    end
end

% Backup save for safety:
%save('VRTimingResults.mat', 'res', '-V6');
%KbStrokeWait;
sca;

close all;
figure;

if useRTbox
    PsychRTBox('Stop', rtbox);
    PsychRTBox('Clear', rtbox);

    if ~IsWin
        % Hack: Disable async background reads:
        fprintf('Stopping background read op on box...\n');
        IOPort ('ConfigureSerialport', res.boxinfo.handle, 'StopBackgroundRead');
        IOPort ('ConfigureSerialport', res.boxinfo.handle, 'BlockingBackgroundRead=0');
        fprintf('...done, now remapping timestamps.\n');
    end

    scanoutToPhotonOffset = 0;

    if strcmpi(hmdinfo.type, 'OpenXR') && strcmpi(hmdinfo.subtype(1:6), 'Monado')
        % Monado v21 has a hard-coded offset from hw present timestamp to reported
        % onset timestamp of 4 msecs, so correct for that to get some
        % "reference" value for simulated HMD mode on a standard display
        % monitor vs. photodiode/ColorCal measurement:
        scanoutToPhotonOffset = 0.004;

        % Monado with a simulated HMD?
        if strcmpi(hmdinfo.modelName, 'Monado: Simulated HMD')
            % This is assumed to be Mario Kleiner's simulated test setup
            % with Monado->GPU->HDMI/DP->Samsung C27HG70 monitor->ColorCal2.
            % This monitor at native modes HDMI:1920x1080@120Hz or
            % DP:2560x1440@144Hz has a reported input lag of 5 msecs from
            % signal reception to pixel switching start. Correct for that
            % offset to make data better readable (Note the counter-
            % intuitive but correct negative sign!):
            scanoutToPhotonOffset = scanoutToPhotonOffset - 0.005;
        end

        % Monado with a Oculus Rift CV-1?
        if strcmpi(hmdinfo.modelName, 'Monado: Rift (CV1) (OpenHMD)')
            % Rift CV-1 has a OLED with essentially "rolling shutter".
            % Estimated to about ~8 msecs in a 11.111 msecs / 90 Hz refresh
            % cycle. (Note the counter-intuitive but correct negative sign!):
            scanoutToPhotonOffset = scanoutToPhotonOffset - 0.008;
        end

        % Monado with a HTC Vive Pro (Eye)?
        if ~isempty(strfind(hmdinfo.modelName, 'Monado: HTC Vive Pro'))
            % HTC Vive Pro (Eye) has a OLED with essentially "rolling shutter".
            % Estimated to about ~8 msecs in a 11.111 msecs / 90 Hz refresh
            % cycle. (Note the counter-intuitive but correct negative sign!):
            scanoutToPhotonOffset = scanoutToPhotonOffset - 0.004;
        end
    end

    if strcmpi(hmdinfo.type, 'OpenXR') && ~isempty(strfind(hmdinfo.subtype, 'SteamVR'))
        % SteamVR/OpenXR with Monado Linux plugin? If so assume this is a
        % Oculus Rift CV-1 driven via Monado, although it could be some
        % other Monado supported HMD as well...
        if strcmpi(hmdinfo.modelName, 'SteamVR/OpenXR : monado')
            % Rift CV-1 has a OLED with essentially "rolling shutter".
            % Estimated to about ~8 msecs in a 11.111 msecs / 90 Hz refresh
            % cycle. (Note the counter-intuitive but correct negative sign!):
            scanoutToPhotonOffset = scanoutToPhotonOffset - 0.008;
        end

        % SteamVR/OpenXR on MS-Windows with HTC Vive Pro Eye?
        if strcmpi(hmdinfo.modelName, 'Vive OpenXR: Vive SRanipal')
            % Vive Pro Eye has a 90 Hz OLED with essentially "rolling shutter".
            % The measurement is 2 msecs earlier than flip mid-display ts
            % with the specific photometer setup of kleinerm, so lets
            % compensate for that to simplify data analysis:
            scanoutToPhotonOffset = scanoutToPhotonOffset + 0.002;
        end
    end

    res.tBase = res.tBase - scanoutToPhotonOffset;
    res.vbl = res.vbl - scanoutToPhotonOffset;

    % Primary save for safety:
    %save('OculusTimingResults.mat', 'res', '-V6');

    % Remap box timestamps to GetSecs timestamps:
    res.measuredTime = PsychRTBox('BoxsecsToGetsecs', rtbox, res.measuredTime);
    fprintf('...done, saving backup copy of data, then closing box.\n');

    plot(1:length(res.vbl), 1000 * (res.vbl - res.tBase), 1:length(res.measuredTime), 1000 * (res.measuredTime - res.tBase));
    title('Absolute measured (red) and flip (blue) relative to tbase [msecs]:')

    % Close connection to box:
    PsychRTBox('CloseAll');

    dT = res.vbl - res.measuredTime;
    dT = dT(~isnan(dT)) * 1000;

    figure;
    plot(1:length(dT), dT);
    title('Difference flip - measured [msecs]:');

    figure;
    hist(dT, 100);
    title('Difference histogram flip - measured [msecs]:');

    ifi = ifi * 1000;
    fprintf('Mean difference Flip - Measured: %f msecs [stddev %f msecs] range %f msecs [frames %f], frames %f\n', mean(dT), std(dT), range(dT), range(dT) / ifi, mean(dT) / ifi);
    res.dT = dT;
else
    plot(1:length(res.vbl), 1000 * (res.vbl - res.tBase));
    title('Corrected data [msecs]:');
end
