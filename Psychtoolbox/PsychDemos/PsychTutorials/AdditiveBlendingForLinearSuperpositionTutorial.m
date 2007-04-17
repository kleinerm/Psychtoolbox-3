function AdditiveBlendingForLinearSuperpositionTutorial
% AdditiveBlendingForLinearSuperpositionTutorial
%
% Illustrates use of floating point textures in combination with
% source-weighted additive alpha blending to create linear superpositions
% of image patches, in this case of two superimposed gratings.
%
% Needs hardware with support for imaging pipeline (GLSL shaders and
% floating point framebuffers). Should work well on ATI Radeon X1000 and
% later, Geforce 6000 and later.

% History:
% 16.04.2007 Written (MK).

KbName('UnifyKeyNames');
UpArrow = KbName('UpArrow');
DownArrow = KbName('DownArrow');
LeftArrow = KbName('LeftArrow');
RightArrow = KbName('RightArrow');
esc = KbName('ESCAPE');
space = KbName('space');

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
	screenNumber=max(Screen('Screens'));

    % Open a double-buffered fullscreen window with a gray (intensity =
    % 128) background and support for 16 bpc floating point framebuffers:
	w=Screen('OpenWindow',screenNumber, 128,[],[],[],[],[],kPsychNeed16BPCFloat);
    [width, height]=Screen('WindowSize', w);

    % Enable alpha blending. We switch it into additive mode which takes
    % source alpha into account:
    Screen('BlendFunction', w, GL_SRC_ALPHA, GL_ONE);

	inc=0.5;
	
	% Compute one frame of a static grating: It has a total size of third
	% the screen size:
    s=min(width, height) / 6;
	[x,y]=meshgrid(-s:s-1, -s:s-1);
	angle=30*pi/180; % 30 deg orientation.
	f=0.01*2*pi; % cycles/pixel
    a=cos(angle)*f;
	b=sin(angle)*f;
                
    % Build grating texture:
    m=sin(a*x+b*y);
    tex=Screen('MakeTexture', w, m,[],[], 1);
    
    % Show the gray background:
    Screen('Flip', w);

    i=0;
    rotate = 0;
    yd =0;
    show2nd = 1;
    
    Screen('TextSize', w, 18);
    HideCursor;
    
    % Animation loop:
    while 1
        Screen('DrawTexture', w, tex, [], [], [], [], 0.5);
        i=i+rotate;
        [x,y,buttons]=GetMouse(w);
        if any(buttons)
            if buttons(1)
                i=i+0.1;
            end
            if buttons(2)
                i=i-0.1;
            end
        end

        if show2nd
            dstRect=CenterRectOnPoint(Screen('Rect', tex), x, y+yd);
            Screen('DrawTexture', w, tex, [], dstRect, i, [], inc);
        end
        
        [d1 d2 keycode]=KbCheck;
        if d1
            if keycode(UpArrow)
                yd=yd-0.1;
            end

            if keycode(DownArrow)
                yd=yd+0.1;
            end

            if keycode(LeftArrow) && inc >= 0.001
                inc=inc-0.001;
            end

            if keycode(RightArrow) && inc <= 0.999
                inc=inc+0.001;
            end

            if keycode(space);
                show2nd = 1-show2nd;
                while KbCheck; end;
                if show2nd
                    HideCursor;
                else
                    ShowCursor;
                end
            end

            if keycode(esc);
                break;
            end
        end

        txt0= 'At startup:\ngrating = sin(f*cos(angle)*x + f*sin(angle)*y);         % Compute luminance grating matrix in Matlab.\n';
        txt1= 'tex = Screen(''MakeTexture'', win, grating, [], [], 1); % Convert it into a floating point texture.\n';
        txt2= 'Screen(''BlendFunction'', win, GL_SRC_ALPHA, GL_ONE);   % Enable additive alpha-blending.\n\nIn Display loop:\n\n';
        txt3= 'Screen(''DrawTexture'', win, tex, [], [], [], [], 0.5); % Draw static grating at center of screen.\n';
        txt4 = sprintf('Screen(''DrawTexture'', win, tex, [], [%i %i %i %i], %f, [], %f);', dstRect(1), dstRect(2), dstRect(3), dstRect(4), i, inc);
        DrawFormattedText(w, [txt0 txt1 txt2 txt3 txt4], 0, 0, 255);
        
        Screen('Flip', w);
    end
    
    % We're done: Close all windows and textures:
    Screen('CloseAll');    
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    ShowCursor;
    psychrethrow(psychlasterror);
end %try..catch..
