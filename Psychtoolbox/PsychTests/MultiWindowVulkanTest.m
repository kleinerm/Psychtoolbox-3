function MultiWindowVulkanTest(nmax, reverse, screenId)
% MultiWindowVulkanTest([nmax=inf][, reverse=0][, screenId=max])
%
% Test multi-window / multi-display operation in fullscreen exclusive direct
% display mode under Linux/X11 with Vulkan.
%
% This mostly for regression testing. Currently, as of October 2021, requires
% an AMD gpu with amdvlk installed. Or launching multiple instances in separate
% processes if you want to test with non-amdvlk drivers (radv) or other gpu's
% like Intel, Adreno etc.
% The script allowed to diagnose and fix a X-Server bug in RandR output leasing,
% bug fix is part of master, should be part of X-Server 21.1, maybe with a bit
% of luck at some point also of server 1.20. For reference, server master commit:
%
% https://gitlab.freedesktop.org/xorg/xserver/-/commit/f467f85ca1f780d5c7cf3c20888e399708d761ac
%
% Press any key to close all fullscreen windows again, after they've been opened,
% and end the test.
%
% Optional parameters:
%
% 'nmax' number of outputs to test simultaneously.
% 'reverse' Test closing windows in reverse order if set to 1.
% 'screenId' Which screen to test.

% History:
% 16-Oct-2021  mk  Written.

    PsychDefaultSetup(2);

    % We use Linux/X11 specific code below, so...
    if ~IsLinux || IsWayland
        fprintf('This function is only supported on Linux/X11 atm. Bye.\n');
        return;
    end

    if nargin < 1 || isempty(nmax)
        nmax = inf;
    end

    if nargin < 2 || isempty(reverse)
        reverse = 0;
    end

    if nargin < 3 || isempty(screenId)
        screenId = max(Screen('Screens'));
    end

    % At most nmax outputs:
    n = min(nmax, Screen('ConfigureDisplay', 'NumberOutputs', screenId));

    for i = 1:n
        output = Screen('ConfigureDisplay', 'Scanout', screenId, i-1);

        PsychImaging('PrepareConfiguration');
        if i > 1
            % As of Mesa 21, Mesa drivers do not support more than 1 exclusive
            % display per session, so if we want to test more simultaneous displays,
            % we need to enforce use of non-Mesa drivers, e.g., amdvlk, which we
            % trick the system into using by requesting another not-yet-supported
            % by Mesa feature - 10 bpc framebuffers:
            PsychImaging('AddTask', 'General', 'EnableNative10BitFramebuffer');
        end
        PsychImaging('AddTask', 'General', 'UseVulkanDisplay', output.name);
        win(i) = PsychImaging('OpenWindow', screenId, [1 0 0]);
        DrawFormattedText(win(i), sprintf('Window %i : %s', i, output.name), 'center', 'center');
        Screen('Flip', win(i));
    end

    % Give user some time to appreciate the view:
    KbStrokeWait(-1);

    % Can close windows in creation order, or reverse order:
    if reverse
        for i = n:-1:1
            Screen('Close', win(i));
        end
    else
        for i = 1:n
            Screen('Close', win(i));
        end
    end

    % Just to be safe and extra tidy:
    sca;
end
