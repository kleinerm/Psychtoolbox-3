function Beeper(frequency, fVolume, durationSec);
% function Beeper(frequency, [fVolume], [durationSec]);
%
% Play a beep sound.  Default is 400 Hz for .15 sec.
% frequency can be a number, or else the string 'high', 'med', or 'low'.
%
% fVolume - normalized to range of 0 - 1.  Default is 0.4;  
% Warning:  1 is the maximum volume and is often very loud!
%
% NOTE: Beeper() uses Snd() internally. If you want to use Beeper() - and therefore
% Snd() in parallel with PsychPortAudio, read the notes in "help Snd" about pahandle
% sharing, especially about Snd('Open', pahandle, 1)!
%
% Funny name is because Matlab 6 contains a built-in function called "beep".
%
% 2006-02-15 - cburns
%   -   Added fVolume param
%   -   Swapped parameter order
%
% 2006-02-02 - cburns
%   -   Scaled down the volume of the sound to match the system volume better.  It was at maximum volume.
%       Would scare you enough to rip the bite bar off it's mount!
%       And switched to using the sound() function, instead of the soundsc() function
%       which, by default, maximizes the volume.
%   -   Update, using the PsychToolbox Snd function which should return immediately.
%       Were experiencing delays with sound function
%
% 12/2/00 Backus - original version
persistent onetimedone;

if ~exist('frequency', 'var') || isempty(frequency)
    frequency = 400;
end

if ~exist('durationSec', 'var') || isempty(durationSec)
    durationSec = 0.15;
end

if ~exist('fVolume', 'var') || isempty(fVolume)
    fVolume = 0.4;
else
    % Clamp if necessary
    if fVolume > 1.0
        fVolume = 1.0;
    elseif fVolume < 0
        fVolume = 0;
    end
end

if ischar(frequency)
    if strcmp(lower(frequency), 'high') frequency = 1000;
    elseif strcmp(lower(frequency), 'med') frequency = 400;
    elseif strcmp(lower(frequency), 'medium') frequency = 400;
    elseif strcmp(lower(frequency), 'low') frequency = 220;
    end
end

% Silence any console output of Snd():
if isempty(onetimedone)
    onetimedone = 1;
    oldverbo = Snd('Verbosity', 1);
else
    oldverbo = Snd('Verbosity', 0);
end

sampleRate = Snd('DefaultRate');

nSample = sampleRate*durationSec;
soundVec = sin(2*pi*frequency*(0:nSample-1)/sampleRate);

% Scale down the volume
soundVec = soundVec * fVolume;

try % this part sometimes crashes for unknown reasons. If it happens replace sound with normal beep:
    % Play:
    Snd('Play', soundVec, sampleRate);

    % Wait for playback done:
    Snd('Wait');

    % Close sound driver again:
    Snd('Close');

    % Restore old verbosity:
    Snd('Verbosity', oldverbo);
catch
    % Restore old verbosity:
    Snd('Verbosity', oldverbo);

    % Primitive beep() fallback:
    beep
end
