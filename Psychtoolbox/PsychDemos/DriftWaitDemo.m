function DriftWaitDemo(movieDurationSecs, waitframes)
% DriftWaitDemo([movieDurationSecs=10][, waitframes=1])
% ___________________________________________________________________
%
% Display an animated grating using the new Screen('DrawTexture') command.
% In Psychtoolbox-3 Screen('DrawTexture') replaces Screen('CopyWindow').
% 
% This demo illustrates on how to emulate the old Screen('WaitBlanking'...)
% behaviour: If you set waitframes > 1 then the screen is only updated
% every waitframes'th monitor refresh interval.
%
% Optional parameters:
%
% movieDurationSecs == Requested total duration of movie in seconds.
% waitframes == Number of monitor refresh intervals to wait before each
% frame is drawn.
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
% _________________________________________________________________________
% 
% see also: PsychDemos, MovieDemo, DriftDemo

% HISTORY
%  6/28/04    awi     Adapted from Denis Pelli's DriftDemo.m for OS 9 
%  7/18/04    awi     Added Priority call.  Fixed.
%  9/8/04     awi     Added Try/Catch, cosmetic changes to comments and see also.
%  4/23/05    mk      Added Priority(0) in catch section, moved Screen('OpenWindow')
%                     before first call to Screen('MakeTexture') in
%                     preparation of future improvements to 'MakeTexture'.
%  5/10/05    mk      Added demo-code for WaitBlanking - emulation.
%  2/28/09    mk      Smallish refinements, cleanups, updated comments.

if nargin < 2
    waitframes = 1;
end;

if nargin < 1
    movieDurationSecs = [];
end

if isempty(movieDurationSecs)
    movieDurationSecs = 10;
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

    % Open a double buffered fullscreen window and set default background
	% color to gray:
	w = Screen('OpenWindow',screenNumber, gray);

    % compute each frame of the movie and convert the those frames, stored in
	% MATLAB matices, into Psychtoolbox OpenGL textures using 'MakeTexture';
	numFrames=12; % temporal period, in frames, of the drifting grating
	for i=1:numFrames
		phase=(i/numFrames)*2*pi;
        %phase=mod(i,2)*pi+pi/2;
		% grating
		[x,y]=meshgrid(-200:200,-200:200);
		angle=30*pi/180; % 30 deg orientation.
		f=0.05*2*pi; % cycles/pixel
		%f=0;
        a=cos(angle)*f;
		b=sin(angle)*f;
		m=exp(-((x/90).^2)-((y/90).^2)).*sin(a*x+b*y+phase);
		tex(i)=Screen('MakeTexture', w, gray+inc*m); %#ok<AGROW>
	end
		
	% Run the movie animation for a fixed period.  
	frameRate=Screen('FrameRate',screenNumber);

    % If MacOSX does not know the frame rate the 'FrameRate' will return 0.
    % That usually means we run on a flat panel with 60 Hz fixed refresh
    % rate:
    if frameRate == 0
        frameRate=60;
    end

    movieDurationFrames=round(movieDurationSecs * frameRate / waitframes);
	movieFrameIndices=mod(0:(movieDurationFrames-1), numFrames) + 1;
	priorityLevel=MaxPriority(w);
	Priority(priorityLevel);
    
    Screen('TextSize', w, 24);
    Screen('DrawText', w, 'Measuring monitor refresh interval... This can take up to 20 seconds...', 10, 10, 255);
    Screen('Flip', w);
    
    % NEW: Perform extra calibration pass to estimate monitor refresh
    % interval. We want at least 100 valid samples, requiring standard
    % deviation of the measurements below 50 microseconds, but timing out
    % after 20 seconds if we can't get that level of accuracy:
    [ ifi nvalid stddev ]= Screen('GetFlipInterval', w, 100, 0.00005, 20);
    fprintf('Measured refresh interval, as reported by "GetFlipInterval" is %2.5f ms. (nsamples = %i, stddev = %2.5f ms)\n', ifi*1000, nvalid, stddev*1000);

    % Perform initial Flip to sync us to the VBL and for getting an initial
    % VBL-Timestamp for our "WaitBlanking" emulation:
    vbl=Screen('Flip', w);
    
    for i=1:movieDurationFrames
        % Draw image:
        Screen('DrawTexture', w, tex(movieFrameIndices(i)));
        
        % NEW: We only flip every 'waitframes' monitor refresh intervals:
        % For this, we calculate a point in time after which Flip should flip
        % at the next possible VBL.
        % This should happen waitframes * ifi seconds after the last flip
        % has happened (=vbl). ifi is the monitor refresh interval
        % duration. We subtract 0.5 frame durations, so we have some
        % headroom to take possible timing jitter or roundoff-errors into
        % account.
        % This is basically the old Screen('WaitBlanking', w, waitframes)
        % as known from the old PTB...
        vbl = Screen('Flip', w, vbl + (waitframes - 0.5) * ifi);
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
