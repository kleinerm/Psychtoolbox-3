function Rush(rushedCode, priorityLevel)

% Rush(rushedCode, priorityLevel)
%
% RUSH.mex runs a critical bit of your Matlab code with minimal
% interruption by other tasks. The first argument is a
% string containing Matlab code to be passed to EVAL. Within the string,
% you can have multiple statements separated by ";" or ",".
% 
% The optional "priorityLevel" argument specifies how much to block
% interrupt tasks. Use MaxPriority to determine the highest priority that allows normal
% operation of the functions you use, e.g. SND and SCREEN 'WaitBlanking'.
% We suggest you always call MaxPriority rather than hard coding any
% particular priorityLevel, so that your program will gracefully adapt to
% run optimally on any computer. Here's a typical use:
% 
% 	SCREEN('Screens');	% Make sure all functions (SCREEN.mex) are in memory.
% 	i=0;				% Allocate all variables.
% 	loop={
% 		'for i=1:100;'
% 			'SCREEN(window,''WaitBlanking'');'
% 			'SCREEN(''CopyWindow'',w(i),window);'
% 		'end;'
% 	};
% 	priorityLevel=MaxPriority(window,'WaitBlanking');
% 	RUSH(loop,priorityLevel);
% 
% For a complete example, see MovieDemo.
% 
% OS X: ___________________________________________________________________
%
% The allowed priority values are any number between 0 and 9.  
%
% On OS 9 and Windows Rush is better than Priority becasue it prevents your
% computer from locking up if a MATLAB script crashes while running at high
% priority.  However, on OS X, if a script crashes even at highest priority
% the MATLAB prompt will be restored to you without using Rush; Priority is
% just as safe as Rush.   You should continue to use Rush on OS X if you
% want your scripts to handle crashes graciously with Windows and OS 9
% Psychtoolboxes. 
%
% OS 9: ___________________________________________________________________
%
% The allowed priority values are 0, 0.5, 1, 2, 3, 4, 5, 6, and 7.
%
% A priorityLevel of 0 gives normal execution: simply calls EVAL. RUSH
% offers two approaches to minimizing interruption, selected by setting
% priorityLevel 0.5 (the default) or higher (1 ... 7). Both approaches
% temporarily block the processing of deferred tasks, which lessens
% interruption of your code. ("Deferred" tasks are called by the Mac OS to
% do the time-consuming work scheduled by an interrupt service routine.)
% Setting priorityLevel>0.5 also blocks interrupts, blocking more
% interrupts as the priorityLevel is raised higher. Raising priority
% disables important functions, which is okay if your rushed code doesn't
% use them.
%   
% Matlab 5.2.1 allows dynamic control of backgrounding. RUSH uses that
% to save the old state, turn backgrounding off while Rushing your
% code, and then restores backgrounding to its former state. See
% SCREEN Preference Backgrounding.
% 
% WARNING: If you're using Matlab older than version 5.2.1, then you
% must turn off Matlab's Backgrounding Preference before calling Rush.
% This is partially enforced by Rush. It checks when you first call
% it. It only checks once because the check is slow (reads the Matlab
% Prefs file). You could fool Rush by turning Backgrounding back on
% after Rush has run, and then calling Rush again. Don't do that. It's
% dangerous to allow Matlab to share time with other Mac processes
% while you've raised processor priority. If they wait for an
% interrupt (eg to access the disk) the computer will hang.
% 
% WARNING: You must remember to load all to-be-Rushed functions into
% memory before calling Rush at priority>0. Otherwise you'll hang when
% Matlab tries to read the function from disk at raised priority.
% 
% WARNING: When priorityLevel is nonzero (default), RUSH blocks all
% deferred tasks. The Macintosh File Manager uses deferred tasks, so
% you can't use it from within the string of Matlab code you pass to
% RUSH. If RUSH runs your string at nonzero priorityLevel and your
% code causes Matlab to access the Macintosh File Manager (e.g. to
% open, read, write, or close a file) then your code will wait forever
% for the File Manager to finish. The first time you access any Matlab
% function that isn't built-in, Matlab asks the File Manager to read
% the function from disk into memory. Thus you must make sure that
% you've already loaded (i.e. used) all the functions that appear in
% your string before calling Rush with nonzero priorityLevel. (At zero
% priorityLevel Rush is safe but doesn't minimize interrupts.) Though
% not required, it's good to pre-allocate all variables, so that
% memory allocation delays don't slow down the code you're RUSHing.
% 
% WARNING: Apple's Virtual Memory uses the hard disk to swap pages of
% memory. Thus any memory access, eg using a Matlab array, may require
% a disk access. As noted above, any attempt to access the disk at
% raised priority will hang forever. RAM Doubler mostly works without
% disk access, by doing compression and grabbing unused bits of
% memory, but will resort to disk access if it runs out of real
% memory. RUSH.mex will run your code at whatever priority you ask
% for, so be careful. MaxPriority returns 0 when VM in on, and prints
% a warning. Rushing at priority 0 is safe, but the only benefit is
% that RUSH turns off backgrounding. For RAM Doubler, MaxPriority
% merely prints a warning, returning the usual values, assuming that
% RAM Doubler won't access the disk. We suggest turning off Virtual
% Memory and RAM Doubler, and buying plenty of memory, since it's now
% so cheap.
% 
% With priorityLevel 0.5, everything is close to normal, and you can
% do anything except use the File Manager, as noted above. On all
% Macs, raising priority to 1 or more blocks keyboard and mouse
% interrupts (mouse and keyboard are dead). On a PowerMac with 
% Mac OS 8.6 or better, GETSECS works
% fine at all values of priorityLevel. On other Macs, GETSECS is
% impaired if the priority is raised above 0.5 (advancing in coarse
% steps of 0.3 ms and overflowing after about 0.1 s). Don't bother
% trying to figure this out; just call MaxPriority with a list of the
% functions you use.
% 
% We anticipate that the most common application of RUSH will be to
% show movies, as in the example above. How big a movie you can show
% depends on how many pixels your computer can transfer during one
% frame interval. However, since RUSH won't block all interruptions,
% you should allow a bit of slack, so that you'll finish moving all
% the pixels within the frame interval even if you're briefly
% interrupted. At a higher priorityLevel there will be less
% interruption and you can allow less slack. At priorityLevel 7 there
% should be hardly any interruption. Use MaxPriority to select the
% highest priority compatible with the functions you're rushing.
%
% See Priority for a tutorial on processor priority.
% 
% See MaxPriority, MovieDemo, ScreenTest, RushTest, SCREEN Preference?.
% 
% 	times=Rush(numberOfSamples,[priorityLevel])
% This alternate calling form is solely for testing purposes, to
% distinguish Mac OS (and device driver) interrupts from Matlab delays. If
% the first argument is a positive integer (instead of the usual string),
% then Rush will collect that many time samples, using Seconds.c (like
% GetSecs), in a C loop that iterates about once per millisecond. See
% RushTest.
%
% WINDOWS: ________________________________________________________________
%
% The allowed priority values are 0, 1 and 2.
%
% _________________________________________________________________________
%
% see also: Priority


% HISTORY
% 4/21/97   dgp Wrote it.
% 4/22/97   dgp Added alternate calling form.
% 4/23/97   dgp Updated.
% 4/30/97   dgp Updated to incorporate results of running RushTest.
% 5/6/97    dgp Explain how to choose a priorityLevel.
% 5/30/97   dgp Explain WaitForVBLInterrupt Preference.
% 6/22/97   dgp WaitBlanking calls cscSetEntries, not cscGetEntries.
% 2/1/98    dgp MaxPriority.
% 2/8/98    dgp add priorityLevel 0.5.
% 3/24/98   dgp eliminate priorityLevel -1.
% 7/16/98   dgp demonstrate use of cell array in example.
% 7/22/98   dgp Added warnings about loading functions and Backgrounding.
% 3/15/99   xmz Added comment for Windows version.
% 3/18/99   dgp Added warnings about VM. Document dynamic control of backgrounding.
% 2/4/00    dgp Updated for Mac OS 9.
% 12/16/03  awi Wrote OS X Documentation and merged with OS 9 + Windows Rush.m.
% 7/16/04   awi Cosmetic.  Restored HISTORY.

% On all platforms there should be a mex version of this file which
% executes instead of this file.  See Help PsychAssert. 
PsychAssertMex;     
   
    
