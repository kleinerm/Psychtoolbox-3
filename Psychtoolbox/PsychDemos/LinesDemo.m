
function LinesDemo
% Line motion demo using SCREEN('DrawLines') subfunction
% Derived from DotDemo, whose original author was Keith Schneider, 12/13/04
%
% Caution: This demo is a quick & dirty hack to get you started. It
% demonstrates a lot of inefficient techniques of how not to do this!!!

%HISTORY
%
% mm/dd/yy
% 12/03/06 Derived it from DotDemo. (MK)

AssertOpenGL;

try

    % ------------------------
    % set line field parameters
    % ------------------------

    nframes     = 1000; % number of animation frames in loop
    mon_width   = 39;   % horizontal dimension of viewable screen (cm)
    v_dist      = 60;   % viewing distance (cm)
    dot_speed   = 2;    % line speed (deg/sec)
    ndots       = 2000; % number of lines
    max_d       = 15;   % maximum radius of  annulus (degrees)
    min_d       = 1;    % minumum
    dot_w       = 0.1;  % width of line (deg)
    fix_r       = 0.15; % radius of fixation point (deg)
    f_kill      = 0.01; % fraction of lines to kill each frame (limited lifetime)
    differentcolors =1; % Use a different color for each point if == 1. Use common color white if == 0.
    differentsizes = 0; % Use different sizes for each point if >= 1. Use one common size if == 0.
    waitframes = 1;     % Show new line-images at each waitframes'th monitor refresh.

    if differentsizes>0  % drawing large lines is a bit slower
        ndots=round(ndots/5);
    end

    % ---------------
    % open the screen
    % ---------------

    screens=Screen('Screens');
    screenNumber=max(screens);
    [w, rect] = Screen('OpenWindow', screenNumber, 0);

    % Enable alpha blending with proper blend-function. We need it
    % for drawing of smoothed points:
    Screen('BlendFunction', w, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    [center(1), center(2)] = RectCenter(rect);
    fps=Screen('FrameRate',w);      % frames per second
    ifi=Screen('GetFlipInterval', w);
    if fps==0
        fps=1/ifi;
    end;

    white = WhiteIndex(w);
    HideCursor;	% Hide the mouse cursor

    % Do initial flip...
    vbl=Screen('Flip', w);

    % ---------------------------------------
    % initialize line positions and velocities
    % ---------------------------------------

    ppd = pi * (rect(3)-rect(1)) / atan(mon_width/v_dist/2) / 360;    % pixels per degree
    pfs = dot_speed * ppd / fps;                            % line speed (pixels/frame)
    s = dot_w * ppd;                                        % line size (pixels)
    fix_cord = [center-fix_r*ppd center+fix_r*ppd];

    rmax = max_d * ppd;	% maximum radius of annulus (pixels from center)
    rmin = min_d * ppd; % minimum
    r = rmax * sqrt(rand(ndots,1));	% r
    r(r<rmin) = rmin;
    t = 2*pi*rand(ndots,1);                     % theta polar coordinate
    cs = [cos(t), sin(t)];
    xy = [r r] .* cs;   % line positions in Cartesian coordinates (pixels from center)

    mdir = 2 * floor(rand(ndots,1)+0.5) - 1;    % motion direction (in or out) for each line
    dr = pfs * mdir;                            % change in radius per frame (pixels)
    dxdy = [dr dr] .* cs;                       % change in x and y per frame (pixels)

    % Create a vector with different colors for each single line, if
    % requested:
    if (differentcolors==1)
        colvect = uint8(round(rand(3,ndots*2)*255));
    else
        colvect=white;
    end;

    % Create a vector with different point sizes for each single line, if
    % requested:
    if (differentsizes>0)
        s=(1+rand(1, ndots)*(differentsizes-1))*s;
    end;

    xymatrix=zeros(2, ndots*2);

    % --------------
    % animation loop
    % --------------
    for i = 1:nframes
        if (i>1)
            Screen('FillOval', w, uint8(white), fix_cord);	% draw fixation dot (flip erases it)
            Screen('DrawLines', w, xymatrix, s, colvect, center,1);  % change 1 to 0 to draw non anti-aliased lines.
            Screen('DrawingFinished', w); % Tell PTB that no further drawing commands will follow before Screen('Flip')
        end;

        if KbCheck % break out of loop
            break;
        end;

        oldxy = xy - 15*dxdy;
        xy = xy + dxdy;						% move lines
        r = r + dr;							% update polar coordinates too

        % check to see which lines have gone beyond the borders of the
        % annuli
        r_out = find(r > rmax | r < rmin | rand(ndots,1) < f_kill);	% lines to reposition
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
            oldxy(r_out, :)= xy(r_out,:);
        end;

        % Set this 1 to 0 to test performance of slooow non-vectorized code:
        if 1
            % Vectorized synthesis of lines matrix for next frame:
            xymatrix(:, 1:2:(1+(2*ndots-2))) = xy';
            xymatrix(:, 2:2:(2+(2*ndots-2))) = oldxy';
        else
            % Slow synthesis of lines matrix for next frame:
            % This is 10-13x slower on Matlab, 320x slower on Octave 3.2!
            for j=0:ndots - 1
                xymatrix(:, 1 + i*2) = transpose(xy(i+1, :));
                xymatrix(:, 2 + i*2) = transpose(oldxy(i+1, :));
            end
        end

        vbl=Screen('Flip', w, vbl + (waitframes-0.5)*ifi);
    end;

    ShowCursor
    Screen('CloseAll');
catch
    ShowCursor
    Screen('CloseAll');
end
