% MouseTraceDemo3
% Allow multiple people to draw simultaneously by use of multiple
% mice or other pointing devices, utilizing Multi-Pointer-X on Linux.
%
% See also: PsychDemos, MouseTraceDemo, GetMouse.
%
% HISTORY
%                       
% 7/30/11  mk       Derived from MouseTraceDemo2.
%

AssertOpenGL;

if ~IsLinux
  error('Sorry, this demo currently only works on Linux.');
end

try
    % Open up a window on the screen and clear it.
    whichScreen = max(Screen('Screens'));
    [theWindow,theRect] = Screen('OpenWindow', whichScreen, 0);

    % Get handles for all virtual pointing devices, aka cursors:
    mice = GetMouseIndices('masterPointer')

    % Move the cursor to the center of the screen
    for mouse = mice
      theX(mouse+1) = theRect(RectRight)/2;
      theY(mouse+1) = theRect(RectBottom)/2;
      SetMouse(theX(mouse+1), theY(mouse+1), whichScreen, mouse);
      col(mouse+1, 1:3) = rand(1,3) * 255;
    end

    % Wait for a click and hide the cursor
    Screen(theWindow,'FillRect',0);
    Screen(theWindow,'TextSize',24);
    Screen(theWindow,'DrawText','Drag mice (i.e. hold button down) to draw. Any key for exit.',50,50,255);

    % Set the 'dontclear' flag of Flip to 1 to prevent erasing the
    % frame-buffer:
    Screen('Flip', theWindow, 0, 1);
    KbReleaseWait(-3);

    while ~KbCheck(-3)
      for mouse = mice
        [x(mouse+1), y(mouse+1), buttons] = GetMouse(theWindow, mouse);
	if any(buttons(1:3))
          if (x(mouse+1) ~= theX(mouse+1) | y(mouse+1) ~= theY(mouse+1)) %#ok<OR2>
            Screen('DrawDots', theWindow, [x(mouse+1), y(mouse+1)], 10, col(mouse+1, :));
            theX(mouse+1) = x(mouse+1); theY(mouse+1) = y(mouse+1);
          end
	end
      end
      Screen('Flip', theWindow, 0, 1);
    end

    Screen('CloseAll');
catch
    sca;
    psychrethrow(psychlasterror);
end %try..catch..
