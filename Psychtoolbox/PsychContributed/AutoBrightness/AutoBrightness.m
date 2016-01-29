function [oldSetting, status] = AutoBrightness(screenNumber, newSetting)
% [oldSetting, status] = AutoBrightness([screenNumber=0][, newSetting])
%
% AUTOBRIGHTNESS Get and set the "Automatically adjust brightness" checkbox
% on the Mac OS X: System Preferences: Displays panel. The function
% argument "newSetting" (integer 0 or 1) indicates whether you want to turn
% the autobrightness feature on (newSetting==1) or off (newSetting==0). If
% you call without an argument (or anything other than 0 or 1) then nothing
% is changed. The current state is always reported in the returned
% oldSetting (0 or 1). The optionally returned "status" is always zero
% unless the applescript failed.
%
% If the user has not yet given permission for MATLAB to control the
% computer (in System Preferences:Security & Privacy:Accessibility), then
% the AutoBrightness applescript will put up a helpful alert and wait for
% the user to enable control. CAUTION: Call AutoBrightness BEFORE you open
% a Psychtoolbox window: If the alert window is hidden by a Psychtoolbox
% window, then the user won't know that the program is waiting for him or
% her. For this reason the AutoBrightness function will abort with an error
% if you try to call it while an onscreen window is already open.
%
% AutoBrightness.m uses AutoBrightness.applescript to allow you to turn off
% a pesky feature of Apple's liquid crystal displays. In Mac OSX, this
% feature is manually enabled/disabled by the"Automatically adjust
% brightness"checkbox in theSystem Preferences: Displays panel. While the
% feature is enabled, your Mac slowly adjusts the screen luminance of your
% Apple liquid crystal display, depending the the luminance of the room.
% That's bad for screen calibration, and perhaps also bad for your
% experiments. My AutoBrightness routines allow your programs to read the
% on/off setting of that feature, and enable or disable it. For use in
% MATLAB, please put both files anywhere in MATLAB's path. I hope they will
% be added to the Psychtoolbox.
%
% Written by denis.pelli@nyu.edu for the Psychtoolbox, May 21, 2015.
%
% This Psychtoolbox MATLAB function calls my AutoBrightness applescript,
% which allows you to temporarily disable a feature of Apple Macintosh
% laptops that is undesirable for vision experiments and display
% calibration. The applescript is equivalent to manually opening the System
% Preference:Displays panel and clicking to turn on or off the "Automatic
% brightness adjustment" checkbox. I wrote the script to be invoked from
% MATLAB, but you could call in from any application. One important use of
% the script is to prevent changes of brightness in response to the room
% luminance while calibrating a display. The automatic adjustments are
% slow, over many seconds, which could invalidate your display calibration.
% When "Automatically adjust brightness" is checked, the Mac OS uses the
% video camera to sense the room luminance and slowly dims the display if
% the room is dark. It does this by adjusting the "brightness" setting,
% which controls the luminance of the fluorescent light that is behind the
% liquid crystal display. I believe that the "brightness" slider controls
% only the luminance of the source, and does not affect the liquid crystal
% itsef, which is controlled by the color lookup table. The luminance at
% the viewer's eye is presumably the product of the two factors: luminance
% of the source and transmission of the liquid crystal, at each wavelength.
%
% INSTALLATION. To work with MATLAB, please put both files anywhere in
% MATLAB's path. I hope they will be added to the Psychtoolbox.
%
% CAUTION. This uses the "System Preferences: Displays" panel, which takes
% 30 s to open if it isn't already open. I set up the AutoBrightness
% applescript to always leave System Preferences open, so you won't waste
% your observer's time waiting 30 s for System Preferences to open every
% time you call AutoBrightness.
%
% BRIGHTNESS. Psychtoolbox for MATLAB and Macintosh already has a Screen
% call to get and set the brightness, so we don't need applescript for
% that. The Psychtoolbox call is:
% [oldBrightness]=Screen('ConfigureDisplay','Brightness', screenId [,outputId][,brightness]);
%
% APPLE SECURITY. The first time any application (e.g. MATLAB) calls
% AutoBrightness.applescript, the request will be blocked and an error
% dialog window will appear saying the application is "not allowed
% assistive access." This means that the application needs an
% administrator's permission to access the System Preferences. A user with
% admin privileges should then click as requested to provide that
% permission. This needs to be done only once (for each application).
%
% MULTIPLE SCREENS: All my computers have only one screen, so I haven't
% implemented support for multiple screens. I think that would be
% straightforward. I would add a second, optional, argument that specifies
% which screen.
%
% LINUX and WINDOWS. Applescript works only under Mac OS X. When running
% under any operating system other that Mac OS X, this program ignores the
% newSetting argument and always returns zero as the oldSetting. It is
% conceivable that Apple's auto brightness feature is implemented on
% Macintoshes running Linux or Windows. If that applies to you, please
% consider enhancing this program to return a correct answer for that case,
% and sharing the result with me and the Psychtoolbox forum.

% See also:
% ScreenProfile.m
% http://www.manpagez.com/man/1/osascript/
% https://developer.apple.com/library/mac/documentation/AppleScript/Conceptual/AppleScriptLangGuide/reference/ASLR_cmds.html
% https://discussions.apple.com/thread/6418291
%
% Thanks to Mario Kleiner for explaining how Mac OSX "brightness" works.
% Thanks to nick.peatfield@gmail.com for sharing his applescript code for 
% dimmer.scpt and brighter.scpt.

if ~IsOSX
    % I believe that Applescript works only within Mac OS X. It is
    % conceivable that Apple's auto brightness feature is implemented on
    % Macintoshes running Linux or Windows, in which case someone might
    % enhance this program to return a correct answer for those cases.
    oldSetting = 0;

    % Signal failure on this unsupported OS:
    status = 1;

    return;
end

if length(Screen('Windows')) > 0
    error('AutoBrightness called while onscreen windows are open. Only call this function before opening the first onscreen window!');
end

scriptPath = which('AutoBrightness.applescript');
command = ['osascript ', scriptPath];

if nargin > 0
    command = [command, ' ', num2str(screenNumber)];
end

if nargin > 1
    command = [command, ' ', num2str(newSetting)];
end

[status, oldSetting] = system(command);

end
