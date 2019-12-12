function varargout = PsychPhotodiode(cmd, handle, varargin)
% PsychPhotodiode - Measure visual stimulus onset via photo-diodes and PsychPortAudio.
%
% Record a signal from a photo-diode connected to the audio input of a soundcard
% via PsychPortAudio, and timestamp the signal to compute a stimulus onset timestamp
% from the light flash picked up by the photo-diode.
%
% Iow. we abuse the sound card as a mini-oszillograph with automatic timestamping.
%
% Subfunctions and their meaning:
% -------------------------------
%
% pdiode = PsychPhotodiode('Open' [deviceIndex][, sampleRate][, lrMode]);
% - Open audio device 'deviceIndex' for recording. Audio sampling will be
% performed at the given optional 'sampleRate' with the given number of
% audio input channels 'lrMode' (0 = Mono: Average of left and right
% channel, 1 = Only left channel, 2 = Only right channel, 3 = Stereo).
% NOTE: Currently only lrMode 1 is implemented, iow. lrMode does not yet have
%       any effect!
%  
%
% PsychPhotodiode('Close', pdiode);
% - Close audio device.
%
%
% startTime = PsychPhotodiode('Start', pdiode [, maxDurationSecs=30]);
% - Start audio capture immediately (ie., with minimum possible delay on
% your system), return a 'startTime' timestamp of when capture actually started
% or will start.
%
% The option 'maxDurationSecs' allows to define an upper limit onto the duration
% of the audio capture operation. The operation will stop automatically after the
% given number of seconds. By default, capture will run for 30 seconds.
%
%
% [onsetTimeSecs, audiodata] = PsychPhotodiode('WaitSignal', pdiode [, maxAmountSecs=current][, blocking=1][, stopCapture=0]);
% - Try to fetch available captured data from a running capture operation. Only
% call this function after a capture operation has been started via
% PsychPhotodiode('Start'), or it will error out.
%
% The optional 'maxAmountSecs' asks the driver to return exactly
% 'amountSecs' worth of audio data. By default it will return whatever
% is available at time of call. If 'amountSecs' is specified, but the
% requested amount is not yet available, the optional 'blocking' flag will
% define behaviour: If set to 1 (default), the driver will wait until the
% specified amount becomes available. If set to 0, the driver will return
% with empty [] return arguments so you can retry later. The optional
% 'stopCapture' flag if set to 1 will stop audio capture, its default
% setting of zero will keep capture running until manually stopped or until
% it stops by itself.
%
% 'audiodata' is the vector or matrix of captured audiodata. 1 row for mono
% recording, or a 2-row matrix (one row for each audio channel) in stereo
% recording modes. Each value is an audio signal sample in range [-1 ; 1].
%
% 'onsetTimeSecs' is the time since start of audio capture when a certain signal
% strength (as set by PsychPhotodiode('TriggerLevel')) was first exceeded, e.g.,
% due to onset of a visual stimulus and corresponding light flash.
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
% PsychPhotodiode('AutoSetTriggerLevel', pdiode) [, triggerMult=20];
% - Auto-Select trigger threshold level for signal onset timestamping.
% This captures signal for 3 seconds, assuming the photo-diode(s) point to an
% idle screen with black/background color. From that an optimal trigger level
% is computed and assigned, which is 'triggerMult' times higher than the brightest
% sample in the recorded "darkness" block and then use that triggerlevel as if
% PsychPhotodiode('TriggerLevel') had been called with that auto-selected level.
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
        maxDurationSecs = 30;
    end

    % Drain buffer:
    PsychPortAudio('GetAudioData', pdiode.pa);

    % Preallocate an internal audio recording  buffer with a capacity of 10 seconds:
    PsychPortAudio('GetAudioData', pdiode.pa, maxDurationSecs);

    if nargout > 0
        % Return GetSecs mapped time as well:
        varargout{1} = PsychPortAudio('Start', pdiode.pa, 1, [], 1);
    else
        varargout{1} = PsychPortAudio('Start', pdiode.pa, 1, [], 0);
    end

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

if strcmpi(cmd, 'AutoSetTriggerLevel')
    pdiode = pdiodes{handle};

    % Preallocate an internal audio recording  buffer with a capacity of 3 seconds:
    PsychPortAudio('GetAudioData', pdiode.pa, 3.001);

    % Record the "sound of silence" - or rather "sound of darkness" for 3 seconds:
    ts = PsychPortAudio('Start', pdiode.pa, 1, [], 1);

    % Get the data, 3 seconds:
    audiodata = PsychPortAudio('GetAudioData', pdiode.pa, [], 3.0, 3.0);

    % Stop capture, drain leftovers:
    PsychPortAudio('Stop', pdiode.pa);
    PsychPortAudio('GetAudioData', pdiode.pa);

    if length(varargin) >= 1 && ~isempty(varargin{1})
        mult = varargin{1};
    else
        mult = 20.0;
    end

    % Find optimal trigger level. We set a level that is 30% more than the "loudest"
    % signal sample in the "silence/darkness" block of samples:
    triggerLevel = mult * max(abs(audiodata(1, 1:floor(pdiode.samplerate * 3))));

    if triggerLevel > 0.9
        warning('Psychtoolbox:PsychPhotodiode:AutoTriggerHigh', 'Auto-Selected triggerLevel %f exceeds 90% of max possible signal amplitude. Strong noise or something wrong with your setup?!?', triggerLevel);
    end

    % Assign value as new triggerLevel:
    pdiode.triggerLevel = triggerLevel;

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
        numSecs = inf;
    end

    if length(varargin) < 2 || isempty(varargin{2})
        blocking = 1;
    else
        blocking = varargin{2};
    end
    
    % Wait or poll for requested amount of audiodata:
    while 1
        % Retrieve one msec of recorded data:
        [audiodata, absrecposition, overflow, cstarttime] = PsychPortAudio('GetAudioData', pdiode.pa, [], 0.001);

        % Compute timestamp in seconds since start of capture of when the
        % triggerLevel was exceeded the first time:
        triggerTime = min(find(abs(audiodata(1,:)) > triggerLevel));
        triggerTime = cstarttime + (absrecposition + triggerTime) / pdiode.samplerate; %#ok<MXFND>

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

    % Either requested amount of audio data available or no specific amount
    % requested. Stop capture if requested by usercode:
    if length(varargin) >= 3 && ~isempty(varargin{3}) && varargin{3} == 1
        PsychPortAudio('Stop', pdiode.pa);
    end

    % Drain buffer:
    PsychPortAudio('GetAudioData', pdiode.pa);

    % Return all data:
    varargout{1} = triggerTime;
    varargout{2} = audiodata;

    pdiodes{handle} = pdiode;

    return;
end

error('PsychPhotodiode: Unknown subcommand provided!');
