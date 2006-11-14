function AlphaRotateDemo(numFrames, ifis)
% AlphaRotateDemo(numFrames, ifis)
%
% numFrames Number of grating textures to use for the drifting grating...
%
% ifis = Number of monitor refreshes to wait between drawing single
% textures...
%
%
% OS X: ___________________________________________________________________
%
% Display a rotating grating using the new Screen('DrawTexture') command.
% In the OS X Psychtoolbox Screen('DrawTexture') replaces
% Screen('CopyWindow').     
%
% This illustrates an application of Alpha blending by masking the rotating
% grating with a gaussian transparency mask.
%
% In each frame, first the grating is drawn. Then a texture acting as a
% transparency mask is drawn "over" the grating, masking out selected
% parts of the grating.
%
% _________________________________________________________________________
% 
% see also: PsychDemos, MovieDemo

% HISTORY
%  6/28/04    awi     Adapted from Denis Pelli's DriftDemo.m for OS 9 
%  7/18/04    awi     Added Priority call.  Fixed.
%  9/8/04     awi     Added Try/Catch, cosmetic changes to comments and see also.
%  1/9/05     mk      Adapted from awi's DriftDemoOSX.                 
%  5/13/05    mk      Alpha-Blending reenabled, new VBL-Syncing enabled...
%  5/23/05    mk      Bugs fixed that made it fail on Matlab 6

if nargin<2
    ifis=2;
end;

if nargin<1
    numFrames=1
end;

try
	% This script calls Psychtoolbox commands available only in OpenGL-based 
	% versions of the Psychtoolbox. (So far, the OS X Psychtoolbox is the
	% only OpenGL-base Psychtoolbox.)  The Psychtoolbox command AssertPsychOpenGL will issue
	% an error message if someone tries to execute this script on a computer without
	% an OpenGL Psychtoolbox
	AssertOpenGL;
	
	% Get the list of screens and choose the one with the highest screen number.
	% Screen 0 is, by definition, the display with the menu bar. Often when 
	% two monitors are connected the one without the menu bar is used as 
	% the stimulus display.  Chosing the display with the highest dislay number is 
	% a best guess about where you want the stimulus displayed.  
	screens=Screen('Screens');
	screenNumber=max(screens);

    % Open a double-buffered fullscreen window:
	w=Screen('OpenWindow',screenNumber);
    [width, height]=Screen('WindowSize', w);

    % Enable alpha blending with proper blend-function. We need it
    % for drawing of our alpha-mask (gaussian aperture):
    Screen('BlendFunction', w, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	% Find the color values which correspond to white and black.  Though on OS
	% X we currently only support true color and thus, for scalar color
	% arguments,
	% black is always 0 and white 255, this rule is not true on other platforms will
	% not remain true on OS X after we add other color depth modes.  
	white=WhiteIndex(screenNumber);
	black=BlackIndex(screenNumber);
	gray=(white+black)/2;
	if round(gray)==white
		gray=black;
	end
	inc=white-gray;
	
	% compute each frame of the grating and convert those frames, stored in
	% MATLAB matrices, into Psychtoolbox OpenGL textures using 'MakeTexture';
	% numFrames= temporal period, in frames, of the drifting grating
    s=min(width, height) / 4;
	[x,y]=meshgrid(-s:s-1, -s:s-1);
	angle=30*pi/180; % 30 deg orientation.
	f=0.01*2*pi; % cycles/pixel
    a=cos(angle)*f;
	b=sin(angle)*f;
                
    % Build grating textures:
    for i=1:numFrames
		phase=(i/numFrames)*2*pi;
        m=sin(a*x+b*y+phase);
        gratingtex=(gray+inc*m);
        tex(i)=Screen('MakeTexture', w, gratingtex);
    end
    
    tavg=0;
    
    % We create a Luminance+Alpha matrix for use as transparency mask:

    % Layer 1 (Luminance) is filled with luminance value 'gray' of the
    % background.
    maskblob=ones(2*s, 2*s, 2) * gray;
    % Layer 2 (Transparency aka Alpha) is filled with gaussian transparency
    % mask.
    maskblob(:,:,2)=255 - exp(-((x/90).^2)-((y/90).^2))*255;

    % Build a single transparency mask texture:
    masktex=Screen('MakeTexture', w, maskblob);
    
    % Query the real duration of a monitor refresh interval, gained through
    % some measurement during Screen('OpenWindow')...
    ifi_duration = Screen('GetFlipInterval', w)
    
	% Run the movie animation for a fixed period of max 20 seconds.  
	movieDurationSecs=20;

    frameRate=Screen('FrameRate',screenNumber);
	if(frameRate==0)  % if MacOSX does not know the frame rate the 'FrameRate' will return 0. 
        frameRate=60; % 60 Hz is a good guess for flat-panels...
    end    

    movieDurationFrames=round(movieDurationSecs * frameRate / ifis);
	movieFrameIndices=mod(0:(movieDurationFrames-1), numFrames) + 1;

    % Switch to realtime:
    priorityLevel=MaxPriority(w);
    Priority(priorityLevel)

    % Prepare screen for animation:
    % Draw gray full-screen rectangle to clear to a defined
    % background color:
    Screen('FillRect',w, gray);

    % Show the gray background, return timestamp of flip in 'vbl'
    vbl = Screen('Flip', w);

    % Animation loop:
    for i=1:movieDurationFrames
        t1=GetSecs;
        % Draw grating for current frame:
        % Please note the new (optional) 5th parameter 'i'!
        % This specifies the rotation angle (in degree) of the texture.
        Screen('DrawTexture', w, tex(movieFrameIndices(i)), [], [], i);

        % Overdraw -- and therefore alpha-blend -- with gaussian alpha
        % mask:
        Screen('DrawTexture', w, masktex);
        
        % Show result on screen: We only want to show a new frame every
        % ifis monitor refresh intervals. Therefore we calculate a proper
        % presentation time that is '(ifis - 0.5) * ifi_duration' after the
        % time 'vbl' when the previous frame was shown.
        % This is the equivalent of WaitBlanking on old PTB:
        vbl=Screen('Flip', w, vbl + (ifis - 0.5) * ifi_duration);
        
        t1=GetSecs - t1;
        if (i>numFrames)
            tavg=tavg+t1;
        end;
        
        % We also abort on keypress...
        if KbCheck
            break;
        end;
	end;

    % Shut down realtime-mode:
    Priority(0);

    tavg=tavg / (movieDurationFrames - numFrames)

    % We're done: Close all windows and textures:
    Screen('CloseAll');
    
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Priority(0);
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end %try..catch..
