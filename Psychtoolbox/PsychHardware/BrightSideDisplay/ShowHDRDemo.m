function ShowHDRDemo(imfilename, dummymode)
% ShowHDRDemo([imfilename][, dummymode]) -- Load and show a high dynamic range image
% on the BrightSide Technologies High Dynamic Range display device.
%
% 'imfilename' - Filename of the HDR image to load. Will load our standard
% low-dynamic range 'konijntjes' if 'imfilename' is omitted.
%
% 'dummymode' - If set to 1 we only run in emulation mode without use of
% the HDR device or BrightSide core library.
%

% Make sure we run on OpenGL-Psychtoolbox. Abort otherwise.
AssertOpenGL;

% Run demo in dummy mode?
if nargin < 2
    dummymode = 0;
end

% Name of an image file passed? If so, then load it. Load our default LDR
% image otherwise.
if nargin>=1 && ~isempty(imfilename)
    if ~isempty(findstr(imfilename, '.exr'))
        % Load a real EXR high dynamic range file:
        img = exrRead(imfilename);
    else
        % Load a standard low dynamic range file:
        img = imread(imfilename);
    end
else
    % No. Just load our default low dynamic range image file:
    img = imread([PsychtoolboxRoot '/PsychDemos/konijntjes1024x768.jpg']);
end

% Is this really a LDR image?
if max(max(max(img)))>1
    % Seems so. Convert it to double precision and normalize to range 0-1
    % to create a fake HDR image:
    img=double(img) / 255.0;
end;

% No Alpha channel provided?
if size(img, 3) < 4
    % Add a dummy one with alpha set to fully opaque:
    img(:,:,4) = ones(size(img,1), size(img,2));
end;

% img is now a height by width by 4 matrix with the (R,G,B,A) channels of our
% image. Now we use Psychtoolbox to make a HDR texture out of it and show
% it.
try
    % Find screen to display: We choose the one with the highest number,
    % assuming this is the HDR display:
    screenid=max(Screen('Screens'));

    % Initialize OpenGL mode of Psychtoolbox:
    InitializeMatlabOpenGL;
    
    % Open a standard fullscreen onscreen window, double-buffered with black
    % background color, instead of the default white one: win is the window
    % handle for this window:
    win = Screen('OpenWindow', screenid, 0);

    % Initialize the BrightSide HDR display library:
    BrightSideHDR('Initialize', win, dummymode);
    
    % Enable OpenGL mode for our onscreen window: This is needed for HDR
    % texture processing.
    Screen('BeginOpenGL', win);

    % Build a Psychtoolbox texture from the image array:
    texid = moglMakeHDRTexture(win, img);

    % End of OpenGL processing:
    Screen('EndOpenGL', win);

    rotAngle = 0;
    while ~KbCheck
        % Select the HDR backbuffer for drawing:
        BrightSideHDR('BeginDrawing', win);

        % Clear it by overdrawing with a black full screen rect:
        Screen('FillRect', win, 0);
        
        % Draw our texture into the backbuffer:
        Screen('DrawTexture', win, texid, [], [], rotAngle, 0);

        % End of drawing. Prepare HDR framebuffer for flip:
        BrightSideHDR('EndDrawing', win);
        
        % Show updated HDR framebuffer:
        Screen('Flip', win);

        % Increase rotation angle to make it a bit more interesting...
        rotAngle = rotAngle + 0.1;
    end
    
    % We're done.
    
    % Shutdown BrightSide HDR:
    BrightSideHDR('Shutdown', win);
    
    % Release all textures, close all windows:
    Screen('CloseAll');
    
    % Well done!
    fprintf('Bye bye!\n');
catch
    % Error handler: If something goes wrong between try and catch, we
    % close the window and abort.

    % Shutdown BrightSide HDR if it is online:
    BrightSideHDR('Shutdown', win);

    % Release all textures, close all windows:
    Screen('CloseAll');

    % Rethrow the error, so higher level routines can handle it:
    psychrethrow(psychlasterror);
end;
