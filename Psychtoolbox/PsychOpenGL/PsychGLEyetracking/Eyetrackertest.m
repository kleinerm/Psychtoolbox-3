function Eyetrackertest(moviename, graythreshold, ycb_low, ycb_high, ycr_low, ycr_high)
% Eyetrackertest -- Testscript to test out a few weird
% ideas about Computervision + GLSL based eye tracking
% Not seriously useful for anything in the near future.
%
% Written by Mario Kleiner.
global GL;

%try
    KbName('UnifyKeyNames');
    esc   = KbName('ESCAPE');
    space = KbName('space');
    upArrow = KbName('UpArrow');
    downArrow = KbName('DownArrow');
    keya = Kbname('a');
    keys = Kbname('s');
    keyd = Kbname('d');
    keyf = Kbname('f');
    keyg = Kbname('g');
    keyh = Kbname('h');
    keyj = Kbname('j');
    keyk = Kbname('k');
    key1 = Kbname('1!');
    key2 = Kbname('2@');
    key3 = Kbname('3#');
    key4 = Kbname('4$');
    
    % Assign default name for test-image or test-movie:
    if nargin < 1 || isempty(moviename)
        moviename = [];
    end;
    moviename
    
    AssertOpenGL;
    oldsynctest = Screen('Preference','SkipSyncTests',1);
    %InitializeMatlabOpenGL;
    
    imagingmode = kPsychNeedFastBackingStore;
    
    [win , winRect] = Screen('OpenWindow', max(Screen('Screens')), 0, [], [], [], [], [], imagingmode);
    vbl = Screen('Flip', win);
    
    width=RectWidth(winRect);
    height=RectHeight(winRect);

    AssertGLSL;

    % Build gauss filter kernel for lowpass filtering:
    kwidth = 5;
    stddev = 3.5;
    kernel = fspecial('gaussian', kwidth, stddev);
    blurshader = EXPCreateStatic2DConvolutionShader(kernel, 3, 3, 0, 1);
%     bluroperator = CreateGLProcessingOperatorFromShader(win, blurshader, 'Gaussian Blur Operator: 5x5, stddev=1.5');
    bluroperator = Screen('OpenProxy', win);
    Screen('HookFunction', bluroperator, 'PrependShader', 'UserDefinedBlit', 'Blur and Mirror operator', blurshader, 'Builtin:IdentityBlit:Offset:640:0:Scaling:-1.0:1.0');
    Screen('HookFunction', bluroperator, 'Enable', 'UserDefinedBlit');

    % Build color classifier:
    colorshader = LoadGLSLProgramFromFiles('SimpleColorClassificationShader', 1);
    mingreenintensity = glGetUniformLocation(colorshader, 'minintensity');
    maxgreenintensity = glGetUniformLocation(colorshader, 'maxintensity');
    minratio = glGetUniformLocation(colorshader, 'minratio');

    mingreen = 0.2;
    maxgreen = 0.9;
    minrgratio = 1.2;

    glUseProgram(colorshader);
    glUniform1f(mingreenintensity, mingreen);
    glUniform1f(maxgreenintensity, maxgreen);
    glUniform1f(minratio, minrgratio);
    glUseProgram(0);

    markerdetector = CreateGLProcessingOperatorFromShader(win, colorshader, 'Simple color classification operator');

    
    eyeclassifiershader = LoadGLSLProgramFromFiles('YRGColorClassificationShader', 1);
    preprocoperator = CreateGLProcessingOperatorFromShader(win, eyeclassifiershader, 'RGB to Gray conversion shader');
    
    rgb2grayshader = LoadGLSLProgramFromFiles('RGB2GrayConversionShader', 1);
    %preprocoperator = CreateGLProcessingOperatorFromShader(win, rgb2grayshader, 'RGB to Gray conversion shader');
    
    diskdetectorshader = LoadGLSLProgramFromFiles('HoughDiskDetectionShader', 1);
    diskdetector = CreateGLProcessingOperatorFromShader(win, diskdetectorshader, 'Hough Disk detection shader');
        
    meantrackershader = LoadGLSLProgramFromFiles('SimpleFeatureMeanPositionTrackerShader', 1);
    glUseProgram(meantrackershader);
    glUniform1i(glGetUniformLocation(meantrackershader, 'OldPositions'), 0);
    glUniform1i(glGetUniformLocation(meantrackershader, 'Image'), 1);
    glUseProgram(0);
    % Build final hook slot for the tracker operator: Pass the shader handle for the tracking shader.
    meantrackeroperator = CreateGLProcessingOperatorFromShader(win, meantrackershader, 'Simple feature mean position tracking operator', kPsychNeed16BPCFloat);

    newpositionstex = 0;
    
    if ~isempty(moviename)
        % Open movie file:
        [movie duration fps width height count] = Screen('OpenMovie', win, moviename);
        capture = -1;
    else
        capture = Screen('OpenVideoCapture', win, [], [0 0 640 480]);
        Screen('StartVideoCapture', capture, 60, 1);
        movie = -1
        count = intmax
        texid = Screen('GetCapturedImage', win, capture)
        width = RectWidth(Screen('Rect', texid))
        height = RectHeight(Screen('Rect', texid))
        Screen('Close', texid)
    end
    
    % Build resolution pyramid:
    lw = width;
    lh = height;
    for level = 1:6
        inlevel(level) = Screen('OpenOffscreenWindow', win, 0, [0 0 lw lh], 32);
        lw = lw / 2;
        lh = lh / 2;
    end
    lw
    lh

    
    HideCursor;
    
    texid = 0;
    idx = 0;
    ox = -1;
    oy = -1;
    roirect = [0 0 0 0];
    
    while texid>=0 && idx < count
        if movie>=0
            texid = Screen('GetMovieImage', win, movie, 1);
        else
            texid = Screen('GetCapturedImage', win, capture);
        end
        
%        Screen('TransformTexture', texid, win);
        
        if texid>0
            dstRect=Screen('Rect', texid);
            Screen('DrawTexture', win, texid); % , [], dstRect);
            vbl = Screen('Flip', win, vbl + 0.04);
            idx = idx + 1;

            [isdown secs keycode] = KbCheck;
            if isdown
                if keycode(esc)
                    break;
                end

                if keycode(space)
                    % Stop "playback" let the user select a frame:
                    while(1)
                        [x y buttons] = GetMouse(win);

                        if buttons(1)
                            if ox==-1
                                ox = x;
                                oy = y;
                                roirect = [ox oy x y];
                            else
                                roirect = [min(ox,x) min(oy, y) max(ox, x) max(oy, y)];
                            end
                        else
                            if ox~=-1
                                roirect = [min(ox,x) min(oy, y) max(ox, x) max(oy, y)];
                                ox = -1;
                                oy = -1;
                            end
                        end

                        if buttons(2)
                            break;
                        end

                        Screen('DrawTexture', win, texid, [], dstRect);
                        Screen('DrawLine', win, [0 255 0], x, 0, x, 1000);
                        Screen('DrawLine', win, [0 255 0], 0, y, 1000, y);

                        Screen('FrameOval', win, [255 255 0], roirect);
                        [xc, yc] = RectCenter(roirect);
                        halfwidth = 30;
                        Screen('FrameRect', win, [255 255 0], [xc-halfwidth, yc-halfwidth, xc+halfwidth, yc+halfwidth]);

                        Screen('DrawLine', win, [255 0 0], x-5, y-5, x+5, y+5);
                        Screen('DrawLine', win, [255 0 0], x-5, y+5, x+5, y-5);
                        
                        Screen('Flip', win);
                    end
                    
                    % Break out of outer while loop:
                    break;
                end
            end
            %startimage = Screen('GetImage', texid);
            Screen('Close', texid);
            texid = 0;
        end
    end

    % Get a sample of the iris and pupil color:
    irispupil = Screen('OpenOffscreenWindow', win, [0 0 0 0], roirect);
    Screen('BlendFunction', irispupil, GL_ONE, GL_ZERO);
    Screen('FillOval', irispupil, [255 255 255 255]);
    Screen('BlendFunction', irispupil, GL_DST_ALPHA, GL_ZERO);
    Screen('DrawTexture', irispupil, texid, roirect, []);
    Screen('BlendFunction', irispupil, GL_ONE, GL_ZERO);

    if nargin < 6
        global pupilirisimage
        pupilirisimage = Screen('GetImage', irispupil);

        pupilirisycbcr = rgb2ycbcr(pupilirisimage);
        ycb_channel = pupilirisycbcr(:,:,2);
        ycr_channel = pupilirisycbcr(:,:,3);
        validones = find(pupilirisycbcr(:,:,1)>0);
        ycb_channel = ycb_channel(validones);
        ycr_channel = ycr_channel(validones);
        ycb_low = double(min(min(ycb_channel)))
        ycb_high = double(max(max(ycb_channel)))
        ycr_low = double(min(min(ycr_channel)))
        ycr_high = double(max(max(ycr_channel)))
        graythreshold = 0.25;
    end
    
%     %sca;
%     imshow(pupilirisimage);
%     pupilchannel1=pupilirisimage(:,:,1);
%     pupilchannel2=pupilirisimage(:,:,2);
%     pupilchannel3=pupilirisimage(:,:,3);
%     plot3(pupilchannel1(:),pupilchannel2(:),pupilchannel3(:),'.');
    
    %return;
    
    ShowCursor;
    
    % Do we have a roirect ROI for tracking?
    if ~isempty(roirect)
        % Determine center of rect - The startposition for tracking:
        [xc, yc] = RectCenter(roirect);
        yc = RectHeight(Screen('Rect', texid)) - yc;
        radius= 0.25 * (RectWidth(roirect) + RectHeight(roirect));
        halfwidth = radius*1.5;
        roiwidth = halfwidth;
        trackedrect = [xc-roiwidth, yc-roiwidth, xc+roiwidth, yc+roiwidth];
        % trackedrect = dstRect;
        glUseProgram(diskdetectorshader);
        glUniform1f(glGetUniformLocation(diskdetectorshader, 'RadiusSquared'), radius*radius);
        glUniform1f(glGetUniformLocation(diskdetectorshader, 'HalfWidth'), halfwidth);
        glUniform4fv(glGetUniformLocation(diskdetectorshader, 'Roi'), 1, trackedrect);
        
        glUseProgram(eyeclassifiershader);
        glUniform4fv(glGetUniformLocation(eyeclassifiershader, 'Roi'), 1, trackedrect);
        glUniform1f(glGetUniformLocation(eyeclassifiershader, 'GrayThreshold'), graythreshold);
        glUniform4f(glGetUniformLocation(eyeclassifiershader, 'ColorRoi'), ycb_low/255, ycb_high/255, ycr_low/255, ycr_high/255);
        glUseProgram(0);
    
        initialpositions = zeros(1, 1, 4);
        initialpositions(1,:,1) = xc ; % xc - roiwidth + rand(1, 10) * 2 * roiwidth;
        initialpositions(1,:,2) = yc ; % - roiwidth + rand(1, 10) * 2 * roiwidth;
        oldpositionstex = Screen('MakeTexture', win, initialpositions, [], [], 1);
        pupil = [xc yc];
        
        % Cut out ROI anc copy it to 'templatetex' for use as
        % trackingtemplate:
        templatetex = Screen('OpenOffscreenWindow', win, 0, roirect, 32);
        Screen('DrawTexture', inlevel(1), texid);
        Screen('DrawTexture', templatetex, inlevel(1), roirect, [], [], 0);
        
        texRect=Screen('Rect', texid);
        base_y = texRect(RectBottom);

        % Release old frame:
        Screen('Close', texid);
        
        houghimage = Screen('OpenOffscreenWindow', win, 0, texRect, 64);
        inimage = Screen('OpenOffscreenWindow', win, 0, texRect);
        
        markerbinimage = 0; nmarkers = 4;
        initialmarkerpositions = zeros(1, nmarkers, 4);
        
        for i=1:4
            [clicks,x,y] = GetClicks(win);
            initialmarkerpositions(1,i,1) = x;
            initialmarkerpositions(1,i,2) = base_y - y;
        end
        oldmarkerpositionstex = Screen('MakeTexture', win, initialmarkerpositions, [], [], 1);
        newmarkerpositionstex = 0;
        
%        bottomleft  = squeeze(initialmarkerpositions(1,4,1:2))';
%        bottomright = squeeze(initialmarkerpositions(1,5,1:2))';
%        refdeltavect = (bottomright - bottomleft)
%        refscale = norm(refdeltavect);
%        meanposition = squeeze(mean(initialmarkerpositions(1,:,1:2)))';
%        refoffsetvect = (pupil - meanposition) / refscale;
%        refoffsetvect = (pupil - meanposition);
        trackthemarkers = 0;
        idx = 0;
        tlgaze = [];
        trgaze = [];
        blgaze = [];
        brgaze = [];
        pupil_x = 0;
        pupil_y = 0;
        
        % Tracking loop: Try to track until end of movie (texid<0):
        while texid>=0
            
            % Fetch next image from cam:
            if movie>=0
                texid = Screen('GetMovieImage', win, movie, 1);
            else
                texid = Screen('GetCapturedImage', win, capture, 1);
            end
            
            if texid>0
                idx = idx + 1;
                
                Screen('TransformTexture', texid, bluroperator, [], inimage);
%                 glUseProgram(blurshader);
%                 Screen('DrawTexture', inimage, texid);
%                 glUseProgram(0);

                if trackthemarkers || idx==1 || movie>=0
                    markerbinimage = Screen('TransformTexture', inimage, markerdetector, [], markerbinimage);
                    % Tracking step: We transform the old feature position state texture into the new one, performing the tracking update step.
                    newmarkerpositionstex = Screen('TransformTexture', oldmarkerpositionstex, meantrackeroperator, markerbinimage, newmarkerpositionstex);

                    % Retrieve tracking state texture:
                    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, Screen('GetOpenGLTexture', win, newmarkerpositionstex));
                    markertrackeroutput = double(glGetTexImage( GL.TEXTURE_RECTANGLE_EXT, 0, GL.RGBA, GL.FLOAT));
                    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

                    % Swap tracking state texture handles for next tracking step:
                    dummytex = oldmarkerpositionstex;
                    oldmarkerpositionstex = newmarkerpositionstex;
                    newmarkerpositionstex = dummytex;
                end

                
                %bottomleft  = markertrackeroutput(1:2,4)';
                %bottomright = markertrackeroutput(1:2,5)';
                %offsetvect = bottomleft + (refoffsetvect * norm(bottomright - bottomleft));

                markermeanposition = mean(markertrackeroutput(1:2, :)');
roileft = min(markertrackeroutput(1,:));
roiright = max(markertrackeroutput(1,:));
roitop = min(markertrackeroutput(2,:));
roibottom = max(markertrackeroutput(2,:));
roirect = [roileft roitop roiright roibottom];

                %                offsetvect = markermeanposition + (refoffsetvect * norm(bottomright - bottomleft));
                %offsetvect = markermeanposition + refoffsetvect;
                offsetvect = markermeanposition;
                
                x=offsetvect(1);
                y=offsetvect(2);
                %y = base_y - y;
                %y=y+roiwidth;
                
%                roirect = [x-1.5*roiwidth, y-1*roiwidth, x+1.5*roiwidth, y+1*roiwidth];
                glUseProgram(diskdetectorshader);
                glUniform4fv(glGetUniformLocation(diskdetectorshader, 'Roi'), 1, roirect);
                glUseProgram(0);
                y = base_y - y;
%                roirect = [x-1.5*roiwidth, y-roiwidth, x+1.5*roiwidth, y+roiwidth];
                %trackedrect = [x-roiwidth, y-roiwidth, x+roiwidth, y+roiwidth];

                % Preprocess it, store to preallocated inlevel(1):
                Screen('TransformTexture', inimage, preprocoperator, [], inlevel(1));
                
                % Perform hough classification:
                Screen('FillRect', houghimage, 0);
                houghimage = Screen('TransformTexture', inlevel(1), diskdetector, [], houghimage);

                % Tracking step: We transform the old feature position state
                % texture into the new one, performing the tracking update step
                % while doing this.
                for iters=1:5
                    newpositionstex = Screen('TransformTexture', oldpositionstex, meantrackeroperator, houghimage, newpositionstex);
                    % Swap tracking state texture handles for next tracking step:
                    dummytex = oldpositionstex;
                    oldpositionstex = newpositionstex;
                    newpositionstex = dummytex;
                end
                
                % Retrieve tracking state texture:
                glBindTexture(GL.TEXTURE_RECTANGLE_EXT, Screen('GetOpenGLTexture', win, newpositionstex));
                trackeroutput = double(glGetTexImage( GL.TEXTURE_RECTANGLE_EXT, 0, GL.RGBA, GL.FLOAT));
                glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

                                
                % Visualize results:
%                Screen('DrawTexture', win, inimage, [], dstRect);
                Screen('DrawTexture', win, inlevel(1), [], dstRect);
                Screen('DrawTexture', win, houghimage, [], AdjoinRect(dstRect, dstRect, RectRight));
                Screen('DrawTexture', win, markerbinimage, [], AdjoinRect(dstRect, dstRect, RectBottom));
                Screen('FrameRect', win, [0 0 255], [roileft, base_y - roibottom, roiright, base_y - roitop]);
                Screen('FrameRect', win, [255 255 0], [640 480 640+256 480+256]);
                Screen('FrameRect', win, [0 255 0], [(640 + ycb_low) (480+256-ycr_high) (640 + ycb_high) (480+256-ycr_low)]);
                
                % Draw little yellow ovals at the tracked pupil positions:
                for i=1:size(trackeroutput, 2)
                    Screen('FrameOval', win, [255 trackeroutput(4,i)*255 0], CenterRectOnPoint([0 0 2*radius 2*radius], double(trackeroutput(1,i)), base_y - double(trackeroutput(2,i))));
                end

                % Draw little blue ovals at the tracked marker positions:
                for i=1:size(markertrackeroutput, 2)
                    Screen('FillOval', win, [0 markertrackeroutput(4,i)*255 255], CenterRectOnPoint([0 0 10 10], double(markertrackeroutput(1,i)), base_y - double(markertrackeroutput(2,i))));
                end

                % Release this frame:
                Screen('Close', texid);
                texid = 0;
                
                [x y buttons] = GetMouse(win);
                if buttons(2)
                    if trackthemarkers == 1
                        Screen('Close', oldmarkerpositionstex);
                        oldmarkerpositionstex = Screen('MakeTexture', win, initialmarkerpositions, [], [], 1);
                        trackthemarkers = 0;
                    else
                        trackthemarkers = 1;
                    end

                    while any(buttons)
                        [x y buttons] = GetMouse(win);
                    end
                end
                
                if buttons(1)
                    y = RectHeight(texRect) - y;
                    roirect = [x-roiwidth, y-roiwidth, x+roiwidth, y+roiwidth];
                    glUseProgram(diskdetectorshader);
                    glUniform4fv(glGetUniformLocation(diskdetectorshader, 'Roi'), 1, roirect);
                    glUseProgram(0);
                    
                    Screen('Close', oldpositionstex);
                    initialpositions(1,:,1) = x ; %- roiwidth + rand(1, 10) * 2 * roiwidth;
                    initialpositions(1,:,2) = y ; % - roiwidth + rand(1, 10) * 2 * roiwidth;
                    oldpositionstex = Screen('MakeTexture', win, initialpositions, [], [], 1);

                    y = RectHeight(texRect) - y;
                    trackedrect = [x-roiwidth, y-roiwidth, x+roiwidth, y+roiwidth];
                else
                    trackedones = find(trackeroutput(3,:)>20);
                    if ~isempty(trackedones)
                        consensus = median(trackeroutput(1:2, trackedones), 2);
                        x=consensus(1);
                        y=consensus(2);

                        if ~IsInRect(x,base_y - y, roirect)
                            trackedones = [];
                        end
%                         roirect = [x-roiwidth, y-roiwidth, x+roiwidth, y+roiwidth];
%                         glUseProgram(diskdetectorshader);
%                         glUniform4fv(glGetUniformLocation(diskdetectorshader, 'Roi'), 1, roirect);
%                         glUseProgram(0);
% 
                         y = RectHeight(texRect) - y;
                         trackedrect = [x-roiwidth, y-roiwidth, x+roiwidth, y+roiwidth];
                         pupil_x = x - offsetvect(1); 
                         pupil_y = y - offsetvect(2); 

                    
                         if ~isempty(tlgaze) && ~isempty(trgaze) && ~isempty(blgaze) && ~isempty(brgaze) 
                             % Valid eye position and calibration done: Try
                             % to estimate gaze:
                             gaze_x = ((pupil_x - tlgaze(1))/(trgaze(1)-tlgaze(1))) * width
                             gaze_y = ((pupil_y - tlgaze(2))/(blgaze(2)-tlgaze(2))) * height
                             Screen('FillOval', win, 255, CenterRectOnPoint([0 0 10 10], gaze_x, gaze_y));
                         end
                    end

                    if isempty(trackedones)
                        Screen('Close', oldpositionstex);
                        [x y] = RectCenter(roirect);
                        initialpositions(1,:,1) = x ; %- roiwidth + rand(1, 10) * 2 * roiwidth;
                        initialpositions(1,:,2) = y ; % - roiwidth + rand(1, 10) * 2 * roiwidth;
                        oldpositionstex = Screen('MakeTexture', win, initialpositions, [], [], 1);
                    end
                end
                
                Screen('FrameRect', win, [255 0 255], trackedrect);
                Screen('Flip', win);

                % Check keyboard:
                [isdown secs keycode] = KbCheck;
                if isdown
                    if keycode(esc)
                        break;
                    end

                    if keycode(space)
                        while KbCheck; end;
                        KbWait;
                        while KbCheck; end;
                    end
                    
                    if keycode(key1)
                        tlgaze = [pupil_x pupil_y]
                    end
                    
                    if keycode(key2)
                        trgaze = [pupil_x pupil_y]
                    end
                    if keycode(key3)
                        brgaze = [pupil_x pupil_y]
                    end
                    if keycode(key4)
                        blgaze = [pupil_x pupil_y]
                    end

                    if keycode(upArrow)
                        graythreshold = graythreshold + 0.01;
                    end
                    
                    if keycode(downArrow)
                        graythreshold = graythreshold - 0.01;
                    end

                    if keycode(keya) && ycb_low > 0
                        ycb_low = ycb_low - 1;
                    end
                    
                    if keycode(keys) && ycb_low < ycb_high
                        ycb_low = ycb_low + 1;
                    end

                    if keycode(keyd) && ycb_high > ycb_low
                        ycb_high = ycb_high - 1;
                    end
                    
                    if keycode(keyf) && ycb_high < 255
                        ycb_high = ycb_high + 1;
                    end


                    if keycode(keyg) && ycr_low > 0
                        ycr_low = ycr_low - 1;
                    end
                    
                    if keycode(keyh) && ycr_low < ycr_high
                        ycr_low = ycr_low + 1;
                    end

                    if keycode(keyj) && ycr_high > ycr_low
                        ycr_high = ycr_high - 1;
                    end
                    
                    if keycode(keyk) && ycr_high < 255
                        ycr_high = ycr_high + 1;
                    end

                    fprintf('Params: ,%f,%f,%f,%f,%f);\n', graythreshold, ycb_low, ycb_high, ycr_low, ycr_high);
                    
                    glUseProgram(eyeclassifiershader);
%                    glUniform1f(glGetUniformLocation(diskdetectorshader, 'RadiusSquared'), radius*radius);
                    glUniform1f(glGetUniformLocation(eyeclassifiershader, 'GrayThreshold'), graythreshold);
                    glUniform4f(glGetUniformLocation(eyeclassifiershader, 'ColorRoi'), ycb_low/255, ycb_high/255, ycr_low/255, ycr_high/255);
 %                   glUniform1f(glGetUniformLocation(diskdetectorshader,
 %                   'HalfWidth'), halfwidth);
                    glUseProgram(0);
                end
            end
        end
    end
        
    sca;
return;
