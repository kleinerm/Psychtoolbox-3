-- AutoBrightness.applescript
-- Denis G. Pelli, denis.pelli@nyu.edu
-- February 29, 2016. 
--
-- HISTORY
-- May 21, 2015. First version.
-- May 29, 2015 Enhanced to allow specification of screenNumber.
-- June 1, 2015. Polished the comments.
-- July 25, 2015 Previously worked on Mavericks (Mac OS X 10.9). 
-- Now enhanced to also support Yosemite (Mac OS X 10.10.4).
-- August 3, 2015 Now uses try-blocks to try first group 1 and then group 2
-- to cope with variations in Apple's Mac OS. I find that under Mac OS X 10.9 
-- the checkbox is always in group 1. Under Mac OS X 10.10 I have previously
-- found it in group 2, but now I find it in group 1, so it seems best for the
-- program to keep trying groups until it finds the checkbox: first 1, then 2, then give up.
-- February 29, 2016 Improved the wording of the pop-up screen to spell 
-- out what the user needs to do to allow MATLAB to control the computer.
--
-- COMPATIBILITY
-- Works on Mavericks, Yosemite, and El Capitan (Mac OS X 10.9 to 10.11).
-- Not yet tested on earlier versions of Mac OS X (< 10.9).
-- I hope this will work internationally, with Macs running under Mac OS X
-- localized for any language, not just English. That is why we select
-- the Display/Colors panel by the internal name "displaysDisplayTab" 
-- instead using the localized name "Display". 
--
-- INTRODUCTION
-- This applescript allows you to disable (or re-enable) a feature of Apple 
-- Macintosh liquid crystal displays that is undesirable for vision experiments 
-- and display calibration. This applescript is equivalent to manually opening
-- the System Preference:Displays and turning on or off the checkbox for
-- "Automatic brightness adjustment". I wrote the script to be invoked from
-- MATLAB, but you could call it from any application running under Mac OS X.
-- While "Automatically adjust brightness" is checked, the Mac OS uses the
-- video camera to sense the room luminance and slowly dims the display if
-- the room is dark. It does this by adjusting the "brightness" setting,
-- which controls the luminance of the fluorescent light that is behind the
-- liquid crystal display. I believe that this "brightness" setting controls only the
-- luminance of the source, and does not affect the liquid crystal display,
-- which is controlled by the color lookup table. The screen luminance is 
-- presumably the product of the two factors: luminance of the source 
-- and transmission of the liquid crystal, at each wavelength.
--
-- BEWARE DELAY: This script uses the "System Preferences: Displays" panel,
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
-- returned value (0 or 1). However, the returned value is -99 if your 
-- application (e.g. MATLAB) does not have permission to control your computer 
-- (see APPLE SECURITY below).
--
-- In MATLAB, use the corresponding MATLAB Psychtoolbox function, which calls 
-- this script:
--
-- oldStatus=AutoBrightness(newStatus);
--
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
-- BEWARE OF DELAY: This script uses the "System Preferences: Displays" panel,
-- which takes 30 s to open, if it isn't already open.  You should either
-- open System Preferences in advance, or be prepared to wait 30 s when you
-- call this script. Whether or not System Preferences was already open, this script 
-- leaves it open, so you don't waste the observer's time waiting 30 s for System 
-- Preferences to open every time you call AutoBrightness. 
--
-- APPLE SECURITY. Unless the application (e.g. MATLAB) calling this script
-- has permission to control the computer, attempts to changes settings will be blocked.
-- In that case the appropriate Security and Privacy System Preference panel is opened 
-- and an error dialog window asks the user to provide the permission. A user with 
-- admin privileges should then click as requested to provide that permission. 
-- This needs to be done only once for each application that calls this script. 
-- The permission is remembered forever. Once permission granted, 
-- subsequent calls of this script will work. Note that a user lacking admin access 
-- is unable to grant the permission; in that case every time you call this 
-- script, you'll get the error dialog window.
-- 
-- SCRIPTING CONTROL ALLOWED? 
-- You can call Denis Pelli's innocuous script "ScriptingAllowed.applescript" to find out
-- whether scripting permission has been granted. http://psych.nyu.edu/pelli/software.html
--
-- MULTIPLE SCREENS: All my computers have only one screen, so 
-- I haven't yet tested it with values of screenNumber other than zero.
--
-- BRIGHTNESS: You may also want to read and set the display brightness as
-- indicated by the 16-position "brightnes" slider on the System Preferences:Displays
-- panel. I have commented out a line, "set slider 1", in the script below,
-- which would implement that feature. However, the Psychtoolbox for MATLAB
-- and Macintosh already has a Screen call to get and set the brightness, so
-- we don't need applescript for that.
--
-- THANKS. Thanks to Mario Kleiner for explaining how Mac OSX "brightness" works.
-- Thanks to nick.peatfield@gmail.com for sharing his applescript code for dimmer.scpt and brighter.scpt.
-- 
-- SEE ALSO:
-- ScriptingAllowed.applescript (http://psych.nyu.edu/pelli/software.html)
-- The Psychtoolbox call to get and set the Macintosh brightness:
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
				display alert "To set Displays preferences, " & applicationName & " needs your permission to control this computer.  BEFORE you click OK below, please unlock the Privacy panel and click the box that allows the app to control your computer. THEN click OK."
				delay 1
			end tell
			return -99
		end if
		tell process "System Preferences"
			--set os_version to do shell script "sw_vers -productVersion"
			set versionString to system version of (system info)
			considering numeric strings
				--set isYosemiteOrBetter to os_version ≥ "10.10.0"
				set isYosemiteOrBetter to versionString ≥ "10.10.0"
			end considering
			--if not isYosemiteOrBetter then
			--Mac OS X 10.9 Mavericks, or earlier.
			tell tab group 1 of window windowNumber
				--click radio button "Display"-- commented out because it won't work in non-English installations
				try
					tell group 1 -- works on Mac OS X 10.9 Mavericks and sometimes Yosemite
						--set slider 1's value to 0.5 -- Set brightness
						tell checkbox 1 -- Automatically adjust brightness  
							set oldStatus to value
							if newStatus is in {0, 1} and newStatus is not oldStatus then
								click -- It's wrong, so change it.
							end if
						end tell
					end tell
				on error
					try
						tell group 2 -- works other times on Mac OS X 10.10 Yosemite
							--set slider 1's value to 0.5 -- Set brightness
							tell checkbox 1 -- Automatically adjust brightness  
								set oldStatus to value
								if newStatus is in {0, 1} and newStatus is not oldStatus then
									click -- It's wrong, so change it.
								end if
							end tell
						end tell
					on error
						set oldStatus to 0
					end try
				end try
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
