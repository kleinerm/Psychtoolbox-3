function DriftDemo
% DriftDemo
% ___________________________________________________________________
%
% Display an animated grating using the new Screen('DrawTexture') command.
% In Psychtoolbox-3 Screen('DrawTexture') replaces
% Screen('CopyWindow').     
%
% This is a very simple, bare bones demo on how to do frame animation. For
% much more efficient ways to draw gratings and gabors, have a look at
% DriftDemo2, DriftDemo3, DriftDemo4, ProceduralGaborDemo, GarboriumDemo,
% ProceduralGarboriumDemo and DriftWaitDemo.
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
% _________________________________________________________________________
% 
% see also: PsychDemos, MovieDemo

% HISTORY
%  6/28/04    awi     Adapted from Denis Pelli's DriftDemo.m for OS 9 
%  7/18/04    awi     Added Priority call.  Fixed.
%  9/8/04     awi     Added Try/Catch, cosmetic changes to comments and see also.
%  4/23/05    mk      Added Priority(0) in catch section, moved Screen('OpenWindow')
%                     before first call to Screen('MakeTexture') in
%                     preparation of future improvements to 'MakeTexture'.
%  2/28/09    mk      Smallish refinements, cleanups, updated comments.

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
	
    % Find the color values which correspond to white and black: Usually
	% black is always 0 and white 255, but this rule is not true if one of
	% the high precision framebuffer modes is enabled via the
	% PsychImaging() commmand, so we query the true values via the
	% functions WhiteIndex and BlackIndex:
	white=WhiteIndex(screenNumber);
	black=BlackIndex(screenNumber);
    
    % Round gray to integral number, to avoid roundoff artifacts with some
    % graphics cards:
	gray=round((white+black)/2);

    % This makes sure that on floating point framebuffers we still get a
    % well defined gray. It isn't strictly neccessary in this demo:
    if gray == white
		gray=white / 2;
    end
    
    % Contrast 'inc'rement range for given white and gray values:
	inc=white-gray;

    % Open a double buffered fullscreen window and select a gray background 
	% color:
	w=Screen('OpenWindow',screenNumber, gray);
    
	% Compute each frame of the movie and convert the those frames, stored in
	% MATLAB matices, into Psychtoolbox OpenGL textures using 'MakeTexture';
	numFrames=12; % temporal period, in frames, of the drifting grating
	for i=1:numFrames
		phase=(i/numFrames)*2*pi;
		% grating
		[x,y]=meshgrid(-300:300,-300:300);
		angle=30*pi/180; % 30 deg orientation.
		f=0.05*2*pi; % cycles/pixel
		a=cos(angle)*f;
		b=sin(angle)*f;
		m=exp(-((x/90).^2)-((y/90).^2)).*sin(a*x+b*y+phase);
		tex(i)=Screen('MakeTexture', w, gray+inc*m); %#ok<AGROW>
	end
		
	% Run the movie animation for a fixed period.  
	movieDurationSecs=5;
	frameRate=Screen('FrameRate',screenNumber);

    % If MacOSX does not know the frame rate the 'FrameRate' will return 0.
    % That usually means we run on a flat panel with 60 Hz fixed refresh
    % rate:
    if frameRate == 0
        frameRate=60;
    end

    % Convert movieDuration in seconds to duration in frames to draw:
    movieDurationFrames=round(movieDurationSecs * frameRate);
	movieFrameIndices=mod(0:(movieDurationFrames-1), numFrames) + 1;

    % Use realtime priority for better timing precision:
    priorityLevel=MaxPriority(w);
	Priority(priorityLevel);

    % Animation loop:
    for i=1:movieDurationFrames
        % Draw image:
        Screen('DrawTexture', w, tex(movieFrameIndices(i)));
        % Show it at next display vertical retrace. Please check DriftDemo2
        % and later, as well as DriftWaitDemo for much better approaches to
        % guarantee a robust and constant animation display timing! This is
        % very basic and not best practice!
        Screen('Flip', w);
    end

    Priority(0);
	
    % Close all textures. This is not strictly needed, as
    % Screen('CloseAll') would do it anyway. However, it avoids warnings by
    % Psychtoolbox about unclosed textures. The warnings trigger if more
    % than 10 textures are open at invocation of Screen('CloseAll') and we
    % have 12 textues here:
    Screen('Close');
    
    % Close window:
    Screen('CloseAll');

catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    Priority(0);
    psychrethrow(psychlasterror);
end %try..catch..
