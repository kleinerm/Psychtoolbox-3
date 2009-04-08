function EyelinkGetTrackerImageDemo
% This shows you how to register a callback m-file that can display the tracker's eye image in PTB
try
    ListenChar(2)
    w = Screen('OpenWindow', max(Screen('Screens')), [255 255 0], [0 0 800 600]);
    el=EyelinkInitDefaults(w);
    PsychEyelinkDispatchCallback(el);
    if Eyelink('InitializeDummy', 'PsychEyelinkDispatchCallback') ~=0
        error('eyelink failed init')
    end
    
%    Eyelink('Verbosity',10);
    
    helpTxt='hit return (on either ptb comptuer or tacker host computer) to toggle camera image, esc to quit';
    fprintf('\n%s\n',helpTxt)
    
    Eyelink('TestSuite');
    %    result = Eyelink('DriftCorrStart',30,30,1,1,1)
    result = Eyelink('StartSetup',1) %can also use Eyelink('DriftCorrStart',30,30,1,1,1), but you have to hit esc before return

catch
    cleanup
end
cleanup
end

function cleanup
sca
ListenChar(0);
Eyelink('Shutdown');
end
