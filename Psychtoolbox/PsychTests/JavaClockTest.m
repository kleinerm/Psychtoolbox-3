function timeDiffs=JavaClockTest(testDurationMinutes)
% JavaClockTest
%
% Test for divergence between the java.lang.System.currentTimeMillis() and
% GetSecs clocks.  
% 
% see also: CalibrateJavaClock, JavaTimeToGetSecs, GetChar

% HISTORY
%
% 6/7/06   awi  Wrote it.
% 6/15/06  awi  Modified for new JavaTimeToGetSecs which does not cache
%                clock difference. 

if IsOctave
    fprintf('This test is pointless on Octave. Bye.\n');
    timeDiffs = 0;
    return;
end

if nargin<1
    testDurationMinutes=3;
end

testIntervalMinutes = 1;
samplesPerTest= 100;


fprintf(['TestJavaClock will sample clocks at ' num2str(testIntervalMinutes) '-minute intervals for ' num2str(testDurationMinutes) ' minutes.\n' ]);
validKeyFlag=0;
while ~validKeyFlag
    c = input('Proceed with test? [y/n]: ', 's');
    proceedFlag=strcmp(upper(c), 'Y');
    exitFlag=strcmp(upper(c), 'N');
    validKeyFlag= exitFlag || proceedFlag;
    if ~validKeyFlag
        fprintf('Unrecognized response.\n');
    end
end

if exitFlag
    fprintf('Exiting TestJavaClock function without running test.\n');
    return;
end

GetSecs;
java.lang.System.currentTimeMillis();

testTimesMinutes=linspace(0, testDurationMinutes, testDurationMinutes/testIntervalMinutes + 1);
testTimesSecsAbsolute= GetSecs + testTimesMinutes * 60 + 1;  %wait five seconds before first test.  
numTests=length(testTimesMinutes);
i=1;
t1Secs=zeros(1,samplesPerTest);
t2Secs=zeros(1,samplesPerTest);
while i<= numTests
    while GetSecs < testTimesSecsAbsolute(i);
        WaitSecs(testTimesSecsAbsolute(i) - GetSecs);
    end
    fprintf(['Starting time sample loop ' int2str(i) ' of ' int2str(numTests) ' at ' datestr(now) '\n']);
    oldPriority=Priority;
    try 
        Priority(9);
        for s=1:samplesPerTest
            t1Secs(s)=GetSecs;
            t2Secs(s)=java.lang.System.currentTimeMillis() / 1000.0;
        end
        Priority(oldPriority);
    catch
        Priority(oldPriority);
    end
    meanDiffs(i)=mean(abs(t2Secs-t1Secs));
    i=i+1;
end
timeDiffs=meanDiffs-meanDiffs(1);
fprintf('Plotting difference between clocks...');
plot(testTimesMinutes, timeDiffs, 'rx-');
xlabel('Time in Minutes');
ylabel('Java clock - GetSecs');
fprintf(' done.\n');
