% dot motion demo using SCREEN('DrawDots') subfunction
% author: Keith Schneider, 12/13/04

%HISTORY
%
% mm/dd/yy
%
% 12/13/04  kas     Wrote it.
% 1/11/05   awi     Merged into Psychtoolbox.org distribution.  
%                      -Changed name from "dot_demo" to "DotDemo" to match
%                      Psychtooblox conventions.
%                      -Changed calls to "Screen" from "SCREEN" to avoid
%                      case warning.
%                      -Added HISTORY section to comments.
% 1/13/05   awi     Merged in Mario Kleiner's modifications to agree with  
%                   his changes to Screen 'DrawDots' and also time performance:
%                      -Increases number of dots (ndots) by 10x
%                      -Decreases width dot (dot_w) from 0.3 to 0.1
%                      -Changed the 'OpenWindow' call to specify double
%                       buffers for onscreen window and 32-bit depth
%                      -Transpose second argument to Screen 'DrawDots'.
%                       Mario interchanged x&y matrix axes in this 'DrawDots'
%                       argument because it allows a direct copy from a MATLAB matrix
%                       into an OpenGL structure, without memory reordering, which 
%                       is slow.  This is controversial because the Psychtoolbox
%                       uniformly interprets matrix M axis (rows) as screen Y axis and
%                       matrix N (columns) as screen N axis.  This change breaks that 
%                       convention. 
%                      -Add calls to GetSeccs to time performance.                                  
% 3/22/05   mk      Added code to show how to specify different color and
%                   size for each single dot.
% 4/23/05   mk      Add call to Screen('BlendFunction') to reenable
%                   point-smoothing.

try

    % ------------------------
    % set dot field parameters
    % ------------------------

    nframes     = 1000; % number of animation frames in loop
    mon_width   = 39;   % horizontal dimension of viewable screen (cm)
    v_dist              = 60;   % viewing distance (cm)
    dot_speed   = 7;    % dot speed (deg/sec)
    ndots               = 20000;        % number of dots
    max_d               = 15;   % maximum radius of  annulus (degrees)
    min_d               = 1;    % minumum
    dot_w       = 0.1;  % width of dot (deg)
    fix_r       = 0.15; % radius of fixation point (deg)
    f_kill      = 0.05; % fraction of dots to kill each frame (limited lifetime)    
    differentcolors =0; % Use a different color for each point if == 1. Use common color white if == 0.
    differentsizes = 0; % Use different sizes for each point if == 1. Use one commons size if == 0.

    % ---------------
    % open the screen
    % ---------------

    doublebuffer=1
    [w, rect] = Screen('OpenWindow', 0, 0,[],32, doublebuffer+1);
    % Enable alpha blending with proper blend-function. We need it
    % for drawing of smoothed points:
    Screen('BlendFunction', w, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    center = [rect(3) rect(4)]/2;	% coordinates of screen center (pixels)
    fps=Screen('FrameRate',w);      % frames per second
    black = BlackIndex(w);
    white = WhiteIndex(w);
    Screen('FillRect', w, black)
    HideCursor;	% Hide the mouse cursor

    % ---------------------------------------
    % initialize dot positions and velocities
    % ---------------------------------------

    ppd = pi * rect(3) / atan(mon_width/v_dist/2) / 360;    % pixels per degree
    pfs = dot_speed * ppd / fps;                            % dot speed (pixels/frame)
    s = dot_w * ppd;                                        % dot size (pixels)
    fix_cord = [center-fix_r*ppd center+fix_r*ppd];

    rmax = max_d * ppd;	% maximum radius of annulus (pixels from center)
    rmin = min_d * ppd; % minimum
    r = rmax * sqrt(rand(ndots,1));	% r
    r(r<rmin) = rmin;
    t = 2*pi*rand(ndots,1);                     % theta polar coordinate
    cs = [cos(t), sin(t)];
    xy = [r r] .* cs;   % dot positions in Cartesian coordinates (pixels from center)

    mdir = 2 * floor(rand(ndots,1)+0.5) - 1;    % motion direction (in or out) for each dot
    dr = pfs * mdir;                            % change in radius per frame (pixels)
    dxdy = [dr dr] .* cs;                       % change in x and y per frame (pixels)

    % Create a vector with different colors for each single dot, if
    % requested:
    if (differentcolors==1)
        colvect = ones(3, ndots);
        for i=1:ndots
            colvect(1, i)= random('uniform', 0, 1);
            colvect(2, i)= random('uniform', 0, 1);
            colvect(3, i)= random('uniform', 0, 1);
        end;
    else
        colvect=white;
    end;
    
    % Create a vector with different point sizes for each single dot, if
    % requested:
    if (differentsizes==1)
        s=random('uniform', 1, 5, 1, ndots);
    end;
    
        
    % --------------
    % animation loop
    % --------------
    tavg=0;
    
    for i = 1:nframes

        Screen('FillOval', w, white, fix_cord);	% draw fixation dot (flip erases it)

        xy = xy + dxdy;						% move dots
        r = r + dr;							% update polar coordinates too

        % check to see which dots have gone beyond the borders of the annuli

        r_out = find(r > rmax | r < rmin | rand(ndots,1) < f_kill);	% dots to reposition
        nout = length(r_out);

        if nout

            % choose new coordinates

            r(r_out) = rmax * sqrt(rand(nout,1));
            r(r<rmin) = rmin;
            t(r_out) = 2*pi*(rand(nout,1));

            % now convert the polar coordinates to Cartesian

            cs(r_out,:) = [cos(t(r_out)), sin(t(r_out))];
            xy(r_out,:) = [r(r_out) r(r_out)] .* cs(r_out,:);

            % compute the new cartesian velocities

            dxdy(r_out,:) = [dr(r_out) dr(r_out)] .* cs(r_out,:);
        end
        xymatrix = transpose(xy);
        
        t1=GetSecs;
        Screen('DrawDots', w, xymatrix, s, colvect, center,1);  % change 1 to 0 to draw square dots
        if (doublebuffer==1)
            Screen('Flip', w);
        end;
        tavg=tavg + GetSecs - t1;
    end
    ShowCursor
    Screen('CloseAll');
    tavg=tavg / nframes
catch
    ShowCursor
    Screen('CloseAll');
end