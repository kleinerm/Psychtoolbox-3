function varargout = PsychPhotodiode(cmd, handle, varargin)
% PsychPhotodiode - Measure visual stimulus onset via photo-diodes and PsychPortAudio.
%
% Record a signal from a photo-diode connected to the audio input of a
% soundcard via PsychPortAudio, and timestamp the signal to compute a
% stimulus onset timestamp from the light flash picked up by the
% photo-diode.
%
% Iow. we abuse the sound card as a mini-oszillograph with automatic timestamping.
%
% Subfunctions and their meaning:
% -------------------------------
%
% pdiode = PsychPhotodiode('Open' [deviceIndex][, sampleRate][, lrMode]);
% - Open audio device 'deviceIndex' for recording. Audio sampling will be
% performed at the given optional 'sampleRate' with the given number of
% audio input channels. 'lrMode' (0 = Mono: Sum of left and right channel,
% 1 = Only left channel, 2 = Only right channel, 3 = Average of channels).
%
%
% PsychPhotodiode('Close', pdiode);
% - Close audio device.
%
%
% startTime = PsychPhotodiode('Start', pdiode [, maxDurationSecs=3]);
% - Start audio capture immediately (ie., with minimum possible delay on
% your system), return a 'startTime' timestamp of when capture actually started
% or will start.
%
% The option 'maxDurationSecs' allows to define an upper limit onto the duration
% of the capture operation. The operation will stop automatically after the
% given number of seconds. By default, the capture will run for 3 seconds.
%
%
% [onsetTimeSecs, audiodata, rawaudiodata] = PsychPhotodiode('WaitSignal', pdiode [, maxWaitTime=maxDurationSecs][, blocking=1]);
% - Wait for stimulus onset, as picked up from a running capture operation.
% Only call this function after a capture operation has been started via
% PsychPhotodiode('Start'), or it will error out.
%
% The optional 'maxWaitTime' asks the driver to wait at most 'maxWaitTime'
% seconds for stimulus onset. By default it will wait up to as many seconds
% as set in the PsychPhotodiode('Start').
%
% If stimulus onset has not happened yet and 'maxWaitTime' has not been
% exceeded either, then the optional 'blocking' flag will define behaviour:
% If set to 1 (or omitted), the driver will wait until stimulus onset or
% timeout. If set to 0, the driver will return with empty [] return arguments
% so you can retry later with another call to PsychPhotodiode('WaitSignal').
%
% On return from 'WaitSignal', data capture / waiting for a signal will
% stop, unless 'blocking' was set to zero for polling and nothing was
% picked up yet.
%
% 'onsetTimeSecs' is the system (=GetSecs) time when a certain signal
% strength (as set by PsychPhotodiode('TriggerLevel') or more likely by
% PsychPhotodiode('CalibrateTriggerLevel')) was first exceeded since the
% last call to PsychPhotodiode('Start'), ie. most likely due to onset of the
% visual stimulus and corresponding light flash picked up by the photo-diode
% and sent as a voltage spike to the soundcard input.
%
% 'audiodata' is the preprocessed row-vector of audiodata, used for
% actual timestamping.
%
% 'rawaudiodata' is the vector or matrix of captured audiodata. 1 row for mono
% recording, or a 2-row matrix (one row for each audio channel) in stereo
% recording modes. Each value is an audio signal sample in range [-1 ; 1].
%
%
% oldLevel = PsychPhotodiode('TriggerLevel', pdiode [, newLevel]);
% - Return old and optionally set new trigger threshold level for the
% timestamping of onset of signals in PsychPhotodiode('GetResponse').
%
% 'oldLevel' is the current/old level. 'newLevel' is the optional new
% level. Level can be between 0 and 1, with a default level of 0.1 for 10%
% of max signal intensity as trigger level.
%
%
% newLevel = PsychPhotodiode('CalibrateTriggerLevel', pdiode [, window][, triggerMult]);
% - Auto-Calibrate trigger threshold level for signal onset timestamping.
%
% Returns the found triggerLevel in 'newLevel'.
%
% If the optional 'window' onscreen window handle is omitted, then:
%
% This captures signal for 3 seconds, assuming the photo-diode(s) point to an
% idle screen with black/background color. From that an optimal trigger level
% is computed and assigned, which is 'triggerMult' times higher than the brightest
% sample in the recorded "darkness" block and then use that triggerlevel as if
% PsychPhotodiode('TriggerLevel') had been called with that auto-selected level.
% 'triggerMult' defaults to 20 if omitted.
%
% If the optional 'window' onscreen window handle is provided, then:
%
% First the window is turned completely black, 3 seconds of "darkness" are
% captured and maximum signal at black is computed. Then the window is
% turned fully bright white, 3 seconds of "lightness" are captured and
% maximum white signal is computed. Then the window goes dark again, and
% the optimal triggerLevel is computed as weighted average of dark max
% signal and white max signal, with 'triggerMult' defining the weighting
% between 0.0 and 1.0. 'triggerMult' defaults to the reasonable value 0.5
% if omitted.
%
%
% PsychPhotodiode('Stop', pdiode);
% - Stop capture as soon as possible.
%
%

% History:
% 08-Dec-2019   mk  Written.

persistent pdiodes;

if nargin < 1 || isempty(cmd)
    error('Required Subcommand missing or empty!');
end

if (nargin < 2 || isempty(handle)) && ~strcmpi(cmd, 'Open')
    error('Required photo-diode device handle missing!');
end

if strcmpi(cmd, 'Open')
    % Get deviceIndex:
    if nargin > 1 && ~isempty(handle)
        pdiode.deviceIndex = handle;
    else
        if IsWin
            % WASAPI only:
            devs = PsychPortAudio('GetDevices', 13);
        elseif IsLinux
            % ALSA only:
            devs = PsychPortAudio('GetDevices', 8);
        else
            % CoreAudio only:
            devs = PsychPortAudio('GetDevices', 5);
        end

        % Find optimal default device for this:
        hostname = Screen('Computer');
        hostname = hostname.machineName;
        pdiode.deviceIndex = [];

        % First look for Mario's preferred devices:
        for i = 1:length(devs)
            % Must have at least one input channel:
            if devs(i).NrInputChannels == 0
                continue;
            end

            % On Linux, must be a hardware device, not some virtual device:
            if ~IsLinux || ~isempty(strfind(devs(i).DeviceName, 'hw:'))
               if isempty(strfind(devs(i).DeviceName, 'C-Media USB Audio Device'))
                   % Mario's MBP 2017 "groovy" is a special case: As of Linux 5.4
                   % we don't have audio capture support for the builtin HDA codec,
                   % despite it enumerating with 2 inputs:
                   continue
               end

               pdiode.deviceIndex = devs(i).DeviceIndex;
               break;
            end
        end

        % None of the preferred devices found?
        if isempty(pdiode.deviceIndex)
            for i = 1:length(devs)
                % Must have at least one input channel:
                if devs(i).NrInputChannels == 0
                    continue;
                end

                % On Linux, must be a hardware device, not some virtual device:
                if ~IsLinux || ~isempty(strfind(devs(i).DeviceName, 'hw:'))
                   if IsLinux && strcmp(hostname, 'groovy') && ~isempty(strfind(devs(i).DeviceName, 'HDA Intel'))
                       % Mario's MBP 2017 "groovy" is a special case: As of Linux 5.4
                       % we don't have audio capture support for the builtin HDA codec,
                       % despite it enumerating with 2 inputs:
                       continue
                   end

                   pdiode.deviceIndex = devs(i).DeviceIndex;
                   break;
                end
            end
        end
    end

    % Query device properties:
    dev = PsychPortAudio('GetDevices', [], pdiode.deviceIndex);

    % Assign new optional sampleRate:
    if length(varargin) >= 1 && ~isempty(varargin{1})
        pdiode.samplerate = varargin{1};
    else
        pdiode.samplerate = dev.DefaultSampleRate;
    end

    % Assign new optional lrMode:
    if length(varargin) >= 2 && ~isempty(varargin{2})
        pdiode.lrMode = varargin{2};
    else
        pdiode.lrMode = 0;
    end

    % Default triggerLevel:
    pdiode.triggerLevel = 0.1;

    % Open capture device pdiode.deviceIndex for capture only [2], with requested
    % high-timing precisions and very low latency [3], at given samplerate and nr.
    % of input channels:
    pdiode.pa = PsychPortAudio('Open', pdiode.deviceIndex, 2, 3, pdiode.samplerate, dev.NrInputChannels);
    pdiode.channels = dev.NrInputChannels;

    pdiodes{end+1} = pdiode;
    varargout{1} = length(pdiodes);

    return;
end

if strcmpi(cmd, 'Close')
    pdiode = pdiodes{handle};
    PsychPortAudio('Close', pdiode.pa);
    pdiodes{handle} = [];

    return;
end

if strcmpi(cmd, 'Start')
    pdiode = pdiodes{handle};

    if length(varargin) >= 1 && ~isempty(varargin{1})
        maxDurationSecs = varargin{1};
    else
        maxDurationSecs = 3;
    end

    % Drain buffer:
    PsychPortAudio('GetAudioData', pdiode.pa);

    % Preallocate an internal audio recording  buffer with a capacity of 10 seconds:
    PsychPortAudio('GetAudioData', pdiode.pa, maxDurationSecs + 0.001);

    if nargout > 0
        % Return GetSecs mapped time as well:
        varargout{1} = PsychPortAudio('Start', pdiode.pa, 1, [], 1);
    else
        varargout{1} = PsychPortAudio('Start', pdiode.pa, 1, [], 0);
    end

    pdiode.maxDurationSecs = maxDurationSecs;
    pdiodes{handle} = pdiode;

    return;
end

if strcmpi(cmd, 'Stop')
    pdiode = pdiodes{handle};
    PsychPortAudio('Stop', pdiode.pa);
    return;
end

if strcmpi(cmd, 'TriggerLevel')
    pdiode = pdiodes{handle};

    varargout{1} = pdiode.triggerLevel;
    if length(varargin) >= 1 && ~isempty(varargin{1})
        pdiode.triggerLevel = varargin{1};
    end

    pdiodes{handle} = pdiode;
    return;
end

if strcmpi(cmd, 'CalibrateTriggerLevel')
    pdiode = pdiodes{handle};

    % Get onscreen window handle for presenting the calibration stimulus sequence:
    if length(varargin) >= 1 && ~isempty(varargin{1})
        win = varargin{1};
        if Screen('WindowKind', win) ~= 1
            error('PsychPhotodiode:CalibrateTriggerLevel:ERROR: Invalid window handle provided. Not an onscreen window!');
        end
    else
        win = [];
    end

    % Get trigger multiplier:
    if length(varargin) >= 2 && ~isempty(varargin{2})
        mult = varargin{2};
    else
        mult = [];
    end

    % Preallocate an internal audio recording  buffer with a capacity of 3 seconds:
    PsychPortAudio('GetAudioData', pdiode.pa, 3.001);

    if ~isempty(win)
        % Black calibration:
        [width, height] = Screen('WindowSize', win);
        Screen('FillRect', win, 0, [0, 0, width, height - 5]);
        Screen('Flip', win);
        WaitSecs(0.1);
    end

    % Record the "sound of silence" - or rather "sound of darkness" for 3 seconds:
    PsychPortAudio('Start', pdiode.pa, 1, [], 1);

    % Get the data, 3 seconds:
    audiodata = PsychPortAudio('GetAudioData', pdiode.pa, [], 3.0, 3.0);
    audiodata = PreProcess(pdiode, audiodata);

    % Stop capture, drain leftovers:
    PsychPortAudio('Stop', pdiode.pa);
    PsychPortAudio('GetAudioData', pdiode.pa);

    lowLevel = max(abs(audiodata(1, 1:floor(pdiode.samplerate * 3))));

    if ~isempty(win)
        % White calibration:
        Screen('FillRect', win, WhiteIndex(win), [0, 0, width, height - 5]);
        Screen('Flip', win);
        WaitSecs(0.1);

        % Record the "sound of light" for 3 seconds:
        PsychPortAudio('Start', pdiode.pa, 1, [], 1);

        % Get the data, 3 seconds:
        audiodata = PsychPortAudio('GetAudioData', pdiode.pa, [], 3.0, 3.0);
        audiodata = PreProcess(pdiode, audiodata);

        % Stop capture, drain leftovers:
        PsychPortAudio('Stop', pdiode.pa);
        PsychPortAudio('GetAudioData', pdiode.pa);

        highLevel = max(abs(audiodata(1, 1:floor(pdiode.samplerate * 3))));

        % Restore something less eye-burning:
        Screen('FillRect', win, 0, [0, 0, width, height - 5]);
        Screen('Flip', win);
        WaitSecs(0.1);

        % Default mult to 0.5 - the mid-point between black and white signal:
        if isempty(mult)
            mult = 0.5;
        end

        % Choose triggerLevel as weighted value between low and high level
        % for black and white screen, weighted by the multiplier:
        triggerLevel = (mult * highLevel) + ((1 - mult) * lowLevel);
    else
        % Manually controlled calibration from outside:

        % Default mult to 20x, because it suits Mario's test machine.
        if isempty(mult)
            mult = 20;
        end

        % Just take maximum value multiplied by mult:
        triggerLevel = lowLevel * mult;
    end

    if triggerLevel > 0.9
        warning('Psychtoolbox:PsychPhotodiode:CalibrateTriggerLevel', 'Auto-Selected triggerLevel %f exceeds 90% of max possible signal amplitude. Strong noise or something wrong with your setup?!?', triggerLevel);
    end

    % Assign value as new triggerLevel:
    pdiode.triggerLevel = triggerLevel;

    % Return found triggerLevel:
    varargout{1} = triggerLevel;

    pdiodes{handle} = pdiode;
    return;
end

if strcmpi(cmd, 'WaitSignal')
    pdiode = pdiodes{handle};

    % Get triggerLevel for stimulus onset:
    triggerLevel = pdiode.triggerLevel;

    if length(varargin) >= 1 && ~isempty(varargin{1})
        numSecs = varargin{1};
    else
        numSecs = pdiode.maxDurationSecs;
    end

    if length(varargin) < 2 || isempty(varargin{2})
        blocking = 1;
    else
        blocking = varargin{2};
    end
    
    % Wait or poll for requested amount of audiodata:
    while 1
        % Retrieve one msec of recorded data:
        [audiodata, absrecposition, ~, cstarttime] = PsychPortAudio('GetAudioData', pdiode.pa, [], 0.001);
        varargout{3} = audiodata;
        audiodata = PreProcess(pdiode, audiodata);

        % Compute timestamp in seconds since start of capture of when the
        % triggerLevel was exceeded the first time:
        triggerTime = find(abs(audiodata(1,:)) > triggerLevel, 1);
        triggerTime = cstarttime + (absrecposition + triggerTime) / pdiode.samplerate; 

        % Signal onset detected? Then we are done:
        if ~isempty(triggerTime)
            break;
        end

        % Nope. Timeout reached for blocking scan?
        status = PsychPortAudio('GetStatus', pdiode.pa);
        if status.RecordedSecs >= numSecs
            % Requested maximum amount of audio data scanned: Exit loop.
            break;
        else
            % Insufficient amount. If this is a polling request, we
            % simply return no result:
            if ~blocking
                varargout{1} = [];
                varargout{2} = [];
                return;
            end
        end
    end

    % Stop capture, we have what we came for:
    PsychPortAudio('Stop', pdiode.pa);

    % Drain buffer:
    PsychPortAudio('GetAudioData', pdiode.pa);

    % Return all data:
    varargout{1} = triggerTime;
    varargout{2} = audiodata;

    pdiodes{handle} = pdiode;

    return;
end

error('PsychPhotodiode: Unknown subcommand provided!');
end

function audiodata = PreProcess(pdiode, audiodata)
    if size(audiodata, 1) > 1
        switch pdiode.lrMode
            case 0
                audiodata = sum(audiodata);
            case 1
                audiodata = audiodata(1,:);
            case 2
                audiodata = audiodata(2,:);
            case 3
                audiodata = mean(audiodata);
        end
    end
end
