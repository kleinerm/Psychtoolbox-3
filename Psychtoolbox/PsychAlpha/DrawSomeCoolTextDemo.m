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
    
    Screen('Preference', 'TextRenderer', 1);
    Screen('Preference', 'TextAntiAliasing', 1);
    Screen('Preference', 'TextAlphaBlending', 0);
    w=Screen('OpenWindow', screenNumber, 128);
    Screen('Blendfunction', w, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Screen('FillOval', w, [255 255 0], [0 0 400 400]);
    %    Screen('FillRect', w);
    if IsLinux==0
        Screen('TextFont',w, 'Courier New');
        Screen('TextSize',w, 48);
        Screen('TextStyle', w, 0);
    end;
    
    fid = fopen([PsychtoolboxRoot 'PsychDemos/japanese_shiftJIS.txt'], 'r', 'n','Shift_JIS');
    japanesetext = native2unicode(fread(fid),'Shift_JIS');
    fclose(fid);
    disp(japanesetext);
    
    
    
    y=200;
    [x1, y1] = Screen('DrawText', w, double('Hello'), 300, y, [255, 0, 255, 255], [], 1)
    [x2, y2] = Screen('DrawText', w, double('World!'), [], [], [], [], 1)
    Screen('TextFont', w, 29);
    Screen('DrawText', w, double(japanesetext'), 20, y+100, [0 0 0 255], [], 1);
    
%    Screen('DrawText', w, double('Hello World!xyq')+1000, 100, y, [255, 0, 255], [], 0);
    Screen('FrameRect', w, 255, [100 y 600 y+48]);
    Screen('DrawLine', w, [0 255 0], 100, y, 1400, y);
    if IsLinux==0
        Screen('TextFont',w, 'Times');
        Screen('TextSize',w, 86);
    end;
    Screen('DrawText', w, 'Hit any key to exit.', 100, 300, [255, 0, 0, 255]);
    vbl=Screen('Flip',w);
    KbWait;
    while KbCheck; end;
    
    vbl=Screen('Flip',w);
    tstart=vbl;
    count=-100;
    while ~KbCheck
       Screen('DrawLine', w, [0 255 0], 100, count, 1400, count);
       [normRect realRect] = Screen('TextBounds', w, 'Hit any key to exit.', 100, count, 1);
       Screen('DrawText', w, 'Hit any key to exit.', 100, count, [255, 0, 0, 255], [], 1);
       Screen('FrameRect', w, [255 0 255], realRect);

       count=count+1;
       Screen('Flip',w, 0, 0, 2);
    end
%       vbl=Screen('Flip',w);
    count
    tavg = (vbl-tstart)/count * 1000
    
    woff = Screen('OpenOffscreenWindow', w, [255 255 0], [0 0 299 200]);
        Screen('TextFont',woff, 'Times');
        Screen('TextSize',woff, 286);
        Screen('TextStyle', woff, 0);
    Screen('DrawText', woff, 'Hit any key to exit.', 0, 200, [255, 0, 0, 255], [], 1);
	 Screen('DrawTexture', w, woff);    
%    vbl=Screen('Flip',w);
    WaitSecs(1);
    KbWait;
    
    Screen('CloseAll');
catch
    % This "catch" section executes in case of an error in the "try" section
    % above.  Importantly, it closes the onscreen window if it's open.
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end
