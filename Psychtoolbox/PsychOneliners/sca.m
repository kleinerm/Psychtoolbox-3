function sca
% sca -- Execute Screen('CloseAll');
% This is just a convenience wrapper that allows you
% to save typing that long, and frequently needed,  command.
% It also unhides the cursor if hidden, and restores graphics card gamma
% tables if they've been altered.
%

% History:
% 4/6/6   Written (MK).
% 5/31/8  Add CLUT restore call (MK).
% 7/03/13 Add PsychJavaSwingCleanup call (MK).
% 5/13/16 Add mouse pointer repositioning (MK).

% Unhide the cursor if it was hidden:
% Don't do this on Wayland for now...
if ~IsWayland
  ShowCursor;
end

for win = Screen('Windows')
    if Screen('WindowKind', win) == 1
        if Screen('GetWindowInfo', win, 4) > 0
            Screen('AsyncFlipEnd', win);
        end
    end
end

% Reposition mouse cursor to center of screen 0 on multi-screen
% setups. This as a workaround for Linux KDE-5 Plasma in Ubuntu 16.04-LTS,
% which has a weird bug when run on multi X-Screen setups: You can move
% the mouse cursor off X-Screen 0 onto a secondary X-Screen, but sometimes
% you can not move it back to X-Screen 0 where the regular GUI resides.
% Probably some buggy use of pointer barriers which did not take multiple
% X-Screens into account?
% Anyway, SetMouse works, so we reposition the cursor to X-Screen 0 at
% the end of a session:
if length(Screen('Screens')) > 1
    [x,y] = RectCenter(Screen('Rect', 0));
    SetMouse(x,y,0);
end

% Close all windows, release all Screen() ressources:
Screen('CloseAll');

% Restore (possibly altered) gfx-card gamma tables from backup copies:
RestoreCluts;

% Call Java cleanup routine to avoid java.lang.outOfMemory exceptions due
% to the bugs and resource leaks in Matlab's Java based GUI:
if ~IsOctave && exist('PsychJavaSwingCleanup', 'file')
    PsychJavaSwingCleanup;
end

return;
