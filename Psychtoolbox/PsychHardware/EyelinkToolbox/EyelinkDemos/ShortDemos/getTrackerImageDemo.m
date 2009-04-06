%this shows you how to register a callback m-file that can display the tracker's eye image in PTB
function getTrackerImageDemo
try
    ListenChar(2)
    w = Screen('OpenWindow', max(Screen('Screens')));
    el=EyelinkInitDefaults(w);
    %PsychEyelinkDispatchCallback(el);
    if Eyelink('Initialize') ~=0
        error('eyelink failed init')
    end
    %Eyelink('Verbosity',7);
    
    helpTxt='hit return (on either ptb comptuer or tacker host computer) to toggle camera image, esc to quit';
    fprintf('\n%s\n',helpTxt)
    
    result = Eyelink('StartSetup',1) %can also use Eyelink('DriftCorrStart',30,30,1,1,1), but you have to hit esc before return

catch ex
    getReport(ex)
    cleanup
end
cleanup
end

function cleanup
FlushEvents('mouseUp','mouseDown','keyDown','autoKey','update');
ListenChar(0);
Eyelink('Shutdown');
sca
end