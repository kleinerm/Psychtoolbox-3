function BitsPlusCSFDemo(screenid, gamma, method, charttype)
% BitsPlusCSFDemo([screenid=max] [, gamma = 2.2][, method=0][, charttype=0])
%
% This demo utilizes the Psychtoolbox imaging pipeline. Therefore it won't
% work on gfx-hardware older than ATI Radeon X1000 or NVidia Geforce 6000.
%
% Demonstrates advantage of the 14 bpc Mono++ display mode, and similar high
% bit depths display modes over the standard 8 bpc display mode of standard
% graphics hardware. The demo displays either the Campbell-Robson CSF chart
% if you set 'charttype' == 0, or a linear intensity gradient, if you set
% 'charttype' == 1. By default, the CSF chart is shown.
%
% The optional 'method' argument selects among different display output
% modes:
%
% A 'method' of 0 outputs to a regular 8 bit framebuffer. This is the
% default, if no method argument is provided.
%
% A 'method' of 1 tries to utilize the native 10 bpc framebuffers of recent
% AMD/NVidia hardware.
%
% A 'method' of 2 uses a method known as "PseudoGray" or "Bitstealing" for
% output.
%
% In 'method' == 3, the Mono++ display mode of the Bits++ box is used.
%
% A 'method' == 4 uses the Xiangrui Li et al. "VideoSwitcher", video
% attenuator device.
%
% In 'method' == 5, the M16 display mode of the VPixx - DataPixx box is used.
%
% A 'method' of 6 tries to utilize the native ~11 bpc framebuffers of recent
% AMD hardware.
%
% A 'method' of 7 tries to utilize the native up to 16 bpc framebuffers of recent
% AMD hardware.
%
% The optional 'gamma' parameter allows to select the initial gamma value
% of your display to correct for. This can be changed interactively later
% on.
%
% The optional 'screenid' parameter allows to select the id of the output
% display on multi-display setups. By default, the secondary display is
% chosen.
%
% Keyboard control keys:
% ----------------------
%
% At each press of space key, the display alternates between a high bpc
% version and a 8 bpc version to hopefully show a perceptible difference in
% contrast resolution. The ESCape key exits the demo.
%
% The left- and right cursor keys allow you to change the gamma-correction
% setting. The demo starts with standard power-function gamma correction
% for a display with gamma 2.2, i.e., out = in ^ (1/gamma) with gamma =
% 2.2.
%
% This demo is derived from a similar demo (written in C) which is part of
% the sample code collection for Bits++ from Cambridge Research Systems
% support website. It mostly replicates that C sample, however there are
% small differences.
%
% The original description of the CSF chart seems to be in (not checked):
% Campbell, F. W. and Robson, J. G. (1968) Application of Fourier analysis
% to the visibility of gratings. Journal of Physiology (London) 197:
% 551-566.
%
%

% History:
% 16.4.2008  Written - Derived/Converted from CRS sample code (MK).
% 01.11.2008 Extended for pseudogray, videoswitcher and ati output, as well
%            as for display of alternate luminance gradient (MK).
% 14.12.2009 Extended for DataPixx. (MK)
% 26.06.2014 Also show off Native11Bits framebuffers on AMD (MK).
% 17.09.2014 Also show off Native16Bits framebuffers on AMD (MK).

global screenShot;

% Set this to 1 to store a "Screenshot" image of the final converted image
% to the global variable screenShot: For debugging only!
doScreenshot = 0;

% Assure the demo is running under PTB-3:
AssertOpenGL;

% Select screen with highest id for display by default:
if nargin < 1
    screenid =[];
end

if isempty(screenid)
    screenid = max(Screen('Screens'));
end

if nargin < 2
    gamma = [];
end

if isempty(gamma)
    % Start with a encoding gamma for a gamma 2.2 display. Most displays are
    % somewhere around that point:
    gamma = 2.2;
end

if nargin < 3
    method = [];
end

if isempty(method)
    method = 0;
end

if nargin < 4
    charttype = [];
end

if isempty(charttype)
    charttype = 0;
end

% Key mappings, unified across operating systems:
KbName('UnifyKeyNames');
GammaDecrease = KbName('LeftArrow');
GammaIncrease = KbName('RightArrow');
ToggleModes = KbName('space');
Escape = KbName('ESCAPE');
Screen('Preference', 'VisualDebugLevel', 0);

% Define requirements for onscreen window - Setup imaging pipeline:
PsychImaging('PrepareConfiguration');

% Want a full 32 bit floating point precision framebuffer:
% This will provide an effective resolution of 23 bits in the displayable
% luminance range -- Plenty of headroom for the up-to 16 bits output devices.
% Hardware older than NVidia Geforce 8000 or ATI Radeon HD 2000 won't be
% able to do alpha-blending at this mode though. Not an issue here, as we
% don't need alpha-blending...
PsychImaging('AddTask', 'General', 'FloatingPoint32Bit');

if method == 1
    PsychImaging('AddTask', 'General', 'EnableNative10BitFramebuffer');
end

if method == 2
    PsychImaging('AddTask', 'General', 'EnablePseudoGrayOutput');
end

if method == 3
    % Want to use Mono++ mode: The appendix "WithOverlay" enables the color
    % overlay in Mono++ mode, so we can display colorful text:
    PsychImaging('AddTask', 'General', 'EnableBits++Mono++OutputWithOverlay');
end

if method == 4
    PsychImaging('AddTask', 'General', 'EnableVideoSwitcherSimpleLuminanceOutput');
end

if method == 5
    % Want to use M16 mode: The appendix "WithOverlay" enables the color
    % overlay in M16 mode, so we can display colorful text:
    PsychImaging('AddTask', 'General', 'EnableDataPixxM16OutputWithOverlay');
end

if method == 6
    PsychImaging('AddTask', 'General', 'EnableNative11BitFramebuffer');
end

if method == 7
    PsychImaging('AddTask', 'General', 'EnableNative16BitFramebuffer');
end

% Want to have simple power-law gamma correction of stims: We choose the
% method here. After opening the onscreen window, we can set and change
% encoding gamma via PsychColorCorrection() function...
PsychImaging('AddTask', 'FinalFormatting', 'DisplayColorCorrection', 'SimpleGamma');

% Open the onscreen window, get its handle and bounding rectangle. We open
% with a black background ( == 0 ) on display screen 'screenid':
[win, winRect] = PsychImaging('OpenWindow', screenid, 0);

% Get a window handle for the overlay window - This only makes sense for
% Bits++ Mono++ mode and DataPixx M16 mode though:
if method == 3 || method == 5
    overlay = PsychImaging('GetOverlayWindow', win);
else
    % In any other mode, our main 'win'dow and overlay window are the same:
    overlay = win;
    LoadIdentityClut(win);
end

if method == 3 || method == 5
    % Upload a CLUT palette into the Bits++ or DPixx box for definition of overlay
    % colors. We define a nice "blue intensity ramp", this way our text, which
    % is anti-aliased, will look beautiful.
    clut = zeros(256,3);
    clut(1:256, 3) = linspace(0,1,256)';
    Screen('LoadNormalizedGammaTable', win, clut, 2);
end

% Set encoding gamma: It is 1/gamma to compensate for decoding gamma...
PsychColorCorrection('SetEncodingGamma', win, 1/gamma);

% Set larger text size for text in overlay window:
Screen('TextSize', overlay, 24);

if charttype == 0
    % Some info for user, drawn into the overlay plane with color index 255,
    % centered in the center of the display:
    DrawFormattedText(overlay, 'Computing CSF chart - Please standby...', 'center', 'center', 255);
end

Screen('Flip', win);

if charttype == 1
    % Build a chart with a linear gradient. All returned
    % values are encoded linearly with a intensity range of 0.0 to 1.0:
    [CSFImage limitLine] = CreateGradient(RectWidth(winRect), RectHeight(winRect));
else
    % Build a Matlab matrix with the Campbell-Robson CSF chart. All returned
    % values are encoded linearly with a intensity range of 0.0 to 1.0:
    [CSFImage limitLine] = CreateCSFChart(RectWidth(winRect), RectHeight(winRect));
end

% Make a high precision 32 bit floating point luminance texture out of it:
% The '2' requests high precision float representation. Values in the range
% 0.0 - 1.0 correspond to minimum and maximum displayable intensity, but
% other values (even negative ones) are also allowed and useful -- But
% that's a different story for a different demo...
CSF14Tex = Screen('MakeTexture', win, CSFImage, [], [], 2);

% Build a low-resolution 8 bits per component version: Bog-standard
% encoding with 8 bits and values between 0 and 255...
%CSF8Tex = Screen('MakeTexture', win, floor(CSFImage * 256));

% This would do the same -- Encoding a "stripped down to 8 bits" image
% matrix into a 32 bpc float texture. But we don't wanna be wasteful ;-)
CSF8Tex = Screen('MakeTexture', win, floor(CSFImage * 256)/256 + 1/65536, [], [], 2);

% Clear overlay from old text:
Screen('FillRect', overlay, 0);

% Draw new one:
if charttype == 0
    mytxt = 'Campbell-Robson CSF chart\n\nPress ESCape key to exit demo.\n\nPress left- and right- cursor keys to change gamma correction.\n\nPress space key to toggle resolution.';
else
    mytxt = 'Luminance gradient chart\n\nPress ESCape key to exit demo.\n\nPress left- and right- cursor keys to change gamma correction.\n\nPress space key to toggle resolution.';
end

DrawFormattedText(overlay, mytxt, 'center', 'center', 255);
Screen('Flip', win);

% Wait for keystroke to start CSF display:
KbStrokeWait;

% Start with high bits mode toggled on:
toggle = 1;

% Our display loop:
while 1
    % toggle set to 1 for max bits or low for 8 bits?
    if toggle
        % Draw 14 bpc texture: We use 'filterMode' == 0 to disable any kind
        % of filtering so we really get a pixel-exact representation. This
        % shouldn't be neccessary, but some hardware is a bit inexact, so
        % better safe than sorry...
        Screen('DrawTexture', win, CSF14Tex, [], [], [], 0);
        
        txt2 = 'Resolution: Maximum bits per pixel - Space to toggle.';
    else
        % Draw 8 bpc texture:
        Screen('DrawTexture', win, CSF8Tex, [], [], [], 0);
        txt2 = 'Resolution:  8 bits per pixel - Space to toggle.';
    end
    
    % Text for the overlay:
    if charttype == 0
        txt1 = sprintf('Campbell-Robson CSF Chart\nDemo for high precision display devices - ESCape to exit.\nGamma: %f - Left/Right cursor to change.\n', gamma);
    else
        txt1 = sprintf('Luminance gradient Chart\nDemo for high precision display devices - ESCape to exit.\nGamma: %f - Left/Right cursor to change.\n', gamma);
    end
    
    if method == 3 || method == 5
        % Clear the overlay to transparent "background color":
        Screen('FillRect', overlay, 0);
    end
    
    % Draw horizontal line denoting the 8 bits vs. max bits contrast
    % resolution boundary into the overlay at color index 2:
    Screen('DrawLine', overlay, 255, 0, limitLine, RectWidth(winRect), limitLine);
    DrawFormattedText(overlay, 'Not much to see above this line in 8 bit mode...', 0, limitLine, 255);

    % Draw new text into it: We draw with index color 1 out of set of (1-255):
    DrawFormattedText(overlay, [txt1 txt2], 0, 0, 255);

    if doScreenshot
        % Store a screenshot of the final output formatted byte image - as
        % device will see it - in the global variable screenShot.
        Screen('DrawingFinished', win, 0, 0);
        screenShot=Screen('GetImage', win, [1 1 800 800], 'backBuffer');
    end
    
    % Show new image at next retrace:
    Screen('Flip', win);
    
    % Wait for a keypress, returns its 'keyCode':
    [secs keyCode] = KbWait;
    
    % What key pressed?
    if keyCode(Escape)
        % Demo done. Exit...
        break;
    end

    if keyCode(ToggleModes)
        % Toggle resolution:
        toggle = 1 - toggle;
        KbReleaseWait;
    end

    % Change of encoding gamma? Allow for range of 1.0 to 3.0, in steps of
    % 0.001.
    if keyCode(GammaIncrease)
        gamma = min(gamma+0.001, 3.0);
        PsychColorCorrection('SetEncodingGamma', win, 1/gamma);
        WaitSecs(0.010);
    end

    if keyCode(GammaDecrease)
        gamma = max(gamma-0.001, 1.0);
        PsychColorCorrection('SetEncodingGamma', win, 1/gamma);
        WaitSecs(0.010);
    end
    
    % Next loop iteration - Show updated settings...
end

if method == 3
    % Load identity CLUT into Bits++ to restore proper display:
    BitsPlusPlus('LoadIdentityClut', win);
end

% This flip is needed for the 'LoadIdentityClut' to take effect:
Screen('Flip', win);

% Load old gamma tables into gfx-card:
RestoreCluts;

% Demo done. Close Screen...
Screen('CloseAll');

% Bye!
return;

function [CSFImage limitLine] = CreateCSFChart(Xsize, Ysize, NcycLow, NcycHigh, CT_low, CT_high, centre, scale, LimitForBpc)
% [CSFImage limitLine] = CreateCSFChart(Xsize, Ysize [, NcycLow][, NcycHigh][, CT_low][, CT_high][, centre][, scale] [, LimitForBpc])
%
% Creates a image matrix with a chart of the "Campbell-Robson Contrast
% Sensitivity Function" (aka CSF-Chart). The matrix contains values between
% 0.0 for minimum output intensity and 1.0 for maximum output intensity.
%
% Input parameters (Most are optional and have reasonable defaults):
%
% Xsize == Width of chart in pixels.
% Ysize == Height of chart in pixels.
%
% Optional:
% NcycLow - NcycHigh == Spatial frequency range.
% CT_low - CT_high   == Contrast range.
% centre = Intensity value for pixels with zero contrast.
% scale = constrast range around centre.
%
% LimitForBpc == Assumed bit depths of display -> Used to calculate the
% 'limitLine' return argument -- The pixel row above which there is no
% useful content anymore, due to limited bit resolution of output device.
%
% CSFImage itself is the Ysize x Xsize double matrix with luminance values.

if nargin < 2
    error('Must provide at least XSize and YSize of chart in pixels!');
end

% Spatial frequency Range definitions
if ~exist('NcycLow', 'var')
    NcycLow = 1.0;
end

if ~exist('NcycHigh', 'var')
    NcycHigh = 77.0;
end

% Contrast range definitions
if ~exist('CT_low', 'var')
    CT_low  = (1.0/512.0);
end

if ~exist('CT_high', 'var')
    CT_high =  1.0;
end

if ~exist('centre', 'var')
    % centre is the neutral color, scale is the total range around
    % 'centre', ie., all values will be in interval [centre-scale ; centre+scale].
    %
    % 'centre' is chosen as 127/255 instead of 0.5, ie. slighly less than 50%
    % linear output intensity. The reason for this is because in 8 bpc
    % mode, the 50% point (=0.5) can't be hit spot-on, as it would
    % correspond to the 8 bit value 127.5 which is not representable with
    % an integral number.
    %
    % Therefore we need to go for either 127 or 128 to get the same
    % "neutral" base value for both 8 bit and 14 bit mode...
    centre = 127/255;
end

if ~exist('scale', 'var')
    scale  = centre;
end

if ~exist('LimitForBpc', 'var')
    LimitForBpc = 8;
end

% Multiplying factor per step for sweeping contrast.
CTbump = (CT_high/CT_low)^(1.0/Ysize);

% Increase spatial frequency by this factor for each point.
SFbump = (NcycHigh/NcycLow)^(1.0/Xsize);

% initialize contrast to the lowest value
Contrast = CT_low / CTbump;

% Preallocate matrix:
CSFImage = zeros(Ysize, Xsize);

% Fill matrix with content:
for i=0:Ysize-1
    Contrast = Contrast * CTbump;  % increase contrast by a constant factor
    currentSF = (NcycLow/Xsize) / SFbump;     % intialize to base lowest SF

    % 'limitLine' is the pixel row where Contrast drops below the level that
    % can be diplayed on a 'LimitForBpc' bpc display:
    if (Contrast < 1.0/(2^(LimitForBpc-0)))
        limitLine = i;
    end

    % Draw a line of swept-frequency sine wave data:
    for j=0:Xsize-1
        % Increase spatial frequency by constant factor:
        currentSF = currentSF * SFbump;

        % Calculate current linear value. Gamma encoding is done by PTB
        % online, so no need to encode it into matrix:
        CSFImage(i+1, j+1) =  centre + Contrast * sin(j*currentSF*2.0*pi) * scale;
    end
end

% Done.
return;

function [GradientImage limitLine] = CreateGradient(Xsize, Ysize)
% [GradientImage limitLine] = CreateGradient(Xsize, Ysize)
%
% Creates a image matrix with a linear vertical intensity gradient.
% The matrix contains values between 0.0 for minimum output intensity and
% 1.0 for maximum output intensity, linearly increasing from the top of the
% image to the bottom.
%
% Input parameters (Most are optional and have reasonable defaults):
%
% Xsize == Width of chart in pixels.
% Ysize == Height of chart in pixels.
%
% Optional:
%
% LimitForBpc == Assumed bit depths of display -> Used to calculate the
% 'limitLine' return argument -- The pixel row above which there is no
% useful content anymore, due to limited bit resolution of output device.
%
% CSFImage itself is the Ysize x Xsize double matrix with luminance values.

if nargin < 2
    error('Must provide at least XSize and YSize of chart in pixels!');
end

% Preallocate matrix:
GradientImage = zeros(Ysize, Xsize);

% Fill matrix with content:
for i=0:Ysize-1
    GradientImage(i+1, 1:Xsize/4) = i/(Ysize-1) * 0.25;
    GradientImage(Ysize - i, Xsize/4+1:Xsize/2) = i/(Ysize-1) * 0.25 + 0.25;
    GradientImage(i+1, Xsize/2+1:Xsize*3/4) = i/(Ysize-1) * 0.25 + 0.5;
    GradientImage(Ysize - i, Xsize*3/4+1:Xsize) = i/(Ysize-1) * 0.25 + 0.75;
end

limitLine = -50;

% Done.
return;
