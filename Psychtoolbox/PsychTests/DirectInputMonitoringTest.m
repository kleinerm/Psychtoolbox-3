function DirectInputMonitoringTest
% DirectInputMonitoringTest - Test "Zero latency direct input monitoring" feature of some sound cards.
%
% This test will currently only work on a subset of some ASIO 2.0 capable
% soundcards on Microsoft Windows operating systems with the latest
% PsychPortAudio driver and portaudio_x86 ASIO enabled plugin from the
% Psychtoolbox Wiki.
%
% The test allows you to exercise Direct input monitoring features on your
% soundcard if the card supports them.
%
% Use ESCape to exit. Space to toggle mute/unmute. 'i' key to change input
% channel to monitor, 'o' key to select output channel for monitoring. Use
% Cursor Up/Down to increase and decrease amplifier gain. Use Left/Right
% cursor keys to change stereo panning. The 'p' key puts the device into a
% fake playback mode.
%

% History:
% 2.8.2009  mk  Written.

% Preinit driver:
InitializePsychSound(1);

% Choose high level of debug output:
oldverbosity = PsychPortAudio('Verbosity', 10);

% Open auto-detected audio device in full-duplex mode for audio capture &
% playback, with lowlatency mode 1. Lowlatency mode is not strictly
% required, but has the nice side-effect to automatically assing the lowest
% latency audio device, which is an installed ASIO card on Windows or the
% ALSA audio system on Linux -- both are required for direct input
% monitoring to work:
pa = PsychPortAudio('Open', [], 1+2, 1, 48000, 2);

% Create a fake playback buffer and recording buffer, in case we need to do
% fake playback/recording:
PsychPortAudio('Fillbuffer', pa, zeros(2, 10));
PsychPortAudio('GetAudioData', pa, 1);

% Retrieve number of input- and output soundchannels for device pa:
status = PsychPortAudio('GetStatus', pa);
outdev = PsychPortAudio('GetDevices', [], status.OutDeviceIndex);
noutputs = outdev.NrOutputChannels;
inpdev = PsychPortAudio('GetDevices', [], status.InDeviceIndex);
ninputs = inpdev.NrInputChannels;

fprintf('\n\nDevice has %i input channels and %i output channels.\n\n', ninputs, noutputs);

% Select all inputchannels (-1) for monitoring. Could also spec a specific
% channel number >=0 to set monitoring settings on a per-channel basis:
inputchannel = -1;

% Output to channel 0 or channels 0 & 1 on a stereo output channel pair.
% Any even number would do:
outputchannel = 0;

% Start with maximum 12 dB gain: On Windows, values between -1 and +1 are
% valid on ASIO hardware for attenuation (-1 = -inf dB) or amplification
% (+1 = +12 dB).
if IsWin
    gain = 1.0;
end

% On OS/X we also set 12 dB gain, here the value actually specifies the
% requested dB value:
if IsOSX
    gain = 12.0;
end

% Sterat with centered output on a stero channel: Values between 0.0 and
% 1.0 select left <-> right stereo panning, 0.5 is centered:
pan = 0.5;

KbName('UnifyKeyNames');
lKey = KbName('LeftArrow');
rKey = KbName('RightArrow');
uKey = KbName('UpArrow');
dKey = KbName('DownArrow');
oKey = KbName('o');
iKey = KbName('i');
pKey = KbName('p');
space = KbName('space');
esc = KbName('ESCAPE');

KbReleaseWait;
unmute = 1;

% Set initial 'DirectInputMonitoring mode':
diResult = PsychPortAudio('DirectInputMonitoring', pa, unmute, inputchannel, outputchannel, gain, pan);
fprintf('Enabled: %i Inchannel: %i, OutChannel: %i, gain %f, stereopan %f, RC = %i\n', unmute, inputchannel, outputchannel, gain, pan, diResult);

% Lower level of debug output:
PsychPortAudio('Verbosity', oldverbosity);

% Repeat parameter change loop until user presses ESCape or error:
while 1
    % Wait for user keypress:
    [secs, keyCode] = KbStrokeWait;
    
    % Disable old setting - Mute current configuration:
    % Don't know if this is really needed or not, but let's start safe...
    % MK: Disabled for test purpose: diResult = PsychPortAudio('DirectInputMonitoring', pa, 0, inputchannel, outputchannel, gain, pan);
    
    if keyCode(esc)
        % Exit:
        break;
    end

    if keyCode(pKey)
        % Trigger fake playback & recording, in case this is needed:
        fprintf('\n\n == Fake playback & recording started: == \n\n');
        PsychPortAudio('Start', pa, 0, 0, 1);
        KbReleaseWait;

        % Disable pKey for further iterations:
        pKey = 1;
    end
    
    if keyCode(space)
        % Mute/Unmute:
        unmute = 1 - unmute;
    end

    % Change stereo panning left/right:
    if keyCode(lKey)
        pan = min(1.0, max(0.0, pan - 0.1));
    end

    if keyCode(rKey)
        pan = min(1.0, max(0.0, pan + 0.1));
    end
    
    % Change gain between -inf dB (Mute) to +12 dB on ASIO hardware:
    if keyCode(uKey)
        gain = min(1.0, max(-1.0, gain + 0.1));
    end

    if keyCode(dKey)
        gain = min(1.0, max(-1.0, gain - 0.1));
    end
    
    % Switch through possible output channels:
    if keyCode(oKey)
        % Always increment by two so outputchannel stays even, which is
        % required for ASIO hardware:
        outputchannel = mod(outputchannel + 2, noutputs);
    end
    
    % Switch through possible output channels:
    if keyCode(iKey)
        inputchannel = inputchannel + 1;
        % Wrap around to -1 (all channels) if max. reached:
        if inputchannel >= ninputs
            inputchannel = -1;
        end
    end
    
    % Set a new 'DirectInputMonitoring mode':
    diResult = PsychPortAudio('DirectInputMonitoring', pa, unmute, inputchannel, outputchannel, gain, pan);
    fprintf('Enabled: %i Inchannel: %i, OutChannel: %i, gain %f, stereopan %f, RC = %i\n', unmute, inputchannel, outputchannel, gain, pan, diResult);
end

% Done. Try to mute setup. Don't care about error flag...
PsychPortAudio('DirectInputMonitoring', pa, 0, inputchannel, outputchannel, gain, pan);

% Close device and driver:
PsychPortAudio('Close');
fprintf('Done. Bye!\n');

return;
