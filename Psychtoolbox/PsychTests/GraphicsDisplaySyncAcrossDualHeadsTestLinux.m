function GraphicsDisplaySyncAcrossDualHeadsTestLinux(screenids, nrtrials, syncmethod)
% GraphicsDisplaySyncAcrossDualHeadsTestLinux([screenids][, nrtrials=6000][, syncmethod=0])
%
% Test synchronizity between the scanout/refresh cycles of the first two
% display heads on screen 'screenids'. If 'screenids' is omitted, we test the
% two heads on the screen with maximal id. 'nrtrial' sample passes
% with a sampling interval of roughly 1 msecs are conducted. 'nrtrials'
% defaults to 6000 samples.
%
% Each sample consists of querying the current rasterbeam position on each
% display head. At the end, the samples of both heads are plotted against each
% other for comparison. On graphics cards without beamposition query support, e.g.,
% Intel graphics, RaspberryPi, or modern AMD gpu's with DCN display engine, instead
% timestamps of the end of vblank are collected and plotted against each other for
% comparison. In this case, the default duration if 'nrtrials' is omitted will be
% roughly 10 seconds of runtime.
%
% In case of older AMD gpu's with DCE display engines (AMD Radeon Vega and earlier),
% the following applies:
%
% The optional parameter 'syncmethod' if set to 1 will try to synchronize all
% display heads on AMD graphics cards, provided the 'ScreenToHead' mapping
% (see Screen('Preference','ScreenToHead',...); is set up properly for your setup.
% On a single x-screen setup this will always be the case, whereas on a multi
% x-screen setup tweaking will often be required.
% 
% A 'syncmethod' of 2 is also implemented for non-AMD graphics cards, but this
% implementation is so experimental that it is guaranteed to fail miserably in
% practice, so don't bother trying.
%
% The help text printed to the command window tells you how to interpret the plots.

% History:
% 11-Dec-2007 Written (MK).
% 25-Aug-2014 Refined (MK).
% 24-Dec-2020 Add test for systems without beampos queries, e.g., Intel gfx, AMD DCN+, RPi...

if ~IsLinux
    fprintf('\nThis test is for Linux only. Use GraphicsDisplaySyncAcrossDualHeadsTest on legacy operating systems.\n\n');
    return;
end

AssertOpenGL;

if nargin < 1
    screenids = [];
end

if isempty(screenids)
    screenids = max(Screen('Screens'));
end

if length(screenids) > 1
    error('"screenids" parameter contains index of more than 1 X-Screen?!? This tests display heads attached to one screen!');
end

if nargin < 2
    nrtrials = [];
end

if nargin < 3
    syncmethod = [];
end

if isempty(syncmethod)
    syncmethod = 0;
end

% Retrieve backup of current mappings:
oldmappings = Screen('Preference', 'ScreenToHead', screenids);

if size(oldmappings, 2) < 2
    error('Screen %i has only one display head! Can not test that for dual-head sync.', screenids(1));
end

% Open windows:
oldsync = Screen('Preference', 'SkipSyncTests', 1);
w(1) = Screen('OpenWindow', screenids(1), 0);
Screen('Preference', 'SkipSyncTests', oldsync);

% syncmethod 1 - AMD low-level sync?
if syncmethod == 1
    % Trigger a fast sync of all active display outputs attached to screen screenids(1):
    % This only works on AMD graphics cards at the moment.
    Screen('Preference', 'SynchronizeDisplays', 0, screenids(1));
end

% Driftsync aka syncmethod 2 requested?
if syncmethod == 2
    % Yes:
    driftsync = 2;
else
    % No:
    driftsync = 0;
end

if driftsync > 0
    WaitSecs(2);

    % Query current display settings for 1st display:
    oldResolution(1)=Screen('Resolution', screenids(1));

    % Switch 1st display to 60 Hz refresh rate:
    if oldResolution(1).hz ~=0
        Screen('Resolution', screenids(1), oldResolution(1).width, oldResolution(1).height, 60, oldResolution(1).pixelSize, 1);
    end
    
    % Query current display settings for 2nd display:
    oldResolution(2)=Screen('Resolution', screenids(2));

    % Switch 2nd display to 75 Hz refresh rate:
    Screen('Resolution', screenids(2), oldResolution(2).width, oldResolution(2).height, 75, oldResolution(2).pixelSize, 1);
    
    WaitSecs(2);
end

% Some beauty...
Screen('Flip', w(1), 0, 2);
Screen('FillRect', w(1), 255);
Screen('Flip', w(1), 0, 2);

if driftsync > 0
    nrtrials = 100000;
end

% Beamposition queries supported?
wfo = Screen('GetWindowInfo', w(1));
if wfo.VBLEndline == -1
    % No. Use alternative strategy based on comparing VBLTimestamps:
    sca;
    oldsync = Screen('Preference', 'SkipSyncTests', 2);

    try
        scanout = Screen('ConfigureDisplay', 'Scanout', screenids, 0);
        w(1) = Screen('OpenWindow', screenids, 128, [scanout.xStart, scanout.yStart, scanout.xStart+scanout.width, scanout.yStart+scanout.height]);
        scanout = Screen('ConfigureDisplay', 'Scanout', screenids, 1);
        w(2) = Screen('OpenWindow', screenids, 128, [scanout.xStart, scanout.yStart, scanout.xStart+scanout.width, scanout.yStart+scanout.height]);
    catch
        Screen('Preference', 'SkipSyncTests', oldsync);
        error('Openwindow(s) for test failied for some reason.');
    end

    Screen('Preference', 'SkipSyncTests', oldsync);

    if isempty(nrtrials)
        nrtrials = Screen('Framerate', screenids) * 10;
    end

    timinginfo = zeros(2, nrtrials);
    oldTime = zeros(1, 2);

    for i=1:nrtrials
        for j=1:2
            while 1
                winfo(j) = Screen('GetWindowInfo', w(j)); %#ok<AGROW>
                if abs(winfo(j).LastVBLTime - oldTime(j)) > 0.0002
                    oldTime(j) = winfo(j).LastVBLTime;
                    break;
                end
                WaitSecs('YieldSecs', 0.0005);
            end

            timinginfo(j, i) = oldTime(j);
        end
    end

    baseTime = min(timinginfo(1,1), timinginfo(2,1));
    timinginfo = timinginfo - baseTime;
    if abs(abs(timinginfo(1,1) - timinginfo(2,1)) - Screen('GetFlipInterval', w(1))) < 0.0005
        % Difference between 1st timestamp of each head is (almost) exactly 1 video
        % refresh duration. This suggests an offset between both time-series of 1
        % frame due to unlucky start of sampling. Shift them by one unit:
        timinginfo(2, 1:end-1) = timinginfo(2, 2:end);
        timinginfo = timinginfo(:, 1:end-1);
        fprintf('Shift correction performed.\n\n');
    end

    sca;

    fprintf('\n\n\nWhat you should see in the following plots for well synchronized displays:\n');
    fprintf('The "VBL timestamps difference" plot should either show a nice zero flat line or a\n');
    fprintf('flat line with a low microseconds constant offset. You may also see a few spikes, these are artifacts of\n');
    fprintf('the measurement method, and to be expected, as long as the majority of the taken\n');
    fprintf('samples constitute a flat line. Of course a bit of kind of gaussion noisyness has to be\n');
    fprintf('expected in the system, so the flat line will not be perfectly flat if you zoom into the plot.\n\n');
    fprintf('What you should not see is some clear trend in one direction, or a sawtooth-like pattern with an\n');
    fprintf('magnitude of more than a few dozen microseconds, ie. with hundreds of usecs or even msecs magnitude.\n\n');
    fprintf('That would indicate drift between the display heads due to missing sync across the heads.\n\n');

    close all;
    plot(1:nrtrials, timinginfo(1,:), 'r', 1:nrtrials, timinginfo(2,:), 'b');
    title('VBL timestamps both displays: Red = Head 1, Blue=Head 2');

    figure;
    plot(1:nrtrials, 1000 * (timinginfo(1,:) - timinginfo(2,:)));
    title('VBL timestamps difference between displays in msecs:');

    return;
end

% The following remaining code is only executed for systems with beamposition query support:
if isempty(nrtrials)
    nrtrials = 6000;
end

timinginfo = zeros(2, 2, nrtrials);
syncedattrial = -1;

% Test loop one:
for i=1:nrtrials
    % Sample in roughly 1 msec intervals:
    WaitSecs(0.001);
    
    % Sample in close sync:
    for j=1:2
        Screen('Preference', 'ScreenToHead', screenids, oldmappings(1,1), oldmappings(2,j));
        winfo(j) = Screen('GetWindowInfo', w(1), 1); %#ok<AGROW>
    end
    
    for j=1:2
        timinginfo(1, j, i) = winfo(j);
    end
    
    if driftsync > 1
        if abs(winfo(1).LastVBLTime - winfo(2).LastVBLTime) < 0.0005
            % Switch display 2 also to 60 Hz refresh:
            Screen('Resolution', screenids(2), oldResolution(2).width, oldResolution(2).height, 60, oldResolution(2).pixelSize, 1);
            driftsync = 1;
            syncedattrial = i %#ok<NOPRT>
            drawnow;
            Beeper;
        end
    end
    
    if (driftsync == 1) && (i > syncedattrial + 6000)
        stoppedattrial = i %#ok<NASGU,NOPRT>
        drawnow;
        break;
    end
end

timinginfo = timinginfo(:,:,1:i);
nrtrials = i;

% Restore default headid mapping for screen:
Screen('Preference', 'ScreenToHead', screenids, oldmappings(1,1), oldmappings(2,1));
Screen('CloseAll');

if driftsync
    % Restore original display settings:
    Screen('Resolution', screenids(1), oldResolution(1).width, oldResolution(1).height, oldResolution(1).hz, oldResolution(1).pixelSize, 0);
    Screen('Resolution', screenids(2), oldResolution(2).width, oldResolution(2).height, oldResolution(2).hz, oldResolution(2).pixelSize, 0);
end

fprintf('\n\n\nWhat you should see in the following plots for well synchronized displays:\n');
fprintf('The "drift" plots should either show a nice zero flat line or a flat line with\n');
fprintf('a small constant offset. You may also see a few spikes, these are artifacts of\n');
fprintf('the measurement method, and to be expected, as long as the majority of the taken\n');
fprintf('samples constitute a flat line. Of course a bit of kind of gaussion noisyness has to be\n');
fprintf('expected in the system, so the flat line will not be perfectly flat if you zoom into the plot.\n\n');
fprintf('What you should not see is some clear trend in one direction, or a sawtooth-like pattern.\n');
fprintf('That would indicate drift between the display heads due to missing sync across the heads.\n\n');

% Some plotting:
close all;
if (psychrange(timinginfo(1, 1, :)) > 0) && (psychrange(timinginfo(1, 2, :)) > 0)
    plot(1:nrtrials, squeeze(timinginfo(1,1,:)), 'r', 1:nrtrials, squeeze(timinginfo(1,2,:)), 'b');
    title('Beampositions for both displays: Red = Head 1, Blue=Head 2');

    figure;
    plot(1:nrtrials, squeeze(timinginfo(1,1,:)) - squeeze(timinginfo(1,2,:)));
    title('Beamposition drift between displays:');
end

return;
