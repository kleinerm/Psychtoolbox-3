function TurnTableDemo(wavfilename, wavfilenames)
% TurnTableDemo - A simulated turntable to nurture your inner DJ.
%
% Usage:
%
% TurnTableDemo([wavfilename][, wavfilenames])
%
% Free sample loops available for download at:
%
% http://www.musicradar.com/news/tech/free-music-samples-download-loops-hits-and-multis-217833/66/1
%
% Install those into a subfolder of your Homedirectory called
% 'Music/'. Alternatively you can specify a cell array of file names of
% .wav audio files to use as audio sample loops for the different effect
% keys on your keyboard as the 2nd call parameter 'wavfilenames' to this demo.
%
% The optional 1st parameter 'wavfilename' defines the name of the .wav
% audio file to play on the virtual turntable.
%
% Keys and their meaning:
%
% ESCape = End demo.
% space  = Mute/Unmute turntable.
% left SHIFT = Mute turntable temporarily while holding the key.
% 'r' = Switch between rotational and linear scratch control for turntable.
% Any key = One-shot playback of associated sound sample loop.
% Any key + left-ALT = Continous looped playback of associated sample loop.
% Any key + right SHIFT = Stop continous looped playback for sample loop.
%
% Mouse key / touch tablet press with mouse cursor inside turntable, or
% finger touch on touchscreen inside turntable == manual "scratch" control
% of turntable. Otherwise the turntable turns by itself.
%
% If you have a Griffin PowerMate knob or compatible input device
% connected, that dial will be used to drive the virtual turntable
% for a more fulfilling scratching experience!
%
% Free your inner DJ!
%

% History:
% 18-Oct-2013  mk  Written, but not released.
% 10-Apr-2016  mk  Revamped, tweaked, PowerMate support added.
% 14-Apr-2016  mk  Bug fixes.

% Simulated RPM of turntable:
rpm = 30;

% User input rotational, like on a real turntable? Else linear.
rotational = 1;

% Unified keycodes, startup checks, normalized colorspace:
PsychDefaultSetup(2);

escape = KbName('ESCAPE');
rotateToggle = KbName('r');
leftmuteButton = KbName('LeftShift');
togglemuteButton = KbName('space');
repeatOnKey = KbName('LeftAlt');
repeatOffKey = KbName('RightShift');

%% Define key mappings to sound sample loops:

% F1-F10 function keys:
for i=1:10
    key(i) = KbName(sprintf('F%i', i));
end

% Keys a - z:
for i=11:11+25
    key(i) = KbName(sprintf('%s', char('a' + i - 11)));
end

% Keys 1 - 9:
for i=11+25+1:11+25+1+8
    key(i) = KbName(sprintf('%s', char('1' + i - 11 - 25 - 1)));
end

%% Display init:

% Open a window with black "transparent" background:
win = PsychImaging('OpenWindow', 0, [0, 0, 0, 0]); %, [0, 0, 1024, 1024], [], [], [], [], []); %, kPsychGUIWindow);

% Lock basic audio update timing to video refresh timing to keep audio
% scratching in sync with visual feedback:
prefetchSecs = Screen('GetFlipInterval', win);

% Compute active area of turntable:
[xc, yc] = RectCenter(Screen('Rect', win));
turntableradius = min(xc, yc);

% Setup a nice "spinning turntable" image in 'tabletex':
tabletex = Screen('OpenOffscreenwindow', win, [0,0,0,0], [0, 0, 2*turntableradius, 2*turntableradius]);
Screen('FillOval', tabletex, [0.1,0.1,0.1]);
Screen('FillOval', tabletex, [1,1,0], CenterRect([0 0 200 200], Screen('Rect', tabletex)));
Screen('TextSize', tabletex, 24);
DrawFormattedText(tabletex, 'FUNK  +  BABY!\nScratch Me!', 'center', 'center', [1 0 0]);
Screen('FillOval', tabletex, [1,1,1], CenterRect([0 0 20 20], Screen('Rect', tabletex)));

% Use our default sound file if none provided:
if nargin < 1 || isempty(wavfilename)
    wavfilename = [ PsychtoolboxRoot 'PsychDemos' filesep 'SoundFiles' filesep 'funk.wav'];
end

% Read WAV file from filesystem:
[y, freq] = psychwavread(wavfilename);
wavedata = y';
nrchannels = size(wavedata,1); % Number of rows == number of channels.

% Make sure we have always 2 channels stereo output. Why? Because some
% low-end and embedded soundcards only support 2 channels, not 1 channel,
% and we want to be robust in our demos.
if nrchannels < 2
    wavedata = [wavedata ; wavedata];
end

numSamples = size(wavedata, 2);

% Filenames provided?
if nargin < 2
    wavfilenames = [];
end

if isempty(wavfilenames)
    % No sound file provided. Load our standard sounds:
    samplesassigned = 0;
    
    % Do we have a set of good sample loops?
    baseSampleDir = [PsychHomeDir 'Music/musicradar-funk-samples/'];
    if exist([baseSampleDir '80bpm loops/Guitar/'], 'dir')
        sounddir = [ baseSampleDir '80bpm loops/Guitar/' ];
        wavfilenames = appendNewWavFileNames(wavfilenames, sounddir);
        samplesassigned = 1;
    end
    
    if exist([baseSampleDir '80bpm loops/Bass/'], 'dir')
        sounddir = [ baseSampleDir '80bpm loops/Bass/' ];
        wavfilenames = appendNewWavFileNames(wavfilenames, sounddir);
        samplesassigned = 1;
    end

    if exist([baseSampleDir 'BonusBeats/80BPM/'], 'dir')
        sounddir = [ baseSampleDir 'BonusBeats/80BPM/' ];
        wavfilenames = appendNewWavFileNames(wavfilenames, sounddir);
        samplesassigned = 1;
    end
    
    % Fallback to our boring default set, which is reasonably useless for
    % this purpose:
    if ~samplesassigned
        sounddir = [ PsychtoolboxRoot 'PsychDemos' filesep 'SoundFiles' filesep ];
        wavfilenames = appendNewWavFileNames(wavfilenames, sounddir);
        warning('Could not find a dedicated set of suitable sound sample loops for this demo. Using default samples - This will suck!'); %#ok<WNTAG>
    end
end

nfiles = length(wavfilenames);

% Always init to 2 channels, for the sake of simplicity:
nrchannels = 2;

% Does a function for resampling exist?
if exist('resample') %#ok<EXIST>
    % Yes: Select a target sampling rate of 44100 Hz, resample if
    % neccessary:
    freq = 44100;
    doresample = 1;
else
    % No. We will choose the frequency of the wav file with the highest
    % frequency for actual playback. Wav files with deviating frequencies
    % will play too fast or too slow, because we can't resample:
    % Init freq:
    freq = 0;
    doresample = 0;
end

% Perform basic initialization of the sound driver:
InitializePsychSound(1);

% Read all sound files and create & fill one dynamic audiobuffer for
% each read soundfile:
buffer = [];
j = 0;

for i=1:nfiles
    try
        % Make sure we don't abort if we encounter an unreadable sound
        % file. This is achieved by the try-catch clauses...
        [audiodata, infreq] = psychwavread(char(wavfilenames(i)));
        dontskip = 1;
    catch
        fprintf('Failed to read and add file %s. Skipped.\n', char(wavfilenames(i)));
        dontskip = 0;
        psychlasterror
        psychlasterror('reset');
    end
    
    if dontskip
        j = j + 1;
        
        if doresample
            % Resampling supported. Check if needed:
            if infreq ~= freq
                % Need to resample this to target frequency 'freq':
                fprintf('Resampling from %i Hz to %i Hz... ', infreq, freq);
                audiodata = resample(audiodata, freq, infreq);
            end
        else
            % Resampling not supported by Matlab/Octave version:
            % Adapt final playout frequency to maximum frequency found, and
            % hope that all files match...
            freq = max(infreq, freq);
        end
        
        [samplecount, ninchannels] = size(audiodata); %#ok<*ASGLU>
        audiodata = repmat(transpose(audiodata), nrchannels / ninchannels, 1);
        
        buffer(end+1) = PsychPortAudio('CreateBuffer', [], audiodata); 
        [fpath, fname] = fileparts(char(wavfilenames(j)));
        fprintf('Filling audiobuffer handle %i with soundfile %s ...\n', buffer(j), fname);
    end
end

% Recompute number of available sounds:
nfiles = length(buffer);

% Open the default audio device [], with default mode [] (==Only playback),
% and a required latencyclass of zero 0 == no low-latency mode, as well as
% a frequency of freq and nrchannels sound channels. This returns a handle
% to the audio device:
try
    % Try with the 'freq'uency we wanted:
    pamaster = PsychPortAudio('Open', [], 1+8, 0, freq, nrchannels);
catch %#ok<*CTCH>
    % Failed. Retry with default frequency as suggested by device:
    fprintf('\nCould not open device at wanted playback frequency of %i Hz. Will retry with device default frequency.\n', freq);
    fprintf('Sound may sound a bit out of tune, ...\n\n');
    
    psychlasterror('reset');
    pamaster = PsychPortAudio('Open', [], 1+8, 0, [], nrchannels);
end

% Set the masterVolume for the master: This volume setting affects all
% attached sound devices. We set this to 0.5, so it doesn't blow out the
% ears of our listeners...
PsychPortAudio('Volume', pamaster, 0.5);

% Create to slave audio devices for sound playback (+1), with same
% frequency, channel count et. as master. Attach them to master. As they're
% attached to the same sound channels of the master (actually the same
% single channel), their audio output will mix together:
patable1 = PsychPortAudio('OpenSlave', pamaster);

% Open 1 actual slave devices, all attached to all channels of the master:
for i=1:1
    pahandle(i) = PsychPortAudio('OpenSlave', pamaster);
end

% Fill the audio playback buffer with the audio data 'wavedata':
prefetchCnt = ceil(prefetchSecs * freq * 10);
PsychPortAudio('FillBuffer', patable1, wavedata(:, 1:prefetchCnt));

% Setup key mappings and variables:
playhead = prefetchCnt + 1;
oldangle = [];
visangle = 0;
oldkc = [];
KbReleaseWait;
GetMouse(win);

% Realtime for real!
Priority(MaxPriority(win));
ListenChar(-1);

% Start master wait for it to be started. We won't stop the
% master until the end of the session.
PsychPortAudio('Start', pamaster, 0, 0, 1);

% PowerMate available? If so use it for the turntable.
powermate = PsychPowerMate('Open');

% Start audio playback immediately, for infinite repetition:
oldsecs = GetSecs;
PsychPortAudio('Start', patable1, 0);
muteTurntable1 = 0;
oldTableVolume = PsychPortAudio('Volume', patable1);

try
    
    % Turntable loop, runs until ESCape key press:
    while 1
        %% Query mouse position and key state:
        [xm,ym,buttons] = GetMouse(win);
        [isdown, secs, keycode] = KbCheck; %#ok<ASGLU>
        
        %% Turntable handling/emulation:
        
        % Compute elapsed time since last audio push pass:
        dT = secs - oldsecs;
        dT = min(dT, prefetchSecs * 2);
        oldsecs = secs;
        
        % Abort if ESCape key is pressed:
        if keycode(escape)
            break;
        end
        
        % Toggle between rotational and linear drive via 'r' key:
        if keycode(rotateToggle)
            keycode(rotateToggle) = 0;
            rotational = ~rotational;
            KbReleaseWait;
        end
        
        % Mute key mutes or unmutes the audio signal from the turntable:
        if keycode(togglemuteButton)
            keycode(togglemuteButton) = 0;
            muteTurntable1 = ~muteTurntable1;
            if muteTurntable1
                oldTableVolume = PsychPortAudio('Volume', patable1, 0);
            else
                PsychPortAudio('Volume', patable1, oldTableVolume);
            end
            
            KbReleaseWait;
        end

        if ~isempty(powermate)
            % Query PowerMate button state and rotation angle in "clicks"
            [pmbutton, angle] = PsychPowerMate('Get', powermate);

            % 1st button is the "or" of the 1st mouse button and the actual PowerMate button,
            % so either pressing a mouse button or the PowerMate knob will trigger "scratching":
            buttons(1) = buttons(1) | pmbutton;

            % Convert clicks to degrees:
            angle = angle * 3.8298;
            radius = 0;
        else
            % Decode position relative to "turntable" center of rotation:
            x = xm - xc;
            y = ym - yc;

            % Compute radius within turntable, ie., distance to center:
            radius = norm([x,y]);

            % True rotational drive?
            if rotational
                % Yes: Compute angle of finger relative to turntable, aka turn
                % angle of the turntable:
                angle = (atan2(y,x) * 180 / pi) + 180;
            else
                % No: Just fake an "angle" based on vertical y position:
                angle = ym;
            end
        end

        % Mouse cursor or finger within turntable, ie., inside its radius, and
        % pressed on touchpad?
        %if (keycode(leftmuteButton) || any(buttons)) && (radius < turntableradius)
        if (any(buttons)) && (radius < turntableradius)
            % Yes: Drive sound due to scratching / movement by user:
            
            % Valid old angle from previous sampling?
            if ~isempty(oldangle)
                % Yes. Compute delta angle of finger movement since last
                % sampling:
                delta = angle - oldangle;
            else
                % No. Assume static turntable:
                delta = 0;
            end
            
            % Update oldangle with current angle:
            oldangle = angle;
            
            % Wraparound handling at 360 deg -> 0 deg transition:
            if delta > 180
                delta = delta - 360;
            elseif delta < -180
                delta = delta + 360;
            end
        else
            % No: Table turns by itself at rpm revolutions per minute:
            oldangle = [];
            delta = 360 * (rpm / 60) * dT;
            
            if 0 && (radius < turntableradius)
                mcx = round(xc + radius * cos((angle + delta - 180) / 180 * pi));
                mcy = round(yc + radius * sin((angle + delta - 180) / 180 * pi));
                SetMouse(mcx, mcy, win);
            end
        end
        
        % Translate delta angle into delta rotations and then into
        % deltaSamples, given freq samples per second and rpm/60 revolutions
        % per second:
        deltaSamples = (delta / 360) * (freq / (rpm / 60));
        
        % Compute start and end of sampling interval:
        startfetch = playhead;
        endfetch   = playhead + ceil(deltaSamples);
        
        % Compute sample positions within interval to resample to proper output
        % sample rate - crude:
        samplepos  = 1 + mod(floor(linspace(startfetch, endfetch, floor(dT * freq))), numSamples);
        
        % Sample 'pushdata' to push to driver:
        pushdata = wavedata(:, samplepos);
        
        % Update current playposition (current for next loop iteration):
        playhead = mod(endfetch, numSamples);
        
        % Turn signal into silence if mute key is held down:
        if keycode(leftmuteButton)
            pushdata = zeros(size(pushdata));
        end
        
        % Perform streaming buffer refill. As long as we don't push more than a
        % buffer size, the driver will take care of the rest. The 1st return
        % argument is a buffer underflow flag, which we check here in the if
        % statement:
        if PsychPortAudio('FillBuffer', patable1, pushdata, 1)
            % Ohoh, underflow flag was non-zero -> Underflow! Let's reset
            % ourselves to a good start:
            PsychPortAudio('Stop', patable1);
            fprintf('Oops, underflow! Recovering...\n');
            PsychPortAudio('FillBuffer', patable1, zeros(nrchannels, prefetchCnt));
            PsychPortAudio('Start', patable1, 0);
        end
        
        % Some eye candy for realism:
        visangle = visangle + delta;
        Screen('DrawTexture', win, tabletex, [], [], visangle);
        Screen('Flip', win, [], [], 0);
        
        repeatOff = keycode(repeatOffKey);
        repeatOn  = keycode(repeatOnKey);
        
        keycode(leftmuteButton)  = 0;
        keycode(repeatOffKey) = 0;
        keycode(repeatOnKey) = 0;
        
        %% Keyboard based triggering of samples:
        if isdown
            % Yes. Respond to it:
            
            % First keyboard debouncing:
            filterkc = oldkc;
            oldkc = find(keycode);
            keycode(filterkc) = 0;
            
            % Then key mapping:
            kc = find(keycode); %#ok<MXFND>
            kc = min(kc);
            if ~isempty(kc)
                kc = find(key == kc);
            end
            
            if ~isempty(kc)
                % A key in array 'key' pressed (F1 to F10 or a letter key).
                % Map it to a audio buffer handle:
                kc = mod(kc - 1, nfiles) + 1;
                %fprintf('KEY %s pressed: Playing buffer %i\n', KbName(min(find(keyCode))), buffer(kc)); %#ok<MXFND>
                
                if repeatOff
                    % Stop of a repeating sound requested.
                    stopkc = min(find(activekc == kc)); %#ok<MXFND>
                    if ~isempty(stopkc)
                        PsychPortAudio('Stop', pahandle(stopkc), 3, 0, 1);
                        activekc(stopkc) = 0; %#ok<*AGROW>
                    end
                else
                    % Start of a single-shot or repeating sound requested.
                    % Find first audio slave device that is idle:
                    while 1
                        mydev = [];
                        ac = 0;
                        for i=1:length(pahandle)
                            % Query current playback status:
                            s = PsychPortAudio('GetStatus', pahandle(i));
                            ac = ac + s.Active;
                            if ~s.Active && isempty(mydev)
                                % This one's idle. Use it!
                                mydev = i;
                            end
                        end
                        
                        %fprintf('Load: %i of %i active.\n', ac, length(pahandle));
                        
                        if ~isempty(mydev)
                            break;
                        end
                        
                        % All devices are busy. Create a new one on-the-fly:
                        mydev = PsychPortAudio('OpenSlave', pamaster);
                        pahandle(end+1) = mydev; %#ok<AGROW>
                        activekc(end+1) = 0;
                    end
                    
                    PsychPortAudio('FillBuffer', pahandle(mydev), buffer(kc));
                    reps = 1;
                    
                    % Trigger infinite playback?
                    if repeatOn
                        reps = 0;
                        activekc(mydev) = kc;
                    end
                    
                    PsychPortAudio('Start', pahandle(mydev), reps);
                end
            end
        else
            oldkc = [];
        end
        
        % Next update loop iteration...
    end
    
    % Standard scheduling:
    Priority(0);

    % Standard keyboard input to applications:
    ListenChar(0);

    % Close PowerMate, if any:
    if ~isempty(powermate)
        PsychPowerMate('Close', powermate);
    end

    % Delete all dynamic audio buffers:
    PsychPortAudio('DeleteBuffer');
    
    % Close engine and exit.
    PsychPortAudio('Close');
    
    % Close onscreen window:
    sca;
    
    % Done.
    fprintf('Demo finished, bye!\n\n');
    
    return;
    
catch
    Priority(0);
    ListenChar(0);
    PsychPortAudio('DeleteBuffer');
    PsychPortAudio('Close');
    sca;
    if ~isempty(powermate)
        PsychPowerMate('Close', powermate);
    end
    psychrethrow(psychlasterror);
end

% End of main function.
end

% Helper: Adds all .wav files in directory 'sounddir' to list of .wav
% filenames in 'wavfilenames':
function wavfilenames = appendNewWavFileNames(wavfilenames, sounddir)
    infilenames = dir( [ sounddir '*.wav' ] );
    infilenames.name;

    for i=1:length(infilenames)
        wavfilenames{end+1} = [ sounddir infilenames(i).name ]; %#ok<AGROW>
    end
end
