function StereoDemo(stereomode)
% Demo on how to use Psychtoolbox for OS-X to drive stereo-displays
% that are supported by MacOS-X, e.g., the CrystalEyes products...
%
% Press any key to abort demo.
%
% Draws a rectangle into view for left-eye and a displaced rectangle
% into the view for the right-eye, thereby creating some disparity
% between both eyes.
%
% This is a quick & dirty hack that shows how to use PTB for this,
% but is by no means correct with respect to binocular vision.
%
% Use this as a starting point... ;-)
%
% Author: Mario Kleiner mario.kleiner at tuebingen.mpg.de
%

% Stereomode defaults to 'OpenGL - Stereo'...
if nargin<1
    stereomode=1
end;

try
	% This script calls Psychtoolbox commands available only in OpenGL-based 
	% versions of the Psychtoolbox. (So far, the OS X Psychtoolbox is the
	% only OpenGL-base Psychtoolbox.)  The Psychtoolbox command AssertPsychOpenGL will issue
	% an error message if someone tries to execute this script on a computer without
	% an OpenGL Psychtoolbox
	AssertOpenGL;
	
	% Get the list of Screens and choose the one with the highest screen number.
	% Screen 0 is, by definition, the display with the menu bar. Often when 
	% two monitors are connected the one without the menu bar is used as 
	% the stimulus display.  Chosing the display with the highest dislay number is 
	% a best guess about where you want the stimulus displayed.  
	screens=Screen('Screens');
	screenNumber=max(screens);
    screensize=Screen('Rect', screenNumber);

    white=WhiteIndex(screenNumber);
	black=BlackIndex(screenNumber);
	gray=(white+black)/2;
	if round(gray)==white
		gray=black;
    end

    % Query size of screen:
    screenwidth=screensize(3)
    screenheight=screensize(4)

    % Open a double-buffered stereoscopic full-screen display window:
    % Number of buffers must be set == 2 for selecting this mode.
    w=Screen('OpenWindow',screenNumber, 0,[],32, 2, stereomode);

    % Clear image buffers and select clear color as "black":
    Screen('FillRect', w, 0);
    
    % Initial double-buffer flip for synching to vertical retrace:
    Screen('Flip',w,0,0);
    Screen('Flip',w,0,0);
    
    n=1000
    
    Priority(9)
    
    tb=GetSecs();
    f=300;
    j=200;
    while KbCheck == 0
        for i=1:n
            d=f * sin(i / 100);
            s=1.1 + 0.2*sin(i/100);
            % Paint image for left eye:
            % Select image buffer for left eye view:
            Screen('SelectStereoDrawBuffer', w, 0);
            Screen('FillRect', w, [255 255 255], [j+20 j+20 j+20+s*400 j+20+s*400]);
            % Paint image for right eye:
            % Select image buffer for right eye view:
            Screen('SelectStereoDrawBuffer', w, 1);
            Screen('FillRect', w, [255 255 255], [j+d+20 j+20 j+d+20+s*400 j+20+s*400]);

            % Done with painting both images. Show them to subject on next
            % vertical retrace:
            Screen('Flip', w, 0, 0);
            
            if (KbCheck)
                break;
            end;
        end;
    end;

    finalprio = Priority(0)

    Screen('CloseAll');
    
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    finalprio = Priority(0)
    Screen('CloseAll');
    rethrow(lasterror);
end %try..catch..

return
