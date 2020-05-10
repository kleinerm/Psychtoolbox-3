function sca
% sca -- Execute Screen('CloseAll');
% This is just a convenience wrapper that allows you to save typing the long,
% and frequently needed, command.
%
% It also unhides the cursor if hidden, restores graphics card gamma tables if
% they've been altered, and restores other low-level display system state, like
% centering the mouse cursor on screen 0 of a multi-X-Screen Linux setup. For
% Matlab it implements a workaround for Matlab Java GUI bugs related to resolution
% switching and entering/leaving fullscreen display mode.
%

% History:
% 4/6/6   Written (MK).
% 5/31/8  Add CLUT restore call (MK).
% 7/03/13 Add PsychJavaSwingCleanup call (MK).
% 5/13/16 Add mouse pointer repositioning (MK).
% 1/6/20  Add X-Screen size restore (MK).

% screenRestoreCmd is defined by PsychImaging() in certain situations:
global screenRestoreCmd;

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

% Restore X-Screen sizes of not-yet-restored X-Screens:
if IsLinux && ~isempty(screenRestoreCmd)
    for i=1:length(screenRestoreCmd)
        if ~isempty(screenRestoreCmd{i})
            eval(screenRestoreCmd{i}, 'fprintf(''Oopsie!\n'')');
            screenRestoreCmd{i} = [];
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
if IsLinux && length(Screen('Screens')) > 1
    [x,y] = RectCenter(Screen('Rect', 0));
    SetMouse(x,y,0);
end

% Close all windows, release all Screen() ressources:
Screen('CloseAll');

% Restore (possibly altered) gfx-card gamma tables from backup copies:
RestoreCluts;

% Call Java cleanup routine to avoid java.lang.outOfMemory exceptions due
% to the bugs and resource leaks in Matlab's Java based GUI:
if exist('PsychJavaSwingCleanup', 'file')
    PsychJavaSwingCleanup;
end

return;
