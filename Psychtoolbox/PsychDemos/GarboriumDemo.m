function GarboriumDemo(ngabors, internalRotation)
% GarboriumDemo([ngabors=200] [, internalRotation=0]) -- An aquarium full of cute little gabors!
%
% This demo shows how to use the Screen('DrawTextures') command to draw a
% large number of similar images quickly - in this case, Gabor patches of
% different position, size and orientation. It also shows how to achieve
% fast, mathematically correct, linear superposition of image patches by
% use of alpha blending and the Psychtoolbox imaging pipeline. While you
% could always achieve proper superposition by precomputing large images in
% Matlab, then drawing them as textures, the use of alpha blending allows
% to offload the computations to your graphics hardware - this allows
% speedups by factors of more than 100x in some cases!
%
% The demo shows "an aquarium" of many cute little gabor patches, each moving
% into a random direction. Sometimes the gabors intersect, in that case
% you'll see a linear superposition of them.
%
% Each frame of the animation contains 'ngabors' patches, ngabors defaults
% to 200. Change the number as first optional parameter 'ngabors' if you want
% to exercise your graphics hardware and cpu.
%
% Normally the gabor paches rotate as a whole, a different method of
% rotation is so called internal texture rotation (see comments in code),
% which can be selected by setting the optional 2nd argument
% 'internalRotation' to a value of 1.
%
% You can exit the demo by pressing any key on the keyboard.
%
% This demo needs recent graphics hardware with floating point framebuffer
% support: ATI Radeon X1000 and later, NVidia GeForce 6000 and later.

% History:
% 07/08/2007 Written (MK).
% 05/18/2008 Rewritten, beautified, adapted to current PTB (MK).

% PTB-3 correctly installed and functional? Abort otherwise.
AssertOpenGL;

% Set number of gabor patches to 200 if no number provided:
if nargin < 1 || isempty(ngabors)
    ngabors = 200;
end

fprintf('Will draw %i gabor patches per frame.\n', ngabors);

if nargin < 2 || isempty(internalRotation)
    internalRotation = 0;
end

% Select screen with maximum id for output window:
screenid = max(Screen('Screens'));

% Open a fullscreen, onscreen window with gray background. Enable 32bpc
% floating point framebuffer via imaging pipeline on it, if this is possible
% on your hardware while alpha-blending is enabled. Otherwise use a 16bpc
% precision framebuffer together with alpha-blending. We need alpha-blending
% here to implement the nice superposition of overlapping gabors. The demo will
% abort if your graphics hardware is not capable of any of this.
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'FloatingPoint32BitIfPossible');
[win winRect] = PsychImaging('OpenWindow', screenid, 128);

% Retrieve size of window in pixels, need it later to make sure that our
% moving gabors don't move out of the visible screen area:
[w, h] = RectSize(winRect);

% Query frame duration: We use it later on to time 'Flips' properly for an
% animation with constant framerate:
ifi = Screen('GetFlipInterval', win);

% Enable alpha-blending, set it to a blend equation useable for linear
% superposition with alpha-weighted source. This allows to linearly
% superimpose gabor patches in the mathematically correct manner, should
% they overlap. Alpha-weighted source means: The 'globalAlpha' parameter in
% the 'DrawTextures' can be used to modulate the intensity of each pixel of
% the drawn patch before it is superimposed to the framebuffer image, ie.,
% it allows to specify a global per-patch contrast value:
Screen('BlendFunction', win, GL_SRC_ALPHA, GL_ONE);

% Define prototypical gabor patch of 65 x 65 pixels default size: si is
% half the wanted size. Later on, the 'DrawTextures' command will simply
% scale this patch up and down to draw individual patches of the different
% wanted sizes. To avoid resampling artifacts, you should choose the size of
% the prototypical patch roughly the mean size of the wanted patch sizes, or
% alternatively the biggest size -- Downscaling with bilinear filtering is
% a graceful operation, it only needs to discard information in a
% artifact-free manner, but upscaling a too low resolution patch will
% create artifacts, as it can't create information about structure from
% nothing, just try to hide the most ugly artifacts:
si = 32;

% 'internalRotation' allows to rotate the gabor patches within the
% rectangular area of each drawn patch -- the texture is rotated
% "internally" to each destination patch rectangle. The default setting is
% to rotate the whole destination patch rectangle itself around its center.
% Whatever suits you best...
if internalRotation
    % Need to make patches by squareroot of 2 bigger to avoid border
    % artifacts during internal rotation of the sampled texture:
    s = ceil(si * sqrt(2));
else
    s = si;
end

if 1
    % Definition of a Gabor patch as Matlab grayscale matrix 'm' with
    % proper sign -- a value of zero in m denotes zero contrast at that
    % pixel location: This is ugly cut & copy & paste code which could be
    % written much nicer by a person with good taste ;-)
    res = 2*[s s];
    phase = 0;
    sc = 5;
    freq = 0.05;
    tilt = 0;
    contrast = 5;
    x=res(1)/2;
    y=res(2)/2;
    sf = freq;
    [gab_x gab_y] = meshgrid(0:(res(1)-1), 0:(res(2)-1));
    a=cos(deg2rad(tilt))*sf*360;
    b=sin(deg2rad(tilt))*sf*360;
    multConst=1/(sqrt(2*pi)*sc);
    x_factor=-1*(gab_x-x).^2;
    y_factor=-1*(gab_y-y).^2;
    sinWave=sin(deg2rad(a*(gab_x - x) + b*(gab_y - y)+phase));
    varScale=2*sc^2;
    m=contrast*(multConst*exp(x_factor/varScale+y_factor/varScale).*sinWave)';
else
    % Old style: Sine gratings with pretty hard contrast:
    [x,y]=meshgrid(-s:s, -s:s);
    angle=0*pi/180; % 30 deg orientation.
    f=0.1*2*pi; % cycles/pixel
    a=cos(angle)*f;
    b=sin(angle)*f;
    m=sin(a*x+b*y);
end

% Build drawable texture from gabor matrix: We set the 'floatprecision' flag to 2,
% so it is internally stored with 32 bits of floating point precision and
% sign. This allows for effectively 8 million (23 bits) of contrast levels
% for both the positive- and the negative "half-lobe" of the patch -- More
% than enough precision for any conceivable display system:
gabortex=Screen('MakeTexture', win, m, [], [], 2);

% Preallocate array with destination rectangles:
% This also defines initial gabor patch orientations, scales and location
% for the very first drawn stimulus frame:
texrect = Screen('Rect', gabortex);
inrect = repmat(texrect', 1, ngabors);

dstRects = zeros(4, ngabors);
for i=1:ngabors
    scale(i) = 2*(0.1 + 0.9 * randn);
    dstRects(:, i) = CenterRectOnPoint(texrect * scale(i), rand * w, rand * h)';
end

% Preallocate array with rotation angles:
rotAngles = rand(1, ngabors) * 360;

% If internalRotation, we need to explicitely specify the size of a texture
% source rectangle 'srcRect', as it slightly deviates from the default
% size, which would be exactly the size of the texture itself. We also need
% to specify the optional 'sflags' parameter for 'DrawTextures' to tell the
% command it should use internal rotation:
if internalRotation
    sflags = kPsychUseTextureMatrixForRotation;
    srcRect = CenterRect([0 0 (2*si+1) (2*si+1)], Screen('Rect', gabortex));
else
    sflags = 0;
    srcRect = [];
end

% Initially sync us to VBL at start of animation loop.
vbl = Screen('Flip', win);
tstart = vbl;
count = 0;

% Animation loop: Run until any keypress:
while ~KbCheck
    % Step one: Batch-Draw all gabor patches at the positions and
    % orientations computed during last loop iteration: Here we fix
    % 'globalAlpha' - and therefore contrast - to 0.5, ie., 50% of
    % displayable range. Actually its less than 0.5, depending on the
    % inherent contrast of the gabor defined above in matrix 'm'.
    Screen('DrawTextures', win, gabortex, srcRect, dstRects, rotAngles, [], 0.5, [], [], sflags);
    
    % Mark drawing ops as finished, so the GPU can do its drawing job while
    % we can compute updated parameters for next animation frame. This
    % command is not strictly needed, but it may give a slight additional
    % speedup, because the CPU can compute new stimulus parameters in
    % Matlab, while the GPU is drawing the stimuli for this frame.
    % Sometimes it helps more, sometimes less, or not at all, depending on
    % your system and code, but it only seldomly hurts.
    % performance...
    Screen('DrawingFinished', win);
    
    % Compute updated positions and orientations for next frame. This code
    % is vectorized, but could be probably optimized even more. Indeed,
    % these few lines of Matlab code are the main speed-bottleneck for this
    % demos animation loop on modern graphics hardware, not the actual drawing
    % of the stimulus. The demo as written here is CPU bound - limited in
    % speed by the speed of your main processor.

    % Compute new random orientation for each patch in next frame:
    rotAngles = rotAngles + 1 * randn(1, ngabors);
    
    % Compute centers of all patches, then shift them in new direction of
    % motion 'rotAngles', use the mod() operator to make sure they don't
    % leave the window display area. Its important to use RectCenterd and
    % CenterRectOnPointd instead of RectCenter and CenterRectOnPoint,
    % because the latter ones would round all results to integral pixel
    % locations, which would make for an odd and jerky movement. It is
    % also important to feed all matrices and vectors in proper format, as
    % these routines are internally vectorized for higher speed.
    [x y] = RectCenterd(dstRects);
    x = mod(x + cos(rotAngles/360*2*pi), w);
    y = mod(y + sin(rotAngles/360*2*pi), h);

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

 % Next loop iteration...
    count = count + 1;
end

% Done. Last flip to take end timestamp and for stimulus offset:
vbl = Screen('Flip', win);

% Print the stats:
count
avgfps = count / (vbl - tstart)

% Close onscreen window, release all ressources:
sca;

% Done.
return;

% Little helper function: Converts degrees to radians, included for
% backward compatibility with old Matlab versions:
function radians = deg2rad(degrees)
    radians = degrees * pi / 180;
return
