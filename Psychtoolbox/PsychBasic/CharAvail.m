function avail = CharAvail
% avail = CharAvail
% 
% Return 1 if a character is available in the event queue, 0 if not. Note
% that this routine leaves the character in the queue.  Call GetChar to
% remove the character from the event queue.
% 
% Mac:
% 	Command-Period always causes an immediate exit.
% 
% 	GetChar and CharAvail are character-oriented (and slow), whereas KbCheck
% 	and KbWait are keypress-oriented (and fast). See KbCheck.
%
% 	WARNING: When BACKGROUNDING is enabled, Matlab removes all
% 	characters from the event queue before executing each Matlab
% 	statement, so CharAvail and EventAvail('keyDown') always report 0.
% 	So turn off BACKGROUNDING:
% 
% 	Screen('Preference','Backgrounding',0); % Until Matlab 5.2.1, this call required a disk access, which is slow.
% 
% See also: GetChar, ListenChar, FlushEvents, KbCheck, KbWait, KbDemo, Screen Preference Backgrounding.

% 11/5/94   dhb Added caveat about delay.
% 1/22/97   dhb Added comment and pointer to TIMER routines.
% 3/6/97    dhb Updated for KbWait, KbCheck.
% 8/2/97    dgp Explain difference between key and character. See KbCheck.
% 8/16/97   dgp Call the new EventAvail.mex instead of the obsolete KbHit.mex.
% 3/24/98   dgp Explain backgrounding. Omit obsolete GetKey and KbHit.
% 3/19/99   dgp Update explanation of backgrounding. 
% 3/28/99   dgp Show how to turn off backgrounding.
% 3/8/00    emw Added PC comments
% 3/12/00   dgp Fix platform dependency test.
% 9/20/05   awi Removed outdated notice in help mentioning problems with
%                   CharAvail on Windows.  The Problem was fixed.
%               Added platform conditional for OS X ('MAC').
% 1/22/06   awi Commented out Cocoa wrapper version and wrote Java wrapper.
% 3/28/06   awi Detect buffer overflow.
% 6/20/06   awi Use AddPsychJavaPath instead of AssertGetCharJava.



if IsOS9
    avail=EventAvail('keyDown');
elseif IsOSX
    AddPsychJavaPath;
    %AssertGetCharJava;
    global PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW  
    if isempty(PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW)
        avail=0;
    else
        numCharsAvail=PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.numChars();
        if numCharsAvail==-1
            error('GetChar buffer overflow. Use "FlushEvents(''KeyDown'')" to clear error'); 
        end
        avail= numCharsAvail > 0;
    end
end





% This is the old Cocoa wrapper version. Retained here in case we have to resort to it.
% 
% if strcmp(computer,'MAC2')
%    avail=EventAvail('keyDown');
% elseif strcmp(computer, 'MAC')
%     InitCocoaEventBridge;
%     avail=CocoaEventBridge('CharAvail');
% end
    

