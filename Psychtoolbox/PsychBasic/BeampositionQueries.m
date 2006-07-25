% BeampositionQueries -- What they are used for, and what can go wrong.
%
% MacOS-X provides a mechanism that allows to query the scanline which
% is currently updated by the scanning beam of a CRT display or by
% the equivalent mechanism in a video beamer or flat panel display,
% the so called "beamposition".
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
% on MacOS-X.
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
% On Microsoft Windows, there is currently nothing we can do about this - the
% price that people pay for running the worst operating system designed ever.
% The only "solution" would be to write special proprietary kernel mode drivers
% that implement a mechanism like on MacOS-X -- this would be time consuming and
% extremely painful, given the "openness" of Microsoft Windows.
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
% In case that beamposition queries should not work properly,
% PTB will use the same fallback strategy as on Linux and Windows: Just acquire
% a timestamp and live with the reduced accuracy and robustness.
% If the beampos query test fails, you will see some warning message about
% "SYNCHRONIZATION TROUBLE" in the Matlab/Octave command window.
%
% There are two possible causes for failure:
%
% 1. System overload: Too many other applications are running in parallel to
% Psychtoolbox, introducing severe timing noise into the calibration and test loop.
% See 'help SyncTrouble' on what to do.
%
% 2. Driver bug: Not much you can do, except submit a bug report to Apple for
% your specific hardware + software setup.
%

% History:
% 17.06.2006 Written (MK).
