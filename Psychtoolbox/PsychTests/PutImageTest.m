function PutImageTest
% PutImageTest
%
% Test Screen('PutImage') when used with 'NormalizedHighresColorRange'.
%
% Derived from PutImageBug.m from Denis Pelli
% This didn't work as expected in PTB releases before June 2017,
% so now confirms a correct bug fix.

PsychDefaultSetup(0);
deferLoading=2; 
PsychImaging('PrepareConfiguration'); 
PsychImaging('AddTask','General','NormalizedHighresColorRange',1);
[window, screenRect]=PsychImaging('OpenWindow', 0, 1);

Screen('PutImage',window,(0:255)/255,screenRect);
DrawFormattedText(window, 'A ramp, 0 to 1. Click to continue.', 'center', 'center', [1 0 0]);
Screen('Flip',window);
GetClicks;

Screen('PutImage',window,uint8(0:255),screenRect);
DrawFormattedText(window, 'A byte ramp 0 to 255. Click to quit.', 'center', 'center', [1 0 0]);
Screen('Flip',window);
GetClicks;
sca;
