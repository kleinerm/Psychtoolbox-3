function FillPolyTest
% FillPolyTest
%
% Test Screen 'FillPoly' for speed and correctness.
%
% Screen 'FillPoly' renders both convex and concave polygons, but treats
% them differently; Convex polygons it renders quickly using GL_POLYGON.
% Concave polygons it renders slowly using GLU tesselators.  
% 
% FillPolyTest draws a "Butterfly" concave polygon to test if FillPolly
% correctly detects and renders concave polygons.  If FillPoly failed to 
% detect concavity, it would use GL_POLYGON, which renders concave polygons
% incorrectly.

% HISTORY
%
% mm/dd/yy  
%
% 1/15?/05   mk     created it. 
% 1/25/05    awi    Merged into Psychtoolbox.org distribution. 
%                   Renamed to TestFillPolyOSX.
%                   Minor changes to description text. 
%                   Added a history section.  

doublebuffer=1;

try
	% This script calls Psychtoolbox commands available only in OpenGL-based 
	% versions of the Psychtoolbox. (So far, the OS X Psychtoolbox is the
	% only OpenGL-based Psychtoolbox.)  The Psychtoolbox command AssertPsychOpenGL will issue
	% an error message if someone tries to execute this script on a computer without
	% an OpenGL Psychtoolbox
	AssertOpenGL;
	
	% Get the list of OldScreens and choose the one with the highest screen number.
	% Screen 0 is, by definition, the display with the menu bar. Often when 
	% two monitors are connected the one without the menu bar is used as 
	% the stimulus display.  Chosing the display with the highest dislay number is 
	% a best guess about where you want the stimulus displayed.  
	screens=Screen('Screens');
	screenNumber=max(screens);
    % Open a double buffered fullscreen window and draw a gray background 
    % and front and back buffers.  On OS X we 
    w=Screen('OpenWindow',screenNumber, 0,[],32,doublebuffer+1);

    if(doublebuffer)
        Screen('Flip', w);
    end;

    % Polydrawtest:
    pointList=zeros(36, 2);
    for i=1:36
        j=abs(sin(i*20/1*3.14/180));
        pointList(i,1)=300 + 100 * j * cos((36-i)*10/1 * 3.14 / 180);
        pointList(i,2)=300 + 100 * j * sin((36-i)*10/1 * 3.14 / 180);
    end;

    n=10;
    t=GetSecs;
    for i=1:n
        Screen('FillPoly', w ,[255, 0, 0], pointList);
    end;
    t=(GetSecs - t) / n * 1000

    Screen('FramePoly', w ,[0 255 0], pointList, 2);

    if(doublebuffer)
        Screen('Flip', w);
    end;

    KbWait;

    %The same commands wich close onscreen and offscreen windows also close
    %textures.
    Screen('CloseAll');    
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    Priority(0);
    rethrow(lasterror);
end %try..catch..



    




