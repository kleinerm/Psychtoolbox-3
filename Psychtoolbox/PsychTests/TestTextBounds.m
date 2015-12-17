% TestTextBounds - Test function of the TextBounds() function.

% History:
% 16-Dec-2015  dgp  Written.

x=50;
y=98;
textSize = 48;
string = 'Click to quit';

w=Screen('OpenWindow',0,255);
woff=Screen('OpenOffscreenWindow',w,[],[0 0 2*textSize*length(string) 2*textSize]);
Screen(woff,'TextFont','Arial');
Screen(woff,'TextSize',textSize);
for yPositionIsBaseline=0:1
    textSize=48;
    if yPositionIsBaseline
        string='Origin at baseline. ';
    else
        string='Origin at upper left.';
    end
    t=GetSecs;
    bounds=TextBounds(woff,string,yPositionIsBaseline);
    fprintf('TextBounds took %.3f seconds.\n',GetSecs-t);
    Screen(w,'TextFont','Arial');
    Screen(w,'TextSize',textSize);
    [newX,newY]=Screen('DrawText',w,string,x,y,0,255,yPositionIsBaseline);
    Screen('FrameRect',w,0,InsetRect(OffsetRect(bounds,x,y),-1,-1));
    x=newX;
    y=newY;
end
Screen('Close',woff);
DrawFormattedText(w, 'Click to quit', 'center', 'center');
Screen('Flip',w);
GetClicks;
Screen('Close',w);
