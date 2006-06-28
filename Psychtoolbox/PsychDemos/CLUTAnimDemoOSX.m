function CLUTAnimDemoOSX
%
% OS X: ___________________________________________________________________
%
% Display an animated grating using CLUT animation via the
% Screen('LoadNormalizedGammaTable') command.
%
% _________________________________________________________________________
% 
% see also: PsychDemosOSX, MovieDemoOSX

% HISTORY
%  7/05/05    mk     Wrote it. 
%  22/0705  fwc       added SkipSyncTests preference call, slightly smaller texture,
%                   as drawtexture failed on s=400 on a 1024x768 pix screen
%                   in catch section, test if OrigLut exists before
%                   applying it.

% This doesn't work under M$-Windows, as Screen('LoadNormalizedGammaTable') doesn't
% allow us to set the kind of LUTs needed for this demo to work :(

% Are we running OpenGL PTB?
AssertOpenGL;

% Is this the M$-Windows version? This demo doesn't work under Windows...
if IsWin
    error('CLUTAnimDemoOSX does not work under M$-Windows yet, only on MacOS-X. Aborting...');   
    return;
end;

try
    % We disable the sync tests at startup. They are not necessary for this
    % demo...
    Screen('Preference', 'SkipSyncTests', 1);

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
	% Open a double buffered fullscreen window. 
	w=Screen('OpenWindow',screenNumber, 0,[],32,2);
    
    % Make a backup copy of original LUT into origLUT.
    origLUT=Screen('ReadNormalizedGammaTable', screenNumber);

    newLUT=origLUT;
    
	% Find the color value which corresponds to black.  Though on OS
	% X we currently only support true color and thus, for scalar color
	% arguments,
	% black is always 0 and white 255, this rule is not true on other platforms will
	% not remain true on OS X after we add other color depth modes.  
	black=BlackIndex(screenNumber);
	
    % Build a simple gray-level ramp as a single texture.
    [width, height]=Screen('WindowSize', w);
    s=floor(min(width, height)/2)-1;
    [x,y]=meshgrid(-s:s, -s:s);
    fintex=ones(2*s+1,2*s+1);
    fintex(:,:)=mod(x,255)+1;
    tex=Screen('MakeTexture', w, fintex);

    % Switch to realtime-mode for smooth animation:
   	priorityLevel=MaxPriority(w)
	%Priority(priorityLevel)

    % Black background:
    Screen('FillRect',w, black);
    % Single static gray-level ramp drawn as texture.
    Screen('DrawTexture', w, tex(1));
    % Show it by flipping the buffers:
    Screen('Flip', w);
    % Draw same image into backbuffer, so they're identical:
    Screen('DrawTexture', w, tex(1));

    i=0;
    tavg=0;
    t0 = GetSecs;
    
    % newLUT will contain the color lookup table for each frame of the
    % animation.
    % Set up slot 1 (which corresponds to color index zero == black) to a
    % DAC output intensity of 0.5 for all three guns --> color index zero -> gray background.
    newLUT(1,:)=0.5;
    
    % Animation by CLUT color cycling loop:
	while (1)
        % Shift/Cycle all LUT entries: Entry 3 -> 2, 4 -> 3, 5 ->4 , ... ,
        % 256 -> 255, 2 -> 256, ... we just leave slot 1 alone, it defines
        % the DAC output values for the background.
        backupLUT=newLUT(2, :);
        newLUT(2:255, :)=newLUT(3:256, :);
        newLUT(256, :)=backupLUT;
        % This 'Flip' waits for vertical retrace...
        Screen('Flip', w, 0, 2);
        % Update the hardware CLUT with our newLUT:
        Screen('LoadNormalizedGammaTable', screenNumber, newLUT);
        t1=GetSecs;
        tavg=tavg+(t1-t0);
        t0=t1;
        i=i+1;
        
        % Abort after 1000 video refresh intervals or on a key-press:
        if KbCheck | (i>1000)
            break;
        end;
    end;

    tavg=tavg / i
    
	%The same commands wich close onscreen and offscreen windows also close
	%textures.
	Priority(0);
    % Restore the original origLUT CLUT lookup table as defined by Apple's
    % ColorSync - mechanism:
    Screen('LoadNormalizedGammaTable', screenNumber, origLUT);
	Screen('CloseAll');
    Screen('Preference', 'SkipSyncTests', 0);

catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Priority(0);
    if exist('origLUT', 'var')
        Screen('LoadNormalizedGammaTable', screenNumber, origLUT);
    end
    Screen('CloseAll');
    Screen('Preference', 'SkipSyncTests', 0);
    psychrethrow(lasterror);
end %try..catch..
