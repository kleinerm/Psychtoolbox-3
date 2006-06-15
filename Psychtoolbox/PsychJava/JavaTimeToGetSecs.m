function timeSecs=JavaTimeToGetSecs(javaTime)

% timeSecs=JavaTimeToGetSecs(javaTime)
%
% Convert time as reported by "java.lang.System.currentTimeMillis()" to
% GetSecs units.  
%
% The Java timebase and JavaTimeToGetSecs are used internally by GetChar
% because Java calls underlying GetChar timestamp keyboard events in those
% units.
%
%
% see also:

% HISTORY
%
% 6/7/06    awi Wrote it.
% 6/13/06   awi Do not cache difference between Java and GetSecs clocks;
%               their clocks run at different rates. 
% 6/14/06   awi Commented out Priority calls surrounding the loop which 
%               samples GetSecs and java.lang.System.currentTimeMillis()
%               for purpose of finding their difference.  Those calls would
%               make JavaTimeToGetSecs impractically slow for use by
%               GetSecs, JavaTimeToGetSecs' raison d'tre.  This is not 
%               really so bad; if you want better timing, raise Priority 
%               before calling GetChar.  


persistent JAVA_CURRENTTIMEMILIS_GETSECS_CLOCKS_DIFF;

updateIntervalSecs=0;

updateLoops=10;

% Periodically re-estimate and cache the difference between the java.lang.System.currentTimeMillis() and GetSecs clocks.       
if isempty(JAVA_CURRENTTIMEMILIS_GETSECS_CLOCKS_DIFF) ||  GetSecs - JAVA_CURRENTTIMEMILIS_GETSECS_CLOCKS_DIFF.lastSampleTimeSecs  > updateIntervalSecs
    JAVA_CURRENTTIMEMILIS_GETSECS_CLOCKS_DIFF.lastSampleTimeSecs = GetSecs;
    timeASecs=zeros(1,updateLoops);
    timeBMillisecs=zeros(1,updateLoops);
    timeCSecs=zeros(1,updateLoops);
    for i = 1:updateLoops
        timeASecs(i)=GetSecs;
        timeBMilliSecs(i)=java.lang.System.currentTimeMillis();
        timeCSecs(i)=GetSecs;
    end
    timeBSecs= timeBMilliSecs/1000;
    passIntervals= timeCSecs-timeASecs;
    [shortPassTime, shortPassIndex]=min(passIntervals);
    getSecsAtJavaCall= (timeCSecs(shortPassIndex) - timeASecs(shortPassIndex)) / 2 + timeASecs(shortPassIndex);
    JAVA_CURRENTTIMEMILIS_GETSECS_CLOCKS_DIFF.diffSecs= timeBSecs(shortPassIndex) - getSecsAtJavaCall;

end
% using estimated difference between clocks, calculate GetSecs time for given Java currentTimeMillis() value.  
timeSecs= javaTime / 1000.0 - JAVA_CURRENTTIMEMILIS_GETSECS_CLOCKS_DIFF.diffSecs;
