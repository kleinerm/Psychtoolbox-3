function FlushEvents(varargin)
% FlushEvents(['mouseUp'],['mouseDown'],['keyDown'],['autoKey'],['update'],...)
% 
% Removes all events of the specified types from the event queue.
% The arguments can be in any order. Empty strings are ignored.
%
% OS X: ___________________________________________________________________
%
% FlushEvents will accept all arguments, but only 'keyDown' removes events.
%  
% 
% See also: GetChar, CharAvail, FlushEvents, EventAvail.

% 3/25/97  dgp	Wrote it.
% 9/20/05  awi  Added AssertMex call for OS 9 and Win and added OS X
%                   conditional.
% 1/22/06  awi  Commented out Cocoa wrapper and wrote Java wrapper.
% 6/20/06  awi  Use AddPsychJavaPath instead of AssertGetCharJava.
% 8/16/06  cgb  Now using the new GetChar system.

global OSX_JAVA_GETCHAR;

if IsOSX    
    % Make sure that the GetCharJava class is loaded and registered with
    % the java focus manager.
    if isempty(OSX_JAVA_GETCHAR)
        OSX_JAVA_GETCHAR = GetCharJava;
        OSX_JAVA_GETCHAR.register;
    end
    
    OSX_JAVA_GETCHAR.clear;
end
