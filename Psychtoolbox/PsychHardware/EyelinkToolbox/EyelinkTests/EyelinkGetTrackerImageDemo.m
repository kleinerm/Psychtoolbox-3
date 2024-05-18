function EyelinkGetTrackerImageDemo(sndinit)
% EyelinkGetTrackerImageDemo([sndinit = 0])
% This shows you how to register a callback m-file that can display the
% tracker's eye image in PTB.
%
% sndinit = 0: No sound init / legacy behavior.
%           1: EyelinkUpdateDefaults for default init.
%           2: Pass in our own PsychPortAudio sound handle for optimal method.
%

if nargin < 1 || isempty(sndinit)
    sndinit = 0;
end

PsychDefaultSetup(1);

try
    % Disable key output to Matlab window:
    ListenChar(-1);

    % Open a window for eye image display:
    w = Screen('OpenWindow', max(Screen('Screens')), [255 255 0], [0 0 800 600]);

    % Initialize 'el' eyelink struct with proper defaults for output to
    % window 'w':
    el = EyelinkInitDefaults(w);

    % Initialize Eyelink connection (real or dummy). The flag '1' requests
    % use of callback function and eye camera image display:
    if ~EyelinkInit([], 1)
        fprintf('Eyelink Init aborted.\n');
        cleanup;
        return;
    end

    if sndinit == 2
        el.ppa_pahandle = PsychPortAudio('Open', [], 1, 0, [], 2);
    end

    % This would display additional debug output, if commented in:
    %    Eyelink('Verbosity',10);

    if sndinit > 0
        EyelinkUpdateDefaults(el);
    end

    % Run synthetic test suite for the fun of it, if commented in:
    Eyelink('TestSuite');

    % Perform tracker setup - interactive setup with video display:
    result = EyelinkDoTrackerSetup(el);

    % Perform drift correction: The special flags 1,1 request interactive
    % correction with video display: You have to hit esc before return.
    result = EyelinkDoDriftCorrection(el, 30, 30, 1, 1);

    % Done.
catch
    % In case of error, be tidy:
    cleanup;
end

% Shutdown everything at regular end:
cleanup;

if sndinit == 2
    PsychPortAudio('Close', el.ppa_pahandle);
end

end

% Cleanup routine:
function cleanup
    % Shutdown Eyelink:
    Eyelink('Shutdown');

    % Close window:
    sca;

    % Restore keyboard output to Matlab:
    ListenChar(0);
end
