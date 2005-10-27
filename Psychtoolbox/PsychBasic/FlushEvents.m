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
% See also EventAvail.

% 3/25/97  dgp	Wrote it.
% 9/20/05  awi  Added AssertMex call for OS 9 and Win and added OS X
%                   conditional.

AssertMex('OS9', 'Windows');

if IsOSX
    InitCocoaEventBridge;
    if(any(strcmp('KEYDOWN', upper(varargin))))
        CocoaEventBridge('FlushChars');
    end
end

