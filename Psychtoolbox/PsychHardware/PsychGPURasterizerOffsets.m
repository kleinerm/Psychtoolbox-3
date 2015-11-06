function [rpfx, rpfy, rpix, rpiy, vix, viy, vfx, vfy] = PsychGPURasterizerOffsets(win, drivername)
% PsychGPURasterizerOffsets - Test GPU drivers for spatial misplacement of content.
%
% The function tests if the GPU places content where it is told
% to place it. It warns if this isn't the case and returns corrective
% offsets to compensate for the problem.
%
% Different postioning methods are tested and potentially different
% offsets are reported for the different methods.
%
% Input args:
% 'win' Window handle to open onscreen window.
% 'drivername' driver name string to prefix to output messages.
%
% Returns args: (x,y) offsets created by driver inaccuracy. You will need
% to apply the negation of these to correct for driver misplacement!
%
% (rpfx, rpfy) Corrective offsets for glRasterPos2f()
% (rpix, rpiy) Corrective offsets for glRasterPos2i()
% (vix, viy) Corrective offsets for glVertex2i()
% (vfx, vfy) Corrective offsets for glVertex2f()

    global GL;

    if isempty(GL);
        InitializeMatlabOpenGL();
    end

    winfo = Screen('GetWindowInfo', win);
    if bitand(winfo.ImagingMode, kPsychNeedFastBackingStore)
        % Imaging pipeline: Read from drawBuffer. Important in case imaging
        % pipeline applies geometric transformations, e.g., gpu panel
        % fitting. Otherwise we'd get false positives in test below.
        readbuffer = 'drawBuffer';
    else
        % Read from system backbuffer:
        readbuffer = 'backBuffer';
    end
    
    % Test for off-by-one bugs in graphics drivers / GPU's and compute
    % corrective offsets for our Bits++ T-Lock blitters...

    % glRasterPos2f(): Used by Screen('PutImage') for output-positioning:
    
    % Clear out top-left 20x20 rectangle of framebuffer:
    Screen('FillRect', win, 0, [0 0 20 20]);

    % Define drawposition via glRasterPos2f:
    glRasterPos2f(2, 1);

    % Draw RGB = [128, 0, 0] pixel to that location:
    testpixel = uint8([128 0 0]);
    glDrawPixels(1, 1, GL.RGB, GL.UNSIGNED_BYTE, testpixel);

    % Sync the pipeline, so we know the backbuffer contains the result:
    Screen('DrawingFinished', win, 0, 1);

    % Read top-left 4x4 rectangle back, only the red channel:
    testreadback = Screen('GetImage', win, [0 0 4 4], readbuffer, 0, 1);

    % Must flip here, to clear the "drawingfinished" state from above:
    Screen('Flip', win);
    
    % Find location of red == 128 pixel:
    pixposition = find(testreadback == 128);
    if ~isempty(pixposition)
        [pixy, pixx] = ind2sub(size(testreadback), pixposition);
        % Map from Matlab indexing to OpenGL indexing: Only x is remapped,
        % y-offset is consistent due to 1 offset inside our y-origin inside
        % Screen:
        pixx = pixx - 1;
    else
        pixy = -1;
        pixx = -1;
    end

    rpfx = pixx - 2;
    rpfy = pixy - 1;

    % At expected location?
    if rpfx~=0
        fprintf('%s:GPU-Rasterizertest: Warning: glRasterPos2f() command draws at wrong position (Offset %i, %i)!\n', drivername, rpfx, rpfy);
    end

    % glRasterPos2i(): Used by our DIO T-Lock blitter for output-positioning:
    
    % Clear out top-left 20x20 rectangle of framebuffer:
    Screen('FillRect', win, 0, [0 0 20 20]);

    % Define drawposition via glRasterPos2i:
    glRasterPos2i(2, 1);

    % Draw RGB = [128, 0, 0] pixel to that location:
    testpixel = uint8([128 0 0]);
    glDrawPixels(1, 1, GL.RGB, GL.UNSIGNED_BYTE, testpixel);

    % Sync the pipeline, so we know the backbuffer contains the result:
    Screen('DrawingFinished', win, 0, 1);

    % Read top-left 4x4 rectangle back, only the red channel:
    testreadback = Screen('GetImage', win, [0 0 4 4], readbuffer, 0, 1);

    % Must flip here, to clear the "drawingfinished" state from above:
    Screen('Flip', win);

    % Find location of red == 128 pixel:
    pixposition = find(testreadback == 128);
    if ~isempty(pixposition)
        [pixy, pixx] = ind2sub(size(testreadback), pixposition);
        % Map from Matlab indexing to OpenGL indexing: Only x is remapped,
        % y-offset is consistent due to 1 offset inside our y-origin inside
        % Screen:
        pixx = pixx - 1;
    else
        pixy = -1;
        pixx = -1;
    end

    rpix = pixx - 2;
    rpiy = pixy - 1;

    % At expected location?
    if rpix~=0
        fprintf('%s:GPU-Rasterizertest: Warning: glRasterPos2i() command draws at wrong position (Offset %i, %i)!\n', drivername, rpix, rpiy);
    end

    % glVertex2i(): Used by Screen's CLUT T-Lock blitter for output-positioning:
    
    % Clear out top-left 20x20 rectangle of framebuffer:
    Screen('FillRect', win, 0, [0 0 20 20]);

    glPointSize(1);
    glBegin(GL.POINTS);
    % Draw RGB = [128, 0, 0] pixel:
    glColor3ub(128, 0, 0);
    % Submit glVertex2i at test location:
    glVertex2i(2, 1);
    glEnd;
    
    % Sync the pipeline, so we know the backbuffer contains the result:
    Screen('DrawingFinished', win, 0, 1);

    % Read top-left 4x4 rectangle back, only the red channel:
    testreadback = Screen('GetImage', win, [0 0 4 4], readbuffer, 0, 1);

    % Must flip here, to clear the "drawingfinished" state from above:
    Screen('Flip', win);
    
    % Find location of red == 128 pixel:
    pixposition = find(testreadback == 128);
    if ~isempty(pixposition)
        [pixy, pixx] = ind2sub(size(testreadback), pixposition);
        % Map from Matlab indexing to OpenGL indexing: Only x is remapped,
        % y-offset is consistent due to 1 offset inside our y-origin inside
        % Screen:
        pixx = pixx - 1;
    else
        pixy = -1;
        pixx = -1;
    end

    vix = pixx - 2;
    viy = pixy - 1;

    % At expected location?
    if vix~=0
        fprintf('%s:GPU-Rasterizertest: Warning: glVertex2i() command draws at wrong position (Offset %i, %i)!\n', drivername, vix, viy);
    end

    % glVertex2f(): Used by Screen's drawing commands, e.g., 'DrawTexture' for output-positioning:

    % Clear out top-left 20x20 rectangle of framebuffer:
    Screen('FillRect', win, 0, [0 0 20 20]);

    glPointSize(1);
    glBegin(GL.POINTS);
    % Draw RGB = [128, 0, 0] pixel:
    glColor3ub(128, 0, 0);
    % Submit glVertex2f at test location:
    glVertex2f(2, 1);
    glEnd;

    % Sync the pipeline, so we know the backbuffer contains the result:
    Screen('DrawingFinished', win, 0, 1);

    % Read top-left 4x4 rectangle back, only the red channel:
    testreadback = Screen('GetImage', win, [0 0 4 4], readbuffer, 0, 1);

    % Must flip here, to clear the "drawingfinished" state from above:
    Screen('Flip', win);

    % Find location of red == 128 pixel:
    pixposition = find(testreadback == 128);
    if ~isempty(pixposition)
        [pixy, pixx] = ind2sub(size(testreadback), pixposition);
        % Map from Matlab indexing to OpenGL indexing: Only x is remapped,
        % y-offset is consistent due to 1 offset inside our y-origin inside
        % Screen:
        pixx = pixx - 1;
    else
        pixy = -1;
        pixx = -1;
    end

    vfx = pixx - 2;
    vfy = pixy - 1;

    % At expected location?
    if vfx~=0 || vfy~=0
        fprintf('%s:GPU-Rasterizertest: Warning: glVertex2f() command draws at wrong position (Offset %i, %i)!\n', drivername, vfx, vfy);
    end

end
