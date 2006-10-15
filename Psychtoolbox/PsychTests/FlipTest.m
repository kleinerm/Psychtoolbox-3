% FlipTest
%
% FlipTest times and plots the intervals between flips of the front and
% back display buffers in a tight loop.  Flips should occur at the video
% frame period.  Result of lipTest are a test of Priority's ability to
% gurantee time to MATLAB for real-time displays.  
%
% On OS X, the Screen subfunction 'Flip' replaces 'WaitBlanking' as a means 
% of sycnrhonizing updates of video memory with the vertical retrace of your
% monitor.  This prevents vertical shearing of displays.
%
% Onscreen windows in OS X are double buffered, having two surfaces:
% front and back.  The front surface is memory from which video DACs read
% and is displayed on your montitor.  The back surface is video memory to 
% which rendering commands draw.
%
% A call to Screen 'Flip' delays until the next vertical retrace, then
% instantaneously interchanges front and back video surfaces, and then
% returns.    

% HISTORY
% 
% mm/dd/yy
%
% 6/28/04   awi     Wrote it.
% 1/11/05   awi     Merged in bug fixes given by David Jones.
%                   Expanded description.
%                   Give feedback to user in command window.
%                   Add try ...                                   

try
    screenNumber=max(Screen('Screens'));
    numFlips = 100;
    fprintf('Raising priority for test...\n');
    Priority(1);
    fprintf('Priority raised.\n')
    fprintf('Opening window and flipping frames. This should take about ');
    fprintf('%d seconds...\n', ceil(1/Screen('FrameRate',screenNumber) * numFlips));
    w=Screen('OpenWindow', screenNumber, [], [], [], 2);
    t=[1:numFlips];
    GetSecs;
    Screen('Flip',w);
    for i=1:numFlips
        Screen('Flip',w);
        t(i)=GetSecs;
    end
    Priority(0);
    fprintf('Priority restored.\n');
    fprintf('Test Results:.\n');
 
    Screen('CloseAll');
    td=diff(t);
    plot(td);
    fprintf('   Median frame period: %f seconds\n', median(td));
    fprintf('   Max frame period: %f seconds\n', max(td));
    fprintf('   Min frame period: %f seconds\n', min(td));
    fprintf('   Median frame frequency: %f seconds\n', 1/median(td));
    plot(td);
catch
    Priority(0);
    Screen('CloseAll');
    lasterr
end
