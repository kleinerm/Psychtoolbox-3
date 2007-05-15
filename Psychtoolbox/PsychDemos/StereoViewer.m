function StereoViewer(leftimfile, rightimfile, stereoMode, imaging)
% StereoViewer(leftimfile, rightimfile [,stereoMode=8] [,imaging=0])
%
% Minimalistic viewer for stereo image pairs. Reads image for left-eye from
% file 'leftimfile', reads right-eye image from file 'rightimfile'.
% 'stereoMode' mode of presentation, defaults to mode 8 (Red-Blue
% Anaglyph). 'imaging' if set to 1, will use the Psychtoolbox imaging
% pipeline for stereo display -- allows to set gains for anaglyph stereo.
%
% The viewer just shows the image pair. The left image is centered on the
% screen, the right images position can be moved by moving the mouse cursor
% to align for inter-eye distance. Press any key to quit the viewer.

% History:
% 14.05.2007 Written (MK)

AssertOpenGL;

if nargin < 2
    error('You must at least provide names of image files for stereo pair.');
end

if nargin < 3
    stereoMode = [];
end

if isempty(stereoMode)
    stereoMode = 8;
end

if nargin < 4
    imaging = 0;
end

if imaging > 0
    imaging = kPsychNeedFastBackingStore;
end

screenid = max(Screen('Screens'));
[win, winRect] = Screen('OpenWindow', screenid, 0, [], [], [], stereoMode, [], imaging);

if imaging
    % Set color gains. This depends on the anaglyph mode selected:
    switch stereoMode
        case 6,
            SetAnaglyphStereoParameters('LeftGains', win,  [1.0 0.0 0.0]);
            SetAnaglyphStereoParameters('RightGains', win, [0.0 0.6 0.0]);
        case 7,
            SetAnaglyphStereoParameters('LeftGains', win,  [0.0 0.6 0.0]);
            SetAnaglyphStereoParameters('RightGains', win, [1.0 0.0 0.0]);
        case 8,
            SetAnaglyphStereoParameters('LeftGains', win, [0.4 0.0 0.0]);
            SetAnaglyphStereoParameters('RightGains', win, [0.0 0.2 0.7]);
        case 9,
            SetAnaglyphStereoParameters('LeftGains', win, [0.0 0.2 0.7]);
            SetAnaglyphStereoParameters('RightGains', win, [0.4 0.0 0.0]);
        otherwise
            %error('Unknown stereoMode specified.');
    end
end


imgl=Screen('MakeTexture', win, imread(leftimfile));
imgr=Screen('MakeTexture', win, imread(rightimfile));

imgrect = Screen('Rect', imgr);

% Position mouse on center of display:
[x , y] = RectCenter(winRect);
SetMouse(x, y, win);

% Hide mouse cursor:
HideCursor;

while ~KbCheck
    
    % Query mouse position:
    [x,yd] = GetMouse(win);
    
    Screen('SelectStereoDrawBuffer', win, 0);
    Screen('DrawTexture', win, imgl);

    Screen('SelectStereoDrawBuffer', win, 1);
    % Draw right image centered on mouse position -- mouse controls image
    % offsets:
    Screen('DrawTexture', win, imgr, [], CenterRectOnPoint(imgrect, x, y));

    Screen('Flip', win);
end

% Show mouse cursor:
ShowCursor;

Screen('CloseAll');
return;
