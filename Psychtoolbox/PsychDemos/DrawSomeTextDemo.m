% DrawSomeText
%
% ___________________________________________________________________
%
%  Trivial example of drawing text.  
% _________________________________________________________________________
%
% see also: PsychDemos

% 3/8/04    awi     Wrote it.
% 7/13/04   awi     Added comments section.  
% 9/8/04    awi     Added Try/Catch, cosmetic changes to documentation.
% 1/21/05   awi     Replaced call to GetChar with call to KbWait. 
% 10/6/05	awi		Note here cosmetic changes by dgp between 1/21/05 and 10/6/05	.

try
    % Choosing the display with the highest dislay number is
    % a best guess about where you want the stimulus displayed.
    screens=Screen('Screens');
    screenNumber=max(screens);
    w=Screen('OpenWindow', screenNumber);
    Screen('FillRect', w);
    Screen('TextFont',w, 'Courier New');
    Screen('TextSize',w, 50);
    Screen('TextStyle', w, 1+2);
    Screen('DrawText', w, 'Hello World!', 100, 100, [0, 0, 255, 255]);
    fprintf('Requested font: %s, got: %s\n', 'Courier New', Screen('TextFont', w));
    Screen('TextFont',w, 'Times');
    Screen('TextSize',w, 30);
    Screen('DrawText', w, 'Hit any key to exit.', 100, 300, [255, 0, 0, 255]);
    fprintf('Requested font: %s, got: %s\n', 'Times', Screen('TextFont', w));
    Screen('Flip',w);
    KbWait;
    Screen('CloseAll');
catch
    % This "catch" section executes in case of an error in the "try" section
    % above.  Importantly, it closes the onscreen window if it's open.
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end
