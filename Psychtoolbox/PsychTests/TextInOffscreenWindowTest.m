% TextInOffscreenWindowTest
%
% Compare text drawn into an offscreen window to text drawn into an
% onscreen window.
%
% Screen currently fails this test; Screen('DrawText') draws slightly
% differently into onscreen and offscreen windows. Visual examination of
% the difference between onscreen and offscreen text shows that the
% onscreen texts is larger by about two pixels.
%
% Until this is resolved, we recommend not mixing onscreen and offscreen
% text rendering if you require exact matching between text.
% 
% We don't yet know why this fails. The problem seems to have to do with
% differences between how textures are rendered into onscreen and offscreen
% windows.  That could be either because of a bug in Screen, or because the
% OpenGL software renderer used for offscreen windows rasterizes textures
% slightly differently than the  hardware renderer used for onscreen
% windows. We don't yet have a way to compare textures in onscreen and
% offscreen windows indepently of drawing text because 'DrawTexture' still
% uses OpenGL extensions not provided in offscreen contexts.
%

% HISTORY
%
% mm/dd/yy
%
%  3/18/04  awi     Wrote it.      


s=max(Screen('Screens'));
[w, wRect]=Screen('OpenWindow', s, [], [], 32, 2);
[osw, oswRect]=Screen('OpenOffscreenWindow', w);


Screen('FillRect', osw, 0);
Screen('TextFont',osw, 'Courier');
Screen('TextSize', osw, 50);
Screen('TextStyle', osw, 0);
Screen('DrawText', osw, 'This is text.', 100, 100, [255, 255, 255]);
oswImage=double(Screen('GetImage', osw));


Screen('FillRect', w, 0);
Screen('TextFont',w, 'Courier');
Screen('TextSize', w, 50);
Screen('TextStyle', w, 0);
Screen('DrawText', w, 'This is text.', 100, 100, [255, 255, 255]);
Screen('Flip', w);
wImage=double(Screen('GetImage', w));

Screen('PutImage', w, abs(oswImage - wImage));
%Screen('PutImage', w, oswImage);
Screen('Flip', w);

% % wait for a mouse press
% fprintf('Click the mouse to proceed\n');
% [x,y,buttons] = GetMouse();
% while ~any(buttons)
%     [x,y,buttons] = GetMouse();
% end
% fprintf('\n');

Screen('CloseAll');

doesMatch=isempty(find(oswImage ~= wImage));
di=oswImage - wImage;
maxDiff=abs(max(max(max(di))));
if maxDiff ~= 0;  
    ampFactor=floor(255/maxDiff);
else
    ampFactor=1;
end

fprintf('TextInOffscreenWindowTest results:\n\n');
if doesMatch
    fprintf('     Passed the test; the contents of onscreen and offscreen windows match\n');
else
    fprintf('     Failed the test; the contents of onscreen and offscreen windows do not match.\n');
    fprintf('     Displaying the difference between onscreen and offscreen text using MATLAB''s\n');
    fprintf('     "IMSHOW" command.  To make small errors visible, the difference image has been \n');
    fprintf(['     multiplied by ' num2str(ampFactor) '.\n']);
    fprintf('\n');
    fprintf('     For further explanation and advice, see help for "TextInOffscreenWindowTest"\n');  
    imshow(di* ampFactor);
end



   

    

    

