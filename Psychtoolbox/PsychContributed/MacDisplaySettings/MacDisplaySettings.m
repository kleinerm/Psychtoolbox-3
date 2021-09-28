function [oldSettings,errorMsg] = MacDisplaySettings(arg1,arg2)
% [oldSettings,errorMsg] = MacDisplaySettings([screenNumber,][newSettings])
%
%% MacDisplaySettings uses an applescript subroutine of the same name to
% allow you to peek and poke seven settings in the macOS System
% Preferences:Displays panel. You use corresponding fields in
% MacDisplaySettings's input and output arguments newSettings and
% oldSettings. You poke by setting one or more fields in the newSettings
% struct, and you peek by looking at fields in the oldSettings struct. This
% allows you to temporarily override (and later restore) any user
% customization of your display (via the Displays System Preference), so
% you can calibrate the display and test users with stable display
% settings. (To be clear, we help you handle the macOS built-in System
% Preferences; we do nothing about the zillion third-party apps and
% extensions that your users might install.) All the parameters refer only
% to the screen selected by screenNumber (optional argument with default 0,
% the main screen). When you have multiple displays, Apple's Display panel
% seems to always offer the Color Profile options for each display. But the
% Display panel's support for non-Apple displays is limited, so you
% probably won't have Brightness, True Tone, or Night Shift controls for
% your external display unless it's made by Apple.
%
%% DISPLAY
% brightness            the Brightness slider
% automatically         the "Automatically adjust brightness" checkbox
% trueTone              the "True Tone" checkbox
%% COLOR
% showProfilesForThisDisplayOnly	the checkbox
% profileRow            row # of selection in Color Profile menu
% profile               name of selection in Color Profile menu
% profileFilename       name of file
% profileFolder         path of folder
%% NIGHT SHIFT
% nightShiftSchedule    the Night Shift Schedule pop up menu
% nightShiftManual      the Night Shift Manual checkbox
%
%% INPUT ARGS. Both arguments are optional: the integer screenNumber
% and the struct newSettings. You can provide either, neither, or both.
% It's ok to provide just "newSettings", omitting the "screenNumber"
% (default is 0, the main screen). The struct "newSettings" can include
% any number, from none to all, of the eight allowed fields:
%
% "brightness" (in the range 0.0 to 1.0)
% "automatically" (true or false)
% "trueTone" (true or false)
% "nightShiftSchedule" ('Off','Custom', or 'Sunset to Sunrise')
% "nighShiftManual" (true or false)
% "showProfilesForThisDisplayOnly" (true or false)
% "profileRow" (integer)
% "profile" (text name)
%
% If newSettings.profileRow is specified then newSettings.profile is
% ignored. True Tone is not available on Macs manufactured before 2018.
%
%% OUTPUT ARGS: The struct oldSettings uses the fields listed above to
% report the prior state of all available parameters. errorMsg is a second,
% optional, output argument. If everything worked then errorMsg is an empty
% string. Otherwise it will describe one failure, even if there were
% several. In peeking, the fields corresponding to a parameter that could
% not be read will be empty [], and that is not considered an error. In
% poking, if you got an error (nonempty errorMsg), you might call
% MacDisplaySettings again to compare the new peek with what you poked.
%
%% EXAMPLES. Typical uses of MacDisplaySettings include just typing the
% function name to learn the current settings:
%
% MacDisplaySettings
%
% ans =
%
%   struct with fields:
%
%                         brightness: 0.8700
%                      automatically: 0
%                           trueTone: []
%                 nightShiftSchedule: 'Off'
%                   nightShiftManual: 0
%     showProfilesForThisDisplayOnly: 0
%                         profileRow: 1
%                            profile: 'Color LCD'
%                    profileFilename: 'Color LCD-1FF8F5E7-4684-FAAF-396B-715F8AE0AA7F.icc'
%                      profileFolder: '/Library/ColorSync/Profiles/Displays'
%
% In one call to MacDisplaySettings you can both peek the old settings and
% poke new settings.
%
% newSettings.brightness=0.87;
% newSettings.automatically=false;
% newSettings.trueTone=false;
% newSettings.nightShiftSchedule='Off';
% newSettings.nightShiftManual=false;
% newSettings.showProfilesForThisDisplayOnly=false;
% newSettings.profileRow=1; % Specify Profile by row.
% newSettings.profile='Display P3'; % Or, select Profile by name.
%
% [oldSettings,errorMsg]=MacDisplaySettings(screen,newSettings)
%
% Now use the display to run your experiment, and then restore the old
% settings as you found them. You can omit the "screen" specifier if you're
% working on the main screen (i.e. 0).
%
% MacDisplaySettings(screen,oldSettings);
%
%% PRESERVING THE DISPLAY STATE. Apple invites Macintosh users to adjust
% many parameters in the System Preferences Displays panel to customize
% their display color and brightness including the enabling of dynamic
% adjustments of all displayed images in response to personal preference,
% ambient lighting, and time of day. Many users enjoy this, but, unless
% reliably overriden, those adjustments defeat our efforts to calibrate the
% display one day and, at a later date, use our calibrations to reliably
% present an accurately specified stimulus. MacDisplaySettings aims to
% satisfy everyone, by allowing your calibration and test programs to use
% the computer in a fixed state, unaffected by individual user whims,
% ambient lighting, and time of day, while saving and later restoring
% whatever custom states the users have selected. MacDisplaySettings
% reports and controls seven settings. It allows you to read their current
% state, set them to standard values for your critical work, and, when
% you're done, restore them to their original values.
%
%% ERROR REPORTING. If everything worked the optional output argument
% errorMsg is an empty string. Otherwise errorMsg will contain an error
% message string, just one, even if there are mutiple faults.
%
%% ERROR CHECKING. Most of the controls are straightforward. 
% You are just peeking and poking a logical value (true or false), a small
% integer with a known range, a float with a known range, or a string.
% Brightness and Profile are more subtle, so MacDisplaySettings always
% checks by peeking immediately after poking Brightness or Profile (whether
% by name or by row). A discrepancy will be flagged by a nonempty string in
% errorMsg. Note that you provide a float to Brightness but within the
% macOS it's quantized to roughly 18-bit precision. In my testing on a
% MacBook and a MacBook Pro, poking random numbers between 0.0 and 1.0, the
% discrepancy between peek and poke is uniformly distributed over the range
% -5e-6 to +5e-6, provided you wait at least 0.1 s after the latest poke.
% (When you move the slider, the macOS does a slow fade to the new value.)
% For MacDisplaySettings's built-in peek-after-poke test,
% MacDisplaySettings accepts the peek of Brightness if it is within 0.001
% of what we poked, otherwise it waits 0.1 s and peeks again, and if the
% second peek is still out of range, then reports the discrepancy in
% errorMsg.
%
%% RELIABLE. MacDisplaySettings is reliable, 
% unlike my previous applescript efforts: AutoBrightness.m, Brightness.m,
% and ScreenProfile.m. The improvement results from discovering, first,
% that the applescript operations proceed MUCH more quickly while System
% Preferences is frontmost (so we now bring it to the front), and, second,
% we now follow the example of pros and have wait loops in the applescript
% to make sure each object is available before accessing it. With these
% enhancements, it now reliably takes 2 s on MacBook Pro and 8 s on
% MacBook, instead of the long 60 s delays, and occasional timeout errors,
% that afflicted the old routines.
%
%% INPUT ARGUMENT RANGE. 
% newSettings.brightness has range 0.0 to 1.0; "automatically", trueTone,
% nightShiftManual, and showProfilesForThisDisplayOnly are logical (true or
% false); nightShiftSchedule is a text field corresponding to any of the
% items in the Displays pop up menu ('Off', 'Custom', 'Sunset to Sunrise').
% profile (a text string) specifies the desired Color Profile by name, and
% profileRow (an integer) specifies it by row.
%
%% INTERNATIONAL. nightShiftSchedule is compatible with macOS international
% localization, provided you use the English field names when calling
% MacDisplaySettings.m. The profile row number will work internationally. I
% don't know whether the profile names change with international
% localization. Just use whatever profile names you see in the System
% Preference: Display panel.
%
%% PROFILE ROW NUMBERING. Note that when you look at the list of profiles
% in System Preferences:Displays:Color there is a line separating the top
% and bottom sections of the list. Apple assigns a row number to that line,
% but trying to select that row has no effect and returns an error in
% errorMsg.
%
%% Your screen's display profile is a video lookup table, it
% affects the color and luminance of everything you display. Apple allows
% programmers to read and write the current color profile, which is in
% memory, and I think that there are several consumer apps that do that (in
% much the same spirit as Apple's Night Shift and TrueTone). System
% Preferences: Displays is unaware of such changes. Clicking on the profile
% name that is currently in use has no effect. Clicking on any other
% profile causes it to be loaded, fresh from the disk master. So, when you
% ask MacDisplaySettings to activate a profile that is already current, it
% plays safe and first clicks another profile and then clicks on the one
% you specified, to be sure that it loads fresh from disk.
%
%% ERROR REPORTING is strict. Out-of-range or unrecognized arguments
% produce fatal errors if detected by MacDisplaySettings.m. When such
% errors are detected in MacDisplaySettings.applescript they are merely
% flagged by a message in the optional output argument errorMsg. When
% throwing a fatal error, if Psychtoolbox is present on the MATLAB path,
% then MacDisplaySettings first closes any open windows (by calling
% Psychtoolbox "sca"), so the error message won't be hidden behind your
% window.
%
%% REQUIREMENTS: macOS, MATLAB, and Psychtoolbox. 
% The current version of MacDisplaySettings has only been tested on macOS
% Big Sur (11.3) localized for USA. The previous version was extensively
% tested on Mojave. Based on earlier testing, I expect this to also
% supported macOS 10.9 to 10.14. It's designed to work internationally, but
% that hasn't been tested yet. It was tested on MATLAB 2019a and 2021a, and
% probably works on any version of MATLAB new enough to include structs. I
% think, but haven't checked, that the MATLAB code is pure basic MATLAB (no
% toolboxes). We use the Psychtoolbox to get the global rect of the main
% screen, and to close all psychtoolbox windows in case of error. The
% routine MacDisplaySettings.applescript only needs macOS. It works on any
% screen, including an external monitor, but testing with external monitors
% has been very limited.
%
%% DEVELOPERS. To write Applescript like this, I strongly recommend that
% you buy the Script Debugger app from Late Night Software.
% https://latenightsw.com/
% and the UI Browser app from UI Browser by PFiddlesoft.
% https://pfiddlesoft.com/uibrowser/ 
% The Script Debugger is the best Applescript editor and debugger. The UI
% Browser allows you to discover the user interface targets in System
% Preferences (or any macOS app) that your script will read and set. With
% it you can do in an hour what would otherwise take days of trial and
% error.
%
%% APPLE PRIVACY. Unless MATLAB has the needed user-granted
% permissions to control the computer, attempts by MacDisplaySettings to
% change settings will be blocked by the macOS. The needed permissions
% include Accessibility, Full Disk Access, and Automation, all in System
% Preferences: Security & Privacy: Privacy. Here are Apple pages on
% privacy in general, and accessibility in particular:
% https://support.apple.com/guide/mac-help/change-privacy-preferences-on-mac-mh32356/mac
% https://support.apple.com/guide/mac-help/allow-accessibility-apps-to-access-your-mac-mh43185/mac
% New versions of macOS may demand more permissions. In some cases
% MacDisplaySettings will detect the missing permission, open the
% appropriate System Preference panel, and provide an error dialog window
% asking the user to provide the permission. In other cases
% MacDisplaySettings merely prints the macOS error message. The granting of
% permission needs to be done only once for your specific MATLAB app. When
% you upgrade MATLAB it typically has a new name, and will be treated by
% macOS as a new app, requiring new granting of permissions. Only users
% with administator privileges can grant permission. When you grant
% permission, sometimes the macOS doesn't seem to notice right away, and
% keeps claiming MATLAB lacks permission. It may help to restart MATLAB or
% reboot.
%
%% WHAT "BRIGHTNESS" CONTROLS: Adjusting the "brightness" setting in an LCD
% controls the luminance of the fluorescent light that is behind the liquid
% crystal display. I believe that the "brightness" slider controls only the
% luminance of the light source, and does not affect the liquid crystal
% itsef, which is driven by the GPU. The luminance at the viewer's eye is
% presumably the product of the two factors: luminance of the source and
% transmission of the liquid crystal, at each wavelength.
%
%% MULTIPLE SCREENS: Seems to work, though not yet thoroughly tested.
% For each screen, MacDisplaySettings provides access to all the controls
% you see in the windows of System Preferences: Displays. However, Apple
% provides fewer Display options for external monitors, especially
% non-Apple monitors.
%
%% HISTORY
% June 25, 2017. denis.pelli@nyu.edu wrote "Brightness" for the
% Psychtoolbox, and later "AutoBrightness".
%
% July 16,2019 Improved by looking at code here:
% https://apple.stackexchange.com/questions/272531/dim-screen-brightness-of-mbp-using-applescript-and-while-using-a-secondary-mon/285907
%
% April 2020. Wrote MacDisplaySettings, based on Brightness, but enhanced
% to also support Automatically, True Tone, Night Shift, and Profile.
%
% April 14, 2020. Added wait loops (in the applescript) to wait for "tab
% group 1" before accessing it. This has nearly eliminated the occasional
% time out failures, in which MacDisplaySettings.m returns [] for
% brightness and automatic, but returns correct values for night shift.
%
% May 3, 2020. In the Applescript, I now "activate" System Preferences at
% the beginning (and reactivate the former app when we exit), and this runs
% much faster. Formerly, delays of 60 s were common, with occasional time
% outs. Now it reliably takes 2 s on MacBook Pro and 8 s on MacBook.
%
% May 7, 2020. Shortened the help text, reducing redundancy. Check for
% unrecognized fields in newSettings. Improved error reporting.
%
% May 8, 2020. Enhanced to support arbitrary screenNumber, i.e. external
% monitors.
%
% May 9, 2020. Improved speed (by 30%) by replacing fixed delays in
% applescript with wait loops. Enhanced the built-in peek of brightness
% afer poking. Now if the peek differs by more than 0.001,
% MacDisplaySettings waits 100 ms and tries again, to let the value settle,
% as the visual effect is a slow fade. Then it reports in errorMsg if the
% new peek differs by more than 0.001. In limited testing, waiting for a
% good answer works: the peek-poke difference rarely exceeds +/-5e-6 and
% never exceeds 0.001. It's my impression that if we always waited 100 ms,
% then the discrepancy would always be less than +/-5e-6.
%
% May 9, 2020. APPLESCRIPT: Improved speed by replacing fixed delays in
% applescript with wait loops. Enhanced the peek of brightness afer poking.
% Now if the peek differs by more than 0.001, MacDisplaySettings waits 100
% ms and tries again, to let the value settle, as the visual effect is a
% slow fade. Then it reports in errorMsg if the new peek differs by more
% than 0.001. In limited testing, waiting for a good answer works: the
% peek-poke difference rarely exceeds +/-5e-6 and never exceeds 0.001. It's
% my impression that if we always waited 100 ms, then the discrepancy would
% always be less than +/-5e-6.
%
% May 14, 2020. Added to Psychtoolbox.
%
% May 15, 2020. Now also pass a flag from MacDisplaySettings.m to
% MacDisplaySettings.applescript indicating whether Psychtoolbox has a
% window on the main screen. In that case, AppleScript will not try to
% show a dialog. Added a loop in AppleScript to wait for System Preferences
% window to open; this fixes a rare error.
%
% May 15, 2020. APPLESCRIPT: Added a loop in AppleScript to wait for System
% Preferences window to open; this fixes a rare error. Replaced every error
% code with a message in errorMsg.
%
% May 17, 2020. Improved handling of empty args, which are now replaced by
% default values, just like missing args.
%
% May 20, 2020. APPLESCRIPT: MacDisplaySettings hung up on my student Benji
% Luo with the Night Shift panel showing. I suspect it was in an endless
% loop waiting for the menu to pop up after a click. I rewrote the loop to
% throw an error if the menu doesn't appear after three attempts of
% clicking and waiting up to 500 ms each time.
%
% July 14, 2020. Added two new output fields, settings.profileFilename and
% settings.profileFolder, which give the ColorSynch profile's filename and
% folder path. With these, you can use iccread to read the profile.
%
% August 25, 2020. Save and restore current directory. Bug reported by Jan
% Kurzawski.
%
% February 17, 2021. Not yet compatible with macOS Catalina. If macOS is
% too new we return a dummy result (and print a warning) that hopefully
% will allow your code to run.
%
% February 25, 2021. Partially compatible with macOS Catalina. If macOS is
% Catalina or newer we print a warning and return a partial result.
%
% May 13, 2021. Now fully compatible with Big Sur (macOS 10.3), and hopefully 
% also compatible with Catalina. We do not expect the enhancements to have 
% affected the well-tested compatiblity with earlier versions of macOS, Mojave 
% and before.
%
% May 14, 2021. Polished the help text, above.
%
% May 24, 2021. Now also compatible with macOS Catalina. Various releases
% have been tested on every major version of macOS from 10.9 (Mavericks) to
% 11.3 (Big Sur). This release has been tested only on Catalina and Big
% Sur. The changes are all in the applescript file coping with Apple's
% tendency, with each new major version of macOS, to change the hierarchy
% of elements in System Preferences:Displays.

%% ACKNOWLEGEMENTS. Thanks to Mario Kleiner for explaining how macOS
% "brightness" works. Thanks to nick.peatfield@gmail.com for sharing his
% applescript code for dimmer.scpt and brighter.scpt. And to Hormet Yiltiz
% for noting that we need to control Night Shift.
%
%% SEE ALSO:
% https://support.apple.com/en-us/HT208909 % True Tone
% https://support.apple.com/en-us/HT207513 % Night Shift
% Screen ConfigureDisplay? % In Psychtoolbox
% http://www.manpagez.com/man/1/osascript/
% https://developer.apple.com/library/mac/documentation/AppleScript/Conceptual/AppleScriptLangGuide/reference/ASLR_cmds.html
% https://discussions.apple.com/thread/6418291
% ScriptDebugger app from Late Night Software.
% https://latenightsw.com/
% UIBrowser app from PFiddlesoft.
% https://pfiddlesoft.com/uibrowser/

% The Psychtoolbox Screen.mex function sets Brightness more quickly than
% MacDisplaySettings does, because applescript is slow, but the macOS
% support of Screen.mex Brightness is flakey. When I last tested it, in
% Fall 2019, brightness changed immediately, but the brightness slider in
% Screen Preferences: Displays did not budge, and sometimes the macOS would
% later revert to the brightness corresponding to the stale slider
% position. The method used here is slower, but more reliable.

if ~exist('PsychtoolboxVersion','file')
    error('MacDisplaySettings requires the Psychtoolbox (free from http://psychtoolbox.org).');
end

% Print warning and return partial data if our Applescript doesn't fully
% support this macOS version.
if ~ismac
    error('Sorry, MacDisplaySettings requires macOS.');
end
[~,OSVersion]=system('sw_vers -productVersion');
OSVersion=strtrim(OSVersion);
macOSVersionString=['macOS ' OSVersion];
vv=sscanf(OSVersion,'%d.%d.%d');
if length(vv)<2
    vv(2)=0;
end
if length(vv)<3
    vv(3)=0;
end
v=vv(3);
for i=[2 1]
    v=v/10^ceil(log10(v+0.1));
    v=v+vv(i);
end
if false && macOSVersion>10.14
    % May 13, 2021. MacDisplaySettings fully supports macOS 10.3 Big Sur,
    % and earlier versions, with various degrees of testing.
    warning(['Your %s is too new for MacDisplaySettings, '...
        'which require macOS Big Sur 10.3 or older.'],macOSVersionString);
%     oldSettings.brightness=[];
%     oldSettings.automatically=logical([]);
%     oldSettings.nightShiftSchedule='';
%     oldSettings.nighShiftManual=logical([]);
%     oldSettings.profile='';
%     oldSettings.profileRow=[];
%     errorMsg='MacDisplaySettings only supports macOS 10.14.x Mojave and older.';
%     return
end

useScreenBrightness=false;

oldSettings.brightness=[];
oldSettings.automatically=[];
oldSettings.trueTone=[];
oldSettings.nightShiftSchedule=[];
oldSettings.nightShiftManual=[];
oldSettings.showProfilesForThisDisplayOnly=[];
oldSettings.profileRow=[];
oldSettings.profile='';
oldSettings.profileFilename='';
oldSettings.profileFolder='';
failed=false;

if ~ismac
    % Applescript requires macOS.
    failed = true; % Report failure on this unsupported OS:
    return;
end

switch nargin
    case 0
        screenNumber=0;
        newSettings=oldSettings;
    case 1
        switch class(arg1)
            case 'double'
                screenNumber=arg1;
                newSettings=oldSettings;
            case 'struct'
                screenNumber=0;
                newSettings=arg1;
            otherwise
                CloseWindows
                error('Single argument must be either double or struct.');
        end
    case 2
        screenNumber=arg1;
        newSettings=arg2;
    otherwise
        CloseWindows
        error('At most two arguments are allowed.');
end
if isempty(screenNumber)
    screenNumber=0;
end
if isempty(newSettings)
    newSettings=oldSettings;
end
if exist('Screen','file') && ~ismember(screenNumber,Screen('Screens'))
    str=sprintf(' %d',Screen('Screens'));
    error('screenNumber %d is not one of the currently valid screen numbers:%s.',screenNumber,str);
end
if ~isstruct(newSettings)
    CloseWindows
    error('The newSettings argument must be a struct.');
end
if length(newSettings)~=1
    CloseWindows
    error('The newSettings argument must be a struct, not an array.');
end
if any(~ismember(fieldnames(newSettings),fieldnames(oldSettings)))
    CloseWindows
    illegal=find(~ismember(fieldnames(newSettings),fieldnames(oldSettings)),1);
    s=fieldnames(newSettings);
    error('Input argument newSettings.%s field not recognized.',s{illegal});
end
if isfield(newSettings,'brightness') && ...
        ~isempty(newSettings.brightness) && ...
        (~isfloat(newSettings.brightness) || newSettings.brightness<0 || newSettings.brightness>1)
    CloseWindows
    error('newSettings.brightness %.1f must be float (not logical) in the range 0.0 to 1.0, or [] to ignore it.',...
        newSettings.brightness)
end
if isfield(newSettings,'automatically') && ...
        ~isempty(newSettings.automatically) && ...
        (~islogical(newSettings.automatically) || ...
        ~ismember(newSettings.automatically,[true false]))
    CloseWindows
    error('newSettings.automatically %.1f must be true or false, otherwise [] or omitted to ignore it.',...
        newSettings.automatically)
end
if isfield(newSettings,'trueTone') && ...
        ~isempty(newSettings.trueTone) && ...
        (~islogical(newSettings.trueTone) || ...
        ~ismember(newSettings.trueTone,[true false]))
    CloseWindows
    error('newSettings.trueTone %.1f must be true or false, otherwise [] or omitted to ignore it.',...
        newSettings.trueTone)
end
if isfield(newSettings,'nightShiftSchedule') && ~isempty(newSettings.nightShiftSchedule) && ...
        ~ismember(newSettings.nightShiftSchedule,{'Off','Custom','Sunset to Sunrise'})
    CloseWindows
    error('newSettings.nightShiftSchedule %s must be ''Off'',''Custom'', or ''Sunset to Sunrise'', otherwise [] or omitted to ignore it.',...
        newSettings.nightShiftSchedule)
end
if isfield(newSettings,'nightShiftManual') && ...
        ~isempty(newSettings.nightShiftManual) && ...
        (~islogical(newSettings.nightShiftManual) || ...
        ~ismember(newSettings.nightShiftManual,[true false]))
    CloseWindows
    error('newSettings.nightShiftManual %.1f must be true or false, otherwise [] or omitted to ignore it.',...
        newSettings.nightShiftManual)
end
if isfield(newSettings,'showProfilesForThisDisplayOnly') && ...
        ~isempty(newSettings.showProfilesForThisDisplayOnly) && ...
        (~islogical(newSettings.showProfilesForThisDisplayOnly) || ...
        ~ismember(newSettings.showProfilesForThisDisplayOnly,[true false]))
    CloseWindows
    error('newSettings.showProfilesForThisDisplayOnly %.1f must be true or false, otherwise [] or omitted to ignore it.',...
        newSettings.showProfilesForThisDisplayOnly)
end
if isfield(newSettings,'profileRow') && ...
        ~isempty(newSettings.profileRow) && ...
        (~isfloat(newSettings.profileRow) || ...
        newSettings.profileRow<1 || ...
        mod(newSettings.profileRow,1)~=0)
    CloseWindows
    error(['newSettings.profileRow %d must be a positive integer '...
        'ColorSync Profile row number, otherwise [] '...
        'or omitted to ignore it.'],newSettings.profileRow)
end
if isfield(newSettings,'profile') && ...
        ~isempty(newSettings.profile) && ...
        ~ischar(newSettings.profile)
    CloseWindows
    error('newSettings.profile ''%s'' must be Profile name, otherwise [] or omitted to ignore it.',newSettings.profile)
end
fields=fieldnames(newSettings);
ok=ismember(fields,fieldnames(oldSettings));
if any(~ok)
    CloseWindows
    error('Unknown newSetting field ''%s''.',fields{find(~ok,1)});
end
if useScreenBrightness
    if isfield(newSettings,'brightness') && ~isempty(newSettings.brightness)
        oldSettings.brightness=Screen('ConfigureDisplay','Brightness',...
            screenNumber,0,newSettings.brightness);
    else
        oldSettings.brightness=...
            Screen('ConfigureDisplay','Brightness',screenNumber,0);
    end
    newSettings.brightness=[]; % Remove it from the to-do list.
end
try
    % Call MacDisplaySettings.applescript
    scriptPath = which('MacDisplaySettings.applescript');
    command = ['osascript "' scriptPath '"']; % Double quotes cope with spaces in scriptPath.
    command = [command ' ' num2str(screenNumber) ' '];
    globalRect=Screen('GlobalRect',screenNumber);
    windowIsOpenOnMainScreen=false;
    mainRect=Screen('GlobalRect',0);
    w=Screen('Windows');
    for i=1:length(w)
        if Screen(w(i),'WindowKind')~=1
            continue
        end
        r=Screen('GlobalRect',w(i));
        if IsInRect(r(1),r(2),mainRect)
            windowIsOpenOnMainScreen=true;
            break;
        end
    end
    command=sprintf('%s %d %d %d %d %d',...
        command,globalRect,windowIsOpenOnMainScreen);
    % MATLAB indicates missing value by [].
    % In passing arguments to AppleScript we indicate missing values as -1.
    if ~isfield(newSettings,'brightness') || isempty(newSettings.brightness)
        newSettings.brightness=-1;
    end
    if ~isfield(newSettings,'automatically') || ...
            isempty(newSettings.automatically)
        newSettings.automatically=-1;
    end
    if ~isfield(newSettings,'trueTone') || isempty(newSettings.trueTone)
        newSettings.trueTone=-1;
    end
    if ~isfield(newSettings,'nightShiftSchedule') || ...
            isempty(newSettings.nightShiftSchedule)
        newSettings.nightShiftSchedule=-1;
    else
        % Convert pop up menu choice from text to index.
        i=find(ismember({'Off','Custom','Sunset to Sunrise'},...
            newSettings.nightShiftSchedule));
        if i==0
            CloseWindows
            error('newSettings.nightShiftSchedule=''%s'' but should be one of: ''Off'',''Custom'',''Sunset to Sunrise''.',...
                newSettings.nightShiftSchedule);
        end
        newSettings.nightShiftSchedule=i;
    end
    if ~isfield(newSettings,'nightShiftManual') || ...
            isempty(newSettings.nightShiftManual)
        newSettings.nightShiftManual=-1;
    end
    if ~isfield(newSettings,'showProfilesForThisDisplayOnly') || isempty(newSettings.showProfilesForThisDisplayOnly)
        newSettings.showProfilesForThisDisplayOnly=-1;
    end
    if ~isfield(newSettings,'profileRow') || ...
            isempty(newSettings.profileRow)
        newSettings.profileRow=-1;
    end
    if ~isfield(newSettings,'profile') || ...
            isempty(newSettings.profile)
        newSettings.profile='';
    end
    if ~isfield(newSettings,'profileFilename') || ...
            isempty(newSettings.profileFilename)
        newSettings.profileFilename='';
    end
    if ~isfield(newSettings,'profileFolder') || ...
            isempty(newSettings.profileFolder)
        newSettings.profileFolder='';
    end
    command = [command ' ' num2str(newSettings.brightness) ' '...
        num2str(newSettings.automatically) ' ' ...
        num2str(newSettings.trueTone) ' '...
        num2str(newSettings.nightShiftSchedule) ' ' ...
        num2str(newSettings.nightShiftManual) ' '...
        num2str(newSettings.showProfilesForThisDisplayOnly) ' '...
        num2str(newSettings.profileRow) ' '...
        '"' newSettings.profile '"'];
    [failed,oldString]=system(command); % Takes 2 s on MacBook Pro.
    if failed
        CloseWindows
        % MATLAB suggests calling "contains" instead of ~isempty(strfind), but,
        % the last time I checked, Octave does not have the "contains" function,
        % and we are trying to keep our software Octave-compatible.
        if ~isempty(strfind(oldString,'assistive access'))
            % We need sprintf to process the linefeeds.
            s=sprintf(['If the error below mentions "assistive access", '...
                'you may need to give MATLAB permission:\n'...
                'Choose Apple menu  > System Preferences, click Security & Privacy, click upper tab Privacy, click Accessibility, unlock the padlock, and add (+) the MATLAB app.\n'...
                'https://support.apple.com/guide/mac-help/allow-accessibility-apps-to-access-your-mac-mh43185/mac']);
            fprintf('\n');
            warning(s);
        end
        error('Applescript failed with error: %s.',oldString);
    end
    %     if streq('-99',oldString(1:3))
    %         CloseWindows
    %         warning(['If you haven''t already, please unlock System '...
    %             'Preferences: Security & Privacy: Privacy and give MATLAB ' ...
    %             'permission for Full Disk Access and Automation.']);
    %         error('Applescript returned error: %s',oldString);
    %     end
    [v,count,errMsg,extra]=sscanf(oldString,'%f, %d, %d, %d, %d, %d, %d, %d',7);
    nextIndex=1+extra-1;
    if count<7 || ~isempty(errMsg)
        warning('sscanf processed %d of 7 values. sscanf error: %s',...
            count,errMsg);
        fprintf('oldString=''%s''.\n',oldString);
    end
    oldSettings.profile=regexprep(oldString(nextIndex:end),...
        '\|([^\|]*)\|.*','$1','once');
    nextIndex=nextIndex+1+length(oldSettings.profile)+1-1+3; % skip ', '
    oldSettings.profileFilename=regexprep(oldString(nextIndex:end),...
        '\|([^\|]*)\|.*','$1','once');
    nextIndex=nextIndex+1+length(oldSettings.profileFilename)+1-1+3; % skip ', '
    errorMsg=regexprep(oldString(nextIndex:end),'\|([^\|]*)\|.*','$1','once');
    nextIndex=nextIndex+1+length(errorMsg)+3;
    if count>=1
        oldSettings.brightness=v(1);
    end
    if count>=2
        oldSettings.automatically=v(2);
    end
    if count>=3
        oldSettings.trueTone=v(3);
    end
    if count>=4
        oldSettings.nightShiftSchedule=v(4);
    end
    if count>=5
        oldSettings.nightShiftManual=v(5);
    end
    if count>=6
        oldSettings.showProfilesForThisDisplayOnly=v(6);
    end
    if count>=7
        oldSettings.profileRow=v(7);
    end
    if oldSettings.brightness==-1
        oldSettings.brightness=[];
    end
    if oldSettings.automatically==-1
        oldSettings.automatically=[];
    end
    if oldSettings.trueTone==-1
        oldSettings.trueTone=[];
    end
    if ~isfloat(oldSettings.nightShiftSchedule) || length(oldSettings.nightShiftSchedule)~=1
        oldSettings.nightShiftSchedule
        error('oldSettings.nightShiftSchedule should be an integer.');
    end
    if oldSettings.profileRow==-1
        oldSettings.profileRow=[];
    end
    % Convert pop up menu choice from index to text.
    switch oldSettings.nightShiftSchedule
        case 1
            oldSettings.nightShiftSchedule='Off';
        case 2
            oldSettings.nightShiftSchedule='Custom';
        case 3
            oldSettings.nightShiftSchedule='Sunset to Sunrise';
        case -1
            oldSettings.nightShiftSchedule='';
        otherwise
            CloseWindows
            error('Illegal values of oldSettings.nightShiftSchedule %d.',...
                oldSettings.nightShiftSchedule);
    end
    if oldSettings.nightShiftManual==-1
        oldSettings.nightShiftManual=[];
    end
    if oldSettings.showProfilesForThisDisplayOnly==-1
        oldSettings.showProfilesForThisDisplayOnly=[];
    end
    oldSettings.automatically=logical(oldSettings.automatically);
    oldSettings.trueTone=logical(oldSettings.trueTone);
    oldSettings.nightShiftManual=logical(oldSettings.nightShiftManual);
    oldSettings.showProfilesForThisDisplayOnly=logical(oldSettings.showProfilesForThisDisplayOnly);
    if failed || isempty(oldSettings.brightness)
        msg=sprintf('\nApplescript failed. Here follows some diagnostic output:');
        warning(msg);
        failed
        screenNumber
        oldString
        oldSettings
        CloseWindows
        msg=sprintf(['MacDisplaySettings.applescript failed. ' ...
            'Make sure you have admin privileges, \n' ...
            'and that System Preferences is not tied up in a dialog. \n' ...
            'MacDisplaySettings applescript error: %s. '],oldString);
        error(msg);
    end
catch ME
    CloseWindows
    failed=true;
    rethrow(ME);
end

%% TRY TO FIND profileFilename in the icc folders of the local disk, to return profileFolder.
if ~isempty(oldSettings.profileFilename)
    if ~exist('iccroot','file')
        iccPath=fullfile(filesep,'Library','ColorSync','Profiles');
    else
        iccPath=iccroot;
    end
    paths={iccPath};
    paths{end+1}=fullfile(filesep,'System','Library','ColorSync','Profiles');
    d=[];
    saveCurrentDirectory=cd;
    for i=1:length(paths)
        try
            cd(paths{i});
            % "**/" requests that the search include all subfolders.
            dNew=dir(['**' filesep oldSettings.profileFilename]);
        catch ME
            continue
        end
        if isempty(d)
            d=dNew;
        else
            for j=1:length(dNew)
                try
                    d(end+1)=dNew(j);
                catch ME
                    warning('Unable to include a file ''%s'' because of mismatching dir structs.',dNew(j).name);
                end
            end
        end
    end
    cd(saveCurrentDirectory); % Restore.
    if ~isempty(d)
        % Discard bad files from our list.
        ok=false(size(d));
        for i=1:length(d)
            ok(i)=~d(i).isdir && ~isempty(d(i).name) && ~isempty(d(i).folder) && d(i).bytes>0;
        end
        d=d(ok);
    end
    if length(d)>1
        warning(['Found %d files called ''%s'' in folders ''%s'' and ''%s'' '...
            'and their subfolders. Using the first one.'],...
            length(d),oldSettings.profileFilename,paths{1},paths{2});
    end
    if ~isempty(d)
        % Report the first file we found.
        oldSettings.profileFolder=d(1).folder;
    end
end
% If no match was found, default for oldSettings.profileFolder is ''.
end

function CloseWindows
if exist('PsychtoolboxVersion','file') && ~any(ismember(Screen('Windows'),0))
    % Close any user windows on main screen to make sure that our error
    % message can be seen.
    sca
end
end
