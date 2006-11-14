function DriftDemo3(cyclespersecond, p)
% function DriftDemo3(cyclespersecond, p)
%
% Display an animated grating using the new Screen('DrawTexture') command.
% In the OpenGL-Psychtoolbox Screen('DrawTexture') replaces
% Screen('CopyWindow'). The demo will stop after 60 seconds have
% passed or after the user hits a key.
%
% This demo illustrates how to draw an animated grating online by use of
% only one grating texture with the minimal amount of code and complexity.
% It is restricted in that the spatial period of the grating in pixels
% must divide the total size of the pattern without remainder. For a more
% complex but more general solution see DriftDemoOSX2.
%
% We create one single texture with a static sine grating. The texture
% is a power-of-two texture, one whose width and height are powers of two.
%
% Such textures can be drawn in a special scrolling mode, as if they
% would extend in each direction to infinity, periodically repeating
% the pattern of the texture image in each direction.
%
% In each successive frame we draw a rectangular region of the sine
% texture onto the screen that is the size of the texture. This region,
% the 'srcRect' in the Screen('DrawTexture') command, is shifted each
% frame. As our special texture is repeating infinitely into each direction,
% we create the impression of a moving grating.
%
% Parameters:
%
% cyclespersecond = Speed of grating in cycles per second.
% p = Spatial period of grating in pixels.
%
% CopyWindow vs. DrawTexture:
%
% In the OS 9 Psychtoolbox, Screen ('CopyWindow") was used for all
% time-critical display of images, in particular for display of the movie
% frames in animated stimuli. In contrast, Screen('DrawTexture') should not
% be used for display of all graphic elements,  but only for  display of
% MATLAB matrices.  For all other graphical elements, such as lines,  rectangles,
% and ovals we recommend that these be drawn directly to the  display
% window during the animation rather than rendered to offscreen  windows
% prior to the animation.
% 
% see also: PsychDemos, MovieDemo, DriftDemo, DriftDemo2

% HISTORY
%  5/5/06    mk     Adapted from DriftDemoOSX2.m 

if nargin<1
    % Default speed of grating in cycles per second:
    cyclespersecond=1;
end;

if nargin<2
    % Default grating spatial period:
    p=32;
end;

movieDurationSecs=60;   % Abort demo after 60 seconds.
visiblesize=512;        % Size of the grating image. Needs to be a power of two.

if rem(visiblesize, p)~=0
  error('Period p must divide visiblesize without remainder for this demo to work!');
end;

% This script calls Psychtoolbox commands available only in OpenGL-based 
% versions of the Psychtoolbox. The Psychtoolbox command AssertPsychOpenGL will issue
% an error message if someone tries to execute this script on a computer without
% an OpenGL Psychtoolbox.
AssertOpenGL;

% Get the list of screens and choose the one with the highest screen number.
% Screen 0 is, by definition, the display with the menu bar. Often when 
% two monitors are connected the one without the menu bar is used as 
% the stimulus display.  Chosing the display with the highest dislay number is 
% a best guess about where you want the stimulus displayed.  
screens=Screen('Screens');
screenNumber=max(screens);

% Find the color values which correspond to white and black.  Though on OS
% X we currently only support true color and thus, for scalar color
% arguments, black is always 0 and white 255, this rule is not necessarily
% true on other platforms and will not remain true after we add other color depth modes.  
white=WhiteIndex(screenNumber);
black=BlackIndex(screenNumber);
gray=(white+black)/2;
if round(gray)==white
	gray=black;
end
inc=white-gray;

% Open a double buffered fullscreen window and draw a gray background 
% to front and back buffers:
[w screenRect]=Screen('OpenWindow',screenNumber, gray);

% Flip buffer to show initial gray background:
Screen('Flip', w);

% Calculate parameters of the grating:
f=1/p;
fr=f*2*pi;    % frequency in radians.

% Create one single static grating image:
[x,y]=meshgrid(0:visiblesize-1, 0:visiblesize-1);
grating=gray + inc*cos(fr*x);

% Store grating in texture: Set the 'enforcepot' flag to 1 to signal
% Psychtoolbox that we want a special scrollable power-of-two texture:
gratingtex=Screen('MakeTexture', w, grating, [], 1);

% Query duration of monitor refresh interval:
ifi=Screen('GetFlipInterval', w);    
waitframes = 1;
waitduration = waitframes * ifi;

% Translate requested speed of the grating (in cycles per second)
% into a shift value in "pixels per frame", assuming given
% waitduration: This is the amount of pixels to shift our srcRect at
% each redraw:
shiftperframe= cyclespersecond * p * waitduration;

% Perform initial Flip to sync us to the VBL and for getting an initial
% VBL-Timestamp for our "WaitBlanking" emulation:
vbl=Screen('Flip', w);

% We run at most 'movieDurationSecs' seconds if user doesn't abort via keypress.
vblendtime = vbl + movieDurationSecs;
xoffset=0;

% Animationloop:
while(vbl < vblendtime)
   % Shift the grating by "shiftperframe" pixels per frame:
   xoffset = xoffset + shiftperframe;

   % Define shifted srcRect that cuts out the properly shifted rectangular
   % area from the texture:
   srcRect=[xoffset 0 xoffset + visiblesize visiblesize];

   % Draw grating texture: Only show subarea 'srcRect', center
   % texture in the onscreen window.
   Screen('DrawTexture', w, gratingtex, srcRect);

   % Flip 'waitframes' monitor refresh intervals after last redraw.
   vbl = Screen('Flip', w, vbl + (waitframes - 0.5) * ifi);

   % Abort demo if any key is pressed:
   if KbCheck
      break;
   end;
end;

% The same commands wich close onscreen and offscreen windows also close
% textures.
Screen('CloseAll');

% Well done!
return;




