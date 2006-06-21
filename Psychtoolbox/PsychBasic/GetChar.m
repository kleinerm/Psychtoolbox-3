function [ch,when] = GetChar
% [ch,when] = GetChar
% 
% Wait for a typed character and return it.  If a character was typed
% before calling GetChar then GetChar will return immediatly.  Characters
% typed into the MATLAB command line while no function is executing,  typed
% into the editor window, or flushed by FlushEvents are all ignored by
% GetChar.
% 
% "when" is a struct. It returns the time of the keypress, the "adb"
% address of the input device, and the state of all the modifier keys
% (shift, control, command, option, alphaLock) and the mouse button. If you
% have multiple keyboards connected, "address" may allow you to distinguish
% which is responsible for the key press. "when.ticks" is the value of
% GetTicks when the key was pressed. "when.secs" is an estimate, based on
% when.ticks, of what GetSecs would have been. Since it's derived from a
% tick count, it's coarsely quantized in steps of 1/60.15 s. If you plan to
% use the value of when.secs then you should make sure that the
% Psychtoolbox has a fresh estimate of tick0secs by calling
% Screen('Preference','Tick0Secs',nan).  
% 
% GetChar and CharAvail are character-oriented (and slow), whereas KbCheck
% and KbWait are keypress-oriented (and fast). If only a meta key (like
% <option> or <shift>) was hit, KbCheck will return true, because a key was
% pressed, but CharAvail will return false, because no character was
% generated. See KbCheck.
% 
% CharAvail and GetChar use the Event Manager to retrieve the character
% generated, not the raw key press(es) per se. If the user presses "a",
% GetChar returns 'a', but if the user presses option-e followed by "a",
% this selects an accented a, "‡", which is treated by GetChar as a single
% character, even though it took the user three keypresses (counting the
% option key) to produce it.
% 
% CharAvail and GetChar call the Event Manager, which allows the system to
% get control. Sometimes CharAvail will take tens of milliseconds to
% return, so don't use CharAvail in real-time loops. And there can be some
% delay between when the key is pressed and when CharAvail or GetChar
% detects it. If precise timing of the keypress is important, use KbCheck
% or KbWait.
%
% OS X: ___________________________________________________________________
%
% KEYSTROKES IN THE BACKGROUND: Under OS 9, keyboard input is automatically
% directed to the Getchar queue while a script or function executes in
% MATLAB.  Therefore, your script may do work and then call GetChar to read
% keys pressed while the script worked.  OS X is the same, except that, to
% detect keypresses made before the GetChar call, you must have called
% "ListenChar" earlier.  ListenChar redirects keystrokes to the GetChar
% queue. Calling ListenChar at the begining of your OS X script should
% cause GetChar to behave identically to OS 9 GetChar with respect to
% background keystroke collection.  Note that clicking on a MATLAB window
% assigns it key focus, redirecting keyboard input away from the GetChar
% queue and to the MATLAB window, undoing ListenChar.  
%
% OTHER "when" RETURN ARGUMENT FIELDS: Owing to differences in what
% accessory information the underlying operating systems provides about
% keystrokes, "when' return argument fields differ sbetween OS 9 and OS X.
% GetChar sets fields for which it returns no value to value Nan.  
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
% See also: ListenChar, CharAvail, EventAvail, GetCharTest, KbCheck



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


AssertMex('OS9');

if(IsOSX)
    AddPsychJavaPath;
    ListenChar;
    global PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW; %contents of var previosly instantiated by ListenChar.
    stopLoop=0;
    while ~stopLoop
        %restore window focus if necessary
        if ~PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.isWindowFocused()
            PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.windowFocusOn();
            PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.typingAreaFocusOn()
        end
        %get the character and set the stop bit according to its value
        charValue=PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.getChar();
        stopLoop= charValue~=0;  
    end
    if charValue==-1
        error('GetChar buffer overflow. Use "FlushEvents(''KeyDown'')" to clear error');  
    else
        ch=char(charValue);
        when.address=nan;
        when.mouseButton=nan;
        when.alphaLock=nan;
        when.commandKey=PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.getModifierCommand();
        when.controlKey= PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.getModifierControl();
        when.optionKey= PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.getModifierOptionAlt();
        when.shiftKey= PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.getModifierShift();
        rawEventTimeMs= PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.getEventTime();  % result is in units of ms.
        when.ticks=nan;
        when.secs=JavaTimeToGetSecs(rawEventTimeMs);
    end
    % Java returns ascii 3, the "end of text" character for ctrl  C instead of ascii 99, "c".   
    if double(ch)==3 && when.controlKey
        error('<ctrl>-C break');
    end
end



% This material was deleted from the built-in help when switching to the
% Java implementaiton.  It should be added back when we conditionally
% restore the Cocoa impelmentation.  

% TIMESTAMPS: The "when" return argument contains both "ticks" and "secs"
% fields recording the times of keystrokes in units of Carbon system ticks
% and seconds, respectively.  While OS 9 GetChar returns the same fields,
% there are differences in precision between OS 9 and OS X values.  The OS
% 9 operating system timestamps events in units of system ticks, with a
% precision no better than 1/60.15 second.  GetChar converts ticks to
% seconds, which yields a product of only GetTicks, not GetSecs precision.
% The OS X operating system timestamps events in units of seconds with the
% much higer precision of GetSecs. That precision, typically microseconds
% or better, depends on hardware.  (Call "GetSecsTick" to find the
% precision of GetSecs on your system.) On OS X, the precision of GetChar
% timestamps might be limited by the USB keyboard, usually no better than
% 10ms.
%
% UNICODE CHARACTERS: Some keyboard keys, such as the function keys,
% return unicode characters outside of the 8-bit UTF-8 range.  MATLAB can
% not correctly display these characters.  For unicode keys which MATLAB
% can not display, GetChar returns the numeric unicode value of the
% character.  
% KNOWN BUGS:
%
% GetChar does not exit on ctrl-c or command-period.






% This is the OLD version of MacOSX GetChar.  It used Cocoa events but did
% not work, apparently because of interference with the MATLAB editor
% window.  Left here as fallback until the Java version is complete. - awi
%
% if(IsOSX)
%     InitCocoaEventBridge;
%     [char, when]=CocoaEventBridge('GetChar');
%     [callStack, stackIndex]=dbstack;
%     if(length(callStack) == 1)
%        CocoaEventBridge('RevertKeyWindow');
%     end
% end
