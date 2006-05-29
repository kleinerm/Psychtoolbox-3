function oldPriority=Priority(newPriority)

% oldPriority=Priority([newPriority])
%
% OS X: ___________________________________________________________________
%
% Unlike on OS 9 and Windows, On OS X setting a high priority will not
% disable important parts of your computer such as the keybaord and hard
% drive. Priority calls the Psychtoolbox mex function MachSetPriority.  For
% simple, OS-neutral control of process priority on OS X use Priority. For
% complicated, fine-grained, OS-specific control of process priority use
% MachSetPriority.    
%
% At priority settings greater than 0, your script must limit MATLAB's  use
% of CPU time to stay within the limits specified by the priority value.
% The consequence of failing to restrict the CPU time to the limits
% specified by the priority values is that OS X will demote MATLAB back to
% priority 0.  You can check to see if this has happened by calling
% Priority with no arguments.
%
% Within a script there are two ways to limit MATLAB's use of CPU time:
%
%   1. Call "WaitSecs" 
%   2. Call Screen('Flip',...)
% 
% Both calls will sleep the main MATLAB thread, surrendering CPU time to
% other threads on the system until the MATLAB thread is awakened.
% WaitSecs sleeps the MATLAB thread for a specified period.
% Screen('Flip',...) sleeps the MATLAB thread until the next video blanking
% interval.
%
% The priority value corresponds to the proportion of the video frame
% period  during which MATLAB is guranteed 100 percent of CPU time:
%
%   GuaranteedCPUtimePerFramePeriod = priority/10 * framePeriod
%
% If your computer has multiple video displays running at different frame
% rates then Priority will choose the shortest frame period.
%
% The consequence of a thread exceeding the specified limits on CPU usage
% are that the MACH kernel will revoke its real-time priority status.
% However, the threshold at which the MACH kernel decices to revoke
% priority is not known.  We have observed that at 100% CPU usage the
% Jaguar Mach kernel will revoke priority after 2.5 seconds, indepent of
% the CPU time requested when setting priority.  
%
% Note that if you change the video frame rate after setting priority then
% the framePeriod which Priority used when setting the priority level will
% no longer match the true frame period. Therefore you should change the
% priority level after changing the video mode and not before. 
%
% Because the OS X update process causes setjmp calls within MATLAB to
% delay for up to about 13ms, Priority kills the system update process when
% setting any priority greater than 0.  Priority will restart the update
% proceess when setting priority back to 0, but only if it was Priority
% itself which killed the update process.  If update was already killed
% when Priority was set to a level greater than 0 then priority will not
% restart the update process when restoring priority to 0. You can use
% KillUpdateProcess and StartUpdateProcess to kill and restart the update
% process  without using Prioirity.  Because killing and starting the
% update process sometimes requires that an adminstrator password be
% entered in the MATLAB command window,  you might prefer not kill and
% restart  with every call to Priority, but instead to do this only at the
% beginning and end of a MATLAB session by using KillUpdateProcess and
% StartUpdateProcess.
%
% OS 9: ___________________________________________________________________
%
% This help text is mostly a tutorial on how processor priority affects 
% interrupts, which is equally relevant whether you use Priority or Rush.
% 
% Priority is a MEX function that gets/sets the processor priority. The
% priority is set only if you supply an input argument. Processor priority
% is an integer. It's normally 0, allowing all interrupts to occur. It can
% be raised as high as 7, which blocks most interrupts. Here's a typical
% use:
% 	Screen(window,'WaitBlanking'); % Make sure all RUSHed functions are in memory.
% 	priority(7);
% 	for i=1:100
% 		Screen(window,'WaitBlanking');
% 		Screen('CopyWindow',w(i),window);
% 	end
% 	priority(0);
% 
% WARNING: Unless you're very careful, using Priority may freeze your
% keyboard and mouse, leaving you no alternative other than rebooting your
% computer. Be careful. We strongly suggest that you use the new Rush
% function instead of Priority, since Rush provides a much safer way of
% accomplishing the same thing, and more. In any case, your first step
% should be to read this document, so you'll understand what you're doing.
% 
% It's often useful to temporarily raise priority when you're running a
% critical bit of code, e.g. showing a real-time movie, in which you can't
% tolerate any pauses. On my PowerMac running at priority 0 (the normal
% state), I find that there are pauses of about 0.3 milliseconds that occur
% every few seconds. (Try running LoopTest or RushTest in the PsychTests
% folder.) These pauses represent the activity of interrupt-driven
% processes (Mac OS and device drivers) that steal time whenever they like.
% My testing indicated that showing a real-time movie under these
% conditions would force you to reserve several milliseconds of spare time
% on each iteration, so as to have enough slack to occasionally pause that
% long without running over into the next frame's display interval.
% Reserving this slack time wastes a large fraction of your computer's
% power. By instead raising the priority to 1 or more you greatly reduce
% the frequency of these intrusions, allowing you to waste less time on
% slack, and spend most of the time actually pumping the movie out.
% ScreenTest.m determines, by trial and error, the largest real-time movie
% you can show. By running at a higher priority you can show a larger
% real-time movie, without missing any frames.
% 
% HIGHEST PRIORITY FOR NORMAL OPERATION
% 0 keyboard, mouse, CPUTIME, GETTICKS, GETSECS (on 68K Mac or Mac OS older than 8.6), 
% 	Deferred Task Manager
% 1 PeekBlanking, sound, NuBus interrupts
% 3 serial port (LocalTalk)
% 7 most Matlab operations, GETSECS (on PowerMac with Mac OS 8.6 or better)
% 
% CAUTION: Please don't use the above table as gospel. It's merely a
% combination of information from Apple (which they say may not be true of
% all Macs), and our practical experiments. Instead you should use
% MaxPriority.m to obtain accurate information, at runtime, for your
% specific configuration. MaxPriority accepts a list of the names of the
% functions you want to Rush, and returns the highest priority that will
% allow them all to run normally. Don't hard-code priorities (other than
% 0) into your programs. Use MaxPriority, so that your programs will
% gracefully adapt to run optimally on every computer.
% 
% Many things stop working when you raise the priority, e.g. keypresses
% mouse clicks, and mouse movement are ignored, so any program that uses
% Priority to raise the priority must call Priority again to restore the
% priority to zero before returning control to Matlab. (If you find
% yourself stuck, with a dead keyboard and mouse, try dropping into MacsBug
% and typing SR=0 to clear the status register, and then G to go back. That
% trick doesn't always work. Sometimes I can't get into MacsBug. And
% MacsBug will only understand the reference to the status register SR if
% your computer was in the midst of 68K code when you invoked MacsBug.) A
% key virtue of Rush over Priority is that Rush always restores the
% priority before returning, even if an error occurs in your code (it
% "traps" the error), so you won't get locked out.
% 
% The usual reason to raise processor priority is to prevent lengthy
% interruptions. In fact, Apple says each interrupt task should be brief
% (certainly less than a millisecond), and that it should defer any lengthy
% processing to a "deferred task" that it asks the Deferred Task Manager to
% run later, once all the pending interrupts have been processed and the
% processor priority is about to be brought back down to zero. The Deferred
% Task Manager maintains a first-in first-out queue of deferred tasks, to
% be run when the processor priority drops back to zero. If you raise the
% processor priority to 1 or more then the deferred tasks keep accumulating
% in the queue waiting until you eventually bring the priority back down to
% zero. Thus, raising the processor priority to 1 (or more) prevents
% lengthy interruptions mostly by preventing deferred tasks, rather than by
% blocking interrupts (though it does that too), since the interrupt tasks
% themselves are mostly brief.
% 
% Every interrupt has an associated priority. Keyboard, mouse, and Time
% Manager interrupts all occur at priority 1. VBL (i.e. blanking) and Sound
% Manager interrupts occur at priority 2. The interrupt occurs if the
% processor priority is lower than the interrupt level. Normally the
% processor is at priority zero. The interrupt causes execution to switch
% immediately to the primary interrupt task, which runs at the priority
% level of the interrupt. Apple recommends that primary interrupt tasks be
% as brief as possible, postponing most of the work to a "deferred" task,
% that is put in the Deferred Task Manager's queue. Deferred tasks are run,
% one at a time in the queued order, when the interrupt priority is lowered
% back down to zero. Normally, most of the interruption of user code is due
% to the deferred tasks, so blocking deferred tasks minimizes the
% interruption. Raising priority does that too, but at the expense of
% disabling useful interrupts (keyboard, mouse, Time Manager). Running the
% user's Matlab code as a deferred task (Rush priorityLevel 0.5, the
% default) blocks other deferred tasks (they can't run until this one
% finishes), but without raising priority, so all primary interrupts work
% normally, and the deferred tasks are all performed when Rush's deferred
% task finishes.
% 
% Zip & Jaz: Extensive testing (see RushTest) has revealed a problem with
% the Iomega 5.03 driver installed with Mac OS 7.6.1 for the Zip and Jaz
% removable disk drives. This driver creates a primary interrupt task that
% steals 2 ms once every 3 seconds. 2 ms is a long time to lose in a
% critical loop. I'm told, but haven't confirmed, that if you insert a disk
% into the empty drive the problem goes away. (This is also suggested for
% your floppy disk drive. It's periodic scanning of empty drives, waiting
% for a disk insertion, that seems to be the problem. Another alternative
% would be to remove the Iomega 5.03 driver (remove it from the Extensions
% folder and reboot) before using Rush. Alternatively, consider following
% the lead of the many members of the Macintosh PCI Discussion List
% <mailto:LISTSERV@MITVMA.MIT.EDU>, who recommend using the $100 FWB Hard
% Disk Toolkit <http://www.fwb.com/> driver for all removable media,
% including Iomega Zip and Jaz drives, instead of the drivers that came
% with the drives. (We haven't tried this; we're hoping that Iomega will
% fix their driver soon. For news you might look here:
% <http://www.macintouch.com/jazprobs.html>)
% 
% Another reason to raise processor priority is to figure out which
% interrupt-driven tasks are interrupting you. The priority of the process,
% which you might determine by how high you have to raise the priority in
% order to block it, may help you identify it.
% 
% INTERRUPT PRIORITIES OF PROCESSES
% 1 system-based 60.15 Hz VBL Manager (ticks, cursor),
% 	Time Manager (including Microseconds)
% 2 slot-based VBL Manager, Slot Manager, Sound Manager
% 4 LocalTalk, serial ports
% 
% Interrupts are blocked by setting the processor priority greater than or
% equal to the interrupt's priority. (The table is incomplete, because no
% complete table exists. According to Apple Developer Support, some of the
% values depend on which particular model of Mac you're  running on.)
% 
% Time Manager interrupts are blocked by setting the priority to 1 or more.
% This affects both Time Manager tasks and the Microseconds function. The
% Microseconds time function continues to advance at the right rate, but in
% coarse steps of about 0.3 ms, instead of its usual 20 µs, and overflows
% every 0.1 seconds, if I remember rightly. (The behavior of the
% Microseconds trap when interrupts are suppressed may vary among different
% Macintosh models.)
% 
% GETSECS, a Psychtoolbox MEX function, uses Microseconds unless it's
% running on a PowerMac with Mac OS 8.6 or better. On a PowerMac with  Mac
% OS 8.6 or better, GETSECS uses the excellent UpTime trap that relies on
% an internal counter in the PowerPC chip. UpTime doesn't use interrupts
% and thus returns reliable times, no matter what the processor priority
% is.
% 
% CPUTIME, a Matlab built-in function, uses a Time Manager task. As above,
% if processor priority is 1 or more, CPUTIME advances in coarse steps of
% about 0.3 ms, but it overflows after about a tenth of a second (0.08366
% s). Time intervals that are shorter than the overflow time will mostly be
% measured correctly, but a few--those that straddle an overflow--will be
% reported as negative values, which you can fix up by adding 0.08366.
% (It's modulo arithmetic.)
%
% WINDOWS: ________________________________________________________________
% 
% For the Windows version of Priority (and Rush), the priority levels set
% are  "process priority levels". There are 3 priority levels available,
% levels 0, 1, and 2. Level 0 is "normal priority level", level 1 is "high
% priority level", and level 2 is "real time priority level". Combined with
% thread priority levels, they determine the absolute priority level of the
% matlab  thread. Threads are executed in a "round robin" fashion on
% Windows, with the  lower priority threads getting cpu time slice only
% when no higher priority thread  is ready to execute. Currently, no tests
% had been done to see what tasks are pre-empted by setting the Matlab
% process to real-time priority. It does seem to block keyboard input,
% though, so for example if you have a clut animation going on at priority
% level 2, then the force-quit key combo (Ctrl-Alt-Delete) does not  work.
% However, the keyboard inputs are still sent to the message queue, so
% GetChar or GetClicks still work if they are also called at priority level
% 2.
% 
% _________________________________________________________________________
%
% see also OS X:    Rush
% see also Windows: Rush
% see also OS 9:    Rush, ScreenTest, RushTest, LoopTest

% NOTES:
% priorityStruct looks like this.   
%
% threadID:
% flavor: 'THREAD_TIME_CONSTRAINT_POLICY'
% policy:
%     period: 
%     computation: 
%     constraint: 
%     preemptible: 
% policySize:
% policyFillSize:
% getDefault:
% isDefault:

% HISTORY:
% 4/13/97   dgp  Wrote it
% 4/22/97   dgp  Updated
% 5/1/97    dgp  Updated
% 5/31/97   dgp  Updated
% 6/3/97    dgp  Updated
% 3/15/99   xmz  Added comments for Windows version.
% 2/4/00    dgp  Updated for Mac OS 9.
% 6/13/02   awi  Cosmetic 
% 12/11/03  awi  Wrote documentation and scripting for OS X
% 7/16/03   awi  Merged together documetation for OS X and OS 9 and
%                Windows.
% 8/24/05   awi  Kill and start update under standard, not time constraint
%                priority.
%                When raising priority, issue error and return if we fail
%                to kill the update process.  Do not set time contstraint
%                priority.   
% 2/21/06   mk   Updated for Linux.
% 5/29/06   mk   Merged a fix for properly restarting update process.
%                Bug found & fix provided by Michael Ross.

if IsLinux
    % Not yet implemented on Linux. Just return zero.
    oldPriority=0;
    return;
end;

if IsOSX & IsOctave
    % Not yet implemented on MacOS-X + GNU/Octave. Just return zero.
    oldPriority=0;
    return;
end;

if IsOSX
    
    persistent didPriorityKillUpdate;
    
    %Get the current settings.
    [flavorNameString, priorityStruct] = MachGetPriorityFlavor;
    if strcmp('THREAD_STANDARD_POLICY', flavorNameString)
        oldPriority=0;
    else % strcmp('THREAD_TIME_CONSTRAINT_POLICY', flavorNameString)
        %Values in priority struct returned by MachGetPriorityFlavor are in ticks but should be in seconds
        %it does not matter here because we are only concerned with the ratio
        if priorityStruct.policy.period == 0 | priorityStruct.policy.computation == 0
            %this is an illegitimate setting, so restore to standard priority
            %and go from there.
            MachSetStandardPriority;
            oldPriority=0;      
        else
            oldPriorityRatio= priorityStruct.policy.computation / priorityStruct.policy.period;
            oldPriority=oldPriorityRatio * 10;
        end
    end
    
    
    %if no new setting is given then return
    if nargin==0
        return
    end
    
    %bounds check the input argument
    if newPriority > 9
        error('"newPriority" value exceeds maximum allowable value of 9');
    elseif newPriority < 0
        error('"newPriority" value is less than the minimum allowable value of 0');
    end
    
    %if the priority level calls for time constraint priority...
    if newPriority > 0
        if isempty(didPriorityKillUpdate)
            didPriorityKillUpdate = 0;
        end

        tempWasKilledByUs=KillUpdateProcess;    
        if isnan(tempWasKilledByUs)
            error('Failed to raise priority because the simplepsychtoolboxsetup.sh script had not been run.  Run simplepsychtoolboxsetup.sh and try again.');
        else
            didPriorityKillUpdate=tempWasKilledByUs + didPriorityKillUpdate;
        end
        
        % Find the frame periods.  FrameRate returns the nominal frame rate, which
        % for LCD displays is 0.  We assume 60 in that case.
        defaultFrameRate=60;    %Hz
        screenNumbers=Screen('Screens');
        for i = 1:length(screenNumbers)
            frameRates(i)=Screen('FrameRate', screenNumbers(i));
        end
        [zeroRates, zeroRateIndices]=find(frameRates==0);
        frameRates(zeroRateIndices)=defaultFrameRate;
        framePeriods=1./frameRates;
        
        % MachSetTimeConstraintPriority(periodSecs,computationSecs, constraintSecs, preemptibleFlag) 
        periodSecs=min(framePeriods);
        computationSecs = newPriority/10 * periodSecs;
        constraintSecs=computationSecs;
        preemptibleFlag=1;
        
        
        %set the prioirity
        MachSetTimeConstraintPriority(periodSecs,computationSecs, constraintSecs, preemptibleFlag);
        
        %if the priority level calls for standard priority then ..
    else    %priority==0
        %restore standard priority
        MachSetStandardPriority;
        
        %restore the update process if it was Priority which killed it.  
        if didPriorityKillUpdate        
            StartUpdateProcess;
            didPriorityKillUpdate=0;
        end
    end
end;
