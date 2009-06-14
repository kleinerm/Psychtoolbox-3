function FlushEvents(varargin)
% FlushEvents(['mouseUp'],['mouseDown'],['keyDown'],['autoKey'],['update'],...)
% 
% Remove events from the system event queue.
%
% Windows + Matlab in -nojvm mode: ________________________________________
%
% Removes all events of the specified types from the event queue.
% The arguments can be in any order. Empty strings are ignored.
%
% Windows, Linux, OS-X under Matlab with Java enabled: ____________________
%
% FlushEvents will accept all arguments, but only 'keyDown' (or no
% argument at all) removes keypress events. Events other than keypress
% events are not supported.
%
% Octave or Matlab in -nojvm mode under Linux and OS-X: ___________________
%
% FlushEvents, GetChar and CharAvail are not supported under GNU/Octave or
% under Matlab running in -nojvm mode, except for the Microsoft Windows
% version.
% 
% See also: GetChar, CharAvail, FlushEvents, EventAvail.

% 3/25/97  dgp	Wrote it.
% 9/20/05  awi  Added AssertMex call for OS 9 and Win and added OS X
%                   conditional.
% 1/22/06  awi  Commented out Cocoa wrapper and wrote Java wrapper.
% 6/20/06  awi  Use AddPsychJavaPath instead of AssertGetCharJava.
% 8/16/06  cgb  Now using the new GetChar system.
% 9/18/06  mk   FlushEvents now works on all Matlabs (OS-X, Windows) in JVM
%               mode. In -nojvm mode on Windows, it falls back to the old
%               Windows FlushEvents.dll ...
%               We now check for valid event descriptors.
%
% 11/14/06 mk   Ugly while CharAvail, GetChar; hack to fix more GetChar brain-damage.
% 09/21/07 mk   Added a drawnow() on top to prevent Matlabs GUI from
%               receiving mouse clicks that were meant to be only processed by the
%               experiment script. In other words: Fix more Matlab
%               brain-damage.
%
% 05/31/09 mk   Add support for Octave and Matlab in noJVM mode.

global OSX_JAVA_GETCHAR;

% We only flush the character queue if we are either called without
% any arguments, or an empty argument string (which means: Flush
% all events), or if one of the arguments is the 'keyDown' event.
doclear = 0;
if length(varargin)==0 %#ok<ISMT>
    doclear = 1;
else
    for i=1:length(varargin)
        if strcmp(lower(char(varargin{i})), 'keydown')==1 %#ok<STCI>
            doclear = 1;
        end
    end
end;

% Execute a single drawnow() to kick in Matlabs event processing.
% This will nicely eat up pending mouse-clicks, so they can't "spill over"
% into Matlab GUI after end of an experiment script.
drawnow;

% This is Matlab. Is the Java VM and AWT running?
if psychusejava('desktop')
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

    if doclear == 1
        % Clear the internal queue of characters:
        OSX_JAVA_GETCHAR.clear;
        % This is a stupid hack that hopefully "fixes" GetChar race-conditions as
        % reported by Denis:
        while CharAvail, drawnow; dummy = GetChar; end; %#ok<NASGU>
    end

    return;
else
    % Java VM unavailable, i.e., running in -nojvm mode.
    % On Windows, we can fall back to the old FlushEvents.dll.
    if IsWin
        % FlushEvents.dll has been renamed to FlushEventsNoJVM.dll. Call it.
        FlushEventsNoJVM(char(varargin{:}));
        return;
    end
end

% Running either on Octave or on OS/X or Linux with Matlab in No JVM mode:

if doclear == 1
    % Clear the internal queue of characters:

    % Screen's GetMouseHelper with command code 13 clears the queue of
    % characters on stdin:
    Screen('GetMouseHelper', -13);

    % This is a stupid hack that hopefully "fixes" GetChar race-conditions as
    % reported by Denis:
    while CharAvail, drawnow; dummy = GetChar; end; %#ok<NASGU>
end

return;
