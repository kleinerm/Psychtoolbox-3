function MachSetTimeConstraintPriority(periodSecs,computationSecs, constraintSecs, preemptibleFlag)

% MachSetTimeConstraintPriority(periodSecs,computationSecs, constraintSecs, preemptibleFlag)
%
% Assign the main MATLAB thread "time constraint" status, giving it super
% priority  over other threads on the system.  A thread given "time
% constraint" status  can not be preempted by other threads and cedes CPU
% time to them on its own terms.  
%
%  MachSetTimeConstraintPriority guarantees that within any block of 
% "periodSecs" consecutive seconds the MATLAB thread is granted at least
% computationSecs seconds of CPU time.  
%
% Use "constraintSecs" to specify how  "computationSecs" of CPU time is
% distributed throughought  the period "periodSecs". If the CPU time
% specified by "computationSecs" were broken and spread throught the entire
% interval "periodSecs", then the actual time to complete the
% calculationSecs of computation will be greater than computationSecs
% itself;    computationSecs of CPU usage could require up to periodSecs to
% complete. Argument "constraintSecs" specifies a period in which
% computationSecs of CPU usage  is guranteed  to complete.  Note that
% constraintSecs must be >= computationSecs because computationSecs of CPU
% usage can not complete in less than  computationSecs. The maximum latency
% from the start of a computation to the end is constraintSecs -
% computationSecs.
%
% Setting the preemptibleFlag flag allows MATLAB to be interrupted subject
% to constraintConstraint.   
%
% Granting a thread "time constraint" priority status gives it unlimited
% and uninterruptable   control of the CPU, regardless of the limits
% specified by arguments to MachSetTimeConstraintPriority.  If a thread
% exceeds the limits on CPU time usage as specfied  by arguments to
% MachSetTimeConstraintPriority then the Mach Kernel's task  scheduler will
% eventually notice a problem and revoke "time constraint" priority status.
% On a dual 1.2 GHz G4 running OS X 10.2 the Mach task scheduler demotes
% MATLAB to standard priority after 2.5 seconds of uninterrrupted loop
% execution at "time constraint" priority.   
%
% For your script to abide  by CPU time usage limits set by arguments to
% MachSetTimeConstraintPriority it is necessary that it limit its own use
% of CPU time by calling blocking functions which surrender CPU time to
% other applications.    Use "BlockSecsMex" to surrender a specified period
% of time or use  Use Screen('Flip') to surrender time until the next video
% blanking interval. MATLAB when idle at the command line will
% automatically block.
%
% MachSetTimeConstraintPriority is primarily intended to be used with 
% Screen('CopyWindow') animations to avoid interruptions for periods long
% enough to cause your animation script to miss an entire video frame.  
% For animations:
%       periodSecs- should be set to the video frame period as obtained
%       with 1/Screen('FrameRate').
%
%       computationSecs- should exceed the maximum amount of time which
%       your  script requires to render any single video frame. 
%
%       constraintSecs need only be greater than computationSecs and less
%       than or equal to  periodSecs, within those limits should not effect
%       performance.    
%
% MachSetTimeConstraintPriority should be called immediatly before the
% start of  the animation loop.  To restore standard priority status to
% MATLAB after the  animation ends use "MachSetStandardPriority"
%
% MachSetTimeConstraintPriority converts time arguments specified in units
% of seconds to units of bus ticks and calls MachSetPriorityMex.
% MachSetPriorityMex, in turn, invokes the OS X call thread_policy_set with
% the THREAD_TIME_CONSTRAINT_POLICY constant. For more info on
% thread_policy_set see:
%   Psychtoolbox3/Source/Common/MachPriorityMex/MachGetPriorityMex.c
%   http://developer.apple.com/documentation/Darwin/Conceptual/KernelProgramming/scheduler/chapter_8_section_4.html
%   /usr/include/mach/thread_policy.h 
%
% SEE ALSO: MachSetStandardPriority, MachSetPriorityMex, BlockSecsMex
%
% AUTHORS:
% Allen Ingling     awi     Allen.Ingling@nyu.edu
% Mario Kleiner     mk
%
% HISTORY: 
% 8/13/03   awi     Wrote it.
% 2/17/05   mk      bug-fix: constraintSecs argument was ignored.
% 4/6/05    awi     Added Mario's bug fix to Psychtoolbox.org master.
% 4/6/05    awi     Replaced "GetBusFrequencymex" calls with "MachTimebase"
% 4/8/05    awi     Changed "MachTimebase" to "MachAbsoluteTimeClockFrequency"

if ~(nargin==4 || nargin == 0)
    error('wrong number of arguments')
end
if nargin==0  % ths is not documented in the help.  We use default parameters if no arguments are passed.  
    MachSetPriorityMex('THREAD_TIME_CONSTRAINT_POLICY', 'default');
    return
end

ticksPerSecond= MachAbsoluteTimeClockFrequency; 
periodTicks = round(periodSecs * ticksPerSecond);
computationTicks = round(computationSecs * ticksPerSecond);
constraintTicks = round(constraintSecs * ticksPerSecond);
MachSetPriorityMex('THREAD_TIME_CONSTRAINT_POLICY', periodTicks, computationTicks, constraintTicks, preemptibleFlag);


