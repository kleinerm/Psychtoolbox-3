function [ch, when] = GetChar(getExtendedData, getRawCode)
% [ch, when] = GetChar([getExtendedData], [getRawCode])
% 
% Wait for a typed character and return it.  If a character was typed
% before calling GetChar then GetChar will return that character immediatly.
% Characters flushed by FlushEvents are all ignored by GetChar. Characters
% are returned in the first return argument "ch".
% 
% CAUTION: Do not rely on the keypress timestamps returned by GetChar
% without fully reading and understanding this help text. Run your own
% timing tests on GetChar and KbCheck to verify that the timing is good
% enough and avoid GetChar for timed keypresses if possible at all. Use
% KbWait and KbCheck instead.
%
% The main purpose of GetChar is to reliably collect keyboard input in the
% background while your experiment script is occupied with performing other
% operations, e.g., Matlab computations, sound output or visual stimulus
% drawing. After an initial call to ListenChar, the operating system will
% record all keyboard input into an internal queue. GetChar removes
% characters from that queue, one character per invocation of GetChar. You
% can empty that queue any time by calling FlushEvents('keyDown').
%
% If you want to check the current state of the keyboard, e.g., for triggering
% immediate actions in response to a key press, waiting for a subjects
% response, synchronizing to keytriggers (e.g., fMRI machines) or if you
% require high timing precision then use KbCheck instead of GetChar.
%
% GetChar works on all platforms with Matlab and Java enabled. It works
% also on M$-Windows in "matlab -nojvm" mode. It does not work on MacOS-X or
% Linux in "matlab -nojvm" mode and it also doesn't work under GNU/Octave.
%
% "when" is a struct. It returns the time of the keypress, the "adb"
% address of the input device, and the state of all the modifier keys
% (shift, control, command, option, alphaLock) and the mouse button. If you
% have multiple keyboards connected, "address" may allow you to distinguish
% which is responsible for the key press. "when.secs" is an estimate,
% of what GetSecs would have been. Since it's derived from a timebase
% different from the timebase of GetSecs, times returned by GetSecs are not
% directly comparable to when.secs.
%
% By setting getExtendedData to 0, all extended timing/modifier information
% will not be collected and "when" will be returned empty.  This speeds up
% calls to this function. If ommitted or set to 1, the "when" data structure
% is filled.  getRawCode set to 1 will set "ch" to be the integer ascii code
% of the available character.  If ommitted or set to 0, "ch" will be in
% char format. When running under Windows in "matlab -nojvm" mode, "when"
% will be returned empty.
%
% GetChar and CharAvail are character-oriented (and slow), whereas KbCheck
% and KbWait are keypress-oriented (and fast). If only a meta key (like
% <option> or <shift>) was hit, KbCheck will return true, because a key was
% pressed, but CharAvail will return false, because no character was
% generated. See KbCheck.
% 
% CharAvail and GetChar use the system event queue to retrieve the character
% generated, not the raw key press(es) per se. If the user presses "a",
% GetChar returns 'a', but if the user presses option-e followed by "a",
% this selects an accented a, "?", which is treated by GetChar as a single
% character, even though it took the user three keypresses (counting the
% option key) to produce it.
% 
% There can be some delay between when the key is pressed and when CharAvail
% or GetChar detects it, due to internal processing overhead in Matlabs Java
% implementation. GetChar internally collects timestamps in the timebase
% used by Matlabs Java implementation, whereas other Psychtoolbox timing functions
% (GetSecs, Screen('Flip'), KbCheck, KbWait, ...) use time reported by some
% high precision system timer. The "when.secs" time reported by GetChar is
% converted from Java timebase to Psychtoolboxs timebase. Due to conversion
% errors mostly out of our control, the reported values can be off by
% multiple dozen or even hundreds of milliseconds from what KbWait, KbCheck
% or GetSecs would report. Example: A high-end Pentium-4 3.2 Ghz system
% running Windows-XP has been measured to be off by 40 to 70 milliseconds.
%
% Some Java implementations are also known to have problems/bugs in
% timestamping keyboard presses properly and each Matlab version on each
% operating system is bundled with a different Java version, so some Matlab
% versions may be reliable with respect to GetChars timing, whereas others
% are not.
%
% ---> If precise timing of the keypress is important, use KbCheck or
% KbWait for consistent results!
%
% OS X / Windows / Linux with Matlab and Java enabled: ____________________
%
% JAVA PATH: The GetChar implementation is based on Java. Therefore, the
% Psychtoolbox subfolder PsychJava must be added to Matlabs static
% classpath. Normally this is done by the Psychtoolbox installer by editing
% the Matlab file "classpath.txt" (enter which('classpath.txt') to find the
% location of that file). If the installer fails to edit the file properly,
% you'll need to perform that step manually by following the instructions
% of the installer. See 'help PsychJavaTrouble' for more infos on this.
%
% KEYSTROKES IN THE BACKGROUND: Under OS 9, keyboard input is automatically
% directed to the Getchar queue while a script or function executes in
% MATLAB.  Therefore, your script may do work and then call GetChar to read
% keys pressed while the script worked.  OS X is the same, except that, to
% detect keypresses made before the GetChar call, you must have called
% "ListenChar" earlier.  ListenChar redirects keystrokes to the GetChar
% queue. Calling ListenChar at the begining of your OS X script should
% cause GetChar to behave identically to OS 9 GetChar with respect to
% background keystroke collection.  
%
% KEYSTROKES IN MATLAB WINDOW: By default, all keystrokes are also sent to
% Matlabs window, generating some ugly clutter. You can suppress this by
% calling ListenChar(2), so your Matlab console stays nice and clean. Don't
% forget to call ListenChar(1) or ListenChar(0) though before the end of
% your script. If Matlab returns to its command prompt without reenabling
% keyboard input via ListenChar(0) or ListenChar(1), Matlab will be left
% with a dead keyboard until you press the CTRL+C key combo.
%
% OTHER "when" RETURN ARGUMENT FIELDS: Owing to differences in what
% accessory information the underlying operating systems provides about
% keystrokes, "when' return argument fields differs between OS 9 and OS X.
% GetChar sets fields for which it returns no value to the value 'Nan'.  
%
% OS 9: ___________________________________________________________________
%
% Command-Period always causes an immediate exit.
% When BACKGROUNDING is enabled, Matlab removes all characters from the
% event queue before executing each Matlab statement, so CharAvail and
% EventAvail('keyDown') always report 0. So turn off BACKGROUNDING:
% 	Screen('Preference','Backgrounding',0); 
% _________________________________________________________________________
%
% See also: ListenChar, CharAvail, FlushEvents, TestGetChar, KbCheck,
% KbWait



% 5/7/96  dgp	Wrote this help file.
% 1/22/97 dhb	Added comment and pointer to TIMER routines.
% 3/6/97  dhb	References to KbWait, KbCheck.
% 7/23/97 dgp	It's a character not a keypress.
% 8/2/97  dgp	Explain difference between key and character. See KbCheck.
% 2/7/98  dgp	Streamlined. Eliminated call to GetKey, since it's now GetChar.mex.
% 3/24/98 dgp	Explain backgrounding and meta keys. Don't mention obsolete GetKey and KbHit.
% 3/15/99 xmz	Put in some comment for Windows version.
% 3/19/99 dgp	Update explanation of backgrounding. 
% 3/28/99 dgp	Show how to turn off backgrounding. 
% 8/19/00 dgp	Cosmetic. 
% 4/23/02 dgp   Fixed GetChar.mex to always quit on command-period.
% 4/27/02 dgp	Added optional second return argument.
% 6/1/02  dgp	Mention Tick0Secs.
% 9/21/02 dgp   Added address field to "when", using C code donated by Tom Busey.
% 7/12/04 awi   ****** OS X-specific fork from the OS 9 version *******
%                   Expanded on explantion in the first paragraph about when
%                   GetChar waits, when it returns immediatly, what
%                   it ignores.  Retains OS 9-specific comments.    
% 1/27/04 awi   Issue an error when calling GetChar and suggest KbWait. 
% 1/19/05 awi   Implemented GetChar on OS X.  Added AssertMex for OS 9 and OS X conditional block.
% 7/20/05 awi   Wrote OS X documentation section.
% 2/2/06  awi   Tested to see if this works when the MATLAB text editing
%               window is minimized. It does not.
% 2/22/06 awi  Commented out Cocoa wrapper and wrote Java wrapper.
% 3/28/06 awi  Detect buffer overflow.
%              Handle new double value from .getChar(), was char type.
%              Changed "char" return value to "ch" to avoid name conflict with
%               built-in MATLAB function "char" 
% 6/15/06 awi  Added a second return argument.
%              Updated built-in help for the Java implementation.
% 6/15/06 awi  Added break on CTRL-C
%              Added TO DO section and item to detect genuine KeyDown
%              events.
% 6/20/06 awi  Use AddPsychJavaPath instead of AssertGetCharJava.
% 8/16/06 cgb  Now using the new GetChar system which taps straight into
%              the java keypress dispatcher.
% 9/18/06  mk  GetChar now works on all Matlabs (OS-X, Windows) in JVM
%              mode. In -nojvm mode on Windows, it falls back to the old
%              Windows DLL ...



% TO DO
% 
% The Java script currently detects key events, not keydown events.  This
% should be modified to detect keydown events and tested. That change might also
% allow us to detect the "address" returned in the second argument.


% NOTES:
%
% The second return argument from OS 9 PTB looks like this:
%     ticks: 5760808
%     secs: 1.4681e+05
%     address: 2
%     mouseButton: 0
%     alphaLock: 0
%     commandKey: 0
%     controlKey: 0
%     optionKey: 0
%     shiftKey: 0
% 

global OSX_JAVA_GETCHAR;

% Java based GetChar is only supported on Matlab, not on Octave, due to
% Octaves lack of a Java virtual machine.
if ~IsOctave
    % This is Matlab. Is the Java VM and AWT running?
    if psychusejava('awt')
        % Java virtual machine and AWT are running. Use our Java based
        % GetChar.
        
        % If no command line argument was passed we'll assume that the user only
        % wants to get character data and timing/modifier data.
        if nargin == 0
            getExtendedData = 1;
            getRawCode = 0;
        elseif nargin == 1
            getRawCode = 0;
        end

        % Make sure that the GetCharJava class is loaded and registered with
        % the java focus manager.
        if isempty(OSX_JAVA_GETCHAR)
            try
                OSX_JAVA_GETCHAR = AssignGetCharJava;
            catch
                error('Could not load Java class GetCharJava! Read ''help PsychJavaTrouble'' for help.');
            end
            OSX_JAVA_GETCHAR.register;
        end

        % Loop until we receive character input.
        keepChecking = 1;
        while keepChecking
            % Check to see if a character is available, and stop looking if
            % we've found one.
            charValue = OSX_JAVA_GETCHAR.getChar;
            keepChecking = charValue == 0;
        end

        % Throw an error if we've exceeded the buffer size.
        if charValue == -1
            % Reenable keystroke dispatching to Matlab to leave us with a
            % functional Matlab console.
            OSX_JAVA_GETCHAR.setRedispatchFlag(0);
            error('GetChar buffer overflow. Use "FlushEvents" to clear error');
        end

        % Get the typed character.
        if getRawCode
            ch = charValue;
        else
            ch = char(charValue);
        end

        % Only fill up the 'when' data stucture if extended data was requested.
        if getExtendedData
            when.address=nan;
            when.mouseButton=nan;
            when.alphaLock=nan;
            modifiers = OSX_JAVA_GETCHAR.getModifiers;
            when.commandKey = modifiers(1);
            when.controlKey = modifiers(2);
            when.optionKey = modifiers(3);
            when.shiftKey = modifiers(4);
            rawEventTimeMs = OSX_JAVA_GETCHAR.getEventTime;  % result is in units of ms.
            when.ticks = nan;
            when.secs = JavaTimeToGetSecs(rawEventTimeMs, -1);
        else
            when = [];
        end
    else
        % Java VM unavailable, i.e., running in -nojvm mode.
        % On Windows, we can fall back to the old GetChar.dll, although we
        % only get info about typed characters, no 'when' extended data.
        if IsWin
            % GetChar.dll has been renamed to GetCharNoJVM.dll. Call it.
            ch = GetCharNoJVM;
            when = [];
            return;
        else
            % There's no replacement for Java GetChar on OS-X or Linux :(
            error('Sorry! GetChar is not supported in ''matlab -nojvm'' mode on MacOS-X or GNU/Linux.');
        end
    end
else
    % Running on Octave! That is a no go.
    error('Sorry! GetChar is not yet supported on GNU/Octave.');
end
