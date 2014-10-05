% MexTimingLoopTest
% 
% Run a timing loop within a mex file, as part of the MATLAB process but without returing control to the 
%  MATLAB engine.  
%
% SEE ALSO: MATLABTimingTest.m
%
% AUTHORS:
% Allen Ingling     awi     Allen.Ingling@nyu.edu
%
% HISTORY: 
% 9/22/03   awi     Adapted from previous versions and other stuff.
% 11/04/03  awi     Added axis labels.
% 4/6/05    awi     Replaced "GetBusFrequencymex" calls with "MachTimebase"
% 4/8/05    awi     Updated "MachTimebase" to new name "MachAbsoluteTimeClockFrequency"



% Setting "time constrait" priority settings and blocking duration:
%
% periodSecs= 1/1000;                                  
% computationSecs = periodSecs/10;
% constraintSecs= computationSecs;              
% preemptibleFlag=1;
% blockingIntervalSecs=periodSecs;
%
% Out of every millisecond guarantee the MATLAB process up to 100 microseconds
% of CPU time.  That is, MATLAB will lay claim to up to 1/10 of the total CPU time.
% Gurantee that that MATLAB's 100 microseconds of CPU time fall within a
% 100 microsecond window, from the start of computation to the end of
% computation.  Within that window, allow MATLAB to be interrupted by other
% threads.
%
% Note that we do not know in advance of the timing loop how much CPU time we will
% actually need, that depends on how much CPU time MATLAB expends 
% in processing a scripted loop pass. We can however retroactively validate our
% choice of values by examing timing results:
% 
%  blockingInterval + actualMATLABCPUUsage = loopPassDuration
% 
% therefore:
%
%   loopPassDuration - blockingInterval = actualMATLABCPUUsage
%
% we know everything on the left hand side can find actualMATLABCPUUsage.  If  we have chosen 
% parameters correctly then:
%
% actualMATLABCPUUsage <= computationSeconds
% 
% and, 
%
% actualMATLABCPUUsage/(actualMATLABCPUUsage + blockingInterval) < computationSeconds/periodSeconds)
% 
% unknown quantity actualMATLABCPUUsage is found:
%
% loopPassDuration = actualMATLABCPUUsage + blockingInterval
% actualMATLABCPUUsage = loopPassDuration - blockingInterval
% 
% substituting into the inequality we get:
%
% (loopPassDuration - blockingInterval)/loopPassDuration < computationSeconds/periodSeconds)
%
% Note that the above is sufficient when loopPassDuration < periodSeconds.
% If loopPassDuration > periodSeconds then we must also have
% actualMATLABCPUUsage <= computationSeconds.

clear all;  

% designate an index for the block of test parameters to be used. 
tci = 1;                    %tci stands for  test condition index.  
fixedAxisY=0;
fixedAxisValueY=0.020;

% Test condition 1 shows delays at 30-second intervals.
%
%
% designate an index for this block of test parameters. 
tcb= 1;
%set flags which specify various test conditions
tc(tcb).enableTCPriority=1;
%specify the 'time constraint' parameters
tc(tcb).periodSecs= 1/1000;                                  
tc(tcb).computationSecs = tc(tcb).periodSecs/10 * 5 ;
tc(tcb).constraintSecs= tc(tcb).computationSecs;              
tc(tcb).preemptibleFlag=0;
tc(tcb).blockingIntervalSecs=tc(tcb).periodSecs - tc(tcb).computationSecs;
% specify for how many seconds we want to run out timing loop
tc(tcb).testDurationSeconds = 2*60+1;
%specify a factor by which to mulitply the memory allocation estimate, for
%saftey margin.
tc(tcb).maxDisplayExcessComputationDurations=100;
% specify values describing expected period glitches.  As part of the analysis after
% the timing test we search for regular glitches described by these
% paramters.
tc(tcb).interGlitchIntevalSecs = 30;
tc(tcb).interGlitchIntervalJitter = 0.1;        % tolerance of interGlitchIntevalSecs value.

% Test condition 2 shows glitches at 60 second intervals which increase in
% magnitude over the duration of the test period.  
%
% designate an index for this block of test parameters. 
tcb= 2;
%set flags which specify various test conditions
tc(tcb).enableTCPriority=1;
%specify the 'time constraint' parameters
tc(tcb).periodSecs= 1/60;                                  
tc(tcb).computationSecs = tc(tcb).periodSecs/10 * 4;
tc(tcb).constraintSecs= tc(tcb).computationSecs;              
tc(tcb).preemptibleFlag=0;
tc(tcb).blockingIntervalSecs=tc(tcb).periodSecs - tc(tcb).computationSecs;
% specify for how many seconds we want to run out timing loop
tc(tcb).testDurationSeconds = 4*60+1;
%specify a factor by which to mulitply the memory allocation estimate, for
%saftey margin.
tc(tcb).maxDisplayExcessComputationDurations=100;
% specify values describing period glithes.  As part of the analysis after
% the timing test we search for regular glitches described by these
% paramters.
tc(tcb).interGlitchIntevalSecs = 30;
tc(tcb).interGlitchIntervalJitter = 0.1;        % tolerance of interGlitchIntevalSecs value.
 
% Test condition 3 shows ?
%
% designate an index for this block of test parameters. 
tcb= 3;
%set flags which specify various test conditions
tc(tcb).enableTCPriority=1;
%specify the 'time constraint' parameters
tc(tcb).periodSecs= 1/100;                                  
tc(tcb).computationSecs = tc(tcb).periodSecs/10 * 1 ;
tc(tcb).constraintSecs= tc(tcb).computationSecs;              
tc(tcb).preemptibleFlag=0;
tc(tcb).blockingIntervalSecs=tc(tcb).periodSecs - tc(tcb).computationSecs;
% specify for how many seconds we want to run out timing loop
tc(tcb).testDurationSeconds = 2*60+1;
%specify a factor by which to mulitply the memory allocation estimate, for
%saftey margin.
tc(tcb).maxDisplayExcessComputationDurations=100;
% specify values describing period glithes.  As part of the analysis after
% the timing test we search for regular glitches described by these
% parameters.
tc(tcb).interGlitchIntevalSecs = 30;
tc(tcb).interGlitchIntervalJitter = 0.1;        % tolerance of interGlitchIntevalSecs value.

% Test condition 4 shows ?
%
% designate an index for this block of test parameters. 
tcb= 4;
%set flags which specify various test conditions
tc(tcb).enableTCPriority=1;
%specify the 'time constraint' parameters
tc(tcb).periodSecs= 1/100;                                  
tc(tcb).computationSecs = tc(tcb).periodSecs/10 * 3 ;
tc(tcb).constraintSecs= tc(tcb).computationSecs;              
tc(tcb).preemptibleFlag=0;
tc(tcb).blockingIntervalSecs=tc(tcb).periodSecs - tc(tcb).computationSecs;
% specify for how many seconds we want to run out timing loop
tc(tcb).testDurationSeconds = 2*60+1;
%specify a factor by which to mulitply the memory allocation estimate, for
%saftey margin.
tc(tcb).maxDisplayExcessComputationDurations=100;
% specify values describing period glithes.  As part of the analysis after
% the timing test we search for regular glitches described by these
% parameters.
tc(tcb).interGlitchIntevalSecs = 30;
tc(tcb).interGlitchIntervalJitter = 0.1;        % tolerance of interGlitchIntevalSecs value.


% Test condition 5 is like 1 except for without TC priority. 
%
%
% designate an index for this block of test parameters. 
tcb= 5;
%set flags which specify various test conditions
tc(tcb).enableTCPriority=0;
tc(tcb).useGetSecsMex=1;        % GetSecsMex is a bare-bones mex file. The alternative, GetSecs, compiles in an extra abstraction layer.  
%specify the 'time constraint' parameters
tc(tcb).periodSecs= 1/1000;                                  
tc(tcb).computationSecs = tc(tcb).periodSecs/10 * 5 ;
tc(tcb).constraintSecs= tc(tcb).computationSecs;              
tc(tcb).preemptibleFlag=0;
tc(tcb).blockingIntervalSecs=tc(tcb).periodSecs - tc(tcb).computationSecs;
% specify for how many seconds we want to run out timing loop
tc(tcb).testDurationSeconds = 2*60+1;
%specify a factor by which to mulitply the memory allocation estimate, for
%saftey margin.
tc(tcb).maxDisplayExcessComputationDurations=100;
% specify values describing period glithes.  As part of the analysis after
% the timing test we search for regular glitches described by these
% paramters.
tc(tcb).interGlitchIntevalSecs = 30;
tc(tcb).interGlitchIntervalJitter = 0.1;        % tolerance of interGlitchIntevalSecs value.

clear tcb;      % make sure that we do not use tcb below.  It sneaks in when we cut and past from conditions above and forget to sub in tci.
% --------------------------------------------------------------------------------------------------------------------------------------------------
% 

%convert units of seconds to ticks.  
ticksPerSecond= MachAbsoluteTimeClockFrequency; 
periodTicks = round(tc(tci).periodSecs * ticksPerSecond);
computationTicks = round(tc(tci).computationSecs * ticksPerSecond);
constraintTicks = round(tc(tci).computationSecs * ticksPerSecond);

%     POSITION	DESCRIPTION			    UNITS		SIZE
%     1	        policy period			ticks		1x1
%     2	        policy computation		ticks		1x1
%     3	        policy constraint		ticks		1x1
%     4	        policy preemptible		flag		1x1
%     5	        blocking period 		seconds		1x1
%     6	        test  duration			seconds		1x1	
    

%launch the timing loop within the mex file
fprintf(['Running timing loop within mex file.  This will take about ' num2str(tc(tci).testDurationSeconds) ' seconds.\n']);
tVec=TestPriorityMex(periodTicks, computationTicks, constraintTicks, tc(tci).preemptibleFlag, tc(tci).blockingIntervalSecs, tc(tci).testDurationSeconds);

% plot stuff and calculate stuff
tVec=tVec(1:end-1); 
tDiffVec=diff(tVec);
tDiffVecSampleTimes=tVec(1:end-1)-tVec(1);
plot(tDiffVecSampleTimes, tDiffVec, 'b');
xlabel('loop start time (seconds)');
ylabel('loop pass time (seconds)');

if fixedAxisY
    axis([-5, max(tDiffVecSampleTimes)+5, 0, fixedAxisValueY]);
else
    axis([-5, max(tDiffVecSampleTimes)+5, 0, 1.25 *max(max(tDiffVec), tc(tci).periodSecs)]);
end
minLoopDuration=min(tDiffVec);
maxLoopDuration=max(tDiffVec);
medianLoopDuration=median(tDiffVec);
fprintf('The shortest loop was: %f seconds\n', minLoopDuration);
fprintf('The longest loop was: %f seconds\n', maxLoopDuration);
fprintf('The median loop was: %f seconds\n', medianLoopDuration);

%plot horizontal lines across the graph marking "time constraint"
%parameters "period" and the blocking interval.
hold on;
periodSecsLineX=[0 tc(tci).testDurationSeconds];
periodSecsLineY=[tc(tci).periodSecs tc(tci).periodSecs ];
plot(periodSecsLineX, periodSecsLineY, 'g');
blockingSecsLineX=periodSecsLineX;
blockingSecsLineY=[tc(tci).blockingIntervalSecs tc(tci).blockingIntervalSecs ];
plot(blockingSecsLineX, blockingSecsLineY, 'r');

% Find the n longest glitches.  n is set with the assumption that
% the total number of glitches is predicted by their falling
% at 30-second intervals.  If that assumption is wrong then 
% we will not find all of the long glitches
numExpectedGlitches=floor(tc(tci).testDurationSeconds/tc(tci).interGlitchIntevalSecs);
[sortedtDiffVec, sortedtDiffVecIndices]=sort(tDiffVec);
sortedtDiffVecRev=fliplr(sortedtDiffVec);
sortedtDiffVecIndicesRev=fliplr(sortedtDiffVecIndices);

nLongestGlitches= sortedtDiffVecRev(1:numExpectedGlitches);
nLongestGlitchesIndices=sortedtDiffVecIndicesRev(1:numExpectedGlitches);
nLongestGlitchesTimestamps=tDiffVecSampleTimes(nLongestGlitchesIndices);
[nLongestGlitchesTimestampsSequenced, nLongestGlitchesTimestampsSequencedIndices]=sort(nLongestGlitchesTimestamps);
interGlitchIntervals=diff(nLongestGlitchesTimestampsSequenced);
% for i=1:numExpectedGlitches
%     plot(nLongestGlitchesTimestamps(i), nLongestGlitches(i), 'rx');
% end
hold off;

fprintf([int2str(numExpectedGlitches) ' glitches predicted in ' num2str(tc(tci).testDurationSeconds) ' second test interval, assuming 30-second interval between glitches\n']);
fprintf(['The ' int2str(numExpectedGlitches) ' longest loop delays occured at times and intervals:\n']);
for i=1:numExpectedGlitches
    fprintf([ '\t' num2str(nLongestGlitchesTimestampsSequenced(i)) ' s\n']);
    if  i<numExpectedGlitches
        fprintf([ '\t\tdelta=' num2str(interGlitchIntervals(i)) ' s\n']);
    end
end

%identify those glitches among the longest 4 which fall at 30-second
%intervals.  First find the intervals of 30-second duration, then find
%the samples which mark the end points of those intervals.
synchedIntervalDoubleIndices=find( (interGlitchIntervals < (tc(tci).interGlitchIntevalSecs + tc(tci).interGlitchIntervalJitter)) & (interGlitchIntervals > (tc(tci).interGlitchIntevalSecs - tc(tci).interGlitchIntervalJitter)));
if ~isempty(synchedIntervalDoubleIndices) 
    synchedIntervals=interGlitchIntervals(synchedIntervalDoubleIndices);
    intervalsStartGlitchesTimestamps=nLongestGlitchesTimestampsSequenced(1:end-1);
    intervalsEndGlitchesTimestamps=nLongestGlitchesTimestampsSequenced(2:end);
    synchedGlitchesTimestamps=unique([intervalsStartGlitchesTimestamps(synchedIntervalDoubleIndices) intervalsEndGlitchesTimestamps(synchedIntervalDoubleIndices)]);
else
    synchedGlitchesTimestamps=[];
end
 
    


% find the next longest glitch.  
nextLongestDelay=sortedtDiffVecRev(numExpectedGlitches+1); 
nextLongestDelayIndex=sortedtDiffVecIndicesRev(numExpectedGlitches+1);
nextLongestDelayTimestamp=tDiffVecSampleTimes(nextLongestDelayIndex);

fprintf(['The ' int2str(numExpectedGlitches+1) 'th longest loop delay, ']);
fprintf(['at time ' num2str(nextLongestDelayTimestamp) ' seconds, was ' num2str(nextLongestDelay) ' seconds, ']);
fprintf([num2str(nLongestGlitches(numExpectedGlitches)/nextLongestDelay) ' times smaller than the ' int2str(numExpectedGlitches) 'th longest delay\n']);

% Check to see that we always blocked for the specified period. 
underBlockIndices=find(tDiffVec < tc(tci).blockingIntervalSecs);
underBlocks=tDiffVec(underBlockIndices);
if ~isempty(underBlocks)
    fprintf('Detected loop durations shorter than blocking period.  The durations and times are:\n');
    for i=1:length(underBlocks);
        fprintf(['\t' num2str(underBlocks(i)) '\t' num2str(tDiffVecSampleTimes(underBlockIndices(i))) '\n']);
    end
end

% Check to see when we exceeded the specified "computation" CPU time
% allowance specified when assigning 'time constraint' priority. excessComputationSecsIndices=find(measuredComputationSecs > tc(tci).computationSecs);
measuredComputationSecs=tDiffVec-tc(tci).blockingIntervalSecs;
excessComputationSecsIndices=find(measuredComputationSecs > tc(tci).computationSecs);
excessComputationSecs=measuredComputationSecs(excessComputationSecsIndices);
excessComputationTotalSecs=tDiffVec(excessComputationSecsIndices);
exessComputationSecsTimes=tDiffVecSampleTimes(excessComputationSecsIndices);
numExcessComputationLoops=length(excessComputationSecs);
fprintf(['The timing loop computation time exceeded the allocated computation time on ' int2str(numExcessComputationLoops) ' loops.\n']);
if numExcessComputationLoops > 0   
	fprintf('The loop durations, computation times, and timestamps are listed below.\n');
    if numExcessComputationLoops > tc(tci).maxDisplayExcessComputationDurations 
        fprintf(['(only the first ' int2str(tc(tci).maxDisplayExcessComputationDurations) ' are dislayed)\n']);
    end
	for i = 1:min([numExcessComputationLoops, tc(tci).maxDisplayExcessComputationDurations])
        fprintf('\t%1.5f\t%1.5f\t%2.5f\n', excessComputationTotalSecs(i),  excessComputationSecs(i), exessComputationSecsTimes(i) );
	end
end

% Check for compliance with:
% (loopPassDuration - blockingInterval)/loopPassDuration < computationSeconds/periodSeconds)
% For more info see notes at top of the file about this.
% For the tests which use flip instead of BlockSecs we do not actually
% know the blocking interval. This section should be modified accordingly. 
computationRatioLimit = tc(tci).computationSecs / tc(tci).periodSecs;
computationRatios = (tDiffVec - tc(tci).blockingIntervalSecs) / medianLoopDuration;  
computationRatioViolationIndices=find(computationRatios > computationRatioLimit); 
computationRatioViolationDurations=tDiffVec(computationRatioViolationIndices);
computationRatioViolationTimestamps=tDiffVecSampleTimes(computationRatioViolationIndices);
numComputationRatioViolations=length(computationRatioViolationIndices);

% Check to see if we exceed the "computation" CPU time allowance before the
% first glitch.  One possible explanation for the glitches is that the 
% Mach Kernel revokes realtime status at 30-second intervals as penalty 
% for the main MATLAB thread exceeding "computation" allowance.  However,
% if the thread has not exceeded that allowance before the first
% synchronized glitch, this strongly suggests that in fact the source
% of the glitch is MATLAB.
if ~isempty(synchedGlitchesTimestamps)
    earlyComputationViolations=exessComputationSecsTimes(exessComputationSecsTimes < min(synchedGlitchesTimestamps));
    if isempty(earlyComputationViolations)
        fprintf('The MATLAB thread did not exceed the CPU computation allowance before the first synchronized glitch.\n');
    else
        fprintf('The MATLAB thread exceeded the CPU computation allowance before the first synchronized glitch.\n');
        fprintf(['The violations occured at times: ' num2str(earlyComputationViolations) '\n']);
    end
end


