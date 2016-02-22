function FlipTimingWithRTBoxPhotoDiodeTest(configFile, targetFolder)
% FlipTimingWithRTBoxPhotoDiodeTest([configFile][, targetFolder])
%
% Test visual stimulus onset timing accuracy and visual stimulus onset
% timestamping precision and robustness under varying loads, conditions and
% modes of operation. This requires one of the supported external
% measurement devices to provide the "ground truth" for true stimulus onset
% times. Currently supported: UCST RTBox with photo-diode, VPixx Inc. DataPixx/
% ViewPixx/ProPixx, UBW32/Bitwhacker + UCST VideoSwitcher. The script can also
% be used without external equipment to just test stimulus onset accuracy with
% only indirect test of timestamping.
%
% This documentation is incomplete for now, good luck!
%
% 'configFile' Filename of benchmark configuration file. If none is specified,
% the file fliptimingdefaultconfig.mat from the Psychtoolbox/PsychTests/TestConfigurations/
% folder is used for some reasonable default testing setup.
%
% 'targetFolder' Target folder for result files. If none is specified, a reasonable
% location is selected if that location exists and is writable, if that does not work,
% a fallback location is selected, if that does not work, the users home directory is
% selected. If a folder is specified, that folder is used if it is writable, otherwise
% the users home directory is tried as target.
%
% Mandatory variables in the config file, part of the struct variable 'conf':
% ---------------------------------------------------------------------------
%
% conf.Stereo              = Stereomode to use.
% conf.Priority            = Realtime priority to use.
% conf.DWMEnabled          = 1,0,-1 = On, Off, Auto
% conf.SetForegroundWindow = 1,0,-1 = On, Off, Auto
% conf.VBLTimestampingMode = Mode of high-precision timestamping.
% conf.CheckAsyncFlip      = 0,1,2 = Off-Use sync flip, 1 = Use async flip, 2 = Use async flip with polling, 3 = Use async flip with Waituntilasyncflipcertain.
%
% Per trial parameters for trial i:
%
% conf.waitFramesSched(i)  = Number of ifi's to wait before onset in trial i.
% conf.loadjitter(i)       = Max. random cpu load jitter to apply in frame i.
% conf.gpuLoad(i)          = Number of rects to draw in frame i (GPU load)

% History:
% xx.10.2009  mk  Written.
% 25.10.2015  mk  Try to improve selection of storage location for result file.
%                 Try to select reasonable default file if none is specified.
%                 Allow to override target folder location for results file.
%                 Is this an improvement? I don't know.

global sd;

AssertOpenGL;

if (nargin < 1) || isempty(configFile)
    configFile = [ PsychtoolboxRoot 'PsychTests' filesep 'TestConfigurations' filesep 'fliptimingdefaultconfig.mat' ];
end

if ~exist(configFile, 'file')
    error('configFile ''%s'' does not exist or inaccessible!', configFile);
end

% Get basepath of config file: Will use it as basepath for result file:
[basepath configBaseFile] = fileparts(configFile);
if isempty(basepath) || strcmp(basepath, '.')
    % Replace empty basepath or curdir basepath by fully qualified
    % current working directory:
    basepath = pwd;
end

% Override basepath with provided targetFolder, if any:
if (nargin >= 2) && ~isempty(targetFolder)
    basepath = targetFolder;
end

% Is it the standard path for config files?
if strcmp([PsychtoolboxRoot 'PsychTests' filesep 'TestConfigurations'], basepath)
    % Yes. Then assume target folder is our standard folder in PsychTests.
    % Does this folder exist as a target for our result files and is writable by us?
    [fileex, fprops] = fileattrib([PsychtoolboxRoot 'PsychTests' filesep 'TimingtestResults']);
    if fileex && fprops.directory && fprops.UserWrite
        % Yes. Use it as target:
        basepath = [PsychtoolboxRoot 'PsychTests' filesep 'TimingtestResults'];
        fprintf('Will store results into standard TimingtestResults folder: %s\n', basepath);
    else
        fprintf('\nStandard TimingtestResults folder ''%s'' does not exist or is not writable!\n', ...
                [PsychtoolboxRoot 'PsychTests' filesep 'TimingtestResults']);
        fprintf('Will store to folder which contains the config file instead.\n');
        fprintf('Abort now, if you do not want this.\n\n');
    end
end

% Check again if selected basepath is writable by us:
[fileex, fprops] = fileattrib(basepath);
if ~fileex || ~fprops.directory || ~fprops.UserWrite
    % No. Just store into the users home directory:
    fprintf('Target folder for timing test results ''%s'' does not exist or is not writable.\n', basepath);
    basepath = PsychHomeDir;
    fprintf('Will store result files into your home directory ''%s'' instead as a fallback.\n', basepath);
end

% Load configuration: This will define a struct 'conf' with all
% configuration settings:
load(configFile);
if ~exist('conf','var')
    error('Invalid configFile %s given! Not in a sane format!!', configFile);
end

% Default sound playback to off if undefined:
if ~isfield(conf, 'withSound') %#ok<NODEF>
    conf.withSound = 0;
end

% Default to no 'Drawingfinished' call, if not spec'd in config:
if ~isfield(conf, 'drawingFinished')
    conf.drawingFinished = 0;
end

% Initialize the 'res' result struct with it:
res = conf;

res.measurementType = input('Measure with (p)hotodiode, (v)ideoswitcher or (d)atapixx? Or don''t measure (n)? ', 's');
useRTbox = [];
if strcmpi(res.measurementType, 'p')
    useRTbox = 1;
end
if strcmpi(res.measurementType, 'v')
    useRTbox = 0;
end
if strcmpi(res.measurementType, 'd')
    useRTbox = -1;
end
if strcmpi(res.measurementType, 'n')
    useRTbox = -1000;
end
if isempty(useRTbox)
    error('Invalid measurement method given!');
end

% Build filename for result file: Based on configname, machinename, date
% and time:
res.configFile = configFile;

res.OSName = OSName;
comp = Screen('Computer');
res.computer = comp;
if isfield(comp, 'system') 
    res.OSSystem = comp.system;
else
    res.OSSystem = 'Linux2.6';
end

if isfield(comp, 'machineName')
    res.machineName = comp.machineName;
    res.machineName(isspace(res.machineName)) = '_';
else
    res.machineName = input('What is the machines name? ', 's');
end

res.secondaryCPULoad = input('Is this a run with high secondary CPU load? [y/n] ', 's');
res.secondaryGPULoad = input('Is this a run with high secondary GPU load? [y/n] ', 's');
res.isMultiDisplay   = input('Is this a multi-display setup? [y/n] ', 's');
res.maxGPULoadFrames = str2double(input('Number of rectangles for max GPU load? ','s'));
res.Comments         = input('Any comments to add? ', 's');

res.mydate = datestr(clock);
res.mydate(isspace(res.mydate)) = '_';
res.mydate(res.mydate == ':') = '-';

res.outFilename = sprintf('%s/Res_%s_On_%s_at_%s.mat', basepath, configBaseFile, res.machineName, res.mydate);
[foo, res.outFile] = fileparts(res.outFilename);
fprintf('Will write results to file "%s"\n[path: %s]\n', res.outFile, res.outFilename);

screenId = str2num(input('Which screen? ', 's')); %#ok<ST2NM>
screens = Screen('Screens');
if isempty(screenId)
    screenId = max(screens);
end

if ~ismember(screenId, screens)
    error('Invalid screenId %i! No such screen available.', screenId);
end

res.screenId = screenId;
fprintf('Running on screen %i.\n', res.screenId);
res.Rect = Screen('Rect', screenId);
res.GlobalRect = Screen('GlobalRect', screenId);

% Prepare conserveVRAM override settings:
conserveVRAM = 0;
%conserveVRAM = 4096;
%Screen('Preference', 'VBLEndlineOverride', 1249);
if conf.DWMEnabled == 0
    % Disable DWM:
    conserveVRAM = conserveVRAM + 2^17; % = kPsychDisableAeroDWM
    fprintf('DWM forced OFF.\n');
end
    
% DWM enable state: 1 = On, 0 = Off, -1 = Auto-Select
if conf.DWMEnabled == 1
    % Forecefully enable DWM: 
    conserveVRAM = conserveVRAM + 16384; % = kPsychUseCompositorForFullscreenWindows
    fprintf('DWM forced ON, if the os allows it.\n');
end
    
% SetForegroundWindow enable state: 1 = On, 0 = Off, -1 = Auto-Select
if conf.SetForegroundWindow == 0
    % Disable SetForegroundWindow:
    conserveVRAM = conserveVRAM + 2^18; % = kPsychPreventForegroundWindow
    fprintf('Fullscreen window workarounds on Windows forced OFF.\n');
end
    
if conf.SetForegroundWindow == 1
    % Forecefully enable SetForegroundWindow: 
    conserveVRAM = conserveVRAM + 128; % = kPsychEnforceForegroundWindow
    fprintf('Fullscreen window workarounds on Windows foreced ON.\n');
end

% Apply overrides and bugfixes:
Screen('Preference', 'ConserveVRAM', conserveVRAM);
res.conserveVRAM = conserveVRAM;

if IsLinux && (conf.VBLTimestampingMode == 1)
    conf.VBLTimestampingMode = 4;
end

%conf.VBLTimestampingMode = 3;
%res.VBLTimestampingMode = conf.VBLTimestampingMode;
% VBLTimestampingMode: configVBLTimestampingMode
% -1 = Disable high-precision timestamping.
%  0 = Beamposition only.
%  1 = Beamposition with auto-fallback to VBL-IRQ method in case of failure.
%  2 = Like 1, but with permanent consistency checking between both.
%  3 = VBL-IRQ only (override), or any similar mechanism on non-OS/X.
%  4 = OpenML timestamping, with auto-fallback to 1 in case of failure.
% [] = Auto-Select (Default)
Screen('Preference', 'VBLTimestampingMode', conf.VBLTimestampingMode);
fprintf('Enabling VBLTimestampingMode %i.\n', conf.VBLTimestampingMode);

try
    if useRTbox == -1
        % Setup Datapixx mode for timestamping:
        PsychImaging('PrepareConfiguration');
        PsychImaging('AddTask', 'General', 'UseDataPixx');
        [w, winrect] = PsychImaging('OpenWindow', res.screenId, 0, [], [], [], conf.Stereo);
    else
        [w, winrect] = Screen('OpenWindow', res.screenId, 0, [], [], [], conf.Stereo);
    end
    res.winfo = Screen('GetWindowInfo', w);
    
    % Switch to selectable realtime-priority level to reduce timing jitter
    % and interruptions caused by other applications and the operating
    % system itself:
    if IsOSX && conf.Priority > 0
       conf.Priority = 9;
    end
    
    Priority(conf.Priority);
    res.Priority = round(Priority);
    if res.Priority ~= conf.Priority
        fprintf('WARNING: Effective Priority %i is not equal to requested Priority %i !\n', res.Priority, conf.Priority);
    else
        fprintf('Running at Priority %i.\n', res.Priority);
    end
    
    % Query nominal framerate as returned by Operating system:
    % If OS returns 0, then we assume that we run on a flat-panel with
    % fixed 60 Hz refresh interval.
    nominalFramerate=Screen('NominalFramerate', w);
    if (nominalFramerate==0)
        nominalFramerate=60;
    end;
    fprintf('Nominal Hz %f.\n', nominalFramerate);
    
    % Nominal refresh interval:
    ifiNominal= 1 / nominalFramerate;
    fprintf('The refresh interval reported by the operating system is %2.5f ms.\n', ifiNominal*1000);
    res.ifiNominal = ifiNominal;
    
    % Query calibrated refresh rate and stats:
    [ ifi nvalid stddev ]= Screen('GetFlipInterval', w);
    fprintf('Measured refresh interval, as reported by "GetFlipInterval" is %2.5f ms. (nsamples = %i, stddev = %2.5f ms)\n', ifi*1000, nvalid, stddev*1000);
    res.ifi = ifi;
    res.nvalid = nvalid;
    res.stddev = stddev;
    
    % Number of trials to perform:
    n = size(conf.waitFramesSched, 2);
    
    % Init data-collection arrays for collection of n samples:
    res.rawFlipTime     = zeros(1, n);
    res.finishFlipTime  = zeros(1, n);
    res.vblFlipTime     = zeros(1, n);
    res.onsetFlipTime   = zeros(1, n);
    res.measuredTime    = zeros(1, n);
    res.beamPosition    = zeros(1, n);
    res.skipEstimate    = zeros(1, n);
    res.failFlag        = zeros(1, n);
    res.timingLoad      = zeros(1, n);
    res.gpuLoad         = zeros(1, n);
    res.targetWhentime  = zeros(1, n);
    res.predictedOnset  = zeros(1, n);
    res.waitFramesSched = zeros(1, n);
    res.asyncPredictFlipTime = zeros(1, n);
    res.asyncPredictOnsetFlipTime = zeros(1, n);
    res.asyncSwapCertainTime = zeros(1, n);
    res.swapRequestSubmissionTime = zeros(1, n);
    res.TimeAtSwapBuffers = zeros(1, n);
    res.TimePostSwapBuffers = zeros(1, n);
    
    % Compute random load distribution for provided loadjitter values:
    res.timingLoad      = rand(size(conf.loadjitter)) * ifi .* conf.loadjitter;

    % Use GPU load "as is":
    res.gpuLoad         = conf.gpuLoad;
        
    % Precompute arrays for creating GPU load:
    ww=RectWidth(winrect);
    wh=RectHeight(winrect);
    sizeX=80;
    sizeY=80;
    
    % Generate a matrix which specs n filled rectangles, with randomized
    % position, color and (dot-,line-)size parameter
    for j = 1:ceil(res.maxGPULoadFrames * max(conf.gpuLoad))
        posx = rand * ww;
        posy = rand * wh;
        myrect(j, 1:4) = floor([ posx, posy, posx + rand * sizeX, posy + rand * sizeY]); %#ok<AGROW>
    end

    if j > 0
        myrect = transpose(myrect);
    end
    
    % Hide the cursor!
    HideCursor;
    
    % Perform some initial Flip to get us in sync with retrace:
    % tvbl is the timestamp (system time in seconds) when the retrace
    % started. We need it as a reference value for our WaitBlanking
    % emulation:
    Screen('FillRect', w, 0);
    tvbl=Screen('Flip', w);

    WaitSecs(1);
    
    if useRTbox ~= -1000
        if useRTbox == 1
            rtbox = PsychRTBox('Open'); %, 'COM5');

            % Query and print all box settings inside the returned struct 'boxinfo':
            res.boxinfo = PsychRTBox('BoxInfo', rtbox);
            disp(res.boxinfo);

            % Enable photo-diode input of box, and only that one:
            PsychRTBox('Disable', rtbox, 'all');
            PsychRTBox('Enable', rtbox, 'light');
            WaitSecs(1);

            % Clear receive buffers to start clean:
            PsychRTBox('Clear', rtbox);

            if ~IsWin
                % Hack: Enable async background reads to speedup box operations:
                IOPort ('ConfigureSerialport', res.boxinfo.handle, 'BlockingBackgroundRead=1 StartBackgroundRead=1');
            end
        else
            if useRTbox ~= -1
                % Open with a "debounce time" of 1.5 ifi's:
                bwh = BitwhackerBox('Open', [], [], ifi * 1.5);
                res.boxinfo = BitwhackerBox('Status', bwh);
                disp(res.boxinfo);

                Screen('FillRect', w, 0);
                tvbl=Screen('Flip', w);

                WaitSecs(1);

                % Clear receive buffers to start clean:
                BitwhackerBox('Clear', bwh);
            else
                res.boxinfo = 'Datapixx measurement';
            end
        end
    end
    
    if conf.withSound > 0
        wavfilename = [ PsychtoolboxRoot 'PsychDemos' filesep 'SoundFiles' filesep 'funk.wav'];
        % Read WAV file from filesystem:
        [y, freq ] = psychwavread(wavfilename);
        wavedata = y';
        nrchannels = size(wavedata,1); % Number of rows == number of channels.

        % Perform basic initialization of the sound driver:
        InitializePsychSound;

        % Open the default audio device [], with default mode [] (==Only playback),
        % and a required latencyclass of zero 0 == no low-latency mode, as well as
        % a frequency of freq and nrchannels sound channels.
        % This returns a handle to the audio device:
        pahandle = PsychPortAudio('Open', [], [], 0, freq, nrchannels);

        % Fill the audio playback buffer with the audio data 'wavedata':
        PsychPortAudio('FillBuffer', pahandle, wavedata);

        % Start audio playback for 'repetitions' repetitions of the sound data,
        % start it immediately (0) and wait for the playback to start:
        PsychPortAudio('Start', pahandle, 0, 0, 1);
    end
        
    WaitSecs(1);

    % Flash screen in full intensity white:
    if useRTbox ~= 0
        Screen('FillRect', w, 255);
    else
        Screen('DrawLine', w, [255 255 255], 0, 1, 1000, 1, 5);
    end
    
    if useRTbox == -1
        % Datapixx: Enable single-shot logging of next flip-op:
        PsychDataPixx('LogOnsetTimestamps', 1);
    end
    
    % Test-loop: Collects n samples.
    for i=1:n
        % Presentation time calculation for waiting 'conf.waitFramesSched(i)' monitor refresh
        % intervals before flipping front- and backbuffer:
        res.waitFramesSched(i) = conf.waitFramesSched(i);
        tdeadline = tvbl + (res.waitFramesSched(i) - 0.5) * ifi;

        % If conf.waitFramesSched(i) == 0, flip on next retrace. This should be the same
        % as conf.waitFramesSched(i) == 1, but might make a difference in robustness if
        % the stimulus is **very** complex and the load for the system is at
        % the limit that it can handle in a single video-refresh interval.
        if res.waitFramesSched(i) == 0
            % If user supplied conf.waitFramesSched(i)=0, we force tdeadline=0, so Flip
            % will actually ignore the deadline and just Flip at the next
            % possible retrace...
            tdeadline=0;
        end;

        % Store specified target 'when' onset time:
        res.targetWhentime(i) = tdeadline;
        
        % Predict onsettime, based of current information and request:
        res.predictedOnset(i) = PredictVisualOnsetForTime(w, tdeadline, ifi);
        
        % Do the flip relevant for measurement: Store time at submission.
        res.swapRequestSubmissionTime(i) = GetSecs;
        if conf.CheckAsyncFlip == 0
            % Synchronous standard flip:
            [ tvbl res.onsetFlipTime(i) res.finishFlipTime(i) res.skipEstimate(i) res.beamPosition(i)] = Screen('Flip', w, tdeadline);
        else
            % Async background flip:
            Screen('AsyncFlipBegin', w, tdeadline);
            
            % Waituntilasyncflipcertain?
            if conf.CheckAsyncFlip == 3
                % Yes: Wait until async flip confirmed to be imminent by GPU:
                [res.asyncPredictFlipTime(i) res.asyncPredictOnsetFlipTime(i) res.asyncSwapCertainTime(i)] = Screen('WaitUntilAsyncFlipCertain', w);
            end
            
            if conf.CheckAsyncFlip ~= 2
                % Wait blocking for async-flip completion, return timestamps:
                [ tvbl res.onsetFlipTime(i) res.finishFlipTime(i) res.skipEstimate(i) res.beamPosition(i)] = Screen('AsyncFlipEnd', w);
            else
                % Wait polling for async-flip completion:
                tvbl = 0;
                while tvbl == 0
                    [ tvbl res.onsetFlipTime(i) res.finishFlipTime(i) res.skipEstimate(i) res.beamPosition(i)] = Screen('AsyncFlipCheckEnd', w);
                end
            end
        end
        
        res.vblFlipTime(i) = tvbl;
        winfo = Screen('GetWindowInfo', w);
        res.rawFlipTime(i) = winfo.RawSwapTimeOfFlip;
        % This is flips internal timestamp of when the Swapbuffers call is
        % sent:
        res.TimeAtSwapBuffers(i) = winfo.TimeAtSwapRequest;
        res.TimePostSwapBuffers(i) = winfo.TimePostSwapRequest;
        
        % Perform immediate flip to blank so we don't get extraneous TTL
        % triggers:
        Screen('FillRect', w, 0);
        Screen('Flip', w);

        if useRTbox ~= -1000
            % Measure real onset time:
            if useRTbox == 1
                % Fetch sample immediately to preserve correspondence:
                mytstamp = PsychRTBox('BoxSecs', rtbox, 0.002, 0.002, 1);
                if isempty(mytstamp)
                    % Failed within expected time window. This probably due to
                    % tearing artifacts or GPU malfunction. Mark it as "tearing"
                    % and retry for 1 full more video refresh:
                    res.failFlag(i) = 1;
                    mytstamp = PsychRTBox('BoxSecs', rtbox, ifi + 0.002, ifi + 0.002, 1);
                    if isempty(mytstamp)
                        % Ok, this is fucked up. No way to recover :-(
                        res.failFlag(i) = 2;
                        res.measuredTime(i) = nan;
                    else
                        % Got something:
                        res.measuredTime(i) = mytstamp;
                    end
                else
                    % Success!
                    res.failFlag(i) = 0;
                    res.measuredTime(i) = mytstamp;
                end
                PsychRTBox('EngageLightTrigger', rtbox);
            else
                if useRTbox ~= -1
                    tdeadline = GetSecs + 0.002;
                    evt = [];
                    while isempty(evt) && (GetSecs < tdeadline)
                        evt = BitwhackerBox('GetEvent', bwh);
                    end
                    if isempty(evt)
                        % Failed within expected time window. This probably due to
                        % tearing artifacts or GPU malfunction. Mark it as "tearing"
                        % and retry for 1 full more video refresh:
                        res.failFlag(i) = 1;
                        tdeadline = GetSecs + ifi + 0.002;
                        while isempty(evt) && (GetSecs < tdeadline)
                            evt = BitwhackerBox('GetEvent', bwh);
                        end
                        if isempty(evt)
                            % Ok, this is fucked up. No way to recover :-(
                            res.failFlag(i) = 2;
                            res.measuredTime(i) = nan;
                        else
                            % Got something:
                            res.measuredTime(i) = evt.time;
                        end
                    else
                        % Success!
                        res.failFlag(i) = 0;
                        res.measuredTime(i) = evt.time;
                    end
                end
                
                if useRTbox == -1
                    % Datapixx: Fetch raw timestamp:
                    res.failFlag(i) = 0;
                    res.measuredTime(i) = PsychDataPixx('GetLastOnsetTimestamp');
                    % Reenable one-shot timestamping for next trial:
                    PsychDataPixx('LogOnsetTimestamps', 1);                    
                end
            end
        end

        % Render number of randomly placed, sized, colored filled
        % rectangles to create some load for the OpenGL driver and GPU:
        ndraw = conf.gpuLoad(i);
        if ndraw > 0
            % Convert normalized load 0.0 - 1.0 to number of items to draw:
            ndraw = ceil(res.maxGPULoadFrames * ndraw);
            Screen('FillRect', w, 0, myrect(:, 1:ndraw));
        end
        % fprintf('After Fillrect %f msecs.\n', 1000 * (GetSecs - tvbl));

        % Flash screen in full intensity white: This code in preparation
        % for next loop iterations Screen('Flip') call:
        if useRTbox
            Screen('FillRect', w, 255);
        else
            Screen('DrawLine', w, [255 255 255], 0, 1, 1000, 1, 5);
        end
        
        % Call to 'Drawingfinished' wanted?
        if res.drawingFinished > 0
            % If drawingfinished == 2, do a glFinish, instead of glFlush:
            Screen('DrawingFinished', w, 0, res.drawingFinished - 1);
        end
        
        % Sleep a random amount of time, just to simulate some work being
        % done in the Matlab loop:
        WaitSecs(res.timingLoad(i));
        
        % And give user a chance to abort the test by pressing any key:
        if KbCheck
            break;
        end;
        
        % Draw next frame...
    end;
	    
    % Shutdown realtime scheduling:
    finalprio = Priority(0) %#ok<NASGU,NOPRT>

    % Show the cursor:
    ShowCursor;
    
    % Need to perform remapping on Datapixx before Screen('CloseAll'):
    if useRTbox == -1
        % Datapixx: Remap timestamps to GetSecs time:
        [res.measuredTime, res.sd, clockRatio] = PsychDataPixx('BoxsecsToGetsecs', res.measuredTime);
        fprintf('Estimated clockRatio GetSecs vs. Datapixx is %f.\n', clockRatio);
        % No need to close device, as imaging pipeline will do it
        % automatically for us at Screen('CloseAll') time.
    end

    % Close display: If we skipped/missed any presentation deadline during
    % Flip, Psychtoolbox will automatically display some warning message on the Matlab
    % console:
    Screen('CloseAll');
    WaitSecs(1);

    if useRTbox ~= -1000
        if useRTbox == 1
            PsychRTBox('Stop', rtbox);
            PsychRTBox('Clear', rtbox);

            if ~IsWin
                % Hack: Disable async background reads:
                fprintf('Stopping background read op on box...\n');
                IOPort ('ConfigureSerialport', res.boxinfo.handle, 'StopBackgroundRead');
                IOPort ('ConfigureSerialport', res.boxinfo.handle, 'BlockingBackgroundRead=0');
                fprintf('...done, now remapping timestamps.\n');
            end

            save(res.outFilename, 'res', '-V6');
            % Remap box timestamps to GetSecs timestamps:
            res.measuredTime = PsychRTBox('BoxsecsToGetsecs', rtbox, res.measuredTime);
            fprintf('...done, saving backup copy of data, then closing box.\n');

            % Close connection to box:
            PsychRTBox('CloseAll');
        else
            if useRTbox ~= -1
                BitwhackerBox('Close', bwh);
            end
        end
    end
    
    % Store results to filesystem before we start shutdown and plotting:
    save(res.outFilename, 'res', '-V6');
    res.outFilename = [];
    
    if ~IsOSX
        % Plot all our measurement results:

        % Figure 1 shows time deltas between successive flips in milliseconds:
        % This should equal the product numifis * ifi:
        figure
        hold on
        plot(diff(res.vblFlipTime) * 1000, '-');
        ni = conf.waitFramesSched(2:end);
        ni(conf.waitFramesSched == 0) = 1;
        ni = ni * ifi;
        plot(ni * 1000, 'g--');
        title('Delta between successive Flips in milliseconds: (Dashed = expected delta)');
        hold off

        % Figure 2 shows the recorded beam positions:
        figure
        plot(res.beamPosition);
        title('Rasterbeam position when timestamp was taken (in scanlines):');

        % Figure 3 shows estimated size of presentation deadline-miss in milliseconds:
        figure
        hold on
        plot(res.skipEstimate*1000);
        plot(zeros(1,n));
        title('Estimate of missed deadlines in milliseconds (negative == no miss):');
        hold off

        % Figure 4 shows difference in ms between finish of Flip and estimated
        % start of VBL time:
        figure
        plot((res.finishFlipTime - res.vblFlipTime)*1000);
        title('Time delta between start of VBL and return of Flip in milliseconds:');

        % Figure 5 shows difference in ms between finish of Flip and estimated
        % stimulus-onset:
        figure
        plot((res.finishFlipTime - res.onsetFlipTime)*1000);
        title('Time delta between stimulus onset and return of Flip in milliseconds:');

        if ~isempty(res.measuredTime)
            % Figure 6 shows time delta between measured samples:
            figure
            plot(diff(res.measuredTime) * 1000, '-');
            title('Time delta between TTL triggers:');
        end

        figure;
    end
    
    valids = (res.failFlag == 0);
    fprintf('Measured samples %i [realvalid %i, corrupted %i] vs. Flip samples %i.\n', length(res.measuredTime), length(find(res.failFlag == 0)), length(find(res.failFlag == 1)), i);
    res.measuredTime = res.measuredTime(valids);
    res.onsetFlipTime = res.onsetFlipTime(valids);

    i = min(length(res.measuredTime), i);
    fprintf('Total of %i samples collected: %i\n', i);
    onsets = res.onsetFlipTime(1:i);
    % vbls: onsets = res.vblFlipTime(1:i);
    difference = (onsets - res.measuredTime) * 1000;
    if ~IsOSX
        plot(difference);
        title('Difference dt = FlipOnset - MeasurementBOX in msecs:');
    end
    fprintf('Avg. diff. between Flip stimulus onset time and external timestamping (flip - external [ground truth]) is %f usecs, stddev = %f usecs, range = %f usecs.\n', mean(difference) * 1000, std(difference) * 1000, range(difference) * 1000);
    
    % Count and output number of missed flip on VBL deadlines:
    numbermisses=0;
    numberearly=0;

    for i=2:n
        numifis = conf.waitFramesSched(i);
        if numifis == 0
            numifis = 1;
        end

        if (res.vblFlipTime(i) - res.vblFlipTime(i-1) > ifi * ( numifis + 0.5 ))
            numbermisses=numbermisses+1;
        end

        if (res.vblFlipTime(i) - res.vblFlipTime(i-1) < ifi * ( numifis - 0.5 ))
            numberearly=numberearly+1;
        end
    end

    % Output some summary and say goodbye...
    fprintf('PTB missed %i out of %i stimulus presentation deadlines.\n', numbermisses, n);
    fprintf('One missed deadline is ok and an artifact of the measurement.\n');
    fprintf('PTB completed %i stimulus presentations before the requested target time.\n', numberearly);
    if numberearly > 0
        fprintf('CAUTION: Completing flips too early should *never ever happen*! Your system has\n');
        fprintf('CAUTION: a serious bug or misconfiguration in its graphics driver!!!\n');
    end
    fprintf('Have a look at the plots for more details...\n');
    
    % Close the audio device:
    if exist('pahandle', 'var')
        PsychPortAudio('Close');
    end
    
    % Done.
catch
    % This "catch" section executes in case of an error in the "try" section
    % above. Importantly, it closes the onscreen window if its open and
    % shuts down realtime-scheduling of Matlab:
    ShowCursor;
    Screen('CloseAll');
    
    if ~isempty(res.outFilename)
        % Store results to filesystem before we start shutdown and plotting:
        save(res.outFilename, 'res', '-V6');
    end

    % Disable realtime-priority in case of errors.
    Priority(0);

    if useRTbox ~= -1000
        if useRTbox == 1
            if exist('rtbox','var')
                % Close connection to box:
                PsychRTBox('CloseAll');
            end
        else
            if exist('bwh','var')
                BitwhackerBox('Close', bwh);
            end
        end
    end
    
    % Close the audio device:
    if exist('pahandle', 'var')
        PsychPortAudio('Close');
    end

    psychrethrow(psychlasterror);
end %try..catch..

return
