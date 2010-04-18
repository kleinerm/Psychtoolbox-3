function SadowskiDemo(imgName)
% SadowskiDemo([imgName]);
%
% Demonstrate Sadowski illusion on a specified image file 'imgName'. if
% 'imgName' is omitted, a default image is used.
%
% The demo will first show the color adaptation image. Please fixate the
% fixation cross in the center of the screen. Then press a key. The demo
% will show the grayscale only test image. Be amazed! Press any key to
% repeat the adaptation phase, press ESCape to quit the demo. If you press
% the left cursor key, a white test patch will be show instead of the
% luminance image.
%

% History:
% 17.03.2010  mk  Written. Inspired by a demo shown by Stuart Anstis.

% Check installation:
AssertOpenGL;

KbName('UnifyKeyNames')
escape = KbName('ESCAPE');
left = KbName('LeftArrow');

% Assign our cute rabbits if image filename omitted:
if nargin < 1
    imgName = [];
end

if isempty(imgName)
    imgName = [PsychtoolboxRoot 'PsychDemos/konijntjes1024x768.jpg'];
end

% Read image from filesystem:
img = imread(imgName);

% Convert RGB image to HSV color space for simpler manipulation:
hsv = rgb2hsv(img);

% Extract luminance channel for pure luma image:
luma = hsv(:,:,3) * 255;

% Open window with black background on secondary display:
screenid = max(Screen('Screens'));
[win, winrect] = Screen('OpenWindow', screenid, 0);
Screen('TextSize', win, 24);

HideCursor;

% Make luminance only texture:
lumatex = Screen('MakeTexture', win, luma);

% Compute destination rectangle for a 0.5 scaling:
rect = Screen('Rect', lumatex);
s1 = RectWidth(rect) / RectWidth(winrect);
s2 = RectHeight(rect) / RectHeight(winrect);
sf = min(1/s1, 1/s2) * 0.5;
rect = ScaleRect(rect, sf, sf);
dstRect = CenterRect(rect, winrect);

% "Rotate" hue values by 0.5 or 180 degrees for "anti-colors":
hsv(:,:,1) = mod(hsv(:,:,1) + 0.5, 1);

% MK: Modifying saturation not very effective. Leave untouched. hsv(:,:,2) = 1.0;

% Fix luminance to 50% peak - No luminance info in anti-color image:
hsv(:,:,3) = 0.5;

% Convert back from HSV to RGB space:
anticolors = hsv2rgb(hsv) * 255;

% Make a texture out of it:
antitex = Screen('MakeTexture', win, anticolors);

while 1
    % Display anti-color image, with a superimposed fixation cross:
    Screen('DrawTexture', win, antitex, [], dstRect);
    DrawFormattedText(win, '+', 'center', 'center', 0);
    Screen('Flip', win);

    % Wait for keystroke:
    [secs, keycode] = KbStrokeWait;
    if keycode(left)
        % Left arrow: Draw neutral white:
        Screen('FillRect', win, 255, dstRect);
    else
        % Other key: Draw luminance-only image:
        Screen('DrawTexture', win, lumatex, [], dstRect);
        DrawFormattedText(win, '+', 'center', 'center', 0);
    end
    Screen('Flip', win);

    % Wait for keystroke:
    [secs, keycode] = KbStrokeWait;
    % Escape exits:
    if keycode(escape)
        break;
    end
end

% Done. Close window and release all ressources:
ShowCursor;
Screen('CloseAll');

return;
