function moaldemo

KbName('UnifyKeynames');
space = KbName('space');
esc = KbName('ESCAPE');

% Initialize OpenAL subsystem at debuglevel 2 with the default output
% device:
InitializeMatlabOpenAL(2, 'HelloWorld');

% Generate one sound buffer:
buffers = alGenBuffers(1);
alGetError

% Fill it with data:
%mynoise = int16(randn(1,44100 * 10) * 32767);
mynoise = wavread('/Users/kleinerm/Music/iTunes/iTunes Music/Unknown Artist/Unknown Album/sound_bubbles.wav');
mynoise = int16(mynoise * 32767);

%x=1:44100
%mynoise = int16(sin(1000/2*x/44100)*32767);
alBufferData( buffers, AL.FORMAT_MONO16, mynoise, length(mynoise)*2, 16384);
alGetError

% Create sound source:
source = alGenSources(1);
alGetError

% Attach buffer to it:
alSourceQueueBuffers(source, 1, buffers);
alGetError

% Switch source to looping playback:
alSourcei(source, AL.LOOPING, AL.TRUE);
alGetError

% Set volume to 100%
alSourcef(source, AL.GAIN, 1);
alGetError


% Start playback:
alSourcePlay(source);
x=0;
manual = 0;

% Wait for keypress:
while 1
    %x=mod(x + 0.01, 2);
    [isdown dummy, keycode]=KbCheck;
    if isdown
        if keycode(esc)
            break;
        end
        
        if keycode(space)
            manual=1-manual;
            while KbCheck; end;
        end
    end
    
    [xm ym button]=GetMouse;
    if any(button) 
        x=(xm-700)/400;
        z=(ym-500)/400;
    end
    
    if ~manual
        t=GetSecs;
        x=sin(t)*3;
        z=cos(t)*3;
    end
    
    alSource3f(source, AL.POSITION, x, 0, z);
    alGetSourcef(source, AL.SEC_OFFSET)
    WaitSecs(0.01);
end

% Stop playback:
alSourceStop(source);
alGetError

% Unqueue sound buffer:
alSourceUnqueueBuffers(source, 1);
alGetError

% Wait a bit:
WaitSecs(0.1);

% Delete buffer:
alDeleteBuffers(1, buffers);
alGetError

% Wait a bit:
WaitSecs(0.1);

% Delete source:
alDeleteSources(1, source);
alGetError

% Wait a bit:
WaitSecs(0.1);

% Shutdown OpenAL:
CloseOpenAL;

% Done.
return;
