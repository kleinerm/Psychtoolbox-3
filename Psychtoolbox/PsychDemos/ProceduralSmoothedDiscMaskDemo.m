function ProceduralSmoothedDiscMaskDemo(ndiscs)
% ProceduralSmoothedDiscMaskDemo([ndiscs=700]) -- An aquarium full of discs!
%
% This demo shows how to use the Screen('DrawTextures') command to draw a
% large number of similar images quickly - in this case, smooth edged
% discs.

% History:
% 12/12/2017 modified from ProceduralGarboriumDemo (Ian Andolina)
% 24/04/2018 add per-disc alpha parameter to demo (Junxiang Luo & Ian Andolina)

% Setup defaults and unit color range:
PsychDefaultSetup(2);

% Disable synctests for this quick demo:
oldSyncLevel = Screen('Preference', 'SkipSyncTests', 2);

% Set number of gabor patches to 400 if no number provided:
if nargin < 1 || isempty(ndiscs)
    ndiscs = 700;
end

randSizeFlag = 1;
maskFlag = 1;

PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'FloatingPoint32BitIfPossible');

screenid = max(Screen('Screens'));
[win, winRect] = PsychImaging('OpenWindow', screenid, 0.5);
[w, h] = RectSize(winRect);

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
maskRadius = h/4;
maskSigma = maskRadius;
% smoothing method: cosine (0), smoothstep (1), inverse smoothstep (2)
maskMethod = 2;

[dotstex, dotsrect] = CreateProceduralSmoothedDisc(win,...
	dotSize, dotSize, [], dotRadius, dotSigma, useAlpha, dotMethod);

[masktex, maskrect] = CreateProceduralSmoothedDisc(win,...
	w, h, [], maskRadius, maskSigma, useAlpha, maskMethod);

mydots = repmat([0, 0, 0, 0]', 1, ndiscs);
mymask = [1, 0, 0, 0]';

inrect = repmat(dotsrect', 1, ndiscs);

dotDstRects = zeros(4, ndiscs);
scale = zeros(1,ndiscs);
for i=1:ndiscs
    if randSizeFlag
        scale(i) = 1*(0.1 + 0.9 * randn);
    else
        scale(i) = 1;
    end
    dotDstRects(:, i) = CenterRectOnPointd(dotsrect * scale(i), rand * w, rand * h)';
end

maskDstRects = CenterRectOnPointd(maskrect, w/2, h/2)';

count = 0;
randAlpha = rand(1, ndiscs);
mydots(1,:) = randAlpha;
rotAngles = rand(1, ndiscs) * 360;

ifi = Screen('GetFlipInterval', win);
vbl = Screen('Flip', win);

while ~KbCheck
    Screen('DrawTextures', win, dotstex, [], dotDstRects, rotAngles, [], 1, [1, 1, 0, 1]', [], [], mydots);
    
    if maskFlag
        Screen('DrawTextures', win, masktex, [], maskDstRects, [], [], 1, [0.5, 0.5, 0.5, 1]', [], [], mymask);
    end
   
    Screen('DrawingFinished', win);
   
    rotAngles = rotAngles + 5 * randn(1, ndiscs);  
    mydots(1,:) = (1+sin(randAlpha*100-200 + count*0.05))/2;
   
    [x, y] = RectCenterd(dotDstRects);
    x = mod(x + 0.5 * cos(rotAngles/360*2*pi), w);
    y = mod(y - 0.5 * sin(rotAngles/360*2*pi), h);
   
    dotDstRects = CenterRectOnPointd(inrect .* repmat(scale,4,1), x, y);
   
    vbl = Screen('Flip', win, vbl + 0.5*ifi);
	
	count = count + 1;
end

vbl = Screen('Flip', win);

sca;
