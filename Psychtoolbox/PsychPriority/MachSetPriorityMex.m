function MachSetPriorityMex(policyFlavorString)

% priority=MachSetPriorityMex(policyFlavorString [,arg1] [,arg2] [,arg3] [,arg4])
%
% OS X: ___________________________________________________________________
%
% Set  one of three priority flavors for the main MATLAB thread:
% 'THREAD_STANDARD_POLICY', 'THREAD_TIME_CONSTRAINT_POLICY',
% 'THREAD_PRECEDENCE_POLICY'.
%
% Each of the threed flavors has an associated set of parameters 
% following the flovor argument string in the first position:
%
% MachSetPriorityMex('THREAD_STANDARD_POLICY');
%  There are no parameters.    
%      
% MachSetPriorityMex('THREAD_TIME_CONSTRAINT_POLICY',
% 'periodTicks','computationTicks', 'constraint', 'preemptible');
%  See MachSetTimeConstrainPriority for an explanation of arguments.  Note
%  that MachSetPriorityMex arguments "period","computation", and
%  "constraint" are in units of bus tick rate which depends on you
%  particular model of computer, whereas MachSetTimeConstrainPriority
%  accepts arguments in units of seconds.  Therefore it is better to use
%  MachSetTimeConstrainPriority than MachSetPriorityMex.  
%         
% MachSetPriorityMex('THREAD_PRECEDENCE_POLICY', 'importance');
%  Set the priority of the main MATLAB thread relative to other threads of
%  the MATLAB process.  This is not useful.  
%
% Parameters for any priority flavor may be omitted and the single string
% argument 'default' substituted in their place:  
%   MachSetPriorityMex('THREAD_STANDARD_POLICY', 'default');
%   MachSetPriorityMex('THREAD_TIME_CONSTRAINT_POLICY', 'default');
%   MachSetPriorityMex('THREAD_PRECEDENCE_POLICY', 'default');
% Call MachGetPriorityMex with the 'default' flag argument set to discover
% default values. The default values are provided by the the underlying
% thread_policy_get() function.
%
% There are three policy flavors but a thread may have only one of two 
% policy modes: THREAD_STANDARD_POLICY or THREAD_TIME_CONSTRAINT_POLICY.
% These are mutually-exclusive modes; setting a thread to either one will 
% unset the other mode.  The "importance" parameter associated with 
% THREAD_PRECEDENCE_POLICY is preserved after either THREAD_STANDARD_POLICY
% or THREAD_TIME_CONSTRAINT_POLICY is set, however the
% THREAD_PRECEDENCE_POLICY setting is ignored by the Mach task scheduler
% while a thread is in  THREAD_TIME_CONSTRAINT_POLICY mode.  A thread is
% governed by the   THREAD_PRECEDENCE_POLICY "importance" parameter only
% when in THREAD_STANDARD_POLICY mode.  
%
% MachSetPriorityMex calls the OS X Dawrin function thread_policy_set().
% for more information on thread_policy_set() see:
% Psychtoolbox3/Source/Common/MachPriorityMex/MachGetPriorityMex.c
% Psychtoolbox3/Source/Common/MachPriorityMex/MachSetPriorityMex.c
% http://developer.apple.com/documentation/Darwin/Conceptual/KernelProgramming/scheduler/chapter_8_section_4.html
% /usr/include/mach/thread_policy.h
%
% OS 9: ___________________________________________________________________
%
% MachSetPriorityMex does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% MachSetPriorityMex does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: Priority, Rush, MachGetPriorityMex, MachSetTimeConstraintPolicy

% AUTHORS:
% Allen Ingling     awi     Allen.Ingling@nyu.edu

% NOTES:
% The "Mex" in "MachSetPriorityMex" indicates that that the compiled mex
% binary uses the Mex API instead of of the PsychScriptingGlue API to
% communicated with MATLAB.  This was done to make it more readablle  to
% Mathworks developers and others looking for the timing bug, but that  has
% been explained and MachSetPriority could be re written to
% PsychScriptingGlue.

% HISTORY: 
% 8/13/03   awi     Wrote it.   
% 7/16/04   awi     Cosmetic

