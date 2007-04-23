function ImagingVideoCaptureDemo(filtertype, kwidth)

AssertOpenGL;
screen=max(Screen('Screens'));

if nargin < 1
    filtertype = 1;
end

if nargin < 2
    kwidth=11;
end;

try
    InitializeMatlabOpenGL;
    
    win=Screen('OpenWindow', screen, 0);

    % Initial flip to a blank screen:
    Screen('Flip',win);

    % Set text size for info text. 24 pixels is also good for Linux.
    Screen('TextSize', win, 24);
        
    % Build a filter kernel:
    stddev = kwidth / 3;

    switch(filtertype)
        case 0
            kernel = [];
        case 1
            kernel = fspecial('gaussian', kwidth, stddev);
        case 2
            kernel = fspecial('prewitt');
        case 3
            kernel = fspecial('unsharp');
        case 4
            kernel = fspecial('sobel');
        case 5
            kernel = fspecial('log');
    end

    if filtertype > 0
        % Build shader from kernel:
        shader = EXPCreateStatic2DConvolutionShader(kernel, 3, 3, 0, 1);

        % Enable shader: It will apply to any further drawing operation:
        glUseProgram(shader);
    else
        % No filtering requested: Select fixed-function pipeline
        shader = 0;
    end
    
    grabber = Screen('OpenVideoCapture', win, 0, [0 0 640 480]);
    brightness = Screen('SetVideoCaptureParameter', grabber, 'Brightness',383)
    exposure = Screen('SetVideoCaptureParameter', grabber, 'Exposure',130)
    gain = Screen('SetVideoCaptureParameter', grabber, 'Gain')
    gamma = Screen('SetVideoCaptureParameter', grabber, 'Gamma')
    shutter = Screen('SetVideoCaptureParameter', grabber, 'Shutter',7)
    Screen('SetVideoCaptureParameter', grabber, 'PrintParameters')
    vendor = Screen('SetVideoCaptureParameter', grabber, 'GetVendorname')
    model  = Screen('SetVideoCaptureParameter', grabber, 'GetModelname')

    % Setup mirror transformation for horizontal flipping:

    % xc, yc is the geometric center of the text.
    [xc, yc] = RectCenter(Screen('Rect', win));

    % Translate origin into the geometric center of text:
    Screen('glTranslate', win, xc, 0, 0);
    % Apple a scaling transform which flips the direction of x-Axis,
    % thereby mirroring the drawn text horizontally:
    Screen('glScale', win, -1, 1, 1);
    % We need to undo the translations...
    Screen('glTranslate', win, -xc, 0, 0);
    % The transformation is ready for mirrored drawing:

    Screen('StartVideoCapture', grabber, 60, 1);

    oldpts = 0;
    count = 0;
    t=GetSecs;
    while (GetSecs - t) < 600 
        if KbCheck
            break;
        end;
        
        [tex pts nrdropped]=Screen('GetCapturedImage', win, grabber, 1);
        % fprintf('tex = %i  pts = %f nrdropped = %i\n', tex, pts, nrdropped);
        
        if (tex>0)
            

            % Draw new texture from framegrabber.
            Screen('DrawTexture', win, tex, [], [], 0, 0); %Screen('Rect', win));
            
    
            % Print pts:
%            Screen('DrawText', win, sprintf('%.4f', pts - t), 0, 0, 255);
             if count>0
                % Compute delta:
                delta = (pts - oldpts) * 1000;
                oldpts = pts;
 %               Screen('DrawText', win, sprintf('%.4f', delta), 0, 20, 255);
            end;
            
            % Show it.
            Screen('Flip', win);
            Screen('Close', tex);
            tex=0;
        end;        
        count = count + 1;
    end;
    telapsed = GetSecs - t
    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);
    Screen('CloseAll');
    avgfps = count / telapsed
catch
   Screen('CloseAll');
end;
