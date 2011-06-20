function DrawingToTextureVsOffscreenWindowTest
% Draws some line into an offscreen window, displays it,
% then does the same drawing into a texture.
%
% Result should be the same, unless something is screwed wrt. texture
% format, in which case one of both drawings will be flipped upside-down or
% transposed etc.
%
% This should pass on all hardware. Contributed by some unknown user
% "empedia".

bgd = [127 127 127]; %background colour

% Open window
screencount=size(Screen('screens'),2);
if screencount>1
screenRect=Screen(1,'rect');
else
screenRect=Screen(0,'rect');
end

window = Screen(0, 'OpenWindow', bgd, screenRect, 32,2);
Screen('TextFont', window, 'Arial');
Screen('TextSize', window, 20);
Screen('DrawText', window, 'Please wait...', screenRect(3)/2-100,screenRect(4)/2, 0);
Screen('Flip', window);
Screen('Flip',window,GetSecs+2,1);

%Using off screen window
Screen('DrawText', window, 'OffScreen window ', screenRect(3)/2-100, screenRect(4)/2, 0);
Screen('Flip', window);
Screen('Flip',window,GetSecs+2,1);
offscrwin = Screen(window, 'OpenOffscreenWindow', bgd);
Rect=CenterRect([0 0 300 300],screenRect);
Screen('drawline',offscrwin,[255 0 0],Rect(1),Rect(2),Rect(3),Rect(4),1);

Screen('drawtexture',window, offscrwin);%, window);
Screen('Flip', window);
Screen('Flip',window,GetSecs+2,1);

%Using draw texture
Screen('DrawText', window, 'Draw Texture', screenRect(3)/2-100, screenRect(4)/2, 0);
Screen('Flip', window);
Screen('Flip',window,GetSecs+2,1);
bgd_texture=repmat(127,300,300);
bgd=Screen('MakeTexture',window,bgd_texture);
Screen('drawline',bgd,[255 0 0],0,0,300,300,1);
Screen('DrawTexture', window,bgd);
Screen('Flip', window);
Screen('Flip',window,GetSecs+2,1);

sca;
