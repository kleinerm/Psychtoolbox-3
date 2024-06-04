% TextInOffscreenWindowTest
%
% Compare text drawn into an offscreen window to text drawn into an
% onscreen window.
%

% HISTORY
%
% mm/dd/yy
%
%  3/18/04  awi     Wrote it.
%  6/04/24   mk     Fixups.

s=max(Screen('Screens'));
[w, wRect]=PsychImaging('OpenWindow', s);
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
wImage=double(Screen('GetImage', w, [], 'drawBuffer'));
Screen('Flip', w);

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
if maxDiff ~= 0  
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
