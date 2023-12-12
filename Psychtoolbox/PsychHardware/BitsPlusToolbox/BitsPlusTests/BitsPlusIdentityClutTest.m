function BitsPlusIdentityClutTest(whichScreen, dpixx, winrect, useclutmodeonly, useVulkan)
% Test signal transmission from the framebuffer to your CRS Bits+/Bits#
% device or VPixx Inc. DataPixx/ViewPixx/Propixx device and similar CRS and
% VPixx products.
%
% Test proper function of the T-Lock mechanism on CRS devices and PSYNC
% mechanism on VPixx devices, as well as proper loading of identity gamma
% tables into the GPU, and for bad interference of dithering hardware
% with the DVI stream. This test is meant for Mono++ mode of Bits+/Bits# or
% M16 mode of DataPixx, ViewPixx, ProPixx. It requires a modern graphics card
% which supports at least OpenGL-2.1 and therefore supports these modes.
%
% If you only have older graphics hardware that can only drive Bits++ or
% L48 clut display mode, or you have an old CRS Bits+ device switched to
% Bits++ mode and don't want the hassle of setting it to Mono++ mode then
% this test script can also perform a slightly more limited test in Bits++
% mode or L48 clut mode. Set the optional parameter 'useclutmodeonly' to 1
% to use this simpler test mode.
%
% Disclaimer: This test only exists because of the huge number of serious &
% embarassing bugs in the graphics drivers and operating systems from
% Microsoft, Apple, AMD and NVidia. All problems diagnosed here are neither
% defects in the Bits+ device or similar devices, nor Psychtoolbox bugs. As
% such, sadly they are mostly out of our control and there is only a
% limited number of ways we can try to help you to workaround the problems
% caused by miserable workmanship and insufficient quality control at those
% big companies.
%
% Usage:
%
% BitsPlusIdentityClutTest([whichScreen=max][, usedpixx=0][, winrect=[]][, useclutmodeonly=0][, useVulkan=0]);
%
% How to test:
%
% 1. Make sure your Bits+ box is switched to Mono++ mode by uploading the
%    proper firmware. Or make sure your DataPixx/ViewPixx/ProPixx or CRS
%    Bits# is connected, both DVI cable and USB cable. Alternatively set
%    'useclutmodeonly' to 1 to test in Bits++ mode on a CRS device or L48
%    mode on a VPixx device. In useclutmodeonly = 1 case, skip to step 3.
%
% 2. Run the BitsPlusImagingPipelineTest script to validate that your
%    graphics card can create properly formatted images in the framebuffer
%    for Bits+/Bits# or DataPixx/ViewPixx/ProPixx.
%
% 3. Run this script, optionally passing a screenid. It will test the
%    secondary display on a multi-display setup by default, or the external
%    display on a laptop. For a DataPixx device or similar, set the
%    optional 'usedpixx' flag to 1. Set 'useclutmodeonly' flag to 1 if you
%    want to test in Bits++ or L48 mode instead of Mono++ or M16 mode.
%
% 'useVulkan' Defaults to 0. If set to 1, use Vulkan display
% backend, instead of standard OpenGL backend.
%
% If everything works, what you see onscreen should match the description
% in the blue text that is displayed.
%
% If a wrong gamma lut is uploaded into your GPU due to operating system
% and graphics driver bugs, you can try to toggle through different
% alternative lut's by repeatedly pressing the SPACE key and seeing if the
% display changes for the better. Should you find a setting that works, you
% can press the 's' key to save this configuration and Psychtoolbox will
% use this setting in all future sessions of your experiment scripts.
%
% You can exit the test by pressing the ESCape key, regardless if it was
% successfull or not. The test will then ask you if you rate the results
% as success or failure and use your feedback for further operation.
%
% What could also happen is that you get a partial success: The display
% behaves roughly as described in the text, but you see the T-Lock color
% code line at the top of your display erratically appearing and
% disappearing - flickering. You also don't see a smooth animation of the
% drifting horizontal red gradient or a regular cycling of the "COLORFUL"
% words, but some jerky, irregular, jumpy animation, which may only update
% a few times per second, or even only once every couple seconds or at very
% irregular intervals. You may also see the display overlayed with random
% colorful speckles, or you may not see the gray background image and gray
% rotating gradient patch at all. In that case, the lut uploaded in your
% GPU may be correct, but due to some serious graphics driver or operating
% system bug, the GPU is applying spatial or temporal dithering to the
% video stream which will confuse your display device and cause random
% artifacts and failure of the T-Lock or PSYNC mechanism, as well as display
% of wrong or inaccurate color or luminance values!
%
% You should also double-check the cabling and connections to rule out
% connection problems and other defects.
%

% History:
% 09/20/09    mk  Written.
% 03/xx/2012  mk  Major updates for Bits# and other polishing.
% 06/25/2016  mk  Store validation info if test succeeds.
%                 Implement more limited test mode for Bits++ clut only mode
%                 to allow testing with older graphics cards.
% 01-Sep-2020  mk  Add support for Vulkan.

% Select screen for test/display:
if nargin < 1 || isempty(whichScreen)
    whichScreen = max(Screen('Screens'));
end

if nargin < 2 || isempty(dpixx)
    dpixx = 0;
end

if nargin < 3
    winrect = [];
end

if nargin < 4 || isempty(useclutmodeonly)
    useclutmodeonly = 0;
end

if nargin < 5 || isempty(useVulkan)
    useVulkan = 0;
end

% Enable text anti-aliasing for this test. This does not make sense for the
% purpose of actual anti-aliasing, as the TextAlphaBlending call below
% defeats this. Instead we do it, because disabling text anti-aliasing
% triggers OpenGL graphics driver bugs in the AMD drivers for Microsoft
% Windows, as of November 2023 with at least AMD driver version 23.11.1,
% specifically glTexImage2D()'s support for GL_BITMAP textures seems
% broken:
oldAntialias = Screen('Preference', 'TextAntiAliasing', 1);

% Disable text alpha-blending to avoid color weirdness in the color overlay
% text due to off-by-one color index values indexing into the wrong clut slot:
oldTextAlpha = Screen('Preference', 'TextAlphaBlending', 1);

try
    % Setup imaging pipeline:
    PsychImaging('PrepareConfiguration');

    if useVulkan
        PsychImaging('AddTask', 'General', 'UseVulkanDisplay');
    end

    if ~useclutmodeonly
        % Require a 32 bpc float framebuffer: This would be the default anyway, but
        % just here to be explicit about it:
        PsychImaging('AddTask', 'General', 'FloatingPoint32Bit');

        % Make sure we run with our default color correction mode for this test:
        % 'ClampOnly' is the default, but we set it here explicitely, so no state
        % from previously running scripts can bleed through:
        PsychImaging('AddTask', 'FinalFormatting', 'DisplayColorCorrection', 'ClampOnly');
    end

    if dpixx
        if ~useclutmodeonly
            % Use M16 mode with overlay:
            PsychImaging('AddTask', 'General', 'EnableDataPixxM16OutputWithOverlay');
        else
            % Use L48 clut mode:
            PsychImaging('AddTask', 'General', 'EnableDataPixxL48Output');
        end

        % Reduce timeout for recognition of PSYNC code to about 1 second on
        % a 100 Hz display:
        oldpsynctimeout = PsychDataPixx('PsyncTimeoutFrames', 100);
    else
        if ~useclutmodeonly
            % Use Mono++ mode with overlay:
            PsychImaging('AddTask', 'General', 'EnableBits++Mono++OutputWithOverlay');
        else
            % Use Bits++ clut mode:
            PsychImaging('AddTask', 'General', 'EnableBits++Bits++Output');
        end
    end

    % DataPixx or Bits# used? They allow advanced diagnostics. This does not yet
    % work with the Vulkan display backend though, as the encoder tests would run
    % with only a halfway initialized Vulkan display backend, which would cause
    % the test stims to not get displayed on the video output and thereby test failure.
    if (dpixx || BitsPlusPlus('OpenBits#')) && ~useVulkan
        fprintf('\n\nYou can run extended diagnostics and fixes if you answer the following question\n');
        fprintf('with yes. However, we recommend first running this script once, answering no. Then\n');
        fprintf('if that at least somewhat succeeds, save the closest to good result via ''s'' key,\n');
        fprintf('then rerun the script and answer yes, to either fix remaining errors and glitches,\n');
        fprintf('or verify perfect function of your setup. If you don''t do it in this order, the\n');
        fprintf('test may hang on some setups.\n\n');
        answer = input('Run DataPixx/Bits# based diagnostics as well [Time consuming]? [y/n] ', 's');
        if answer == 'y'
            % Enable one-shot diagnostic of GPU encoders via Data/View/ProPixx or Bits# :
            BitsPlusPlus('TestGPUEncoders');
        end
    end

    % Force skip of validation check, as otherwise we can not run to actually
    % perform validation of the last stage of the pipeline:
    BitsPlusPlus('ForceUnvalidatedRun');

    % Open the window, assign a gray background color with a 50% intensity gray:
    [win, screenRect] = PsychImaging('OpenWindow', whichScreen, 0.5, winrect);

    % Get handle to overlay:
    if ~useclutmodeonly
        overlaywin = PsychImaging('GetOverlayWindow', win);
    else
        overlaywin = win;
        % Use black background color index for clut palette index 0:
        Screen('FillRect', win, 0);
        Screen('Flip', win);
    end

    HideCursor;

    % At this point we should have a fullscreen onscreen window, with automatic
    % output for Bits+ Mono++ mode, with a overlay whose color is controlled
    % via T-Lock based CLUT's. The whole thing for 32 bit floating point
    % luminance precision, ie., about 23 bits of linear precision -- more than
    % sufficient for the 14 bit DAC output of the Bits+.
    %
    % The color overlay should have a white color ramp loaded.
    % The GPU should have an identity gamma table loaded, so our test images
    % will display without artifacts.
    %
    % Let's create a test stim. This one should display nicely if everything is
    % fined, but should screw up in different ways if something is wrong with
    % the GPU's gamma tables, dithering hardware or other components that
    % affect the creation of the final DVI-Digital drive signals from the
    % framebuffer content. We know that the framebuffer content is fine,
    % because the setup has already passed the BitsPlusImagingPipelineTest
    % script (otherwise the user would be unable to run this testscript at
    % all).

    if ~useclutmodeonly
        % Generate a synthetic grating that covers the whole
        % intensity range from 0 to 16384, mapped to the 0.0 - 1.0 range:
        theImage=zeros(256,256,1);
        theImage(:,:)=reshape(double(linspace(0, 2^16 - 1, 2^16)), 256, 256)' / (2^16 - 1);

        % Build HDR texture:
        hdrtexIndex= Screen('MakeTexture', win, theImage, [], [], 2);
    end

    % Create static image in overlay window:
    Screen('TextSize', overlaywin, 18);
    Screen('TextStyle', overlaywin, 1+2);
    mytext = ['This is what you should see if everything works correctly:\n\n' ...
        'This text should be shown in blue.\n' ...
        'The "COLORFUL" couple of lines below should cycle through different\n' ...
        'rainbow colors at a rate of multiple frames per second.\n\n' ...
        'You should see some smoothly "drifting" red gradient-like pattern.\n' ...
        'Also some rotating gray level gradient test patch if you test in Mono++ or M16 mode.\n' ...
        ' All in front of a gray background.\n\n' ...
        'What you should not see is random speckles of color somewhere on the screen, or a\n' ...
        'jerky animation with only occassional updates.\n\n' ...
        'If what you see is not what you should see, try to cycle through different\n' ...
        'settings by repeatedly pressing the SPACE key on the keyboard.\n' ...
        'If some setting works for you, press "s" key to save the configuration.\n' ...
        'Press the ESCape key to exit from this test.\n\n' ...
        'If everything else fails, read the manual ;-) - Or the help to this test.\n\n'];

    [nx, ny] = DrawFormattedText(overlaywin, mytext, 'center', 30, 255);

    [nx, ny] = DrawFormattedText(overlaywin, 'COLORFUL0\n', 'center', ny, 250);
    [nx, ny] = DrawFormattedText(overlaywin, 'COLORFUL1\n', 'center', ny, 251);
    [nx, ny] = DrawFormattedText(overlaywin, 'COLORFUL2\n', 'center', ny, 252);
    [nx, ny] = DrawFormattedText(overlaywin, 'COLORFUL3\n', 'center', ny, 253);
    [nx, ny] = DrawFormattedText(overlaywin, 'COLORFUL4\n', 'center', ny, 254);

    xpos = round((RectWidth(screenRect) - 150*4) / 2);
    for x=100:249
        Screen('DrawLine', overlaywin, x, xpos, ny + 20, xpos+3, ny+10, 5);
        xpos = xpos + 4;
    end

    % Create LUT for Mono++ overlay:
    ovllut = ones(256, 3);

    % This is the gray background color in Bits+ 'useclutmodeonly' mode:
    ovllut(1,:) = [0.5, 0.5, 0.5];

    % This fills the low 99 slots with random colors. Given that our overlay
    % window is cleared to colorindex zero by default - which means:
    % Transparent for the "underlying" Mono++ luminance image, and given that
    % we don't use color indices 2-100 in our overlay anywhere, this random
    % color assignment should have no perceptible effect whatsoever. If the
    % gamma LUT's of the GPU however contains wrong values or some offset is
    % introduced somwhere on the way from the framebuffer to the DVI-Port, then
    % this will cause some or all of the pixel components in the blue channel
    % of the framebuffer image to be output as non-zero values --> Bits+
    % interprets these non-zero components in the blue channel as overlay
    % pixels and assigns these random colors to the displayed image --> we will
    % see lots of random colorful flickering junk on the display.
    ovllut(2:100,:) = rand(99,3);

    % Build red gradient in slots 101 to 250:
    ovllut(101:250, 1) = linspace(0,1,150)';
    ovllut(101:250, 2:3) = 0;

    % Build 5 different easily distinguishable colors in slots 251:255:
    ovllut(251:255,:) = [[1 0 0]; [0 1 0]; [1 1 0]; [0 0 1]; [1 0 1]];

    % Last slot is blue:
    ovllut(256, :) = [0 , 0, 1];

    angle = 0;
    lutidx = -1;
    bluelutenable = 1;

    KbName('UnifyKeyNames');
    escape = KbName('ESCAPE');
    space = KbName('space');
    key_s = KbName('s');
    key_o = KbName('o');
    key_p = KbName('p');

    KbReleaseWait;

    while 1
        % Load new T-Lock CLUT at next flip:
        Screen('LoadNormalizedGammatable', win, ovllut, 2);

        % Draw rotated luminance patch:
        angle = angle + 1;
        if ~useclutmodeonly
            Screen('DrawTexture', win, hdrtexIndex, [], [], angle/10);
        end

        % Update display:
        Screen('Flip', win, [], useclutmodeonly);

        % Update ovllut for color animations:

        % Red color gradient shifts at each refresh:
        ovllut(101:250, :) = circshift(ovllut(101:250, :), 1);

        % Colored text cycles every 30 frames:
        if mod(angle, 30) == 0
            ovllut(251:255, :) = circshift(ovllut(251:255, :), 1);
        end

        % Color in low slots gets re-randomized:
        ovllut(2:100,:) = rand(99,3);

        [isdown, secs, keyCode] = KbCheck; %#ok<*ASGLU>
        if isdown
            if keyCode(escape)
                break;
            end

            if keyCode(key_p)
                % Pause until another key press:
                KbStrokeWait;
            end

            if keyCode(space)
                % Match 'lutidx' with available indices in LoadIdentityClut!!
                % Currently indices 0 to 5 are available, ie., 6 indices total:
                lutidx = mod(lutidx + 1, 6);
                fprintf('Switching GPU identity CLUT to type %i.\n', lutidx);

                % Upload new corresponding gamma table immediately to GPU:
                LoadIdentityClut(win, 0, lutidx);
                Beeper;
            end

            if keyCode(key_s)
                if lutidx ~= -1
                    fprintf('Storing override GPU identity CLUT type %i in configuration file.\n', lutidx);
                    SaveIdentityClut(win, lutidx);
                    Beeper;
                end
            end

            % Playing with gamma lut's for the overlay, only on
            % non-Windows, as Windows doesn't allow loading arbitrary gpu
            % gamma tables:
            if keyCode(key_o) && ~IsWin
                bluelutenable = 1 - bluelutenable;
                if bluelutenable
                    % Reupload identity gamma table to reenable overlay:
                    if lutidx ~= -1
                        LoadIdentityClut(win, 0, lutidx);
                    else
                        LoadIdentityClut(win);
                    end
                else
                    % Clear blue part of the gamma lut to zero:
                    % This disables the overlay channel, assuming no
                    % dithering or other funky stuff interferes:
                    modlut = Screen('ReadNormalizedGammaTable', win);
                    modlut(:,3) = 0;
                    Screen('LoadNormalizedGammaTable', win, modlut);
                end
            end

            KbReleaseWait;
        end
    end

    % Load identity CLUT into Bits++ to restore proper display:
    BitsPlusPlus('LoadIdentityClut', win);

    % This flip is needed for the 'LoadIdentityClut' to take effect:
    Screen('Flip', win);

    % Release our dedicated "encoder test" connection to Bits#
    BitsPlusPlus('Close');

    Screen('FillRect', overlaywin, 0);
    Screen('FillRect', win, 0);
    Screen('Flip', win);
    Screen('TextSize', win, 40);
    Screen('TextStyle', win, 1+2);
    answer = GetEchoString(win, 'Did it work as expected? [y/n + RETURN]: ', 10, 10,255,0,1,-1);
    if ~isempty(answer) && answer(1) == 'y'
        % Write this configuration to file as the framebuffer -> display device being validated:
        BitsPlusPlus('StoreValidation', win, 2);
        outmsg = sprintf('\n\nSUCCESS: BitsPlusIdentityClutTest verified to work correctly. Validation info stored.\n');
    else
        outmsg = sprintf('\n\nTEST FAILED: You did not answer ''y''es when i asked if it worked correctly, so i am taking this as a no.\n');
    end

    % Done. Close everything down:
    ShowCursor;
    Screen('CloseAll');
    RestoreCluts;
    Screen('Preference', 'TextAntiAliasing', oldAntialias);
    Screen('Preference', 'TextAlphaBlending', oldTextAlpha);

    % Restore psync timeout on Datapixx, if any in use:
    if exist('oldpsynctimeout', 'var')
        PsychDataPixx('PsyncTimeoutFrames', oldpsynctimeout);
    end

    fprintf('%s\n\n', outmsg);

catch %#ok<CTCH>
    sca;
    Screen('Preference', 'TextAntiAliasing', oldAntialias);
    Screen('Preference', 'TextAlphaBlending', oldTextAlpha);

    % Release our dedicated "encoder test" connection to Bits#
    BitsPlusPlus('Close');

    % Restore psync timeout on Datapixx, if any in use:
    if exist('oldpsynctimeout', 'var')
        PsychDataPixx('PsyncTimeoutFrames', oldpsynctimeout);
    end
    psychrethrow(psychlasterror);
end

return;
