function TestTextBounds
% TestTextBounds
%
% Test Screen('TestBounds')
%
% OS9: TextBounds will accept an on- or offscreen window pointer.
% OSX: TextBounds accepts only an onscreen window.
% OSX: Screen 'TextBounds' provides width and height, not position.

% 2/3/05    dgp Wrote it.
% 10/26/05  awi Cosmetic 

try;
    red=[255 0 0];
    green=[0 255 0];
    black=0;
    textSize=64;
    string='Wordy';
    if IsOSX
        w=Screen('OpenWindow',0); % Doesn't do FillRect
        Screen('FillRect',w);
        Screen('Flip',w);
	end
    if IsOS9
		%         w=Screen(-1,'OpenOffscreenWindow',[],[0 0 3*textSize*length(string) 2*textSize],1);
        w=Screen(0,'OpenWindow');
	end
    Screen(w,'TextFont','Chicago');
    Screen(w,'TextSize',textSize);
    bounds=TextBounds(w,string);
    fprintf('TextBounds says the bounds of ''%s'' are [%d %d %d %d].\n',string,bounds);
    x=100;
    y=200;
    bounds=OffsetRect(bounds,x,y);
    if IsOS9
		Screen(w,'FillRect');
		Screen(w,'FillRect',red,bounds);
        Screen(w,'DrawText',string,x,y);
	end
    if IsOSX
		Screen('FillRect',w);
		Screen('FillRect',w,red,bounds);
        Screen('DrawText',w,string,x,y,black);
		Screen(w,'DrawText','Hit any key to continue',x,y+400,black);
	end
    if IsOSX
        y=y+200;
        bounds=Screen('TextBounds',w,string);
        fprintf('Screen ''TextBounds'' says the bounds of ''%s'' are [%d %d %d %d].\n',string,bounds);
        bounds=OffsetRect(bounds,x,y);
        Screen('FillRect',w,red,bounds);
        Screen('DrawText',w,string,x,y,black);
        Screen('DrawText',w,'Hit any key to continue',x,y+200,black);
        Screen('Flip',w);
	end
    KbWait;
    while KbCheck
		;
	end
	Screen(w,'Close');
catch;
    clear screen
    if exist('lasterr')
		lasterr
	end
end
