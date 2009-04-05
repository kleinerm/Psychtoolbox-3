%this shows you how to register a callback m-file that can display the tracker's eye image in PTB

w = Screen('OpenWindow', max(Screen('Screens')));
el=EyelinkInitDefaults(w);
PsychEyelinkDispatchCallback(el);
if Eyelink('Initialize', 'PsychEyelinkDispatchCallback') ~=0
	error('eyelink failed init')
end
result = Eyelink('StartSetup',1)

%now hit 'return' on the computer where you are running PTB
%this will be sent to the eyelink host computer to tell it to start sending PTB the eye image

% when you are done, run the following:
%Eyelink('Shutdown'); 
%sca
