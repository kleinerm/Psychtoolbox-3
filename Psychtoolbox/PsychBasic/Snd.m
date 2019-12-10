function err = Snd(command,signal,rate,sampleSize)
% err = Snd(command,[signal],[rate],[sampleSize])
%
% Old Sound driver for Psychtoolbox. USE OF THIS DRIVER IS DEPRECATED FOR
% ALL BUT THE MOST TRIVIAL PURPOSES!
%
% Have a look at the help for PsychPortAudio ("help PsychPortAudio" and
% "help InitializePsychSound") for an introduction into the new sound
% driver, which is recommended for most purposes.
%
% Snd() is a rather dumb and primitive wrapper around the PsychPortAudio()
% driver. It uses PsychPortAudio's most basic functionality to achieve
% "sort of ok" sound playback. The driver is used in high-latency,
% low-timing precision mode, so Snd()'s audio playback timing will likely
% be very unreliable.
%
% Alternatively you can create an empty file named 'Snd_use_oldstyle.txt' in
% the PsychtoolboxConfigDir() folder, ie., [PsychtoolboxConfigDir 'Snd_use_oldstyle.txt']
% This will enable the old-style implementation of Snd(), which is equally
% shoddy and works as follows:
%
% While Snd used to use a special purpose low level driver on MacOS-9 which
% was well suited for cognitive science, Snd for all other operating
% systems (Windows, MacOS-X, Linux) just calls into Matlab's Sound()
% function which is of varying - but usually pretty poor - quality in most
% implementations of Matlab. There are many bugs, latency- and timing
% problems associated with the use of Snd.
%
% GNU/OCTAVE: If you don't use the PsychPortAudio based Snd() function, then
% you must install the optional octave "audio" package from Octave-Forge,
% as of Octave 3.0.5, otherwise the required sound() function won't be
% available and this function will fail!
%
% Audio device sharing for interop with PsychPortAudio:
% -----------------------------------------------------
%
% If you want to use PsychPortAudio and Snd() simultaneously (or one of the
% functions that indirectly use Snd(), e.g., Beeper() for simple beep tones,
% or Eyelink's auditory feedback during tracker setup and recalibration, which
% in turn uses Beeper() and thereby Snd(), then try this:
%
% 1. Open a suitable PsychPortAudio audio device, possibly also a slave audio
%    device and get a pahandle to it, e.g., pahandle = PsychPortAudio('Open',...);
%
% 2. Now open Snd(), passing in this device handle for use as Snd() output device:
%    Snd('Open', pahandle);
%
% 3. Proceed as usual, e.g., Snd('Play', ...) or Beeper(...), etc. Snd() will
%    use the pahandle audio device for playback, and pahandle can also be used
%    by PsychPortAudio calls directly for precisely controlled sound.
%
% Supported functions:
% --------------------
%
% Snd('Play', signal [, rate][, sampleSize]) plays a sound.
%
% rate=Snd('DefaultRate') returns the default sampling rate in Hz, which
% currently is 22254.5454545454 Hz on all platforms for the old style sound
% implementation, and the default device sampling rate if PsychPortAudio is
% used. This default may change in the future, so please either specify a
% rate, or use this function to get the default rate. (This default is
% suboptimal on any system except MacOS-9, but kept for backwards
% compatibility!)
%
% The optional 'sampleSize' argument used with Snd('Play') is only retained
% for backwards compatibility and has no meaning, unless you opt in to use
% the old-style implementation on Matlab with some operating systems. - It
% is checked for correctness, but other than that it is ignored. Allowable
% values are either 8 or 16.
%
% oldverbosity = Snd('Verbosity' [, verbosity]);
% - Query current level of verbosity, optionally set a new 'verbosity' level.
%
% Snd('Open') opens the channel, which stays open until you call
% Snd('Close'). Snd('Play',...) automatically opens the channel if it isn't
% already open. You can use Snd('Open', pahandle); to share an existing
% PsychPortAudio audio device handle 'pahandle' with Snd() for optimal
% interoperation. See instructions above.
%
% Snd('Close') immediately stops all sound and closes the channel.
%
% Snd('Wait') waits until the sound is done playing.
%
% isPlaying=Snd('IsPlaying') returns true if any sound is playing, and
% false (0) otherwise.
%
% Snd('Quiet') stops the sound currently playing and flushes the queue, but
% leaves the channel open.
%
% "signal" must be a numeric array of samples.
%
% Your "signal" data should lie between -1 and 1 (smaller to play more
% softly). If the "signal" array has one row then it's played monaurally,
% through both speakers. If it has two rows then it's played in stereo.
% (Snd has no provision for changing which speaker(s), or the volume, used
% by a named snd resource, so use READSND to get the snd into an array,
% and supply the appropriately modified array to Snd.)
%
% "rate" is the rate (in Hz) at which the samples in "signal" should be
% played. We suggest you always specify the "rate" parameter. If not
% specified, the sample "rate", on all platforms, defaults to OS9's
% standard hardware sample rate of 22254.5454545454 Hz. That value is
% returned by Snd('DefaultRate'). Other values can be specified.
%
% OSX & WIN: "samplesize". Snd accepts the sampleSize argument and passes
% it to the Matlab SOUND command.  SOUND (and therefore Snd also) obeys the
% specified sampleSize value, either 8 or 16, only if it is supported by
% your computer hardware.
%
% Snd('Play',sin(0:10000)); % play 22 KHz/(2*pi)=3.5 kHz tone
% Snd('Play',[sin(1:20000) zeros(1,10000);zeros(1,10000) sin(1:20000)]); % stereo
% Snd('Wait');              % wait until end of all sounds currently in channel
% Snd('Quiet');             % stop the sound and flush the queue
%
% For most of the commands, the returned value is zero when successful, and
% a nonzero error number when Snd fails.
%
% Snd('Play', signal) takes some time to open the channel, if it isn't
% already open, and allocate a snd structure for your sound. This overhead
% of the call to Snd, if you call it in the middle of a movie, may be
% perceptible as a pause in the movie, which would be bad. However, the
% actual playing of the sound, asynchronously, is a background process that
% usually has very little overhead. So, even if you want a sound to begin
% after the movie starts, you should create a soundtrack for your entire
% movie duration (possibly including long silences), and call Snd to set
% the sound going before you start your movie. (Thanks to Liz Ching for
% raising the issue.)
%
% NOTE: We suggest you always specify the "rate" parameter. If not
% specified, the sample rate, on all platforms, defaults to OS9's
% standard hardware sample rate of 22254.5454545454 Hz. That value is returned
% by Snd('DefaultRate').
%
% See also PsychPortAudio, Beeper, AUDIOPLAYER, PLAY, MakeBeep, READSND, and WRITESND.

% 6/6/96    dgp Wrote SndPlay.
% 6/1/97    dgp Polished help text.
% 12/10/97  dhb Updated help.
% 2/4/98    dgp Wrote Snd, based on major update of VideoToolbox SndPlay1.c.
% 3/8/00    emw Added PC notes and code.
% 7/23/00   dgp Added notes about controlling volume of named snd, and updated
%               broken link to ResEdit.
% 4/13/02   dgp Warn that the two platforms have different default sampling rate,
%               and suggest that everyone routinely specify sampling rate to
%               make their programs platform-independent.
% 4/13/02   dgp Enhanced both OS9 and WIN versions so that 'DefaultRate'
%               returns the default sampling rate in Hz.
% 4/13/02   dgp Changed WIN code, so that sampling rate is now same on both platforms.
% 4/15/02   awi fixed WIN code.
% 5/30/02   awi Added sampleSize argument and documented.
%               SndTest would crash Matlab but the problem mysteriously vanished while editing
%               the Snd.m and SndTest.m files.  I've been unable to reproduce the error.
% 3/10/05   dgp Make it clear that the Snd mex is only available for OS9.
%               Mention AUDIOPLAYER, as suggested by Pascal Mamassian.
% 5/20/08    mk Explain that Snd() is deprecated --> Point to PsychPortAudio!
% 1/12/09    mk Make sure that 'signal' is a 2-row matrix in stereo, not 2
%               column.
% 6/01/09    mk Add compatibility with Octave-3.
% 9/03/12    mk Add new implementation via PsychPortAudio(), which is used
%               by default unless user opts out. Cleanup online help by
%               removal of obsolete and outdated information.
% 7/20/15    mk Make commands case insensitive, white-space cleanup, try to
%               make Snd('Open'); Snd('Close'); sequences work to avoid problems
%               like in PTB forum message #19284 on Linux or Windows.
% 1/19/16    mk Make more robust against failing InitializePsychSound. Fallback to sound().
% 7/11/19    mk Allow sharing pahandle with external code / piggyback onto existing pahandle
%               via Snd('Open', pahandle);
% 12/9/19    mk Add 'Verbosity' subcommand to be able to silence Snd() and PsychPortAudio() output.

persistent ptb_snd_oldstyle;
persistent ptb_snd_injected;
persistent pahandle;
persistent verbose;

persistent endTime;
if isempty(endTime)
    endTime = 0;
    ptb_snd_injected = 0;
    verbose = 1;
end

% Default return value:
err = 0;

if nargin == 0
    error('Wrong number of arguments: see Snd.');
end

if strcmpi(command, 'Verbosity')
    if nargin ~= 2
        error('Snd: Called "Verbosity" without specifying verbosity level.');
    end

    err = verbose;
    verbose = signal;
    return;
end

% Snd('Open', pahandle) called to inject a pahandle of an already open
% PsychPortAudio device for sharing? If so, we piggyback onto pahandle
% for our audio playback:
if strcmpi(command,'Open') && nargin == 2
    % Close previous PsychPortAudio handle if it was not injected into us:
    if ~isempty(pahandle) && pahandle ~= signal && ~ptb_snd_injected
        PsychPortAudio('Close', pahandle);
    end

    % Assign 2nd argument as new pahandle:
    pahandle = signal;

    % Obviously we are not in old style mode here:
    ptb_snd_oldstyle = 0;

    % Mark this pahandle as injected -- should not be closed by us ever:
    ptb_snd_injected = 1;

    endTime = 0;

    if verbose
        fprintf('Snd(): Using PsychPortAudio via handle %i until you call Snd(''Close'');\n', pahandle);
    end

    return;
end

% Already defined if we shall use the new-style or old-style
% implementation?
if isempty(ptb_snd_oldstyle)
    % Nope, check if the special "old style" marker file exists:
    if exist([PsychtoolboxConfigDir 'Snd_use_oldstyle.txt'], 'file')
        % User explicitely wants old-style implementation via
        % Matlab/Octave sound() function:
        ptb_snd_oldstyle = 1;

        if verbose
            fprintf('Snd(): Using Matlab/Octave sound() function for sound output.\n');
        end
    else
        % User wants new-style PsychPortAudio variant:
        ptb_snd_oldstyle = 0;

        if verbose
            fprintf('Snd(): Initializing PsychPortAudio driver for sound output.\n');
        end

        % Low-Latency preinit. Not that we'd need it, but doesn't hurt:
        try
            InitializePsychSound(1);
        catch
            fprintf('Snd(): ERROR!\n');
            ple;
            psychlasterror('reset');
            fprintf('Snd(): PsychPortAudio initialization failed - See error messages above. Trying to use old sound() fallback instead.\n');
            ptb_snd_oldstyle = 1;
        end
    end
end

% Don't use PsychPortAudio backend if already a PPA device open on Linux or
% Windows, as that device will often have exclusive access, so we would
% fail here:
if ~(strcmpi(command,'Open') || strcmpi(command,'Quiet') || strcmpi(command,'Close')) && ...
    isempty(pahandle) && ~ptb_snd_oldstyle && ~IsOSX

    if ~verbose
        oldv = PsychPortAudio('Verbosity', 0);
        odc = PsychPortAudio('GetOpenDeviceCount');
        PsychPortAudio('Verbosity', oldv);
    else
        odc = PsychPortAudio('GetOpenDeviceCount');
    end

    if odc > 0
        if verbose
            fprintf('Snd(): PsychPortAudio already in use. Using old sound() fallback instead...\n');
        end

        ptb_snd_oldstyle = 1;
    end
end

if strcmpi(command,'Play')
    if nargin > 4
        error('Wrong number of arguments: see Snd.');
    end

    if nargin == 4
        if isempty(sampleSize)
            sampleSize = 16;
        elseif ~((sampleSize == 8) || (sampleSize == 16))
            error('sampleSize must be either 8 or 16.');
        end
    else
        sampleSize = 16;
    end

    if nargin < 3
        rate = [];
    end

    if nargin < 2
        error('Wrong number of arguments: see Snd.');
    end

    if size(signal,1) > size(signal,2)
        error('signal must be a 2 rows by n column matrix for stereo sounds.');
    end

    if isempty(rate)
        if ptb_snd_oldstyle
            % Old MacOS-9 style default:
            rate = 22254.5454545454;
        else
            % Let PPA decide itself:
            rate = [];
        end
    end

    if ptb_snd_oldstyle
        % Old-Style implementation via sound() function:

        WaitSecs(endTime-GetSecs); % Wait until any ongoing sound is done.

        % Octave special-case:
        if IsOctave
            if exist('sound') %#ok<EXIST>
                sound(signal',rate);
            else
                % Unavailable: Try to load the package, assuming its
                % installed but not auto-loaded:
                try
                    pkg('load','audio');
                catch %#ok<CTCH>
                end

                % Retry...
                if exist('sound') %#ok<EXIST>
                    sound(signal',rate);
                else
                    warning('Required Octave command sound() is not available. Install and "pkg load audio" the "audio" package from Octave-Forge!'); %#ok<WNTAG>
                end
            end
        else
            sound(signal',rate,sampleSize);
        end

        % Estimate 'endTime' for playback:
        endTime=GetSecs+length(signal)/rate;
    else
        % New-Style via PsychPortAudio:
        if ~isempty(pahandle)
            % PPA playback device already open.

            % Wait blocking until end of its playback:
            PsychPortAudio('Stop', pahandle, 1, 1);

            % Correct sampling rate set?
            props = PsychPortAudio('GetStatus', pahandle);
            if ~isempty(rate) && (abs(props.SampleRate - rate) > 1.0)
                % Device sampleRate not within tolerance of 1 Hz to
                % requested samplingrate.
                if ~ptb_snd_injected
                    % Close it, so it can get reopened with proper rate:
                    PsychPortAudio('Close', pahandle);
                    pahandle = [];
                else
                    % Should change samplerate, but can not, as this is a shared
                    % pahandle:
                    if verbose
                        fprintf('Snd(): Shared PsychPortAudio handle. Can not change sample rate from current %f Hz to %f Hz as requested!\n', ...
                                props.SampleRate, rate);
                    end
                end
            end
        end

        if isempty(pahandle)
            % Be silent during driver/device init:
            oldverbosity = PsychPortAudio('Verbosity', 2);

            % Open our own 'pahandle' sound device: Auto-Selected output
            % device [], playback only (1),
            % high-latency/low-timing-precision mode (0), at given
            % samplerate (rate), in stereo (2):
            pahandle = PsychPortAudio('Open', [], 1, 0, rate, 2);

            % Restore standard level of verbosity:
            PsychPortAudio('Verbosity', oldverbosity);

            if ~IsOSX && verbose
                fprintf('Snd(): PsychPortAudio will be blocked for use by your own code until you call Snd(''Close'');\n');
                fprintf('Snd(): If you want to use PsychPortAudio and Snd in the same session, make sure to open your\n');
                fprintf('Snd(): stimulation sound device via calls to, e.g., pahandle = PsychPortAudio(''Open'', ...);\n');
                fprintf('Snd(): *before* the first call to Snd() or any function that might use Snd(), e.g., Beeper()\n');
                fprintf('Snd(): or the Eyelink functions with auditory feedback. Call Snd(''Open'', pahandle) next, to\n');
                fprintf('Snd(): share that pahandle audio device with Snd(), Beeper() et al. for optimal collaboration.\n');
                fprintf('Snd(): Consider using PsychPortAudio(''OpenSlave'', ...); on a master device and pass that slave\n');
                fprintf('Snd(): handle to Snd(''Open'', ...) if you want to allow Snd() to operate fully independently.\n\n');
            end
        end

        % Make signal stereo if it isn't already:
        if size(signal, 1) < 2
            sndbuff = [signal ; signal];
        else
            sndbuff = signal;
        end

        % Play it:
        PsychPortAudio('FillBuffer', pahandle, sndbuff);
        PsychPortAudio('Start', pahandle, 1, 0, 1);
    end

elseif strcmpi(command,'Wait')
    if nargin>1
        error('Wrong number of arguments: see Snd.');
    end

    if ~isempty(pahandle)
        % Wait blocking until end of playback:
        PsychPortAudio('Stop', pahandle, 1, 1);
    else
        WaitSecs(endTime-GetSecs); % Wait until any ongoing sound is done.
    end
    err=0;

elseif strcmpi(command,'IsPlaying')
    if nargin>1
        error('Wrong number of arguments: see Snd.');
    end

    if ~isempty(pahandle)
        props = PsychPortAudio('GetStatus', pahandle);
        err = props.Active;
    else
        if endTime>GetSecs
            err=1;
        else
            err=0;
        end
    end

elseif strcmpi(command,'Quiet') || strcmpi(command,'Close')
    if nargin>1
        error('Wrong number of arguments: see Snd.');
    end

    if ~isempty(pahandle)
        % Stop playback asap, wait for stop:
        PsychPortAudio('Stop', pahandle, 2, 1);

        % Close command?
        if strcmpi(command,'Close')
            if ~ptb_snd_injected
                % Close it:
                PsychPortAudio('Close', pahandle);
            end

            pahandle = [];
            ptb_snd_injected = 0;
        end
    else
        if ~IsOctave
            clear playsnd; % Stop any ongoing sound.
        end
    end
    endTime=0;
    err=0;

elseif strcmpi(command,'DefaultRate')
    if nargin>1
        error('Wrong number of arguments: see Snd.');
    end

    if ptb_snd_oldstyle
        % Old style - old hard-coded default:
        err = 22254.5454545454; % default sampling rate in Hz.
    else
        % Audio device open?
        if isempty(pahandle)
            % No: Fake it - Use the most common sampling rate:
            err = 44100;
        else
            % Yes: Query its current sampling rate:
            s = PsychPortAudio('GetStatus', pahandle);
            err = s.SampleRate;
        end
    end

elseif strcmpi(command,'Open')
    endTime=0;
else
    PsychPortAudio('Close');
    pahandle = [];
    error(['unknown command "' command '"']);
end

return;
