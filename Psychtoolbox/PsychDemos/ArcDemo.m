function ArcDemo
% ArcDemo
%
% Demonstration of the function Screen('FillArc',...) on OSX
%
% Date of creation: 12/02/05
% Author: Kerstin Preuschoff, Caltech
% 
% Previous versions: 
%
% History: 
% Small changes to make it OSX-ish by Mario Kleiner
%          
% Presentation Sequence:
%   - the demo basically draws a pie chart (circle with 3 differently
%   colored arcs
%   - the circle is then partially covered by another arc
%

% Make sure we run on OpenGL Psychtoolbox
AssertOpenGL;

try
    fprintf('ArcDemoOSX - Donated by Kerstin Preuschoff\n');
    fprintf('First image will show drawing of filled arcs (Pie-Chart style) - Press a key to continue.\n');
    fprintf('Second image will show drawing of arc outline with fixed 1 pixel size - Press a key to continue.\n');
    fprintf('Third image will show drawing of arc with outline of 5 pixels thickness - Press a key to end demo.\n');
    fprintf('Press a key to start.\n');
    KbWait;
    WaitSecs(1);

    % Open onscreen window with default settings:
    screenNumber = max(Screen('Screens'));
    [window,screenRect] = Screen('OpenWindow', screenNumber, 0, []);

    HideCursor;
    
    % define positions and angles
    positionOfMainCircle = [350 250 450 350] ;
    startAngle = [0 100 240] ;   % the colors red, blue, green start
    % at 0, 100, 240 deg
    sizeAngle  = [100 140 120] ; % the colors red, blue, green end at
    % 0+100=100, 100+140=240, 240+120=360 deg
    positionOfCover = [345 245 455 355] ; % the cover is in a rectangle a
    % little larger than the other arcs
    coverStart = 40; % deg
    coverAngle = 60; % deg

    % define colors
    white = WhiteIndex(window) ;
    darkgray = white/2.2;
    red = [200 0 0] ;
    green = [0 200 0] ;
    blue = [0 0 200];

    % Clear screen to blue background:
    Screen('FillRect', window, [0 0 255]);
    Screen('Flip', window);

    % Draw filled arcs:
    Screen('FillArc',window, red,positionOfMainCircle,startAngle(1),sizeAngle(1));
    Screen('FillArc',window, blue,positionOfMainCircle,startAngle(2),sizeAngle(2));
    Screen('FillArc',window, green,positionOfMainCircle,startAngle(3),sizeAngle(3));
    Screen('FillArc',window, darkgray, positionOfCover,coverStart,coverAngle);
    
    % Show it:
    Screen('Flip', window);
    % Wait for keyboard press:
    KbWait;
    WaitSecs(1);
    
    % Draw arcs:
    Screen('DrawArc',window, red,positionOfMainCircle,startAngle(1),sizeAngle(1));
    Screen('DrawArc',window, blue,positionOfMainCircle,startAngle(2),sizeAngle(2));
    Screen('DrawArc',window, green,positionOfMainCircle,startAngle(3),sizeAngle(3));
    Screen('DrawArc',window, darkgray, positionOfCover,coverStart,coverAngle);

    % Show it:
    Screen('Flip', window);
    % Wait for keyboard press:
    KbWait;
    WaitSecs(1);

    % Draw framed arcs of thickness 5:
    Screen('FrameArc',window, red,positionOfMainCircle,startAngle(1),sizeAngle(1), 5);
    Screen('FrameArc',window, blue,positionOfMainCircle,startAngle(2),sizeAngle(2), 5);
    Screen('FrameArc',window, green,positionOfMainCircle,startAngle(3),sizeAngle(3), 5);
    Screen('FrameArc',window, darkgray, positionOfCover,coverStart,coverAngle, 5);
    
    % Show it:
    Screen('Flip', window);
    % Wait for keyboard press:
    KbWait;
    
    % Done. Show cursor and close window.
    ShowCursor;
    Screen('CloseAll');

catch
    % This section is executed in case an error happens in the
    % experiment code implemented between try and catch...
    ShowCursor;
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end;
