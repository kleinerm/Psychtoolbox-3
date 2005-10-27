function WaitSecs(s)
% WaitSecs(s)
%
% Waits "s" seconds with high precision.  The timing precision  depends on
% the model of your computer, use GetSecsTick to report the  precision for
% your computer.  
% 
% WaitSecs(s) is similar to Matlab's built-in PAUSE(s) command. The
% advantage of WaitSecs(s) is that it is much more accurate, and can be
% aborted by hitting Command-Period. However, PAUSE can be turned 'ON' and
% 'OFF', which is useful for scripts.
% 
% TIMING ADVICE: the first time you access any MEX function or M file,
% Matlab takes several hundred milliseconds to load it from disk.
% Allocating a variable takes time too. Usually you'll want to omit those
% delays from your timing measurements by making sure all the functions you
% use are loaded and that all the variables you use are allocated, before
% you start timing. MEX files stay loaded until you flush the MEX files
% (e.g. by changing directory or calling CLEAR MEX). M files and variables
% stay in memory until you clear them.
%
% OS X: ___________________________________________________________________
%
% WaitSecs always uses the high-precision uptime clock.  It sleeps the main
% MATLAB thread for the given wait period, surrendering CPU time to other
% processes while waiting.  WaitSecs is now safe to use at any priority
% setting.  
%
% WaitSecs ignores the OX MATLAB <ctrl>-C break key sequenece.
%
% OS 9: ___________________________________________________________________
%
% Uses the extremely accurate PowerPC processor clock if the UpTime trap is
% available (PowerMac  with Mac OS 8.6 or better). Otherwise uses the Time
% Manager's Microseconds trap. The Time Manager seems to lose time when
% interrupts are suppressed, whereas UpTime is unaffected.
%
% GetSecs, WaitSecs, and several other MEX files use the VideoToolbox
% Seconds.c function as their timebase. Seconds.c uses the Mac OS UpTime
% trap if available. This trap is based on a hardware counter in the
% PowerPC processor, which provides extremely stable reliable timing.
% However, the Mac OS conversion from a count to a time uses an
% Apple-supplied conversion factor that is only accurate to about 1%. To
% make your timing more accurate you can scale all values returned by
% Seconds.c by setting the scale factor different from 1 in Screen
% Preference SecondsMultiplier. Run GetSecsTest to do this automatically.
%
% WINDOWS:_________________________________________________________________
%
% WaitSecs uses  Windows QueryPerformanceCounter() call which, in turn, 
% reads a high-performance hardware counter in Pentium and better CPUs.
%
% WaitSecs ignores the Win MATLAB <ctrl>-C break key sequenece.
%
% _________________________________________________________________________
% 
% See also: GetSecs, GetSecsTick, GetTicks, WaitTicks, PAUSE.

% 1/29/97   dhb     Wrote it.
% 3/15/97   dgp     Expanded comments.
% 2/22/99   dgp     Mention PAUSE.
% 2/4/00    dgp     Updated for Mac OS 9.
% 7/2/04    awi     Divided into separate sections for OS X, Mac and Windows.  
% 7/10/04   awi     Edits for clarity.
