% TextInitBugTest
%
% Tests for a bug in DrawText where failing to initialize text paramters
% causes a SCREEN crash


s=max(Screen('Screens'));
w=Screen('OpenWindow', s, [], [], [], 2);
Screen('DrawText', w, 'Hello!');
Screen('Flip', w);
WaitSecs(5);
Screen('CloseAll');
