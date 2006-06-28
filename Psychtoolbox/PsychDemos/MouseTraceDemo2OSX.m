% MouseTraceDemo2OSX
%
% OS X: ___________________________________________________________________
% 
% Draw a curve with the mouse. Same as MouseTraceDemoOSX, but asks
% Screen('Flip') to not clear the framebuffer after flip. This way,
% we don't need to redraw the whole mousetrace in each frame.
%
% The only prominent difference between MouseTraceDemoOSX and
% MouseTraceDemo is that MouseTraceDemoOSX uses a double-buffered display.
% For a complete description of changes made to convert MouseTraceDemo into
% MouseTraceDemoOSX edit this demo and read its history.  
%
% OS 9: ___________________________________________________________________
%
% See GetMouseDemo
%
% WINDOWS: ________________________________________________________________
% 
% See GetMouseDemo
% 
% _________________________________________________________________________
%
% see also: PsychDemos, PsychDemosOSX, GetMouse.

% HISTORY
% 8/12/97  dhb, wtf  Wrote it.
% 8/13/97  dhb			 Small modifications.
% 8/15/97  dgp			 Drag mouse instead of clicking before and after.
% 8/07/01  awi       Added font conditional, changed "button" to "buttons"
%                    and indexed the mouse button result for Windows.
% 4/11/01  awi		   Cosmetic editing of comments.
% 4/13/02  dgp       Use Arial, no need for conditional.
% 11/18/04 awi       Modified to make it work on OS X and renamed to MouseTraceDemoOSX.  
%                       - Changed the open command to specify double
%                       buffers and 32 depth because we don't yet support
%                       8-bit depth on OS X.  
%                       - Double buffer the display;  
%                        the path does not accumulate in the window so
%                        render the entire path on every frame, not just the
%                        path delta since the last segment was drawn.  
%                       - Added flip command because we are double
%                       buffered.
%                       - Added try..catch to close onscreen window in
%                       event of failure.  
%                       - Open on the highest-numbered display and pass
%                       GetMouse the window pointer.  Not necessary for OS
%                       X, but often desirable to use secondary display if
%                       available.
%                       
% 4/23/05  mk       Derived from MouseTraceDemoOSX: Uses new "Don't clear" mode of Flip.
%                   to avoid redrawing the whole past mousetrace after each
%                   Flip --> Faster.

try
    % Open up a window on the screen and clear it.
    whichScreen = max(Screen('Screens'));
    [theWindow,theRect] = Screen(whichScreen,'OpenWindow',0,[],[],2,0,8);

    % Move the cursor to the center of the screen
    theX = theRect(RectRight)/2;
    theY = theRect(RectBottom)/2;
    SetMouse(theX,theY);

    % Wait for a click and hide the cursor
    Screen(theWindow,'FillRect',0);
    %Screen(theWindow,'TextFont','Arial');
    %Screen(theWindow,'TextSize',18);
    Screen(theWindow,'DrawText','Drag mouse (i.e. hold button down) to draw',50,50,255);
    Screen('Flip', theWindow);
    while (1)
        [x,y,buttons] = GetMouse(theWindow);
        if buttons(1)
          break;
        end
    end
    Screen(theWindow,'DrawText','Drag mouse (i.e. hold button down) to draw',50,50,0);
    %Screen(theWindow,'DrawText','Release button to finish',50,50,255);

    HideCursor;

    % Loop and track the mouse, drawing the contour
    [theX,theY] = GetMouse(theWindow);
    thePoints = [theX theY];
    Screen(theWindow,'DrawLine',255,theX,theY,theX,theY);
    % Set the 'dontclear' flag of Flip to 1 to prevent erasing the
    % frame-buffer:
    Screen('Flip', theWindow, 0, 1);
    while (1)
        [x,y,buttons] = GetMouse(theWindow);	
        if ~buttons(1)
            break;
        end
        if (x ~= theX | y ~= theY)
            thePoints = [thePoints ; x y];
            [numPoints, two]=size(thePoints);
            % Only draw the most recent line segment: This is possible,
            % because...
            Screen(theWindow,'DrawLine',128,thePoints(numPoints-1,1),thePoints(numPoints-1,2),thePoints(numPoints,1),thePoints(numPoints,2));
            % ...we ask Flip to not clear the framebuffer after flipping:
            Screen('Flip', theWindow, 0, 1);
            theX = x; theY = y;
        end
    end

    % Close up
    Screen(theWindow,'DrawText','Click mouse to finish',50,50,255);
    ShowCursor;
    Screen(theWindow,'Close');

    % Plot the contour in a Matlab figure
    plot(thePoints(:,1),theRect(RectBottom)-thePoints(:,2));
catch
    Screen('CloseAll')
    Screen('ShowCursor');
    psychrethrow(lasterror);
end %try..catch..
