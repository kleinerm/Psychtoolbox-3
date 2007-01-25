function WaitSecs(s)
% WaitSecs(s)
%
% Waits "s" seconds with high precision.  The timing precision  depends on
% the model of your computer, but a well configured system will be accurate
% to about 1 millisecond if your script is executed with realtime-priority
% (See help Priority) and well written.
% 
% WaitSecs(s) is similar to Matlab's built-in PAUSE(s) command. The
% advantage of WaitSecs(s) is that it is much more accurate. However, PAUSE
% can be turned 'ON' and 'OFF', which is useful for scripts.
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
