function ImageWarpingDemo

% PTB-3 properly installed and working?
AssertOpenGL;

% Select display with max id for our onscreen window:
screenid = max(Screen('Screens'));

try
    % Open onscreen window with black background clear color:
    [win, winRect] = Screen('OpenWindow', screenid, 0);
    [xp, yp] = RectCenter(winRect);
    SetMouse(xp, yp, win);
    
    % Read our beloved bunny image from filesystem:
    bunnyimg = imread([PsychtoolboxRoot 'PsychDemos/konijntjes1024x768.jpg']);
    bunnytex = Screen('MakeTexture', win, bunnyimg);
    
    bunnyRect = CenterRect(Screen('Rect', bunnytex), winRect);
    xoffset = bunnyRect(RectLeft);
    yoffset = bunnyRect(RectTop);
    
    % Create warpoperator for application of the image warp:
    warpoperator = CreateGLOperator(win);
    warpmap = AddImageWarpToGLOperator(warpoperator, winRect);
    
    w = floor(size(bunnyimg,2)/5);
    h = floor(size(bunnyimg,1)/5);
    [x,y]=meshgrid(-w/2:w/2,-h/2:h/2);
    warpimage = zeros(h+1, w+1, 3);
    
    warpedtex = 0;
    count = 0;
    
    while ~KbCheck
        % Update framecounter: This is also used as our "simulation time":
        count = count + 1;

        % Query mouse to find out where to place the "warp map":
        [xp, yp] = GetMouse(win);
        xp = xp - xoffset;
        yp = yp + yoffset;

        % Compute a texture that contains the distortion vectors:
        % Red channel encodes x offset components, Green encodes y offsets:

        % Here we apply some sinusoidal modulation:
        warpimage(:,:,1) = (sin(sqrt((x/10).^2 + (y/10).^2) + count/10) + 1) * 6;
        warpimage(:,:,2) = (cos(sqrt((x/10).^2 + (y/10).^2) + count/10) + 1) * 6;

        % Its important to create a floating point texture, so we can
        % define fractional offsets that are bigger than 1.0 and negative
        % as well:
        modtex = Screen('MakeTexture', win, warpimage, [], [], 1);

        % Update the warpmap. First clear it out to zero offset, then draw
        % our texture into it at the mouse position:
        Screen('FillRect', warpmap, 0);
        Screen('DrawTexture', warpmap, modtex, [], CenterRectOnPoint(Screen('Rect', modtex), xp, yp));
        
        % Delete texture, it will be recreated at next iteration:
        Screen('Close', modtex);
        
        % Apply image warpmap to our bunny image:
        warpedtex = Screen('TransformTexture', bunnytex, warpoperator, warpmap, warpedtex);

        % Draw and show the warped bunny:
        Screen('DrawTexture', win, warpedtex);
        Screen('Flip', win);
    end
        
    % Done. Close display and return:
    Screen('CloseAll');
    return;
    
catch
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end
