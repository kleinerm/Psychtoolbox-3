function ProceduralSmoothedDiscsDemo(ndiscs)
% ProceduralSmoothedDiscsDemo([ndiscs=200]) -- An aquarium full of discs!
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


% Set number of gabor patches to 200 if no number provided:
if nargin < 1 || isempty(ndiscs)
    ndiscs = 200;
end

fprintf('Will draw %i smoothed aperture discs per frame.\n', ndiscs);

% Select screen with maximum id for output window:
screenid = max(Screen('Screens'));

% Open a fullscreen, onscreen window with gray background. Enable 32bpc
% floating point framebuffer via imaging pipeline on it, if this is possible
% on your hardware while alpha-blending is enabled. Otherwise use a 16bpc
% precision framebuffer together with alpha-blending. We need alpha-blending
% here to implement the nice superposition of overlapping of discs. The demo will
% abort if your graphics hardware is not capable of any of this.
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'FloatingPoint32BitIfPossible');
[win, winRect] = PsychImaging('OpenWindow', screenid, 0.5);

% Retrieve size of window in pixels, need it later to make sure that our
% moving discs don't move out of the visible screen area:
[width, height] = RectSize(winRect);

% Query frame duration: We use it later on to time 'Flips' properly for an
% animation with constant framerate:
ifi = Screen('GetFlipInterval', win);

% Enable alpha-blending
Screen('BlendFunction', win, 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA');

% default x + y size
virtualSize = 128;
% radius of the disc edge
radius = virtualSize / 2;
% smoothing sigma in pixel
sigma = 33;
% use alpha channel for smoothing edge of disc?
useAlpha = true;
% smoothing method: cosine (0), smoothstep (1), inverse smoothstep (2)
smoothMethod = 1;

% Build a procedural disc
disctexture = CreateProceduralSmoothedDisc(win, virtualSize, virtualSize, [0 0 0 0], radius, sigma, ...
                                           useAlpha, smoothMethod);

% Preallocate array with destination rectangles:
texrect = Screen('Rect', disctexture);
inrect = repmat(texrect', 1, ndiscs);

dstRects = zeros(4, ndiscs);
scale = zeros(1,ndiscs);
for i=1:ndiscs
    scale(i) = 0.5 + 0.5 * abs(randn);
    dstRects(:, i) = CenterRectOnPointd(texrect * scale(i), rand * width, rand * height)';
end

% Preallocate array with rotation angles:
rotAngles = rand(1, ndiscs) * 360;

% create random colours for discs
colours = rand(ndiscs,4)';
myAlphas = colours(4,:);

% Initially sync us to VBL at start of animation loop.
count = 0;
vbl = Screen('Flip', win);
tstart = vbl;

% Animation loop: Run until any keypress:
while ~KbCheck
    % Step one: Batch-Draw all discs at the positions (dstRects) and
    % orientations (rotAngles) and colors (colours)
    % and with the stimulus parameters 'discParameters'
    Screen('DrawTextures', win, disctexture, [], dstRects, rotAngles, [], [], colours);

    % Mark drawing ops as finished, so the GPU can do its drawing job while
    % we can compute updated parameters for next animation frame. This
    % command is not strictly needed, but it may give a slight additional
    % speedup, because the CPU can compute new stimulus parameters in
    % Matlab, while the GPU is drawing the stimuli for this frame.
    % Sometimes it helps more, sometimes less, or not at all, depending on
    % your system and code, but it only seldomly hurts.
    % performance...
    Screen('DrawingFinished', win);

    % Compute updated per disc alpha values and positions for next frame. This code
    % is vectorized, but could be probably optimized even more. Indeed,
    % these few lines of Matlab code are the main speed-bottleneck for this
    % demos animation loop on modern graphics hardware, not the actual drawing
    % of the stimulus. The demo as written here is CPU bound - limited in
    % speed by the speed of your main processor.

    % compute a new per disc alpha
    colours(4,:) = (1+sin(myAlphas*100-200 + count*0.05))/2;

    % change the direction slightly
    rotAngles = rotAngles + 10 * randn(1, ndiscs);

    % Compute centers of all patches, then shift them in new direction of
    % motion 'rotAngles', use the mod() operator to make sure they don't
    % leave the window display area. Its important to use RectCenterd and
    % CenterRectOnPointd instead of RectCenter and CenterRectOnPoint,
    % because the latter ones would round all results to integral pixel
    % locations, which would make for an odd and jerky movement. It is
    % also important to feed all matrices and vectors in proper format, as
    % these routines are internally vectorized for higher speed.
    [x, y] = RectCenterd(dstRects);
    x = mod(x + 3.33 * cos(rotAngles/360*2*pi), width);
    y = mod(y - 3.33 * sin(rotAngles/360*2*pi), height);

    % Recompute dstRects destination rectangles for each patch, given the
    % 'per gabor' scale and new center location (x,y):
    dstRects = CenterRectOnPointd(inrect .* repmat(scale,4,1), x, y);

    % Done. Flip one video refresh after the last 'Flip', ie. try to
    % update the display every video refresh cycle if you can.
    % This is the same as Screen('Flip', win);
    % but the provided explicit 'when' deadline allows PTB's internal
    % frame-skip detector to work more accurately and give a more
    % meaningful report of missed deadlines at the end of the script. Not
    % important for this demo, but here just in case you didn't know ;-)
    vbl = Screen('Flip', win, vbl + 0.5 * ifi);

    count = count + 1;
end

% A final synced flip, so we can be sure all drawing is finished when we
% reach this point; print some stats
tend = Screen('Flip', win);
avgfps = count / (tend - tstart);
fprintf('\nPresented a total of %i frames at %.2g FPS...\n',count,avgfps);

% Close onscreen window, release all ressources:
sca;

% Restore old settings for sync-tests:
Screen('Preference', 'SkipSyncTests', oldSyncLevel);
