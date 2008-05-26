function BitsPlusPlusAdditiveBlendingForLinearSuperpositionTutorial(outputdevice, overlay)
% BitsPlusPlusAdditiveBlendingForLinearSuperpositionTutorial([outputdevice='Mono++'] [, overlay=1]);
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
GammaIncrease = KbName('i');
GammaDecrease = KbName('d');

if nargin < 1 || isempty(outputdevice)
    outputdevice = 'Mono++';
end

if nargin < 2 || isempty(overlay)
    overlay = 1;
end

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
    % 128) background and support for 16- or 32 bpc floating point framebuffers.
    % This window shall drive a Bits++ system in Mono++ or Color++ mode.
    PsychImaging('PrepareConfiguration');
    % Won't need this on NVidia Geforce 8000 or Radeon HD2000 and later: PsychImaging('AddTask', 'General', 'FloatingPoint16Bit');
    PsychImaging('AddTask', 'General', 'FloatingPoint32BitIfPossible');
    switch outputdevice
        case {'Mono++'}
            if overlay
                PsychImaging('AddTask', 'General', 'EnableBits++Mono++OutputWithOverlay');
            else
                PsychImaging('AddTask', 'General', 'EnableBits++Mono++Output');
            end

        case {'Color++'}
            PsychImaging('AddTask', 'General', 'EnableBits++Color++Output');
            overlay = 0;

        case {'Attenuator'}
            PsychImaging('AddTask', 'General', 'EnableGenericHighPrecisionLuminanceOutput', uint8(rand(3, 2048) * 255));
            overlay = 0;

        case {'VideoSwitcher'}
            PsychImaging('AddTask', 'General', 'EnableVideoSwitcherSimpleLuminanceOutput', 128);

            % Switch the device to high precision luminance mode:
            PsychVideoSwitcher('SwitchMode', screenNumber, 1);
            overlay = 0;

        otherwise
            error('Unknown "outputdevice" provided.');
    end
    
    % Choose method of color correction: 'SimpleGamma' is simple gamma
    % correction of monochrome stims via power-law, ie., Lout = Lin ^ gamma.
    PsychImaging('AddTask', 'FinalFormatting', 'DisplayColorCorrection', 'SimpleGamma');

    %PsychImaging('AddTask', 'General', 'InterleavedLineStereo', 0);
    [w, wRect]=PsychImaging('OpenWindow',screenNumber, 0.5);

    if overlay
        % Get overlay window handle:
        wo = BitsPlusPlus('GetOverlayWindow', w);
    else
        wo = 0;
    end
    
    % FIXME: Integrate this into PsychImaging!! Need identity gamma table in gfx card:
    LoadIdentityClut(w);

    %PsychColorCorrection('SetColorClampingRange', w, 0, 1);
    
    % We set initial encoding gamma to correct for a display with a
    % decoding gamma of 2.0 -- A good tradeoff, given most displays are
    % somewhere between 1.8 and 2.2:
    gamma = 1 / 2.0;
    PsychColorCorrection('SetEncodingGamma', w, gamma);
    
    % From here on, all color values should be specified in the range 0.0
    % to 1.0 for displayable luminance values. Values outside that range
    % are allowed as intermediate results, but the final stimulus image
    % should be in range 0-1, otherwise result will be undefined.
    
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
    tex=Screen('MakeTexture', w, m,[],[], 2);
    
    % Show the gray background:
    Screen('Flip', w);

    i=0;
    rotate = 0;
    yd =0;
    show2nd = 1;
    
    if overlay
        Screen('TextSize', wo, 18);
    else
        Screen('TextSize', w, 18);
    end
    
    % Center mouse on stimulus display, then make mouse cursor invisible:
    [cx, cy] = RectCenter(wRect);
    SetMouse(cx, cy, w);
    HideCursor;
    framecount = 0;
    
    tic;
    
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

            % Change of encoding gamma?
            if keycode(GammaIncrease)
                gamma = min(gamma+0.001, 1.0);
                PsychColorCorrection('SetEncodingGamma', w, gamma);
            end
            
            if keycode(GammaDecrease)
                gamma = max(gamma-0.001, 0.0);
                PsychColorCorrection('SetEncodingGamma', w, gamma);
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
        txt1= 'tex = Screen(''MakeTexture'', win, grating, [], [], 2); % Convert it into a floating point texture.\n';
        txt2= 'Screen(''BlendFunction'', win, GL_SRC_ALPHA, GL_ONE);   % Enable additive alpha-blending.\n\nIn Display loop:\n\n';
        txt3= 'Screen(''DrawTexture'', win, tex, [], [], [], [], 0.5); % Draw static grating at center of screen.\n';
        txt4 = sprintf('Screen(''DrawTexture'', win, tex, [], [%i %i %i %i], %f, [], %f);\n', dstRect(1), dstRect(2), dstRect(3), dstRect(4), i, inc);
        txt5 = sprintf('\nEncoding Gamma is %f --> Correction for a %f gamma display.', gamma, 1/gamma);

        if overlay
            % Need to manually clear the overlay window:
            Screen('FillRect', wo, 0);
            % Need to use text color values in 0-255 range, instead of
            % normalized 0-1 range:
            DrawFormattedText(wo, [txt0 txt1 txt2 txt3 txt4 txt5], 0, 0, 255);
        else
            DrawFormattedText(w, [txt0 txt1 txt2 txt3 txt4 txt5], 0, 0, 1.0);
        end
        
        framecount = framecount + 1;
        Screen('Flip', w);
    end
    
    avgfps = framecount / toc
    
    % We're done: Close all windows and textures:
    Screen('CloseAll');    
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    ShowCursor;
    psychrethrow(psychlasterror);
end %try..catch..

if ~isempty(findstr(outputdevice, 'VideoSwitcher'))
    PsychVideoSwitcher('SwitchMode', screenNumber, 0);
end
