-- AutoBrightness.applescript
-- Denis G. Pelli denis.pelli@nyu.edu
-- St. Pete Beach, Florida, May 21, 2015. 
-- Enhanced May 29, 2015
-- Allow specification of screenNumber.
-- For language independence, now select the Display/Colors panel by the internal 
-- name "displaysDisplayTab" instead using the localized name "Display". 
--
-- This applescript allows you to disable a feature of Apple Macintosh
-- liquid crystal displays that is undesirable for vision experiments and
-- display calibration. This applescript is equivalent to manually opening
-- the System Preference:Displays and turning on or off the checkbox for
-- "Automatic brightness adjustment". I wrote the script to be invoked from
-- MATLAB, but you could call in from any application running under Mac OSX.
-- While "Automatically adjust brightness" is checked, the Mac OS uses the
-- video camera to sense the room luminance and slowly dims the display if
-- the room is dark. It does this by adjusting the "brightness" setting,
-- which controls the luminance of the fluorescent light that is behind the
-- liquid crystal display. I believe that "brightness" controls only the
-- luminance of the source, and does not affect the liquid crystal display,
-- which is controlled by the color lookup table. The luminance at the
-- viewer's eye is presumably the product of the two factors: luminance of
-- the source and transmission of the liquid crystal, at each wavelength.
--
-- CAUTION: This script uses the "System Preferences: Displays" panel,
-- which takes 30 s to open, if it isn't already open.  You should either
-- open System Preferences in advance, or be prepared to wait 30 s when you
-- call this script. If System Preferences was already open, then this script 
-- leaves it open. If it was not already open, there is an option in the code,
-- "leaveSystemPrefsRunning", which I set to true, so you don't waste the
-- observer's time waiting 30 s for System Preferences to open every time
-- you call AutoBrightness.
--
-- AutoBrightness screenNumber newStatus
-- The parameter "newStatus" (integer 0 or 1) indicates whether you want to
-- turn the autobrightness feature on (newStatus==1) or off (newStatus==0).
-- If  the newStatus argument is omitted (or anything other than 0 or 1)
-- then nothing is changed, and the current state is reported in the
-- returned value (0 or 1). 
--
-- However, the returned value is -99 if your application (e.g. MATLAB)
-- does not have permission to control your computer, and thus cannot script System Preferences.
-- In that case the appropriate Security and Privacy System Preference panel is opened 
-- and the user is asked to provide the permission. Once permission granted, 
-- calling this script will work. Note that the user may not have admin access, and 
-- thus may be unable to grant the permission.
-- In that case every time you call this script, you'll get the error dialog window.
--
-- You can call the innocuous script "ScriptingAllowed.applescript" to find out
-- whether scripting permission has been granted.
--
-- In MATLAB, use the corresponding MATLAB Psychtoolbox function, which calls 
-- this script:
-- oldStatus=AutoBrightness(newStatus);
-- To call this directly from MATLAB, 
-- [status,oldAuto]=system('osascript AutoBrightness.applescript 0'); % to disable
-- [status,oldAuto]=system('osascript AutoBrightness.applescript 1'); % to enable
-- system(['osascript AutoBrightness.applescript ' num2str(oldAuto)]); % to restore
-- Use from any other language is very similar.
-- Ignore the returned "status", which seems to always be zero.
-- The string argument to system() is passed without processing by MATLAB.
-- It appears that MATLAB's path is not used in finding the script,
-- "Autobrightness.applescript". When I don't specify a path for the
-- applescript file, it appears that system() assumes
-- that it's in /User/denispelli/Documents/MATLAB/
-- I succeeded in using my applescript from an arbitrary location by
-- specifying its full path. (See AutoBrightness.m.) 
--
-- APPLE SECURITY. The first time any application (e.g. MATLAB) calls this
-- script, the request will be blocked and an error dialog window will
-- appear saying the application is "not allowed assistive access." This
-- means that the application needs an administrator's permission to access
-- the System Preferences. A user will admin privileges should then click as
-- requested to provide that permission. This needs to be done only once for each
-- application that calls this script. The permission is remembered forever.
-- 
-- MULTIPLE SCREENS: All my computers have only one screen, so 
-- I haven't yet tested it with values of screenNumber other than zero.
--
-- You may also want to read and set the display brightness as
-- indicated by the 16-position "brightnes" slider on the System Preferences:Displays
-- panel. I have commented out a line, "set slider 1", in the script below,
-- which would implement that feature. However, the Psychtoolbox for MATLAB
-- and Macintosh already has a Screen call to get and set the brightness, so
-- we don't need applescript for that.
--
-- Thanks to Mario Kleiner for explaining how Mac OSX "brightness" works.
-- Thanks to nick.peatfield@gmail.com for sharing his applescript code for dimmer.scpt and brighter.scpt.
-- See also:
-- The Psychtoolbox call to get and set the Macintosh
-- [oldBrightness]=Screen('ConfigureDisplay','Brightness', screenId [,outputId][,brightness]);
-- http://www.manpagez.com/man/1/osascript/
-- https://developer.apple.com/library/mac/documentation/AppleScript/Conceptual/AppleScriptLangGuide/reference/ASLR_cmds.html
-- https://discussions.apple.com/thread/6418291

on run argv
	-- integer screenNumber. Zero for main screen. Default is zero.
	--  integer newStatus : state of the System Preferences:Displays:checkbox "Automatically adjust brightness"
	--     0 : off (unchecked)  
	--     1 : on (checked)  
	-- Default is to leave the status unchanged.
	try
		set newStatus to item 2 of argv as integer
	on error
		set newStatus to -1 -- Unspecified value, so don't change the setting.
	end try
	try
		set screenNumber to item 1 of argv as integer
	on error
		set screenNumber to 0 -- Default is the main screen.
	end try
	set windowNumber to screenNumber + 1 -- Has been tested only for screenNumber==0
	
	set leaveSystemPrefsRunning to true -- this could be made a third argument
	tell application "System Preferences"
		set wasRunning to running
		set the current pane to pane id "com.apple.preference.displays"
		reveal (first anchor of current pane whose name is "displaysDisplayTab")
	end tell
	tell application "System Events"
		set applicationName to item 1 of (get name of processes whose frontmost is true)
		if not UI elements enabled then
			tell application "System Preferences"
				activate
				reveal anchor "Privacy_Accessibility" of pane id "com.apple.preference.security"
				display alert "To set Displays preferences, " & applicationName & " needs your permission to control the computer.  Please check the appropriate box to allow this. Then try again."
				delay 1
			end tell
			return -99
		end if
		tell process "System Preferences"
			tell tab group 1 of window windowNumber
				--click radio button "Display"-- commented out because it won't work in non-English installations
				tell group 1
					try
						--set slider 1's value to 0.5 -- Set brightness
						tell checkbox 1 -- Automatically adjust brightness  
							set oldStatus to value
							if newStatus is in {0, 1} and newStatus is not oldStatus then
								click -- It's wrong, so change it.
							end if
						end tell
					on error
						set oldStatus to 0
					end try
				end tell
			end tell
		end tell
	end tell
	if wasRunning or leaveSystemPrefsRunning then
		-- Leave it running.
	else
		quit application "System Preferences"
	end if
	return oldStatus
end run
