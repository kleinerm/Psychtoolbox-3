function varargout = PsychPortAudio(varargin)
% PsychPortAudio - High precision sound driver for Psychtoolbox-3.
%
% PsychPortAudio is a new sounddriver for PTB-3. It is meant to become a
% replacement for all other Matlab based sound drivers and PTB's old SND()
% function. It's currently early beta, so use it with care, may still have
% some problems.
%
% PsychPortAudio provides (or will provide) the following features:
%
% - Allows instant start of sound playback with a very low onset latency
%   compared to other sound drivers.
%
% - Allows start of playback at a scheduled future system time: E.g.,
%   schedule sound onset for a specific time in the future (e.g., visual
%   stimulus onset time), then do other things in your Matlab code.
%   Scheduled start of playback can be accurate to the sub-millisecond level
%   on some system setups.
%
% - Wait for sound onset, or continue with execution of Matlab code
%   immediately.
%
% - Asynchronous operation: Sound playback works in the background while
%   your Matlab code continues to do other things.
%
% - Infinitely repeating playback, or playback of a sound for 'n' times.
%
% - Returns timestamps and status for all crucial events.
%
% - Support multi-channel devices, e.g., 8-channel sound cards.
%
% - Supports multi-channel sound capture and full-duplex capture
%   and playback of sound on some systems.
%
% - Enumerate, open and use multiple sound cards in parallel.
%
% - Reliable (compared to Matlabs sound facilities).
%
% - Efficient, causes only very low cpu load.
%
% See the "help InitializePsychSound" for more info on low-latency
% configurations. See "help BasicSoundOutputDemo" for a very basic demo of
% sound output (without special emphasis on low-latency). See
% "BasicSoundInputDemo" for a basic demo of sound capture.
% "PsychPortAudioTimingTest" is a script that we used for testing PA's
% sound onset latency and accuracy...
%
% Type "PsychPortAudio" for an overview of supported subfunctions and
% "PsychPortAudio Subfunctionname?" for help on a specific subfunction.
%
% WARNING: EARLY BETA STAGE - USE WITH CAUTION!
%
% PsychPortAudio is built around a modified version of the free, open-source
% PortAudio sound library for portable realtime sound: http://www.portaudio.com

% History:
% 06/07/2007 Written (MK).

% Some check for not yet supported operating systems:
AssertMex;
