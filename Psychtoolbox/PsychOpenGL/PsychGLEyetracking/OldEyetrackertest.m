function Eyetrackertest(imfilename)
% Eyetrackertest -- Testscript to test out a few weird
% ideas about Computervision + GLSL based eye tracking
% Not seriously useful for anything in the near future.
%
% Written by Mario Kleiner.

global rayvisdisplaylist;

try
    
    % Assign default name for test-image or test-movie:
    if nargin < 1 || isempty(imfilename)
        if IsWin
            imfilename='U:/test/eyeroi.jpg';
        end;
        if IsLinux
            imfilename='/home/kleinerm/test/eyeroi.jpg';
        end;
    end;
    imfilename

    % Read test-image:
    inimage=imread(imfilename);
    inimage=rgb2gray(inimage);
    inimage=inimage(1:128, 1:128);
    size(inimage)

    kernel = fspecial('gaussian', 11, 5.5);
    inimage2=single(inimage);
    myimg=conv2(kernel, inimage2);
    tic;
    for i=1:1000
        myimg=conv2(kernel, inimage2);
    end
    toc
    
    AssertOpenGL;
    oldsynctest = Screen('Preference','SkipSyncTests',1);
    InitializeMatlabOpenGL;
    moglcore('DEBUGLEVEL',2);
    
    [win , winRect] = Screen('OpenWindow', max(Screen('Screens')));
    Screen('FillRect', win, [0 0 255]);
    Screen('Flip', win);

    width=RectWidth(winRect);
    height=RectHeight(winRect);
    AssertGLSL;

    % Create texture from input image:
    imtex=Screen('MakeTexture', win, inimage);
    imrect=Screen('Rect', imtex);
    imloc=CenterRect(imrect, Screen('Rect', win));

    % Show input image:
    Screen('DrawTexture', win, imtex);
    Screen('Flip',win);
    Waitkey;
    
    % Create offscreen window of proper size to hold blurred
    % input image:
    img_blurred=Screen('OpenOffscreenWindow', win, [], imrect);

    % Create offscreen window 1 for remapped image:
    pingpongrect=[0 0 256 360];
    pingpong1=Screen('OpenOffscreenWindow', win, [0 0 0 ], pingpongrect);
    glpingpong1=Screen('GetOpenGLTexture', win, pingpong1);

    pingpong2=Screen('OpenOffscreenWindow', win, [0 0 0 ], pingpongrect);
    glpingpong2=Screen('GetOpenGLTexture', win, pingpong2);
    
    glGetError;

    % Build and initialize horizontal "clamped derivative" shader:
    horizderivshader = LoadGLSLProgramFromFiles('HorizontalDerivativeShader');

    % Build and initialize horizontal "sum" shader:
    horizsumshader = LoadGLSLProgramFromFiles('HorizontalSumShader');

    % Build and initialze horizontal edge detection shader:
    horizedgedistshader = LoadGLSLProgramFromFiles('HorizontalMinimumEdgeDistanceShader');

    % Build and initialize gaussian blur shader (5x5, stddev=1.5):
    blurshader = Create2DGaussianBlurShader(2.5, 11);
    % Select blurshader:
    glUseProgram(blurshader);
    
    % Draw input image imtex into img_blurred, applying the blurshader:
    Screen('CopyWindow', imtex, img_blurred);
glFinish;
tic
for i = 1:1000
    Screen('CopyWindow', imtex, img_blurred);
end
glFinish;
toc
    % Disable blurshader:
    glUseProgram(0);


    % Build visualization display list:

    % Get an OpenGL texture handle for the blurred image:
    [glimg_blurred textarget] = Screen('GetOpenGLTexture', win, img_blurred);

    % Set framebuffer as drawing target:
    glDisable(GL.DEPTH_TEST);
    redrawcount = 0;
    tstart = GetSecs;
    while(1)
        % Query mouse position and button state:
        [mx, my, buttons]=GetMouse;
        if any(buttons)
            break;
        end;
        
        % Redraw input image:
        Screen('DrawTexture', win, img_blurred);
        
        % Remap cursor position to position in blurred input image (cx,cy):
        cx= mx - imloc(RectLeft);
        cy= imloc(RectBottom) - my;
        targetx=0;
        targety=0;

        % Sample into pingpong1 - Window:
        Screen('BeginOpenGL', pingpong1);
        GLSLResampleRayStarToRectangle(glimg_blurred, cx, cy, 0, 359, 1, 256, targetx, targety);
    
        % Visualize sampling star:
        Screen('glPoint', win, 255, mx, my);
        glPushMatrix;
        glTranslatef(mx, my, 0);
        glCallList(rayvisdisplaylist);
        glPopMatrix;

        % Draw pingpong image:
        Screen('DrawTexture', win, pingpong1, [], pingpongrect);
        
        % Compute per-beam summed positive derivative:
%        glUseProgram(horizderivshader);
        Screen('CopyWindow', pingpong1, pingpong2);
        glUseProgram(0);
        
        % Draw derivative image:
        Screen('DrawTexture', win , pingpong2, [], OffsetRect(pingpongrect, 0, 400));
        
        % Compute per-beam sum of derivatives:
 %        glUseProgram(horizsumshader);
        glUseProgram(horizedgedistshader);
        Screen('CopyWindow', pingpong2, pingpong1, [], [0 0 1 360]);
        glUseProgram(0);

        % Draw per-beam sum:
        Screen('DrawTexture', win , pingpong1, [0 0 1 360], OffsetRect(pingpongrect, 800, 0), 0, 0);
        
        % Get it back:
        weights=Screen('GetImage', pingpong1, [0 0 1 360]);
        wvals = (255 - double(weights(:,1,1)))/255;
        %wvals = wvals/sum(wvals);
        % Compute weighted shift vector:
        dx=0; dy=0;
        for angle=1:360
            dx = dx + wvals(angle)*cos(angle/360*2*3.141592654);
            dy = dy + wvals(angle)*sin(angle/360*2*3.141592654);
            Screen('DrawLine', win, [255 255 0], 0, angle-1, (1-wvals(angle))*255, angle-1);
        end;

        ssize=2;
        n=norm([dx dy]);
        if n==0
            n=1;
        end;
        
        dx = dx / n * ssize;
        dy = dy / n * ssize;

        Screen('DrawLine', win, 255, double(mx), double(my), double(mx+dx*100), double(my+dy*100));
        mx=mx+dx;
        my=my+dy;
        
        if mod(redrawcount, 2)
           % SetMouse(mx,my);
        end;
        
        Screen('Flip',win, 0, 0, 2-0);
        redrawcount = redrawcount + 1;
    end;
    avgupdates = redrawcount / (GetSecs - tstart)
    
    Screen('EndOpenGL', win);
    Screen('Close', win);
    return;

    % Close onscreen window and release all other ressources:
    Screen('CloseAll');

    % Reenable Synctests after this simple demo:
    Screen('Preference','SkipSyncTests', oldsynctest);

    % Well done!
    return;
    %catch
    Screen('CloseAll');
    psychrethrow(lasterror);
end;

function Waitkey
    while KbCheck; end;
    KbWait;
    while KbCheck; end;
return;

function GLSLResampleRayStarToRectangle(inputTexture, cx, cy, startAngle, endAngle, incrementAngle, raylength, tx, ty)
global GL;
persistent raydisplaylist;
global rayvisdisplaylist;

if isempty(GL)
    error('GLResampleRayStarToRectangle called without properly setup Matlab-OpenGL!');
end;

if isempty(raydisplaylist)
    % First invocation: Need to setup sampling display list:
    raydisplaylist = glGenLists(1);
    glNewList(raydisplaylist, GL.COMPILE);
        angle=startAngle;
        y=0;
        glBegin(GL.LINES);
        while (angle <= endAngle)
            radians = angle / 360 * 2 * 3.141592654;
            glTexCoord2f(0, 0); % Each sampling line starts at (cx,cy)+(0,0).
            glVertex2f(0, y);   % Target is (0,y) for the y'th line.
            % Sampling line ends at a distance of 'raylength' away in
            % directions of 'angle'
            glTexCoord2f(raylength * cos(radians), raylength * sin(radians));
            glVertex2f(raylength, y);
            % Submit next sampling line...
            angle= angle + incrementAngle;
            y=y+1;
        end;
        glEnd;
    glEndList;    

    % Build the same object, this time for visualization:
    rayvisdisplaylist = glGenLists(1);
    glNewList(rayvisdisplaylist, GL.COMPILE);
        angle=startAngle;
        y=0;
        glColor3f(0,255,0);
        glBegin(GL.LINES);
        while (angle <= endAngle)
            radians = angle / 360 * 2 * 3.141592654;
            glVertex2f(10 * cos(radians), 10 * sin(radians));
            % Sampling line ends at a distance of 'raylength' away in
            % directions of 'angle'
            glVertex2f(raylength * cos(radians), raylength * sin(radians));
            % Submit next sampling line...
            angle= angle + incrementAngle;
            y=y+1;
        end;
        glEnd;
    glEndList;    

end;

if isempty(tx)
    tx=0;
end;

if isempty(ty)
    ty=0;
end;

% Visualize sampling star:
glMatrixMode(GL.MODELVIEW);
glPushMatrix;

% Set drawing position for remapped rectangle:
glTranslatef(tx, ty, 0);

% Setup texture matrix so center of our sampling raystar is at position:
% (cx,cy):
glMatrixMode(GL.TEXTURE);
glPushMatrix;
glLoadIdentity;
glTranslatef(cx, cy, 0);

% Bind input texture:
glPushAttrib(mor(GL.ENABLE_BIT, GL.TEXTURE_BIT));
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, inputTexture);
glEnable(GL.TEXTURE_RECTANGLE_EXT);
glTexEnvfv(GL.TEXTURE_ENV,GL.TEXTURE_ENV_MODE,GL.REPLACE);

% Draw our star, thereby resampling/remapping the colors along the stars
% rays into a rectangular shaped region of the framebuffer or output
% texture:
glCallList(raydisplaylist);

% Restore previous state:
glPopAttrib;
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);
glPopMatrix;
glMatrixMode(GL.MODELVIEW);
glPopMatrix;

return;
