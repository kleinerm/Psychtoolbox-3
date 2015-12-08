function ClutAnimDemo(method)
% ClutAnimDemo([method=2])
%
% Display an animated grating using CLUT animation via the
% Screen('LoadNormalizedGammaTable') command, or via the PsychImaging()
% based clut animation support.
%
% Clut animation is an ancient technique of achieving animation, only
% needed or the best choice for very few use cases nowadays. Think twice
% before using this as your method of choice. It may work, but is inefficient,
% potentially unreliable in the timing domain (except for method 2) and most
% often more painful and inflexible to use than a proper modern approach.
%
% If 'method' is set to 0, hardware gamma tables are immediately updated.
% A setting of 1 will update hardware gamma tables in sync with Screen('Flip'),
% or more accurately, it will try to. Synchronization can't be guaranteed,
% only that a good effort is made to achieve sync.
% A 'method' setting of 2, which is the default, will use the Psychtoolbox image
% processing pipeline to implement clut animation, instead of updating the
% hardware gamma tables. This is the only reliable method with respect to
% timing precision. It is also the only method that works on MS-Windows.
% However, it requires recent graphics hardware and a bit more computation
% time.
%
% Method 2 is recommended for most use cases, method 0 is the least
% reliable one.
%
% see also: PsychDemos, PsychImaging
%

% HISTORY
%  7/05/05    mk    Wrote it.
%  22/07/05   fwc   Added SkipSyncTests preference call, slightly smaller texture,
%                   as drawtexture failed on s=400 on a 1024x768 pix screen
%                   in catch section, test if OrigLut exists before
%                   applying it.
%
%  4/4/11     mk    Add support for 'EnableCLUTMapping' method of
%                   PsychImaging.
%  22/07/14   mk    Fixup for gpu's with > 256 gamma table slots, in which this
%                   didn't work, as it assumed exactly 256 slots.
%  22/11/15   mk    Switch to method 2 (imaging pipeline) by default.

if nargin < 1 || isempty(method)
    method = 2;
end

% Is this the M$-Windows version? This demo doesn't work under Windows...
if (method ~= 2) && IsWin
    error('ClutAnimDemo does not work under M$-Windows with any method but 2. Aborting...');
end

% Check for proper installation of PTB-3, setup default (0 == like AssertOpenGL):
PsychDefaultSetup(0);

try
    % We disable the sync tests at startup. They are not necessary for this
    % demo...
    Screen('Preference', 'SkipSyncTests', 1);

    % Get the list of screens and choose the one with the highest screen number.
    % Screen 0 is, by definition, the display with the menu bar. Often when
    % two monitors are connected the one without the menu bar is used as
    % the stimulus display.  Chosing the display with the highest dislay number is
    % a best guess about where you want the stimulus displayed.
    screens=Screen('Screens');
    screenNumber=max(screens);

    % Make a backup copy of original LUT into origLUT.
    origLUT=Screen('ReadNormalizedGammaTable', screenNumber);

    % origLUT must have exactly 256 slots for this demo to work,
    % so make it so! May look a bit weird on gpu with gamma tables
    % that have more than 256 slots, but such is life...
    if size(origLUT, 1) ~= 256
        origLUT = origLUT(1:256, :);
    end

    % Open a double buffered fullscreen window.
    if method == 2
        % Use imaging pipeline for good results:
        PsychImaging('PrepareConfiguration');
        PsychImaging('AddTask', 'AllViews', 'EnableCLUTMapping');
        w = PsychImaging('OpenWindow', screenNumber, 0);
    else
        % Use old style gamma table animation:
        w=Screen('OpenWindow', screenNumber, 0);
    end

    LoadIdentityClut(w);

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
    x = meshgrid(-s:s, -s:s);
    fintex=ones(2*s+1,2*s+1);
    fintex(:,:)=mod(x,255)+1;
    tex=Screen('MakeTexture', w, fintex);

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

        if method == 0
            % This 'Flip' waits for vertical retrace...
            Screen('Flip', w, 0, 2);
        end

        % Update the hardware CLUT with our newLUT:
        Screen('LoadNormalizedGammaTable', w, newLUT, method);

        if method > 0
            % This 'Flip' waits for vertical retrace...
            Screen('Flip', w, 0, 2);
        end

        t1=GetSecs;
        tavg=tavg+(t1-t0);
        t0=t1;
        i=i+1;

        % Abort after 1000 video refresh intervals or on a key-press:
        if KbCheck || (i>1000)
            break;
        end
    end

    tavg = tavg / i %#ok<NOPRT,NASGU>

    %The same commands wich close onscreen and offscreen windows also close
    %textures.

    % Restore the original origLUT CLUT gamma lookup table:
    RestoreCluts;
    Screen('CloseAll');
    Screen('Preference', 'SkipSyncTests', 0);

catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.

    RestoreCluts;
    Screen('CloseAll');
    Screen('Preference', 'SkipSyncTests', 0);
    psychrethrow(psychlasterror);
end %try..catch..
