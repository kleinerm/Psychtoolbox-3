function err = Snd(command,signal,rate,sampleSize)
% err = Snd(command,[signal],[rate],[sampleSize])
%
% Old Sound driver for Psychtoolbox. USE OF THIS DRIVER IS DEPRECATED FOR ALL BUT
% THE MOST TRIVIAL PURPOSES! Only useful for simple feedback tones, and indirectly
% by Eyelink's calibration routines. Does not trivially mix well with simultaneous
% use of PsychPortAudio(), see below for how to make it work with PsychPortAudio().
%
% Have a look at the help for PsychPortAudio ("help PsychPortAudio" and
% "help InitializePsychSound") for an introduction into the new sound
% driver, which is recommended for most purposes.
%
% By default, Snd() uses Matlabs or Octaves audioplayer() function for sound
% playback. This allows good interoperation with Screen()'s GStreamer based movie
% playback functionality, as well as with other running audio client applications.
% This has been tested under Octave 5+ and Matlab R2022b and R2023b on Windows-10,
% macOS 13 and Ubuntu 20.04-LTS and later. The downside is that audioplayer() operates
% with high audio latency, bad audio timing precision, unreliable and imprecise
% audio timestamps, as well as very limited audio control and functionality. It
% is mostly good enough for simple sound playback of stereo sound, e.g., some
% audio feedback tones, not more.
%
% Snd() can also be used with PsychPortAudio, but as a rather dumb and primitive
% wrapper around the PsychPortAudio() driver. It uses PsychPortAudio's most basic
% functionality to achieve "sort of ok" sound playback. The driver is used in high
% latency, low timing precision mode, so Snd()'s audio playback timing will likely
% be very unreliable.
%
% Alternatively you can create an empty file named 'Snd_use_oldstyle.txt' in
% the PsychtoolboxConfigDir() folder, ie., [PsychtoolboxConfigDir 'Snd_use_oldstyle.txt']
% to enforce the old-style implementation of Snd(), which uses audioplayer().
% The command Snd('Oldstyle') also requests use of this old-style audioplayer() path.
%
% Creating a file named 'Snd_use_newstyle.txt' similar to above will enforce use
% of PsychPortAudio().
%
%
% Audio device sharing for interop with PsychPortAudio:
% -----------------------------------------------------
%
% If you want to use PsychPortAudio and Snd() simultaneously, or one of the
% functions that indirectly use Snd(), e.g., Beeper() for simple beep tones,
% or Eyelink's auditory feedback during tracker setup and recalibration (which
% in turn uses Beeper() and thereby Snd()), then try this:
%
% 1. Open a suitable PsychPortAudio audio device, possibly also a slave audio
%    device and get a pahandle to it, e.g., pahandle = PsychPortAudio('Open',...);
%    or PsychPortAudio('OpenSlave', ...) for a slave device.
%
% 2. Now open Snd(), passing in this device handle for use as Snd() output device:
%    Snd('Open', pahandle);
%
%    If you want to repeatedly call Beeper(), or use auditory feedback from Eyelink,
%    which itself repeatedly calls Beeper(), then you should open the shared pahandle
%    via Snd('Open', pahandle, 1); - This will prevent Snd('Close') from having any
%    effect, so Beeper() won't close the Snd() driver after one beep, and Eyelink
%    will be able to emit multiple auditory feedback tones, not just a single one.
%
% 3. Proceed as usual, e.g., Snd('Play', ...) or Beeper(...), etc. Snd() will
%    use the pahandle audio device for playback, and pahandle can also be used
%    by PsychPortAudio calls directly for precisely controlled sound.
%
% 4. At the end of a session, you could forcefully detach Snd() from the pahandle
%    via a call to Snd('Close', 1).
%
% Supported subfunctions:
% -----------------------
%
% Snd('Play', signal [, rate][, sampleSize]) plays a sound.
%
% rate = Snd('DefaultRate') returns the default sampling rate in Hz, which
% currently is 44100 Hz on all platforms for the old style sound
% implementation, and the default device sampling rate if PsychPortAudio is
% used. This default may change in the future, so please either specify a
% rate, or use this function to get the default rate.
%
% The optional 'sampleSize' argument used with Snd('Play') is only retained for
% backwards compatibility and has no meaning, unless you opt in to use the
% audioplayer() implementation. Otherwise it is checked for correctness, but
% other than that it is ignored. Allowable values are either 8 or 16.
%
% oldverbosity = Snd('Verbosity' [, verbosity]);
% - Query current level of verbosity, optionally set a new 'verbosity' level.
%
% Snd('Open') opens the channel, which stays open until you call Snd('Close').
% Snd('Play',...) automatically opens the channel if it isn't already open.
% You can use Snd('Open', pahandle); to share an existing PsychPortAudio device
% handle 'pahandle' with Snd() for optimal interoperation.
% A Snd('Close') of such a shared 'pahandle' would not close the handle, but it
% would close Snd()'s further use of it. If you call Snd('Open', pahandle, 1);
% then a Snd('Close') will not have any effect, ie. the pahandle not only stays
% open, but also continues to be shared and open for use by Snd().
%
% Snd('Close') immediately stops all sound and closes the channel, unless you
% specified a shared pahandle with PsychPortAudio via Snd('Open', pahandle, 1);
% earlier. Calling Snd('Close', 1) will always really close the channel.
%
% Snd('Wait') waits until the sound is done playing.
%
% isPlaying = Snd('IsPlaying') returns true if any sound is playing, and
% false (0) otherwise.
%
% Snd('Quiet') stops the sound currently playing, but leaves the channel open.
%
% "signal" must be a numeric array of samples.
%
% Your "signal" data should lie between -1 and 1 (smaller to play more
% softly). If the "signal" array has one row then it's played monaurally,
% through both speakers. If it has two rows then it's played in stereo.
%
% "rate" is the rate (in Hz) at which the samples in "signal" should be
% played. We suggest you always specify the "rate" parameter. If not
% specified, the sample "rate", on all platforms, defaults to the most
% common hardware sample rate of 44100 Hz. That value is returned by
% Snd('DefaultRate'). Other values can be specified.
%
% "samplesize". Snd accepts the sampleSize argument and passes it to the
% audioplayer() command. audioplayer (and therefore also Snd) may obey
% the specified sampleSize value, either 8 or 16, only if it is supported by
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
% NOTE: We suggest you always specify the "rate" parameter. If not
% specified, the sample rate, on all platforms, defaults to the most
% common hardware sample rate of 44100 Hz. That value is returned
% by Snd('DefaultRate').
%
% See also PsychPortAudio, Beeper, audioplayer, PLAY, MakeBeep, READSND, and WRITESND.

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
% 08/10/21   mk Try to make Snd - pahandle sharing more reliable via new Snd('Open', pahandle, 1).
% 10/29/21   mk Some more help updates.
% 06/30/23   mk Remove Octave special cases, update help text. Octave 5+ does have
%               sound() builtin, and on par in functionality with Matlab. Both use
%               audioplayer() objects internally. All said, sound() may be now good
%               enough to do without the new PsychPortAudio based path.
% 10/04/23   mk Switch the old style path from use of sound() to use of audioplayer(),
%               as sound() is using that anyway on Octave and Matlab, so direct use
%               gives us more control. Note: Both Octave (since at least version 5)
%               and Matlab use Portaudio internally for audioplayer afaict, with the
%               default audio playback device. This translates to the ALSA default device
%               on Linux, ie. usually a running Pulseaudio or Pipewire desktop sound server.
%               This allows possibly for interop between Snd() and GStreamer and other apps.
% 10/07/23   mk Switch default opmode to use audioplayer() instead of PsychPortAudio, update docs.

persistent ptb_snd_oldstyle;
persistent ptb_snd_injected;
persistent pahandle;
persistent player;
persistent verbose;

if isempty(verbose)
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

if strcmpi(command, 'Oldstyle') && isempty(ptb_snd_oldstyle)
    ptb_snd_oldstyle = 1;
    return;
end

% Snd('Open', pahandle [, noclose=0]) called to inject a pahandle of an already open
% PsychPortAudio device for sharing? If so, we piggyback onto pahandle
% for our audio playback:
if strcmpi(command,'Open') && nargin >= 2 && ~isempty(signal)
    % Close previous PsychPortAudio handle if it was not injected into us:
    if ~isempty(pahandle) && pahandle ~= signal && ~ptb_snd_injected
        PsychPortAudio('Close', pahandle);
    end

    % Assign 2nd argument as new pahandle:
    pahandle = signal;

    % Obviously we are not in old style mode here:
    ptb_snd_oldstyle = 0;

    % "noclose" flag provided as 1?
    if nargin >=3 && ~isempty(rate) && rate == 1
        % Mark this pahandle as injected and uncloseable by us. Neither the PPA
        % device, nor our association with it should be closed/removed, not even
        % if our caller calls Snd('Close'). This is useful for emitting repeated
        % beeps via Beeper(), as Beeper() would Snd('Close') after each beep, and
        % thereby remove the association:
        ptb_snd_injected = 2;

        if verbose
            fprintf('Snd(): Using PsychPortAudio via shared handle %i permanently.\n', pahandle);
        end
    else
        % Mark this pahandle as injected -- PPA device should not be closed by us
        % ever, but our Snd('Close') function can detach us from it, so it will
        % only be usable by PsychPortAudio directly again:
        ptb_snd_injected = 1;

        if verbose
            fprintf('Snd(): Using PsychPortAudio via shared handle %i, until you call Snd(''Close''); to unshare.\n', pahandle);
        end
    end

    return;
end

% Already defined if we shall use the new-style or old-style
% implementation?
if isempty(ptb_snd_oldstyle)
    % Nope, check if the special "old style" marker file exists:
    if exist([PsychtoolboxConfigDir 'Snd_use_oldstyle.txt'], 'file')
        % User explicitly wants old-style implementation via Matlab/Octave
        % audioplayer() function:
        ptb_snd_oldstyle = 1;

        if verbose
            fprintf('Snd(): Forced use of Matlab/Octave audioplayer() function for sound output.\n');
        end
    elseif exist([PsychtoolboxConfigDir 'Snd_use_newstyle.txt'], 'file')
        % User explicitly wants new-style implementation via PsychPortAudio():
        ptb_snd_oldstyle = 0;

        if verbose
            fprintf('Snd(): Forced use of PsychPortAudio() for sound output.\n');
        end
    else
        % Default, if nothing else is specified, is to use "old style" audioplayer(),
        % as the audioplayer() implementation on current Matlab and Octave seems to
        % be good enough as tested on all operating systems and has a good interop story
        % with other audio clients, GStreamer etc., and usually with PsychPortAudio in
        % reqlatencyclass modes 0 and 1, at least on macOS and Windows, and on Linux if
        % Pulseaudio enabled libportaudio 19.8 or later is used:
        ptb_snd_oldstyle = 1;
    end

    if ~ptb_snd_oldstyle
        % User wants new-style PsychPortAudio variant, so do preinit:
        if verbose
            fprintf('Snd(): Initializing PsychPortAudio driver for sound output.\n');
        end

        % Basic sound preinit:
        try
            InitializePsychSound;
        catch
            fprintf('Snd(): ERROR!\n');
            ple;
            psychlasterror('reset');
            fprintf('Snd(): PsychPortAudio initialization failed - See error messages above. Trying to use old audioplayer() fallback instead.\n');
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
            fprintf('Snd(): PsychPortAudio already in use. Using old audioplayer() fallback instead...\n');
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
            % Reasonable default:
            rate = 44100;
        else
            % Let PPA decide itself:
            rate = [];
        end
    end

    if ptb_snd_oldstyle
        % Old-Style implementation via audioplayer() function:

        % Wait until any ongoing sound is done.
        while ~isempty(player) && isplaying(player)
            drawnow;
            WaitSecs('YieldSecs', 0.001);
        end

        % Stop and delete potentially existing old player object:
        if ~isempty(player)
            stop(player);
            clear player;
        end

        % On Octave on Linux, do this little trickery to abuse PsychPortAudio
        % to suppress ALSA debug messages triggered by the audioplayer() object:
        if IsLinux && IsOctave
            PsychPortAudio('Verbosity', PsychPortAudio('Verbosity'));
        end

        % Create new player object and start non-blocking playback:
        player = audioplayer(signal', rate, sampleSize);
        play(player);
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
                fprintf('Snd(): handle to Snd(''Open'', ...) if you want to allow Snd() to operate fully independently.\n');
                fprintf('Snd(): You may want to read ''help Snd'' about other points to consider wrt. pahandle sharing.\n\n');
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
        % Wait until any ongoing sound is done.
        while ~isempty(player) && isplaying(player)
            drawnow;
            WaitSecs('YieldSecs', 0.001);
        end
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
        if ~isempty(player) && isplaying(player)
            drawnow;
            err=1;
        else
            err=0;
        end
    end

elseif strcmpi(command,'Quiet') || strcmpi(command,'Close')
    if nargin > 2
        error('Wrong number of arguments: see Snd.');
    end

    if ~isempty(pahandle)
        % Stop playback asap, wait for stop:
        PsychPortAudio('Stop', pahandle, 2, 1);

        % Close command?
        if strcmpi(command,'Close') && ((ptb_snd_injected ~= 2) || (nargin == 2 && signal == 1))
            if ~ptb_snd_injected
                % Close it:
                PsychPortAudio('Close', pahandle);
            end

            pahandle = [];
            ptb_snd_injected = 0;
        end
    elseif ~isempty(player)
        stop(player);
        if strcmpi(command,'Close')
            clear player;
        end
    end

    err=0;

elseif strcmpi(command,'DefaultRate')
    if nargin>1
        error('Wrong number of arguments: see Snd.');
    end

    if ptb_snd_oldstyle
        if ~isempty(player)
            err = get(player, 'SampleRate');
        else
            % Old style - reasonable hard-coded default:
            err = 44100; % default sampling rate in Hz.
        end
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
    % Nothing to do right now.
else
    PsychPortAudio('Close');
    pahandle = [];
    error(['unknown command "' command '"']);
end

return;
