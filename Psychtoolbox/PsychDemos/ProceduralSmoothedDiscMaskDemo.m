function ProceduralSmoothedDiscMaskDemo(ndiscs)
% ProceduralSmoothedDiscMaskDemo([ndiscs=700]) -- An aquarium full of discs
% within a centered mask.
%
% This demo shows how to use the Screen('DrawTextures') command to draw a
% large number of similar images quickly - in this case, smooth edged
% discs masked by another smoothed disc.

% History:
% 25/04/2018 created (Junxiang Luo)

% Setup defaults and unit color range:
PsychDefaultSetup(2);

% Disable synctests for this quick demo:
oldSyncLevel = Screen('Preference', 'SkipSyncTests', 2);

% Set number of discs to 700 if no number provided:
if nargin < 1 || isempty(ndiscs)
    ndiscs = 700;
end

randSizeFlag = 1;
maskFlag = 1;

% open our screen
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'FloatingPoint32BitIfPossible');
screenid = max(Screen('Screens'));
[win, winRect] = PsychImaging('OpenWindow', screenid, 0);
[width, height] = RectSize(winRect);
Screen('BlendFunction', win, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

% dot parameters
dotRadius = 5;
dotSize = 2*dotRadius+1;
dotSigma = 5;
% use alpha channel for smoothing edge of disc?
useAlpha = true;
% smoothing method: cosine (0), smoothstep (1), inverse smoothstep (2)
dotMethod = 1;

%mask parameters
maskRadius = height/2;
maskSigma = maskRadius;
% smoothing method: cosine (0), smoothstep (1), inverse smoothstep (2)
maskMethod = 2;

% create our procedural textures
[dotstex, dotsrect] = CreateProceduralSmoothedDisc(win,...
    dotSize, dotSize, [], dotRadius, dotSigma, useAlpha, dotMethod);

[masktex, maskrect] = CreateProceduralSmoothedDisc(win,...
    width, height, [], maskRadius, maskSigma, useAlpha, maskMethod);

% create our positions on screen
inrect = repmat(dotsrect', 1, ndiscs);
dotDstRects = zeros(4, ndiscs);
scale = zeros(1,ndiscs);
for i=1:ndiscs
    if randSizeFlag
        scale(i) = 1*(0.1 + 0.9 * randn);
    else
        scale(i) = 1;
    end
    dotDstRects(:, i) = CenterRectOnPointd(dotsrect * scale(i), rand * width, rand * height)';
end
maskDstRects = CenterRectOnPointd(maskrect, width/2, height/2)';

% the rotation angles for each disc
rotAngles = rand(1, ndiscs) * 360;

% create random alphas for yellow discs
colours = repmat([1 1 0 1]',1,ndiscs);
colours(4,:) = rand(ndiscs,1)';
myAlphas = colours(4,:);

% Initially sync us to VBL at start of animation loop.
count = 0;
vbl = Screen('Flip', win);
tstart = vbl;

while ~KbCheck
    % Screen('DrawTextures', windowPointer, texturePointer(s) [, sourceRect(s)]
    %[, destinationRect(s)] [, rotationAngle(s)] [, filterMode(s)] [, globalAlpha(s)]
    %[, modulateColor(s)] [, textureShader] [, specialFlags] [, auxParameters]);
    Screen('DrawTextures', win, dotstex, [], dotDstRects, rotAngles, [], [], colours, [], []);

    if maskFlag
        Screen('DrawTextures', win, masktex, [], maskDstRects, [], [], 1, [0, 0, 0, 1]', [], []);
    end

    Screen('DrawingFinished', win);

    rotAngles = rotAngles + 5 * randn(1, ndiscs);
    colours(4,:) = (1+sin(myAlphas*100-200 + count*0.05))/2;

    [x, y] = RectCenterd(dotDstRects);
    x = mod(x + 0.5 * cos(rotAngles/360*2*pi), width);
    y = mod(y - 0.5 * sin(rotAngles/360*2*pi), height);

    dotDstRects = CenterRectOnPointd(inrect .* repmat(scale,4,1), x, y);

    Screen('Flip', win);

    count = count + 1;
end

Screen('Flip', win);

sca;

% Restore old settings for sync-tests:
Screen('Preference', 'SkipSyncTests', oldSyncLevel);
