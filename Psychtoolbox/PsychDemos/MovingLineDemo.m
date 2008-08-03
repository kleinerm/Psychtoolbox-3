function MovingLineDemo(xv, screenid)
% MovingLineDemo([xv=1][, screenid=max])
%
% Shows a pair of vertical lines which travel horizontally across the
% display from the left to the right, repeating infinitely.
%
% The optional parameter 'xv' defines the speed in pixels per redraw cycle.
% It defaults to 1 pixel per redraw cycle.
%
% The optional parameter 'screenid' selects the display screen to use for
% display, it defaults to the secondary display on multi-display setups.
%
% Hold down the right mouse button to pause the animation.
% Press the left mouse button to exit the demo.
%
% The lines show a greyscale gradient, starting with black at the top of
% the screen, ending in white at the bottom. They are seperated by 'xv'
% pixels.
%
% The purpose of this simple animation is to demonstrate differences in the
% way CRT monitors and TFT flat panel display devices display moving
% stimuli. It shows artifacts that are due to both the display technology
% and due to perceptual effects in the visual system.
%
% On a well working CRT monitor, which is an impulse-type display with fast
% response time, you should see a sharp and clearly separated moving pair
% of lines. On a LCD display with its high latency response behaviour and
% its working principle as a hold-type display, you should see a
% significant "smear" or "blur" of the line pair -- or maybe not even a
% pair of distinctive lines anymore. This is due to technical limitations
% of the display technology and due to the "bad" interaction between
% hold-type displays and smooth pursuit eye movements caused by tracking of
% certain types of moving stimuli.
%

% History:
% 7/28/8 mk Written.

lw = 1;

% Make sure we're running on PTB-3:
AssertOpenGL;

% Use a movement speed of 1 horizontal pixel per redraw cycle by default:
if nargin < 1 
    xv = 1;
end

if isempty(xv)
    xv = 1;
end

% Choose secondary display by default on multi-display setups:
if nargin < 2
    screenid = max(Screen('Screens'));
end

% Open window with black background color, query its size and redraw
% interval, do initial flip to sync us to vertical retrace:
win = Screen('OpenWindow', screenid, 128);
[w, h] = Screen('WindowSize', win);
ifi = Screen('GetFlipInterval', win);
vbl=Screen('Flip', win);

% Init defaults:
x=0;
button = 0;

% Run until left mouse button is pressed:
while ~button(1)
    % Query mouse:
    [xm, ym, button] = GetMouse;
    
    % Move line pair by 'xv' unless right mouse button is pressed, which
    % will pause the animation:
    if button(2)==0
        x=mod(x+xv, w);
    end
    
    % We use 'DrawLines' so we can easily define a vertical intensity
    % gradient:
    Screen('DrawLines', win, [x, x ; 0, h], lw, [0, 255; 0, 255; 0, 255]);
    Screen('DrawLines', win, [x+xv, x+xv ; 0, h], lw, [0, 255; 0, 255; 0, 255]);
    
    % We use 'vbl' based timing, just that the frame-skip detector works
    % accurately and we get notified of possibly skipped frames -- Allows
    % to see if perceived jerks come frome timing issues or are induced by
    % the display or perception:
    vbl=Screen('Flip', win,vbl+ifi/2);
end

% Done. Close windows and exit:
Screen('CloseAll');

return;
