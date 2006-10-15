function TextureChannelsTest
% TextureChannelsTest
%
% Test proper assignment of matrix layers to RGBA texture channels.
%
% What you should see during the test:
%
% 1. A red square on a black background. Keypress!
% 2. A green square on a black background. Keypress!
% 3. A blue square on a black background. Keypress!
% 4. A black screen. Keypress!
% End.
%
% If you see different colors in different order, then
% something in the RGBA path of Screen('MakeTexture') is
% broken, e.g., due to some machine endian issue.
%

% 29.05.2006 Written (MK).

win = Screen('OpenWindow', 0, 0);
for nr=1:4
    img = zeros(256, 256, 4);
    img(:,:,nr)=ones(256,256)*255;
    tex = Screen('MakeTexture', win, img);
    Screen('DrawTexture', win, tex);
    Screen('Flip',win);
    KbWait;
    while KbCheck; end;
end;
Screen('CloseAll');
return;
