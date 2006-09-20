function [x,y,buttons] = GetMouse(windowPtrOrScreenNumber)
% [x,y,buttons] = GetMouse([windowPtrOrScreenNumber])
%
% Returns the current (x,y) position of the cursor and the up/down state
% of the mouse buttons. "buttons" is a 1xN matrix where N is the number of
% mouse buttons. Each element of the matrix represents one mouse button.
% The element is true (1) if the corresponding mouse button is pressed and
% false (0) otherwise. 
%
% % Test if any mouse button is pressed. 
% if any(buttons)
%   fprintf('Someone''s pressing a button.\n');
% end
% 
% % Test if the first mouse button is pressed.
% if buttons(1)
%   fprintf('Someone''s pressing the first button!\n');
% end
%
% % Test if the second mouse button is pressed.
% if length(buttons)>=2 & buttons(2)
%   fprintf('Someone''s pressing the second button!\n');
% end
%
% length(buttons) tells you how many buttons there are on your mouse.
%
% The cursor position (x,y) is "local", i.e. relative to the origin of
% the window or screen, if supplied. Otherwise it's "global", i.e. relative
% to the origin of the main screen (the one with the menu bar).
% 
% NOTE: If you use GetMouse to wait for clicks, don't forget to wait
% for the user to release the mouse button, ending the current click, before
% you begin waiting for the next mouse press.
%
% Alternatively, you can also use the GetClicks() function to wait for
% mouse-clicks and return the mouse position of first click and the number
% of mouse button clicks.
%
% fprintf('Please click the mouse now.\n');
% [x,y,buttons] = GetMouse;
% while any(buttons) % if already down, wait for release
% 	[x,y,buttons] = GetMouse;
% end
% while ~any(buttons) % wait for press
% 	[x,y,buttons] = GetMouse;
% end
% while any(buttons) % wait for release
% 	[x,y,buttons] = GetMouse;
% end
% fprintf('You clicked! Thanks.\n');
% 
% NOTE: GetMouse no longer supports this obsolete usage:
% xy = GetMouse([windowPtrOrScreenNumber])
% where xy is a 1x2 vector containing the x, y coordinates.
%
% OS X: ___________________________________________________________________
%
% Even if your mouse has more than three buttons, GetMouse will return as
% many values as your mouse has buttons.
%
% _________________________________________________________________________
%
% M$-Windows: _____________________________________________________________
%
% Limitations:
%
% GetMouse will always assume a three button mouse and therefore always 
% return the state of three buttons.
%
% _________________________________________________________________________
% See also: GetClicks, SetMouse
%

% 4/27/96  dhb  Wrote this help file.
% 5/12/96  dgp  Removed confusing comment about columns.
%               Added query about coordinates.
% 5/16/96  dhb  Modified MEX file to conform to above usage, answered
%               query about coordinates.
% 5/29/96  dhb  Flushing mouse button events added by dgp.
% 8/23/96  dhb  Added support for windowInfo argument.
% 2/24/97  dgp	Updated.
% 2/24/97  dgp	Updated comments about flushing mouse button events.
% 3/10/97  dgp	windowPtrOrScreenNumber
% 3/23/97  dgp	deleted obsolete comment about flushing mouse button events.
% 5/9/00   dgp  Added note about waiting for release before waiting for next click.
% 8/5/01   awi  Added examples and modified to document new size of returned button matrix
%				on windows.  
% 8/6/01   awi  Added See also line for GetClicks and note about prior Windows version.
% 4/13/02  dgp  Cosmetic.
% 5/16/02  awi  Changed Win GetMouse to return variable number of button values and updated 
%               help accordingly.  
% 5/20/02  dgp  Cosmetic.
% 5/22/02  dgp  Note that obsolete usage is no longer supported.
% 6/10/01  awi  Added SetMouse to see also.
% 7/12/04  awi  ****** OS X-specific fork from the OS 9 version *******
%               Added note that this is not supported in OS X.  When the
%               new OS X mouse functions are in place this will have to be updated.
%               Check to see if the OS 9 GetMouse source would work in
%               Carbon on OS X so that we could still support this.
% 11/18/04 awi  Added support for OS X
% 09/03/05 mk   Add caching for 'numMouseButtons' to get 10-fold speedup.
% 02/21/06 mk   Added Linux support.
% 18/04/06 fwc  fixed bug that prevented use of multiple mice (tested with 3)
% 06/10/06 mk   Added Microsoft Windows support. Removed the old WinPTB GetMouse.dll
%               which worked except for button state queries.
% 09/20/06 mk   Updated help section for Windows: GetMouse now also works without onscreen windows.

% We Cache the value of numMouseButtons between calls to GetMouse, so we
% can skip the *very time-consuming* detection code on successive calls.
% This gives a tenfold speedup - important for tight realtime-loops.
persistent numMouseButtons;
if isempty(numMouseButtons)
    % Exit with an error if we don't find the a mex file on OS 9.
    AssertMex('MAC2');
    
    if IsOSX
        % On OS X we execute this script, otherwise either MATLAB found the mex file
        % file and exuted this, or else this file was exucuted and exited with
        % error on the AssertMex command above.
        
        %get the number of mouse buttons from PsychHID
        mousedices=GetMouseIndices;
        numMice = length(mousedices);
        if numMice == 0
            error('GetMouse could not find any mice connected to your computer');
        end

        allHidDevices=PsychHID('Devices');
        numMouseButtons=-1;
        for i=1:numMice
            b=allHidDevices(mousedices(i)).buttons;
            numMouseButtons=max(b, numMouseButtons);
        end
    else
        % Linux or Windows: We don't need numMouseButtons so we assign a dummy value of 32
        numMouseButtons = 32;
    end;
end;

%read the mouse position and  buttons
[globalX, globalY, rawButtons]=Screen('GetMouseHelper', numMouseButtons);
buttons=logical(rawButtons);

%renormalize to screen coordinates from display space 
if(nargin==1)
    screenRect=Screen('GlobalRect',windowPtrOrScreenNumber);
    x=globalX-screenRect(RectLeft);
    y=globalY-screenRect(RectTop);
else
    x=globalX;
    y=globalY;
end   

