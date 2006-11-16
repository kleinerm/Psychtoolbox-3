% BeampositionQueries -- What they are used for, and what can go wrong.
%
% MacOS-X and M$-Windows provide a mechanism that allows to query the
% scanline which is currently updated by the scanning beam of a CRT
% display or by the equivalent mechanism in a video beamer or flat 
% panel display, the so called "beamposition".
%
% We use this mechanism for two purposes:
%
% 1. Independent measurement of the monitor refresh interval: Psychtoolbox
% executes a measurement loop during Screen('OpenWindow') where it determines
% the monitor refresh interval. It takes a timestamp whenever the beamposition
% resets to zero at the start of a new display refresh cycle. The difference
% between consecutive timestamps is a sample of refresh duration. The samples
% of 50 consecutive refresh cycles are averaged. The "length" of the VBL
% is also computed as the difference between screen height (e.g., 1024 at a
% display resolution of 1280 x 1024 pixels) and the highest scanline value
% encountered during the 50 refresh cycles.
%
% This measurement is used to cross-check the results of the synchronization tests
% (see 'help SyncTrouble') and the value provided by the operating system to 
% make the sync tests and display calibration as robust as possible, even
% if the operating system reports bogus values like 0 Hz, which can happen
% on MacOS-X and Windows with some flat panels.
%
% 2. Highly accurate and robust stimulus onset timestamps in Screen('Flip').
%
% In normal operation, the Screen('Flip') command, after issuing a buffer-swap
% request to the gfx-hardware, pauses execution until the operating system
% signals swap-completion in sync with vertical retrace. Then it takes a high-
% precision system timestamp. Due to the scheduling jitter present in any
% operating system, sometimes the execution of Psychtoolbox is resumed only after
% some random multi-millisecond delay has passed after buffer-swap, so the
% stimulus onset timestamp can be possibly off by multiple milliseconds from
% the real stimulus onset time. This is unwanted timing noise, especially if
% the time stamp is to be used for either computing stimulus onset time for
% future stimuli, or for synchronizing Psychtoolbox execution with other
% stimulus generators or acquisition hardware.
%
% On Microsoft Windows 2000 and later, there is experimental support for
% beamposition queries. This support is only enabled on single display setups
% and multi-display setups which are configured to appear to Psychtoolbox as
% single-display setups, i.e., one "virtual" primary monitor which consists
% of multiple real displays in horizontal spanning mode. We use this mechanism
% to get high precision time stamps as described below for MacOS-X.
%
% On GNU/Linux, timing precision even in non-realtime scheduling mode is far
% superior to all other operating systems, yielding a timing jitter of less
% than 100 microseconds under normal operating conditions. For special needs,
% there exist multiple methods of improving timing down to microsecond level,
% although some of time require some advanced programming skills.
%
% On MacOS-X, whose timing accuracy is somewhere between Linux and Windows,
% and luckily closer to Linux timing than to Windows timing, we use beamposition
% queries to improve accuracy of our timestamps:
%
% When taking the system timestamp, we also query the current rasterbeam position.
% From the known height of the display (in scanlines, including height of VBL),
% and the known refresh interval of our display, we can translate the current
% beam position into "elapsed time since start of VBL == elapsed time since
% double buffer swap". By subtracting this elapsed time value from our system
% timestamp, we get a corrected timestamp - the real system time of double buffer
% swap == start of VBL == aka stimulus onset. This allows for very accurate timestamps,
% despite possible non-deterministic multi-millisecond timing jitter. Psychtoolbox
% goes through great pains during startup to double-check that all required
% calibration values and mechanisms are accurate and work properly. You can assess
% the accuracy of all returned timestamps by use of the script VBLSyncTest.
%
% In case that beamposition queries should not work properly or are not supported,
% PTB will use different fallback strategies:
%
% On Microsoft Windows, only a normal - possibly noisy - timestamp is taken.
%
% On MacOS-X, PTB tries to get low-level access to the kernel interrupt handlers
% for the VBL interrupts and uses its values for timestamping the time of buffer-
% swap. This method is slightly less accurate and robust than the bemposition method,
% but should be still suitable for most applications. If the kernel-level queries should
% fail as well, PTB falls back to pure timestamping without any correction.
%
% The behaviour of PTB can be controlled by the command:
% Screen('Preference', 'VBLTimestampingMode', mode); where mode can be one of the
% following:
%
% -1 = Disable all cleverness, take noisy timestamps.
%  0 = Disable kernel-level fallback method (on OS-X), use beamposition or noisy stamps.
%  1 = Use beamposition, if it fails, use kernel-level, if it fails use noisy stamps.
%  2 = Use beamposition, but cross-check with kernel-level. Use noisy stamps if beamposition
%      mode fails. This is for the paranoid to check proper functioning.
%  3 = Always use kernel-level timestamping, fall back to noisy stamps if it fails.
%
% The default on OS-X and Windows with single display setups is "1". On Windows in
% explicit multi-display mode, we default to "-1" ie noisy timestamps, as the current
% beamposition mechanism is not yet mature and tested enough for multi-display mode on
% Windows.
%
% If the beampos query test fails, you will see some warning message about
% "SYNCHRONIZATION TROUBLE" in the Matlab/Octave command window.
%
% There are two possible causes for failure:
%
% 1. System overload: Too many other applications are running in parallel to
% Psychtoolbox, introducing severe timing noise into the calibration and test loop.
% See 'help SyncTrouble' on what to do.
%
% 2. Driver bug: Not much you can do, except submit a bug report to Apple or Microsoft
% for your specific hardware + software setup.
%
% Accuracy of beamposition method:
%
% Cross-checking of beamposition timestamps and kernel-level timestamps on a single
% display PowerPC G5 1.6 Ghz under OS-X 10.4.8 with NVidia GeforceFX-5200 showed an
% accuracy of beamposition timestamping of better than 100 microseconds, with a maximum
% deviation between the different methods of less than 200 microseconds.
%
% Initial checking on two Window PC's (Dell Inspiron 8000 Laptio, Geforce 2Go, Windows 2000,
% and some 3.2 Ghz Pentium-4 with NVidia Geforce 7800 GTX) shows a precision of about
% 30 microseconds. No further testing on Windows has been performed yet.
%
% Also check the FAQ section of http://www.psychtoolbox.org for latest infos.
%

% History:
% 17.06.2006 Written (MK).
% 16.11.2006 Updated for Windows exp. beampos support. (MK)

