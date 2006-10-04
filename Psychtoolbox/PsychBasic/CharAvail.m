function [avail, numChars] = CharAvail
% [avail, numChars] = CharAvail
%
% CharAvail returns the availability of characters in the keyboard event
% queue and sometimes the queue's current size. "avail" will be 1 if characters are
% available, 0 otherwise.  "numChars" may hold the current number of
% characters in the event queue, but in some system configurations it is
% just empty, so do not rely on numChars providing meaningful results,
% unless you've tested it on your specific setup.
%
% Note that this routine does not actually remove characters from the event
% queue.  Call GetChar to remove characters from the queue.
%
% CharAvail works on all platforms with Matlab and Java enabled. It works
% also on M$-Windows under matlab -nojvm mode. It does not work on OS-X or
% Linux in Matlab -nojvm mode and it also doesn't work under GNU/Octave.
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
% 8/16/06   cgb Now using a much faster method to get characters in Matlab.
%               We now read straight from the keyboard event manager in
%               java.
% 9/18/06  mk   CharAvail now works on all Matlabs (OS-X, Windows) in JVM
%               mode. In -nojvm mode on Windows, it falls back to the old
%               Windows DLL ...

global OSX_JAVA_GETCHAR;

if IsOS9
    avail = EventAvail('keyDown');
else
    if ~IsOctave
        if psychusejava('awt')
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

            % Check to see if any characters are available.
            avail = OSX_JAVA_GETCHAR.getCharCount;

            % Make sure that there isn't a buffer overflow.
            if avail == -1
                error('GetChar buffer overflow. Use "FlushEvents" to clear error');
            end

            numChars = avail;
            avail = avail > 0;
        else
            % Java VM unavailable, i.e., running in -nojvm mode.
            % On Windows, we can fall back to the old CharAvail.dll.
            if IsWin
                % CharAvail.dll has been renamed to CharAvailNoJVM.dll. Call it.
                avail = CharAvailNoJVM;
                numChars = [];
                return;
            else
                % There's no replacement for Java GetChar on OS-X or Linux :(
                error('Sorry! CharAvail is not supported in ''matlab -nojvm'' mode on MacOS-X or GNU/Linux.');
            end
        end
    else
        % Running on Octave! That is a no go.
        error('Sorry! CharAvail is not yet supported on GNU/Octave.');
    end
end
