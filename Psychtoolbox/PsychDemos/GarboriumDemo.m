function GarboriumDemo(ngabors, internalRotation)
% GarboriumDemo([ngabors=200] [, internalRotation=0]) -- An aquarium full of cute little gabors!
%
% This demo shows how to use the Screen('DrawTextures') command to draw a
% large number of similar images quickly - in this case, Gabor patches of
% different position, size and orientation. It also shows how to achieve
% fast, mathematically correct, linear superposition of image patches by
% use of alpha blending and the Psychtoolbox imaging pipeline. While you
% could always achieve proper superposition by precomputing images in
% Matlab, then drawing them as textures, the use of alpha blending allows
% to offload the computations to your graphics hardware - this allows
% speedups by factors of more than 100x.
%
% The demo shows "an aquarium" of many little gabor patches, each moving
% into a random direction. Sometimes the gabors intersect, in that case
% you'll see a linear superposition of them.
%
% Each frame of the animation contains 'ngabors' patches, ngabors defaults
% to 200. You can exit the demo by pressing any key on the keyboard.
%
% This demo needs recent graphics hardware with floating point framebuffer
% support: ATI Radeon X1000 and later, NVidia GeForce 6000 and later.

% History:
% 07/08/2007 Written (MK).

% PTB-3 correctly installed and functional? Abort otherwise.
AssertOpenGL;

% Set number of gabor patches to 200 if no number provided:
if nargin < 1 || isempty(ngabors)
    ngabors = 200;
end

fprintf('Will draw %i gabor patches per frame.\n', ngabors);

if nargin < 2
    internalRotation = 0;
end

% Select screen with maximum id for output window:
screenid = max(Screen('Screens'));

% Open a fullscreen, onscreen window with gray background. Enable 16bpc
% floating point framebuffer via imaging pipeline on it. The demo will
% abort if your graphics hardware is not capable of this.
[win winRect] = Screen('OpenWindow', screenid, 128, [], [], [], [], [], kPsychNeed16BPCFloat);

% Retrieve size of window in pixels, need it later...
[w, h] = Screen('WindowSize', win);

% Query frame duration:
ifi = Screen('GetFlipInterval', win);

% Enable alpha-blending, set it to a blend equation useable for linear
% superposition with alpha-weighted source:
Screen('BlendFunction', win, GL_SRC_ALPHA, GL_ONE);

% Create prototypical gabor patch of 65 x 65 pixels:
si = 32;
if internalRotation
    s = ceil(si * sqrt(2));
else
    s = si;
end

[x,y]=meshgrid(-s:s, -s:s);
angle=0*pi/180; % 30 deg orientation.
f=0.1*2*pi; % cycles/pixel
a=cos(angle)*f;
b=sin(angle)*f;

m=sin(a*x+b*y);

inc = 1.0;
sz = 0.001* s;
period = 2*s;
phase = pi;

for y=-s:+s
    for x=-s:+s
        v(y+s+1,x+s+1) = inc * exp(-((x/sz)*(x/sz))-((y/sz)*(y/sz))) * (cos(2*pi*(cos(angle)*x+sin(angle)*y)/period+phase));
    end
end

%imagesc((cos(2*pi*(cos(angle)*x+sin(angle)*y)/period+phase)));

% Build drawable texture from gabor: We set the 'floatprecision' flag to 1,
% so it is internally stored with 16 bits of floating point precision:
gabortex=Screen('MakeTexture', win, m, [], [], 1);

% Preallocate array with destination rectangles:
texrect = Screen('Rect', gabortex);
dstRects = zeros(4, ngabors);
for i=1:ngabors
    scale(i) = (0.1 + 0.9 * randn);
    dstRects(:, i) = CenterRectOnPoint(texrect * scale(i), rand * w, rand * h)';
end

% Preallocate array with rotation angles:
rotAngles = rand(1, ngabors) * 360;

% Initially sync us to VBL:
vbl = Screen('Flip', win);
tstart = vbl;
count = 0;

if internalRotation
    sflags = kPsychUseTextureMatrixForRotation;
    srcRect = CenterRect([0 0 (2*si+1) (2*si+1)], Screen('Rect', gabortex));
else
    sflags = 0;
    srcRect = [];
end

% Animation loop: Run until keypress:
while ~KbCheck
    % Step one: Batch-Draw all gabor patches at the positions and
    % orientations computed during last loop iteration:
    Screen('DrawTextures', win, gabortex, srcRect, dstRects, rotAngles, [], 0.5, [], [], sflags);
    
    % Mark drawing ops as finished, so the GPU can do its drawing job while
    % we can compute updated parameters for next animation frame:
    Screen('DrawingFinished', win);
    
    % Compute updated positions and orientations for next frame:
    for i=1:ngabors
        [x y] = RectCenter(dstRects(:,i));
        rotAngles(i) = rotAngles(i) + 3 * randn;
        x = mod(x + cos(rotAngles(i)/360*2*pi), w);
        y = mod(y + sin(rotAngles(i)/360*2*pi), h);
        dstRects(:,i) = CenterRectOnPoint(texrect * scale(i), x, y)';
    end
    
    % Done. Flip on next retrace:
    vbl = Screen('Flip', win, vbl + 0.5 * ifi);
    
    % Next loop iteration...
    count = count + 1;
end

% Print the stats:
count
avgfps = count / (vbl - tstart)

% Close onscreen window, release all ressources:
Screen('CloseAll');

% Done.
return;
