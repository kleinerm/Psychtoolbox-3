function DriftDemo
% DriftDemo
% ___________________________________________________________________
%
% Display an animated grating using the new Screen('DrawTexture') command.
% In the OS X Psychtoolbox Screen('DrawTexture') replaces
% Screen('CopyWindow').     
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
	
	% Open a double buffered fullscreen window and draw a gray background 
	% to front and back buffers:
	w=Screen('OpenWindow',screenNumber, 0,[],32,2);
	Screen('FillRect',w, gray);
	Screen('Flip', w);
	Screen('FillRect',w, gray);
    
	% compute each frame of the movie and convert the those frames, stored in
	% MATLAB matices, into Psychtoolbox OpenGL textures using 'MakeTexture';
	numFrames=12; % temporal period, in frames, of the drifting grating
	for i=1:numFrames
		phase=(i/numFrames)*2*pi;
		% grating
		[x,y]=meshgrid(-200:200,-200:200);
		angle=30*pi/180; % 30 deg orientation.
		f=0.05*2*pi; % cycles/pixel
		a=cos(angle)*f;
		b=sin(angle)*f;
		m=exp(-((x/90).^2)-((y/90).^2)).*sin(a*x+b*y+phase);
		tex(i)=Screen('MakeTexture', w, gray+inc*m);
	end
		
	% Run the movie animation for a fixed period.  
	movieDurationSecs=5;
	frameRate=Screen('FrameRate',screenNumber);
	if(frameRate==0)  %if MacOSX does not know the frame rate the 'FrameRate' will return 0. 
        frameRate=60;
    end

    movieDurationFrames=round(movieDurationSecs * frameRate);
	movieFrameIndices=mod(0:(movieDurationFrames-1), numFrames) + 1;
	priorityLevel=MaxPriority(w);
	Priority(priorityLevel);

    for i=1:movieDurationFrames
        Screen('DrawTexture', w, tex(movieFrameIndices(i)));
        Screen('Flip', w);
    end

    Priority(0);
	
	%The same commands wich close onscreen and offscreen windows also close
	%textures.
	Screen('CloseAll');

catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    Priority(0);
    psychrethrow(psychlasterror);
end %try..catch..



    




