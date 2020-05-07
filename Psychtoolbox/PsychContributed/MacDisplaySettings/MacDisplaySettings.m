function [oldSettings,errorMsg] = MacDisplaySettings(arg1,arg2)
% [oldSettings,errorMsg] = MacDisplaySettings([screenNumber,][newSettings])
%
%% MacDisplaySettings allows you to temporarily override any macOS user
% customization of the display, to allow calibration and user testing with
% stable display settings. It allows you to peek and poke many settings in
% the System Preferences: Displays panel.
%
%% INPUT ARGS: There are two optional arguments: the integer screenNumber
% and the struct newSettings. You can provide either, neither, or both.
% It's ok to provide just "newSettings", omitting the "screenNumber"
% (default is 0, the main screen). "newSettings" can include several, from
% none to all eight, of the allowed settings:
% "brightness" (in the range 0.0 to 1.0),
% "automatically" (true or false), 
% "trueTone" (true or false),
% "nightShiftSchedule" ('Off','Custom', or 'Sunset to Sunrise'),
% "nighShiftManual" (true or false), 
% "showProfilesForThisDisplayOnly" (true or false),  
% "profile" (text), and
% "profileRow" (integer). ("profile" is ignored if you set "profileRow".)
%
%% OUTPUT ARGS: The struct oldSettings uses the fields listed above to
% report all available parameters. errorMsg is a second, optional, output
% argument. If everything worked, then errorMsg is an empty string.
% Otherwise it will describe one failure, even if there were several. To
% look into the problem, you might call MacDisplaySettings again to compare
% the new peek with what you poked.
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
%                            profile: 'Color LCD'
%                         profileRow: 1
%
%
% % In one call you can both peek the old settings and poke new settings.
%
% newSettings.brightness=0.87;
% newSettings.automatically=false;
% newSettings.trueTone=false;
% newSettings.nightShiftSchedule='Off';
% newSettings.nightShiftManual=false;
% newSettings.showProfilesForThisDisplayOnly=false;
% newSettings.profile='Display P3'; % Select Profile by name.
% newSettings.profileRow=1; % Specify Profile row, and ignore "profile" field.
%
% [oldSettings,errorMsg]=MacDisplaySettings(newSettings)
%
% % Now use the display, ...
% % and then restore the old settings as you found them.
%
% MacDisplaySettings(oldSettings);
%
%% INTRODUCTION. Apple invites Macintosh users to adjust many parameters in
% the System Preferences Displays panel to customize their display color
% and brightness including the enabling of dynamic adjustments of all
% displayed images in response to personal preference, ambient lighting,
% and time of day. Many users enjoy this, but, unless reliably overriden,
% those adjustments defeat our efforts to calibrate the display one day
% and, at a later date, use our calibrations to reliably present an
% accurately specified stimulus. MacDisplaySettings aims to satisfy
% everyone, by allowing your calibration and test programs to use the
% computer in a fixed state, unaffected by individual user whims, ambient
% lighting, and time of day, while saving and later restoring whatever
% custom states the users have customized it to. MacDisplaySettings reports
% and controls seven settings. It allows you to read their current states,
% set them to standard values for your critical work, and, when you're
% done, restore them to their original values. MacDisplaySettings controls
% seven System Preferences. Please let me know if we should add another.
% denis,pelli@nyu.edu
%
%% SETTINGS. Get and set seven parameters in the macOS: System Preferences:
% Displays panel by using the corresponding fields in the oldSettings and
% newSettings arguments:
%
%% DISPLAY
% brightness            the Brightness slider
% automatically         the "Automatically adjust brightness" checkbox
% trueTone              the "True Tone" checkbox
%% NIGHT SHIFT
% nightShiftSchedule    the Night Shift Schedule pop up menu
% nightShiftManual      the Night Shift Manual checkbox
%% COLOR
% showProfilesForThisDisplayOnly	the checkbox
% profile               name of selection in Color Profile menu
% profileRow            row # of selection in Color Profile menu
%
% In one call you can set any combination of parameters, from none to all.
% The output argument oldSettings always reports the prior state of all
% available fields. (True Tone is only available on some Macs manufactured
% in 2018 or later.) If newSettings.profileRow is specified then
% newSettings.profile is ignored.
%
%% ERROR REPORTING. If everything worked the optional output argument
% errorMsg is an empty string. Otherwise errorMsg will contain an error
% message string, just one even if there are mutiple faults. 
%
%% ERROR CHECKING. Most of the controls are straightforward, you are just
% peeking and poking a Boolean (0 or 1) or a small integer with a known
% range. Brightness and Profile are more subtle, so MacDisplaySettings
% always checks by peeking immediately after poking Brightness, Profile by
% name, and Profile by row. A discrepancy will by flagged by a string in
% errorMsg. Note that you provide a float to Brightness but within the
% macOS it's coarsely quantized; I think it's a 16-point scale. The peek of
% Brightness is considered erroneous only if it differes by more than 0.1
% from what we poked.
%
%% RELIABLE. MacDisplaySettings is fast (3 s) and reliable, unlike my
% previous efforts (Brightness.m and AutoBrightness.m). The improvement
% results from discovering, first, that the applescript operations proceed
% MUCH more quickly while System Preferences is frontmost (so we now bring
% it to the front), and, second, we now follow the example of pros and have
% wait loops in the applescript to make sure each object is available
% before accessing it. Since those enhancements, it now reliably takes 3 s,
% instead of the long 60 s delays, and occasional timeout errors, that
% afflicted the old routines.
%
%% INPUT ARGUMENTS. newSettings.brightness has range 0.0 to 1.0;
% automatically, trueTone, nightShiftManual, and
% showProfilesForThisDisplayOnly are boolean (true or false);
% nightShiftSchedule is a text field corresponding to any of the items in
% the Displays pop up menu ('Off', 'Custom', 'Sunset to Sunrise').
% (nightShiftSchedule is compatible with international systems, provided
% you use the English field names when calling MacDisplaySettings.m.)
% profile (text) specifies the desired Color Profile by name, and
% profileRow (integer) specifies it by row. The row number will work
% internationally. I suspect the names that you read and write will be in
% whatever your macOS takes to be the local language. Thus
% nightShiftSchedule uses English regardless of locality, whereas profile
% uses local names. Thus, you can get consistent international behavior by
% using row numbers to specify profile and English names to specify
% nightShiftSchedule.
%
%% INPUT ARGE\S OPTIONAL. If you omit the newSettings argument, then 
% MacDisplaySettings will only peek, without poking. Any newSettings field
% that is set to [] or omitted is not disturbed.
%
%% OUTPUT ARGUMENTS. oldSettings has many fields which return all available
% settings, and [] for the rest. Note that True Tone is not available on
% Macs manufactured before 2018. If the applescript failed, then all fields
% of oldSettings are [].
%
%% BUIT-IN DOUBLE CHECKING. The display profile is a lookup table, it 
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
%% ERROR REPORTING is aggressive. Out-of-range or unrecognized arguments
% produce fatal errors if detected by MacDisplaySettings.m; when such
% errors are detected in xx.applescript they are merely flagged by a
% message in the optional output argument errorMsg. When throwing a fatal
% error, if you have Psychtoolbox installed, then MacDisplaySettings closes
% any open windows (by calling Psychtoolbox "sca"), so the error message
% won't be hidden behind your window.
%
%% REQUIREMENTS: macOS and MATLAB. (If it detects Psychtoolbox, then it will
% use the "sca" command to close windows before throwing a fatal error.) In
% its current form, MacDisplaySettings has only been tested on macOS Mojave
% (10.14) localized for USA. Earlier versions of this code supported macOS
% 10.9 to 10.14. It's designed to work internationally, but that hasn't
% been tested yet. It was tested on MATLAB 2019a, and very likely works on
% any version of MATLAB new enough to include structs. I think, but haven't
% checked, that the MATLAB code is pure basic MATLAB (no toolboxes) with
% one negligible exception. Before throwing an error, we check for the
% presence of the Psychtoolbox, if present then we call the Psychtoolbox
% routine "sca" (Screen Close All) to close any open windows, so the error
% won't be hidden behind your window. MacDisplaySettings.applescript needs
% only the macOS. It should work on any screen, including an external
% monitor, but it's only been tested on the main screen.
%
%% DEVELOPERS. To write Applescript like this, I strongly recommend that you
% buy the Script Debugger app from Late Night Software.
% https://latenightsw.com/ and the UI Browser app from UI Browser by
% PFiddlesoft. https://pfiddlesoft.com/uibrowser/ The Script Debugger is a
% the best Applescript editor and debugger. The UI Browser allows you to
% discover the user interface targets in System Preferences that your
% script will read and set. With it you can do in an hour what would
% otherwise take days of trial and error.
%
%% APPLE SECURITY. If the user has not yet given permission for MATLAB to
% control the computer (in System Preferences:Security &
% Privacy:Accessibility), then we give an error alerting the user to grant
% this permission. The error dialog window will say the application
% (MATLAB) is "not allowed assistive access." The application needs an
% administrator's permission to access the System Preferences. A user with
% admin privileges should then click as requested to provide that
% permission. This needs to be done only once for each application.
%
%% PROFILE ROW NUMBERING. Note that when you look at the list of profiles in
% System Preferences:Displays:Color there is a line separating the top and
% bottom sections of the list. Apple assigns a row number to that line, but
% trying to select that row has no effect and returns an error in errorMsg.
%
%% WHAT "BRIGHTNESS" CONTROLS: Adjusting the "brightness" setting in an LCD
% controls the luminance of the fluorescent light that is behind the liquid
% crystal display. I believe that the "brightness" slider controls only the
% luminance of the source, and does not affect the liquid crystal itsef,
% which is driven by the GPU output. The luminance at the viewer's eye is
% presumably the product of the two factors: luminance of the source and
% transmission of the liquid crystal, at each wavelength.
%
%% INSTALLATION. Just put both the MacDisplaySettings.m and
% MacDisplaySettings.applescript files anywhere in MATLAB's path.
%
%% MULTIPLE SCREENS: All my computers have only one screen, so I haven't 
% had an opportunity to test the screenNumber argument.
%
%% HISTORY
% June 25, 2017. denis.pelli@nyu.edu wrote "Brightness" for the
% Psychtoolbox, and later "AutoBrightness".
% July 16,2019 Improved by looking at code here:
% https://apple.stackexchange.com/questions/272531/dim-screen-brightness-of-mbp-using-applescript-and-while-using-a-secondary-mon/285907
% April 2020. Wrote MacDisplaySettings, based on Brightness, but enhanced
% to also support Automatically, True Tone, Night Shift, and Profile.
% April 14, 2020. Added wait loops (in the applescript) to wait for "tab
% group 1" before accessing it. This has nearly eliminated the occasional
% time out failures, in which MacDisplaySettings.m returns [] for
% brightness and automatic, but returns correct values for night shift.
% May 3, 2020. In the Applescript, I now "activate" System Preferences at
% the beginning (and reactivate the former app when we exit), and this runs
% much faster. Formerly, delays of 60 s were common, with occasional time
% outs. Now it reliably takes 2 s.
%
%% ACKNOWLEGEMENTS. Thanks to Mario Kleiner for explaining how macOS
% "brightness" works. Thanks to nick.peatfield@gmail.com for sharing his
% applescript code for dimmer.scpt and brighter.scpt. And to Hormet Yiltiz
% for noting that we need to control Night Shift.
%
%% SEE ALSO:
% Screen ConfigureDisplay? % In Psychtoolbox
% http://www.manpagez.com/man/1/osascript/
% https://developer.apple.com/library/mac/documentation/AppleScript/Conceptual/AppleScriptLangGuide/reference/ASLR_cmds.html
% https://discussions.apple.com/thread/6418291
% ScriptDebugger app from Late Night Software. 
% https://latenightsw.com/
% UIBrowser app from PFiddlesoft. 
% https://pfiddlesoft.com/uibrowser/

oldSettings.brightness=[];
oldSettings.automatically=[];
oldSettings.trueTone=[];
oldSettings.nightShiftSchedule=[];
oldSettings.nightShiftManual=[];
oldSettings.showProfilesForThisDisplayOnly=[];
oldSettings.profile=[];
oldSettings.profileRow=[];
failed=false;

if ~ismac
    % Applescript requires macOS.
    failed = true; % Report failure on this unsupported OS:
    return;
end

% The Psychtoolbox Screen function sets Brightness more quickly than
% MacDisplaySettings does, because applescript is slow, but we're setting
% five fields using one call to MacDisplaySettings (in about 3 s), whereas
% Screen.mex only sets Brightness, so there's no time savings in using
% Screen. Most of the time spent in MacDisplaySettings.applescript is in
% getting access to Screen Preferences. Once we're reading it, reading and
% writing extra fields doesn't take long.
useScreenBrightness=false;

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
if ~isstruct(newSettings)
    CloseWindows
    error('The newSettings argument must be a struct.');
end
if isfield(newSettings,'brightness') && ...
        ~isempty(newSettings.brightness) && ...
        (newSettings.brightness<0 || newSettings.brightness>1)
    CloseWindows
    error('newSettings.brightness %.1f must be in the range 0.0 to 1.0, otherwise [] to ignore it.',...
        newSettings.brightness)
end
if isfield(newSettings,'automatically') && ...
        ~isempty(newSettings.automatically) && ...
        ~ismember(newSettings.automatically,0:1) && ...
        ~islogical(newSettings.automatically)
    CloseWindows
    error('newSettings.automatically %.1f must be 0 or 1, otherwise [] or omitted to ignore it.',...
        newSettings.automatically)
end
if isfield(newSettings,'trueTone') && ...
        ~isempty(newSettings.trueTone) && ...
        ~ismember(newSettings.trueTone,0:1) && ...
        ~islogical(newSettings.trueTone)
    CloseWindows
    error('newSettings.trueTone %.1f must be 0 or 1, otherwise [] or omitted to ignore it.',...
        newSettings.trueTone)
end
if isfield(newSettings,'nightShiftSchedule') && ~isempty(newSettings.nightShiftSchedule) && ...
        ~ismember(newSettings.nightShiftSchedule,{'Off','Custom','Sunset to Sunrise'})
    CloseWindows
    error('newSettings.nightShiftSchedule %.1f must be ''Off'',''Custom'', or ''Sunset to Sunrise'', otherwise [] or omitted to ignore it.',...
        newSettings.nightShiftSchedule)
end
if isfield(newSettings,'nightShiftManual') && ...
        ~isempty(newSettings.nightShiftManual) && ...
        ~ismember(newSettings.nightShiftManual,0:1) && ...
        ~islogical(newSettings.nightShiftManual)
    CloseWindows
    error('newSettings.nightShiftManual %.1f must be 0 or 1, otherwise [] or omitted to ignore it.',...
        newSettings.nightShiftManual)
end
if isfield(newSettings,'showProfilesForThisDisplayOnly') && ...
        ~isempty(newSettings.showProfilesForThisDisplayOnly) && ...
        ~ismember(newSettings.showProfilesForThisDisplayOnly,0:1) && ...
        ~islogical(newSettings.showProfilesForThisDisplayOnly)
    CloseWindows
    error('newSettings.showProfilesForThisDisplayOnly %.1f must be 0 or 1, otherwise [] or omitted to ignore it.',...
        newSettings.showProfilesForThisDisplayOnly)
end
if isfield(newSettings,'profile') && ...
        ~isempty(newSettings.profile) && ...
        ~isfloat(newSettings.profile) && ...
        ~ischar(newSettings.profile)
    CloseWindows
    error('newSettings.profile ''%s'' must be Profile name, otherwise [] or omitted to ignore it.',newSettings.profile)
end
if isfield(newSettings,'profileRow') && ...
        ~isempty(newSettings.profileRow) && ...
        ~isfloat(newSettings.profileRow) && ...
        ~ischar(newSettings.profileRow)
    CloseWindows
    error('newSettings.profileRow %d must be Profile row number, otherwise [] or omitted to ignore it.',newSettings.profileRow)
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
    command = [command ' ' num2str(screenNumber)];
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
    command = [command ' ' num2str(newSettings.brightness) ' '...
        num2str(newSettings.automatically) ' ' ...
        num2str(newSettings.trueTone) ' '...
        num2str(newSettings.nightShiftSchedule) ' ' ...
        num2str(newSettings.nightShiftManual) ' '...
        num2str(newSettings.showProfilesForThisDisplayOnly) ' '...
        num2str(newSettings.profileRow) ' '...
        '"' newSettings.profile '"'];
    [failed,oldString]=system(command); % Takes 3 s on MacBook Pro.
    %     fprintf('%s\n',oldString);
    if failed
        CloseWindows
        errorMsg=oldString;
        error('Applescript error: failed=%d, oldString=%s.',...
            failed,oldString);
    end
    if streq('-99',oldString(1:3))
        CloseWindows
        warning(['If you haven''t already, please unlock System '...
            'Preferences: Security & Privacy: Privacy and give MATLAB ' ...
            'permission for Full Disk Access and Automation.']);
        error('Applescript returned error: %s',oldString);
    end
    errorMsg=regexprep(oldString,'"([^"]*)".*','$1','once');
    nextIndex=1+1+length(errorMsg)+3;
    [v,count,errMsg,extra]=sscanf(oldString(nextIndex:end),'%f, %d, %d, %d, %d, %d, %d, %d',7);
    nextIndex=nextIndex+extra-1;
    if count<7 || ~isempty(errMsg)
        warning('sscanf processed %d of 7 values. sscanf error: %s',...
            count,errMsg);
        fprintf('oldString=''%s''.\n',oldString);
    end
    oldSettings.profile=regexprep(oldString(nextIndex:end),...
        '"([^"]*)".*','$1','once');
    nextIndex=nextIndex+1+length(oldSettings.profile)+1-1;
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
        error('oldSettings.nightShiftSchedule is not an integer.');
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
        warning('Applescript failed. Here follows some diagnostic output.');
        failed
        oldString
        oldSettings
        CloseWindows
        error(['MacDisplaySettings.applescript failed. ' ...
            'Make sure you have admin privileges, ' ...
            'and that System Preferences is not tied up in a dialog. ' ...
            'Brightness applescript error: %s. '],oldString);
    end
catch ME
    CloseWindows
    failed=true;
    rethrow(ME);
end
end

function CloseWindows
if exist('PsychtoolboxVersion','file') && ~isempty(Screen('Windows'))
    % Close any user windows to make sure that our error message can be seen.
    sca
end
end
