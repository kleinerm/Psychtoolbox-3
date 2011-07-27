function moaldemo
% moaldemo - Minimalistic demo on how to use OpenAL for
% 3D audio output in Matlab. This is mostly trash code
% for initial testing and development. Better demos will
% follow soon.

% Establish key mapping: ESCape aborts, Space toggles between auto-
% movement of sound source or user mouse controlled movement:
KbName('UnifyKeynames');
space = KbName('space');
esc = KbName('ESCAPE');

% Initialize OpenAL subsystem at debuglevel 2 with the default output
% device:
InitializeMatlabOpenAL(2);

% Generate one sound buffer:
buffers = alGenBuffers(1);

% Query for errors:
alGetString(alGetError)

% Create sound data:

% Start of with 10 seconds of 44.1 Khz random noise as a fallback:
mynoise = randn(1, 44100 * 10);
freq = 44100;

% Try to load some impressive sound...
if IsOSX
   try
      %mynoise = wavread('/Users/kleinerm/Music/iTunes/iTunes Music/Unknown Artist/Unknown Album/sound_bubbles.wav');
      freq = 44100;
      mynoise = wavread('/Users/kleinerm/Documents/One.wav');
   catch
      % Load Matlabs demo sound matrix if everything else fails..
      load handel;
      freq = 8000;
      mynoise = y;
   end
end

if IsWin
   try
      mynoise = wavread('C:\WINNT\MEDIA\Windows-Anmeldeklang.wav');
      freq = 16384;
   catch
      % Load Matlabs demo sound matrix if everything else fails..
      load handel;
      freq = 8000;
      mynoise = y;
   end
end

% Convert to 16 bit signed integer format, map range from -1.0 ; 1.0 to -32768 ; 32768.
% This is one of two sound formats accepted by OpenAL, the other being unsigned 8 bit
% integer in range 0;255. Other formats (e.g. float or double) are supported by some
% implementations, but one can't count on it. This is more efficient anyways...
mynoise = int16(mynoise * 32767);

% Fill our sound buffer with the data from the sound vector. Tell AL that its
% a 16 bpc, mono format, with length(mynoise)*2 bytes total, to be played at
% a sampling rate of freq Hz. The AL will resample this to the native device
% sampling rate and format at buffer load time.
alBufferData( buffers, AL.FORMAT_MONO16, mynoise, length(mynoise)*2, freq);

% Create a sound source:
source = alGenSources(1);

if IsOSX
    alcASASetListener(ALC.ASA_REVERB_ON, 1)
    alcASASetListener(ALC.ASA_REVERB_QUALITY, ALC.ASA_REVERB_QUALITY_Max)
    alcASASetListener(ALC.ASA_REVERB_ROOM_TYPE, ALC.ASA_REVERB_ROOM_TYPE_Cathedral)
    alcASASetSource(ALC.ASA_REVERB_SEND_LEVEL, source, 0.8)
end

% Attach our buffer to it: The source will play the buffers sound data.
alSourceQueueBuffers(source, 1, buffers);

% Switch source to looping playback: It will repeat playing the buffer until
% its stopped.
alSourcei(source, AL.LOOPING, AL.TRUE);

% Set emission volume to 100%, aka a gain of 1.0:
alSourcef(source, AL.GAIN, 1);

alListenerfv(AL.POSITION, [0, 0, 0]);
alListenerfv(AL.VELOCITY, [0, 0, 0]);
%alListenerfv(AL.ORIENTATION, [0, 0, -1, 0, 1, 0]);

% Start playback for this source:
GetSecs;

alSourcePlay(source);
tstart = GetSecs;

x=0;
manual = 0;

% 3D sound animation loop. Runs until ESCape key press:
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
           while KbCheck; end;
        end
    end
    
    % Query mouse:
    [xm ym button]=GetMouse;
    if 1 || any(button) 
       % x,y mouse pos selects sound source position in space:
       x=(xm-700)/400
       z=(ym-500)/400
    end
    
    if ~manual
       % Auto mode, source moves along a 3D circle in space:
       t=GetSecs;
       z=-sin(t)*3;
       x=cos(t)*3;
    end
    g=1;
    % Select 3D position of source in space:
    alSource3f(source, AL.POSITION, g*x, g*z, -1.0);
%    alSource3f(source, AL.POSITION, g*x, 0, g*z);
    
    % Query current playback position in seconds since start of buffer:
    tref = (GetSecs - tstart);
    taud = alGetSourcef(source, AL.SAMPLE_OFFSET)/16384;
    %fprintf('Delta: %f %f %f\n', tref - taud, tref, taud);
    
    % Pause for 10 milliseconds in order to yield the cpu to other processes:
    WaitSecs(0.01);
end

% Stop playback:
alSourceStop(source);

% Wait a bit:
WaitSecs(0.1);

% Unqueue sound buffer:
alSourceUnqueueBuffers(source, 1, buffers);

% Wait a bit:
WaitSecs(0.1);

% Delete buffer:
alDeleteBuffers(1, buffers);

% Wait a bit:
WaitSecs(0.1);

% Delete source:
alDeleteSources(1, source);

% Wait a bit:
WaitSecs(0.1);

% Shutdown OpenAL:
CloseOpenAL;

% Done. Bye.
return;
