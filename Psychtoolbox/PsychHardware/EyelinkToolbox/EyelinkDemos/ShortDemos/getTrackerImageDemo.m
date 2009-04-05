%this shows you how to register a callback m-file that can display the tracker's eye image in PTB
function getTrackerImageDemo
try
    ListenChar(2)
    w = Screen('OpenWindow', 1); %max(Screen('Screens'))
    el=EyelinkInitDefaults(w);
    PsychEyelinkDispatchCallback(el);
    if Eyelink('Initialize', 'PsychEyelinkDispatchCallback') ~=0
        error('eyelink failed init')
    end
    
    fprintf('\nnow hit return on the computer where you are running PTB\n')
    fprintf('this will be sent to the eyelink host computer to tell it to start sending PTB the eye image\n')
    fprintf('then hit esc to quit\n')

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