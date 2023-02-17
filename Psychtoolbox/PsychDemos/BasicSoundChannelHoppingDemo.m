function BasicSoundChannelHoppingDemo(nrChannels, device)
% BasicSoundChannelHoppingDemo([nrChannels][, device])
%
% Opens a multi-channel sound card for output, then uses audio slave devices to
% output a mono beep tone to each audio output channel in succession, "sweeping"
% or "hopping" over all speakers / output channels of the sound card.
%
% Press and hold a key to exit.
%
% Optional parameters:
%
% nrChannels = Number of channels to hop over. Tries to open first suitable audio
% device with nrChannels output channels if 'device' is omitted.
%
% device = Sound device index to choose. Will select 'device' by matching 'nrChannels'
% if omitted, or first audio device if both 'nrChannels' and 'device' are omitted.
%

% History:
% 02-Oct-22 mk  Written.

    if nargin < 1
        nrChannels = [];
    end

    if nargin < 2
        device = [];
    end

    % Initialize sound:
    InitializePsychSound;

    % Try to find suitable device for nrChannels, if specific count requested:
    if isempty(device) && ~isempty(nrChannels)
        devs = PsychPortAudio('GetDevices');
        for i=1:length(devs)
            if devs(i).NrOutputChannels == nrChannels
                device = devs(i).DeviceIndex;
                break;
            end
        end
    end

    % Open 'device' as playback master (1+8) with requested 'nrChannels' at default
    % frequency [], in high timing precision / low latency mode 1:
    pamaster = PsychPortAudio('Open', device, 1+8, 1, [], nrChannels);

    % Start it: This is a master, so won't playback any sound by itself, only
    % sound provided by potentially started slave devices:
    PsychPortAudio('Start', pamaster);

    % Query which device was chosen in the end:
    status = PsychPortAudio('GetStatus', pamaster);
    devinfo = PsychPortAudio('GetDevices', [], status.OutDeviceIndex);
    fprintf('\nWill use following output device:\n');
    disp(devinfo);

    % And the actual number of channels that device has, assuming nrChannels was
    % not already specified by the user:
    if isempty(nrChannels)
        nrChannels = devinfo.NrOutputChannels;
    end

    % Create a 1 second 400 Hz mono beep tone, usable by any [] open audio device via bufferHandle:
    bufferHandle = PsychPortAudio('CreateBuffer', [], 0.8 * MakeBeep(400, 1, status.SampleRate));

    fprintf('\nStarting infinite channel hopping loop over %i channels. Press and hold any key to exit.\n', nrChannels);

    % Ready to do the thing - "trial loop"
    KbReleaseWait;
    while ~KbCheck
        % Do a sweep over all output channels:
        for i=1:nrChannels
            % Open paout, for mono output attached to master channel i:
            % Note: This is fast, but one could also open nrChannels separate paout
            % devices, each attached to one master channel, prep them, and then inside
            % the trial loop just call 'Start' on the one one wants, instead of doing
            % a OpenSlave->FillBuffer->...->Close over and over again...
            paout = PsychPortAudio('OpenSlave', pamaster, 1, 1, i);
            PsychPortAudio('FillBuffer', paout, bufferHandle);

            % Start playback, wait for start:
            PsychPortAudio('Start', paout, 1, [], 1);

            % Wait until end of playback:
            PsychPortAudio('Stop', paout, 1);

            % Close device:
            PsychPortAudio('Close', paout);

            % Next speaker, unless key pressed:
            if KbCheck
                break;
            end
        end
    end

    % Close all audio devices:
    PsychPortAudio('Close');
    fprintf('Done. Bye!\n');
end
