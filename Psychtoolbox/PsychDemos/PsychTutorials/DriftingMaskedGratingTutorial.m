function DriftingMaskedGratingTutorial(angle, cyclespersecond, f, drawmask)
% function DriftDemo2(angle, cyclespersecond, f, drawmask)
% ___________________________________________________________________
%
% Display an animated grating using the new Screen('DrawTexture') command.
% In the OS X Psychtoolbox Screen('DrawTexture') replaces
% Screen('CopyWindow'). The demo will stop after roughly 20 seconds have
% passed or after the user hits a key.
%
% This demo illustrates how to draw an animated grating online by use of
% only one grating texture. We create one texture with a static sine
% grating. In each successive frame we only draw a rectangular subregion of
% the sine-texture onto the screen, basically showing the texture through
% an aperture. The subregion - and therefore our "aperture" is shifted each
% frame, so we create the impression of a moving grating.
%
% The demo also shows how to use alpha-blending for masking the grating
% with a gaussian transparency mask (a texture with transparency layer).
%
% And finally, we demonstrate rotated drawing, as well as how to emulate
% the old OS-9 'WaitBlanking' command with the new 'Flip' command.
%
% Parameters:
%
% angle = Angle of the grating with respect to the vertical direction.
% cyclespersecond = Speed of grating in cycles per second.
% f = Frequency of grating in cycles per pixel.
% drawmask = If set to 1, then a gaussian aperture is drawn over the grating
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
% see also: PsychDemos, MovieDemo

% HISTORY
%  6/7/05    mk     Adapted from Allen Ingling's DriftDemoOSX.m 

if nargin<4
    % By default, we mask the grating by a gaussian transparency mask:
    drawmask=1;
end;

if nargin<3
    % Grating cycles/pixel
    f=0.05;
end;

if nargin<2
    % Speed of grating in cycles per second:
    cyclespersecond=1;
end;

if nargin<1
    % Angle of the grating: We default to 30 degrees.
    angle=30;
end;

movieDurationSecs=20;   % Abort demo after 20 seconds.
texsize=100;            % Half-Size of the grating image.

% Screen('Preference', 'SkipSyncTests', 1);

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
	[w screenRect]=Screen('OpenWindow',screenNumber, gray);

    % Enable alpha-blending:
    Screen('BlendFunction', w, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    % Show initial gray screen:
    Screen('Flip', w);

    % Calculate parameters of the grating:
    p=ceil(1/f);  % pixels/cycle    
    fr=f*2*pi;
    visiblesize=2*texsize+1;

    % Create one single static grating image:
    [x,y]=meshgrid(-2*texsize:2*texsize + p, -texsize:texsize);
    grating=gray + inc*cos(fr*x);

    % Store grating in texture:
    gratingtex=Screen('MakeTexture', w, grating);

    % Create a single gaussian transparency mask and store it to a texture:
    mask=ones(2*texsize+1, 2*texsize+1, 2) * gray;
    [x,y]=meshgrid(-1*texsize:1*texsize,-1*texsize:1*texsize);
    mask(:, :, 2)=white * (1 - exp(-((x/45).^2)-((y/45).^2)));
    masktex=Screen('MakeTexture', w, mask);
    
    % Query duration of monitor refresh interval:
    ifi=Screen('GetFlipInterval', w);
    
    waitframes = 1;
    waitduration = waitframes * ifi;
    
    % Translate requested speed of the grating (in cycles per second)
    % into a shift value in "pixels per frame", assuming given
    % waitduration: This is the amount of pixels to shift our "aperture" at
    % each redraw:
    shiftperframe= cyclespersecond * p * waitduration;
    
    Screen('TextSize', w, 24);
    
    % Slide 1: The static grating texture:
    [xp yp] = DrawFormattedText(w, 'Step 1: Generate a single static texture which shows the periodic grating pattern: The texture must contain at least two full periods of the pattern.\nPress a key to continue.\n\n', 0, 0, 0, 50);
    cmd = 'gratingtex = Screen(''MakeTexture'', w, gratingmatrix);\nScreen(''DrawTexture'', w, gratingtex);';
    [xp yp] = DrawFormattedText(w, cmd, xp, yp, 255);
    Screen('DrawTexture', w, gratingtex);
    Screen('Flip', w);

    GetClicks(w);
    
    % Slide 2: Cut out an aperture from the texture, and render it to a
    % dstRect of same size:

    autodrift = 0;
    xoffset = 0;
    angle = 0;
    i=0;

    while (1)
        [x y buttons] = GetMouse(w);

        if buttons(1) && ~autodrift
            xoffset = x - 100;
            xoffset = max(xoffset, 0);
            xoffset = min(xoffset, 2*texsize);
        end
              
        if buttons(2)
            autodrift = mod(autodrift+1, 3);
            while buttons(2)
                [x y buttons] = GetMouse(w);
            end
        end
        
        if autodrift >= 1
            % Shift the grating by "shiftperframe" pixels per frame:
            xoffset = mod(i*shiftperframe,p);
            i=i+1;
            
            if buttons(1)
                angle = x / RectWidth(screenRect) * 360.0;
            end
        end
                
        if autodrift == 0
            [xp yp] = DrawFormattedText(w, 'Step 2: Cut out a rectangular subregion ''srcRect'' from the texture and draw it to the screen area ''dstRect'' of same size. The texture is shown through an aperture.\nMoving the aperture creates apparent smooth motion. - Try it with the mouse!\nPress a key to continue.\n\n', 0, 0, 0, 50);
        else
            if autodrift >= 1
                [xp yp] = DrawFormattedText(w, 'Step 3: Horizontal shifting of the cut-out aperture region ''srcRect'' creates a nice drifting pattern. Subpixel accurate movements are possible due to hardware bilinear texture interpolation.\nPress a key to continue.\n\n', 0, 0, 0, 50);
                if angle~=0
                    [xp yp] = DrawFormattedText(w, '\nIt is also possible to rotate the ''dstRect'' destination region by some angle to create a rotated grating.\n', 0, yp, 0, 50);
                end
            end
            if autodrift == 2
                [xp yp] = DrawFormattedText(w, '\nOne can mask the ''dstRect'' destination region with a ''transparency mask'' texture by use of alpha-blending.\n', 0, yp, 0, 50);                
            end
        end
        
        cmd = 'Screen(''DrawTexture'', w, gratingtex, ';
        yp=yp+20;
        [xp] = DrawFormattedText(w, cmd, 0, yp, 255);
        cmd = 'srcRect, ';
        [xp] = DrawFormattedText(w, cmd, xp, yp, [255 0 0]);
        cmd = 'dstRect';
        [xp] = DrawFormattedText(w, cmd, xp, yp, [255 255 0]);
        if angle==0
            cmd = ');\n';
        else
            cmd = [', ' num2str(angle) ');\n'];
        end
        [xp yp] = DrawFormattedText(w, cmd, xp, yp, 255);
        
        % Draw static grating texture:
        dstRect = OffsetRect(Screen('Rect', gratingtex), 100, yp + 50);
        Screen('DrawTexture', w, gratingtex, [], dstRect);

        % Define shifted srcRect that cuts out the properly shifted rectangular
        % area from the texture:
        srcRect=[xoffset 0 xoffset + visiblesize visiblesize];
        apertRect = OffsetRect(srcRect, 100, yp + 50);
        Screen('FrameRect', w, [255 0 0], apertRect, 4);
        % Definition of the drawn rectangle on the screen:
        dstRect=[0 0 visiblesize visiblesize];
        dstRect=OffsetRect(dstRect, 600, yp + 150);
        Screen('DrawLine', w, [255 0 0 128], apertRect(1), apertRect(2), dstRect(1), dstRect(2), 4);
        Screen('DrawLine', w, [255 0 0 128], apertRect(3), apertRect(2), dstRect(3), dstRect(2), 4);
        Screen('DrawLine', w, [255 0 0 128], apertRect(1), apertRect(4), dstRect(1), dstRect(4), 4);
        Screen('DrawLine', w, [255 0 0 128], apertRect(3), apertRect(4), dstRect(3), dstRect(4), 4);
        % Draw grating texture:
        Screen('DrawTexture', w, gratingtex, srcRect, dstRect, angle);
        Screen('FrameRect', w, [255 255 0], dstRect, 4);
        
        if autodrift == 2
            maskRect = CenterRectOnPoint(dstRect, x, y);
            xp = 0;
            cmd = sprintf('Screen(''DrawTexture'', w, masktex, [], [%i %i %i %i], %f);\n', maskRect(1), maskRect(2), maskRect(3), maskRect(4), angle);
            [xp] = DrawFormattedText(w, cmd, xp, yp, 255);
            Screen('DrawTexture', w, masktex, [], maskRect, angle);
        end
        
        if KbCheck
            break;
        end
        
        Screen('Flip', w);
    end
        
    while KbCheck; end;
        
    % Perform initial Flip to sync us to the VBL and for getting an initial
    % VBL-Timestamp for our "WaitBlanking" emulation:
    vbl=Screen('Flip', w);

    i=0;
    dstRect = CenterRect(dstRect, screenRect);
    
    % Animationloop:
    while(1)
        % Shift the grating by "shiftperframe" pixels per frame:
        xoffset = mod(i*shiftperframe,p);
        i=i+1;
        
        % Define shifted srcRect that cuts out the properly shifted rectangular
        % area from the texture:
        srcRect=[xoffset 0 xoffset + visiblesize visiblesize];
        
        % Draw grating texture, rotated by "angle":
        Screen('DrawTexture', w, gratingtex, srcRect, dstRect, angle);

        if drawmask==1
            % Draw gaussian mask over grating: We need to subtract 0.5 from
            % the real size to avoid interpolation artifacts that are
            % created by the gfx-hardware due to internal numerical
            % roundoff errors when drawing rotated images:
            Screen('DrawTexture', w, masktex, [0 0 visiblesize visiblesize], dstRect, angle);
        end;

        angle = angle + 0.1;
        
        % Flip 'waitframes' monitor refresh intervals after last redraw.
        vbl = Screen('Flip', w, vbl + (waitframes - 0.5) * ifi);

        % Abort demo if any key is pressed:
        if KbCheck
            break;
        end;
    end;

    %The same commands wich close onscreen and offscreen windows also close
	%textures.
	Screen('CloseAll');

catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end %try..catch..



