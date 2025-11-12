function EyelinkCleanupHelper
    % Restore character output to command window, disable GetChar et al.:
    ListenChar(0);

    % PTB's wrapper for Screen('CloseAll') & related cleanup, e.g. ShowCursor:
    sca;

    % Close EyeLink connection
    Eyelink('Shutdown');

    if ~IsOctave
        % Bring Matlab command window to front:
        commandwindow;
    end
end
