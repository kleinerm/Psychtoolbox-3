function InitializePsychSound(reallyneedlowlatency)
% InitializePsychSound([reallyneedlowlatency=0])
%
% This routine loads the PsychPortAudio sound driver for high-precision,
% low-latency, multi-channel sound playback and recording.
%
% Call it at the beginning of your experiment script, optionally providing
% the 'reallyneedlowlatency' flag set to one to push really hard for low
% latency. Redundant calls within one session will be ignored, only the first
% call counts. However, redundant calls with contradictory settings of the
% 'reallyneedlowlatency' flag will print a "don't do that!" warning, so if you
% want to truly switch the 'reallyneedlowlatency' parameter, e.g., between
% running different experiment scripts, you must call 'clear all' to reset
% everything first.
%
% On macOS and GNU/Linux, the PsychPortAudio driver will just work with
% low latency and highest timing precision after this initialization.
%
% On Microsoft Windows, things are a bit more complicated:
%
% PsychPortAudio on Windows supports three different Windows sound systems,
% MME, WDM/KS and WASAPI. Only WDM/KS and WASAPI are suitable for research grade
% auditory stimulation with support for multi-channel sound cards and for
% high-precision and low-latency sound timing and time-stamping. If you
% want reliable timing and time-stamping with latencies and accuracy better
% than 500 msecs, you *must* use one of these. By default, in low latency mode,
% WASAPI is used on Windows Vista and later, whereas WDM/KS would be used on
% older Windows versions. WDM/KS is completely untested so far, and WASAPI has
% only been tested on Windows 7 and Windows 10. For best results, use of Windows 10
% is recommended.
%
% The Windows MME (MultiMediaExtensions) sound system has typical latencies
% and inaccuracies as high as 500 msecs. WASAPI can achieve latencies as low
% as 10 msecs with onboard sound chips on Windows-10, and maybe even on Windows 8.1.
% On Windows 7 latencies around 20 msecs are possible. Timing should be generally
% accurate to millisecond level with WASAPI.
%
% Using macOS or Linux will usually get you at least as good, or usually better,
% results with most standard sound hardware, due to the technically superior
% sound systems of these operating systems.
%

% History:
% 6/6/2007    Written (MK).
% 10/20/2011  Update: We always use ASIO enabled plugin on Windows by
%             default, as the PTB V3.0.9 MIT style license allows bundling
%             of an ASIO enabled proprietary dll with Psychtoolbox. (MK)
% 09/11/2012  Add support for 64-Bit portaudio_x64.dll for Windows. (MK)
% 10/16/2015  Disable use of our own portaudio_x64 dll On Windows + Octave. (MK)
% 11/08/2018  No ASIO anymore, starting with v3.0.15. (MK)
% 01/16/2022  Prepare for future Linux PulseAudio support, to allow sharing of
%             sound devices between audio clients if reallyneedlowlatency==0,
%             but keep switching disabled for now, until we have an official and
%             fully verified PortAudio release with PulseAudio support. (MK)

persistent previousreallyneedlowlatency

if nargin < 1
    reallyneedlowlatency = [];
end

if isempty(reallyneedlowlatency)
    reallyneedlowlatency = 0; %#ok<NASGU> % Default: Don't push too hard for low latency.
end

% All calls but the first one in a session get turned into a no-op:
if ~isempty(previousreallyneedlowlatency)
    % Warn if successive calls have contradictory reallyneedlowlatency parameter:
    if reallyneedlowlatency ~= previousreallyneedlowlatency
        warning('InitializePsychSound() called again with different setting for reallyneedlowlatency from first call. Keeping original setting, this may cause audio timing trouble. Check your code, or call ''clear all'' between different scripts!');
    end

    % No-Op return:
    return;
end

% First call. Keep track of current requested latency setting:
previousreallyneedlowlatency = reallyneedlowlatency;

% The usual tricks for MS-Windows:
if IsWin
    % Override driver installed?
    if exist([PsychtoolboxRoot 'portaudio_x86.dll'], 'file') || exist([PsychtoolboxRoot 'portaudio_x64.dll'], 'file')
        % Yes! Use override driver:
        fprintf('Detected optional PortAudio override driver plugin in Psychtoolbox root folder. Will use that.\n');
        driverloadpath = PsychtoolboxRoot;
    else
        % No - We use our standard driver:
        driverloadpath = [PsychtoolboxRoot 'PsychSound'];
    end

    % Standard path trick: Change working directory to driver load path,
    % preinit PsychPortAudio so it gets linked against the proper DLL, change
    % path back to old current directory:
    try
        olddir = pwd;
        cd(driverloadpath);
        % We force loading+linking+init of the driver:
        d = PsychPortAudio('GetDevices');
        cd(olddir);
    catch %#ok<*CTCH>
        cd(olddir);
        previousreallyneedlowlatency = [];
        error('Failed to load PsychPortAudio driver for unknown reason! Dependency problem?!?');
    end
end

% Maybe some tricks (in the future) for OS/X? None yet.
if IsOSX
    try
        % We force loading+linking+init of the driver here, so in case
        % something goes wrong we can catch this and output useful
        % troubleshooting tips to the user:
        d = PsychPortAudio('GetDevices'); %#ok<NASGU>
    catch
        fprintf('Failed to load PsychPortAudio driver!\n\n');
        em = psychlasterror;
        fprintf('The exact error message of the linker was: %s\n', em.message);
        fprintf('\n\n');
        previousreallyneedlowlatency = [];
        error('Failed to load PsychPortAudio driver.');
    end
end

if IsLinux
    % DISABLE PulseAudio switching logic for the time being until the upstream
    % code in PortAudio has been stabilized, officially released and carefully
    % tested by us.
    return;

    try
        % We force loading+linking+init of the driver here, so in case
        % something goes wrong we can catch this and output useful
        % troubleshooting tips to the user:
        d = PsychPortAudio('GetDevices', 16); %#ok<NASGU>
    catch
        fprintf('Failed to load PsychPortAudio driver!\n\n');
        em = psychlasterror;
        fprintf('The exact error message of the linker was: %s\n', em.message);
        fprintf('\n\n');
        previousreallyneedlowlatency = [];
        error('Failed to load PsychPortAudio driver.');
    end

    % Does the underlying libportaudio.so audio library support the PulseAudio host api backend?
    % Then the list of available PulseAudio devices d should not be empty:
    if isempty(d)
        % Empty. Our job is done, as all further config only makes sense for PulseAudio enabled drivers:
        return;
    end

    % PulseAudio supported in principle. Any already active audio devices?
    if PsychPortAudio('GetOpenDeviceCount') > 0
        % Yes. We can not change settings then. Let's just no-op:
        return;
    end

    % PulseAudio supported and driver idle, so we can make changes if needed.
    % Query current PulseAudio suspend mode, ie. if we could get 'reallyneedlowlatency'
    [~, ~, ~, havelowlatency] = PsychPortAudio('EngineTunables');

    % Do we already have the config we wanted?
    if havelowlatency == reallyneedlowlatency
        % Yes. We are done.
        return;
    end

    % No. We need to switch the mode to allow or disallow PulseAudio autosuspend.
    % But first we need to force a driver shutdown, so it will latch the new setting:
    PsychPortAudio('Close');

    % Now apply the new setting for suspend mode to enable proper reallyneedlowlatency behaviour:
    PsychPortAudio('EngineTunables', [], [], [], reallyneedlowlatency);
    [~, ~, ~, havelowlatency] = PsychPortAudio('EngineTunables');
end

return;
