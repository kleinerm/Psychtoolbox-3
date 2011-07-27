function AudioTunnel3DDemo
% AudioTunnel3DDemo -- A very sketchy and raw demo of how to use OpenAL to
% create the illusion of walking through a tunnel which has different sound
% sources attached to it.
%
% You'll need a properly configured multi-speaker system or very good
% head-phones attached to a very good soundcard and a lot of luck and
% goodwill for this to sound good.
%
% OpenAL for Matlab is currently only supported on a subset of platforms,
% so this demo may abort quickly with an error.
%


walkthroughduration = 15;
walkspeed = -6;
zcorridorlength = abs( walkspeed * walkthroughduration );
nsources = 5;

% Establish key mapping: ESCape aborts, Space toggles between auto-
% movement of sound source or user mouse controlled movement:
KbName('UnifyKeynames');
space = KbName('space');
esc = KbName('ESCAPE');

% Initialize OpenAL subsystem at debuglevel 2 with the default output device:
InitializeMatlabOpenAL(2);

% Generate one sound buffer:
buffers = alGenBuffers(nsources);

% Query for errors:
alGetString(alGetError)

% Try to load some impressive sound...
sounddir = [PsychtoolboxRoot 'PsychDemos/SoundFiles/'];
%sounddir = '/Library/Application Support/GarageBand/Instrument Library/Sampler/Sampler Files/String Ensemble/';
%sounddir = '/Library/Application Support/GarageBand/Instrument Library/Sampler/Sampler Files/Grand Piano/';

soundfiles = dir([sounddir '*.wav']);

% Velocity of listener: Walks straight down the z-axis:
alListenerfv(AL.VELOCITY, [0, 0, walkspeed]);

% Start position of listener is at the beginning of the corridor:
startpos = - 1.5 * zcorridorlength;
if walkspeed < 0
     startpos = -startpos;
end

alListenerfv(AL.POSITION, [0, 0, startpos]);

if IsOSX
    alcASASetListener(ALC.ASA_REVERB_ON, 1);
    alcASASetListener(ALC.ASA_REVERB_QUALITY, ALC.ASA_REVERB_QUALITY_Max);
    alcASASetListener(ALC.ASA_REVERB_ROOM_TYPE, ALC.ASA_REVERB_ROOM_TYPE_Cathedral);
end

% Create a sound source:
sources = alGenSources(nsources);

perm = randperm(nsources);

for i=1:nsources
    % Assign next soundfilename round-robin:
    soundname = [sounddir soundfiles(mod(perm(i), length(soundfiles))+1).name];
    
    % Load it...
    [mynoise freq]= wavread(soundname);
    
    % Convert it...
    mynoise = int16(mynoise * 32767);

    % Fill our sound buffer with the data from the sound vector. Tell AL that its
    % a 16 bpc, mono format, with length(mynoise)*2 bytes total, to be played at
    % a sampling rate of freq Hz. The AL will resample this to the native device
    % sampling rate and format at buffer load time.
    alBufferData( buffers(i), AL.FORMAT_MONO16, mynoise, length(mynoise)*2, freq);

    % Attach our buffer to it: The source will play the buffers sound data.
    alSourceQueueBuffers(sources(i), 1, buffers(i));

    % Switch source to looping playback: It will repeat playing the buffer until
    % its stopped.
    alSourcei(sources(i), AL.LOOPING, AL.TRUE);

    % Set emission volume to 100%, aka a gain of 1.0:
    alSourcef(sources(i), AL.GAIN, 1);

    alSourcef(sources(i), AL.CONE_INNER_ANGLE, 30);
    alSourcef(sources(i), AL.CONE_OUTER_ANGLE, 270);
    alSource3f(sources(i), AL.DIRECTION, 0, 0, 1);

    % Set position: Nicely lined up in z direction, but randomly placed in
    % x,y direction within a range of +/- 5 meters:
    %     theta = rand * 2 * pi;
    %     x = cos(theta) * 2;
    %     y = sin(theta) * 2;

    y = 0;
    x = mod(i,2) * 8 - 4;
    % nsources, equally spaced along a sound corridor of zcorridorlength
    % meters, the first one at z == 0.
    z = (i-1) * (zcorridorlength / nsources);
    alSource3f(sources(i), AL.POSITION, x, y, z);

    % Sources themselves remain static in space:
    alSource3f(sources(i), AL.VELOCITY, 0, 0, 0);
    
    if IsOSX
        % Source emits some sound that gets reverbrated in room:
        alcASASetSource(ALC.ASA_REVERB_SEND_LEVEL, sources(i), 0.5);
    end
end

% Start playback for these sources:
alSourcePlayv(nsources, sources);

% 3D sound animation loop. Runs until ESCape key press:
manual = 0;
tstart = GetSecs;
zposition = startpos;

while 1
    % Check keyboard:
    [isdown dummy, keycode]=KbCheck;
    if isdown
        if keycode(esc)
            break;
        end

        if keycode(space)
            % Toggle between mouse control and automatic:
            manual=1-manual;
            KbReleaseWait;
        end
    end
    
    if manual
        % Query mouse:
        [xm ym]=GetMouse;
        walkspeed=(ym-500)/100;
    end

    t = GetSecs;
    telapsed = t - tstart;
    tstart = t;
    tdistance = telapsed * walkspeed;
    zposition = zposition + tdistance;
    alListenerfv(AL.POSITION, [0, 0, zposition]);
    fprintf('Z=%f\n', zposition);

    % Pause for 10 milliseconds in order to yield the cpu to other processes:
    WaitSecs(0.01);
end

% Stop playback of all sources:
alSourceStopv(length(sources), sources);

for i=1:nsources
    % Unqueue sound buffer:
    alSourceUnqueueBuffers(sources(i), 1, buffers(i));
end

% Wait a bit:
WaitSecs(0.1);

% Delete buffer:
alDeleteBuffers(nsources, buffers);

% Wait a bit:
WaitSecs(0.1);

% Delete sources:
alDeleteSources(nsources, sources);

% Wait a bit:
WaitSecs(0.1);

% Shutdown OpenAL:
CloseOpenAL;

% Done. Bye.
return;
