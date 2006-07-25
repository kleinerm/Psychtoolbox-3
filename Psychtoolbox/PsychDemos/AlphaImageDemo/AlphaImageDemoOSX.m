function AlphaImageDemoOSX
%
% OS X: ___________________________________________________________________
%
% Display a gaussian masked image, locked to the cursor position,
% using the new Screen('DrawTexture') command.
% In the OS X Psychtoolbox Screen('DrawTexture') replaces
% Screen('CopyWindow').
%
% This illustrates an application of OpenGL Alpha blending by displaying
% an image masked with a gaussian transparency mask.
%
% In each frame, first the image is drawn. Then a texture acting as a
% transparency mask is drawn "over" the image, leaving only selected
% parts of the image.
%
% Please note that we only use two textures: One holding the image,
% the other defining the mask.
%
% OS 9 and WINDOWS : ______________________________________________________
%
% AlphaImageDemoOSX does not exist on OS 9 and Windows.
% _________________________________________________________________________
%
% see also: PsychDemosOSX, MovieDemoOSX, DriftDemo

% HISTORY
%
% mm/dd/yy 
% 
%  6/28/04    awi     Adapted from Denis Pelli's DriftDemo.m for OS 9
%  7/18/04    awi     Added Priority call.  Fixed.
%  9/8/04     awi     Added Try/Catch, cosmetic changes to comments and see also.
%  1/4/05     mk      Adapted from awi's DriftDemoOSX.
%  24/1/05    fwc     Adapted from AlphaImageDriftDemoOSX, bug in
%                     drawtexture prevents it from doing what I really want
%  28/1/05    fwc     Yeah, works great now after bug was removed from
%                     drawtexture by mk, cleaned up code
%  02/07/05   fwc     slightly simplified demo by removing some options
%                       such as automode
%  3/30/05    awi     Added 'BlendFunction' call to set alpha blending mode
%                     The default alpha blending mode for Screen has
%                     changed, this added call sets it to the previous
%                     default.
%  4/23/05    mk      Small modifications to make it compatible with
%                     "normal" Screen.mexmac.
%
%  12/31/05   mk      Small modifications to make it compatible with Matlab-5

try
    fprintf('AlphaImageDemo (%s)\n click on key or mouse to stop\n', datestr(now));

    % This script calls Psychtoolbox commands available only in OpenGL-based
    % versions of the Psychtoolbox. (So far, the OS X Psychtoolbox is the
    % only OpenGL-base Psychtoolbox.)  The Psychtoolbox command AssertPsychOpenGL will issue
    % an error message if someone tries to execute this script on a computer without
    % an OpenGL Psychtoolbox
    AssertOpenGL;

    % Get the list of screens and choose the one with the highest screen number.
    % Screen 0 is, by definition, the display with the menu bar. Often when
    % two monitors are connected the one without the menu bar is used as
    % the stimulus display.  Chosing the display with the highest dislay number is
    % a best guess about where you want the stimulus displayed.

    screenNumber=max(Screen('Screens'));

    % Open a double buffered fullscreen window and draw a gray background
    % and front and back buffers.
    [w, wRect]=Screen('OpenWindow',screenNumber, 0,[],32,2);
    Screen(w,'BlendFunction',GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    % Find the color values which correspond to white and black.  Though on OS
    % X we currently only support true color and thus, for scalar color
    % arguments,
    % black is always 0 and white 255, this rule is not true on other platforms will
    % not remain true on OS X after we add other color depth modes.
    white=WhiteIndex(screenNumber);
    black=BlackIndex(screenNumber);
    gray=GrayIndex(screenNumber); % returns as default the mean gray value of screen

    % import image and and convert it, stored in
    % MATLAB matrix, into a Psychtoolbox OpenGL texture using 'MakeTexture';

    myimgfile= 'konijntjes1024x768';

    fprintf('Using image ''%s''\n', myimgfile);

    imdata=imread(myimgfile, 'jpg');
    %     crop image if it is larger then screen size. There's no image scaling
    %     in maketexture
    [iy, ix, id]=size(imdata);
    [wW, wH]=WindowSize(w);
    if ix>wW | iy>wH
        disp('Image size exceeds screen size');
        disp('Image will be cropped');
    end

    if ix>wW
        cl=round((ix-wW)/2);
        cr=(ix-wW)-cl;
    else
        cl=0;
        cr=0;
    end
    if iy>wH
        ct=round((iy-wH)/2);
        cb=(iy-wH)-ct;
    else
        ct=0;
        cb=0;
    end


    imagetex=Screen('MakeTexture', w, imdata(1+ct:iy-cb, 1+cl:ix-cr,:));
    tRect=Screen('Rect', imagetex);

    [ctRect, dx, dy]=CenterRect(tRect, wRect);

    % We create a Luminance+Alpha matrix for use as transparency mask:
    % Layer 1 (Luminance) is filled with luminance value 'gray' of the
    % background.
    ms=100;
    transLayer=2;
    [x,y]=meshgrid(-ms:ms, -ms:ms);
    maskblob=uint8(ones(2*ms+1, 2*ms+1, transLayer) * gray);
    size(maskblob);
    % Layer 2 (Transparency aka Alpha) is filled with gaussian transparency
    % mask.
    xsd=ms/2.0;
    ysd=ms/2.0;
    maskblob(:,:,transLayer)=uint8(round(255 - exp(-((x/xsd).^2)-((y/ysd).^2))*255));
    % Build a single transparency mask texture:
    masktex=Screen('MakeTexture', w, maskblob);
    mRect=Screen('Rect', masktex);

    fprintf('Size image texture: %d x %d\n', RectWidth(tRect), RectHeight(tRect));
    fprintf('Size  mask texture: %d x %d\n', RectWidth(mRect), RectHeight(mRect));

    Screen('FillRect',w, gray);
    Screen('Flip', w);

    [a,b]=WindowCenter(w);
    WaitSetMouse(a,b,screenNumber); % set cursor and wait for it to take effect
    
    HideCursor;
    buttons=0;

	 priorityLevel=MaxPriority(w);
    Priority(priorityLevel);
    % "pre-load" textures
    Screen('DrawTexture', w, imagetex);
    Screen('DrawTexture', w, masktex);
    Screen('FillRect',w, gray);
    Screen('Flip', w);
    while KbCheck; WaitSecs(0.1); end;
    Screen('Flip', w); % would be useful 
    mxold=0;
    myold=0;
    while (1)
        % We wait at least 10 ms each loop-iteration so that we
        % don't overload the system in realtime-priority:
        WaitSecs(0.01);
        
        [mx, my, buttons]=GetMouse(screenNumber);
        % We only redraw if mouse has been moved:
        if (mx~=mxold | my~=myold)            
            myrect=[mx-ms my-ms mx+ms+1 my+ms+1]; % center dRect on current mouseposition
            dRect = ClipRect(myrect,ctRect);
            sRect=OffsetRect(dRect, -dx, -dy);

            if ~IsEmptyRect(dRect)
                % Draw image for current frame:
                Screen('DrawTexture', w, imagetex, sRect, dRect);
                % Overdraw -- and therefore alpha-blend -- with gaussian alpha mask:
                Screen('DrawTexture', w, masktex, [], myrect);
                % Show result on screen:
                Screen('Flip', w);
            end
        end;

        mxold=mx;
        myold=my;

        if KbCheck | find(buttons) % break out of loop
            break;
        end;
    end;
    Screen('DrawTexture', w, imagetex);
    Screen('Flip', w);

    %The same commands which closes onscreen and offscreen windows also
    %closes textures.
    Screen('CloseAll');
    ShowCursor;
    Priority(0);
    fprintf('End of AlphaImageDemo\n\n');

catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    ShowCursor;
    Priority(0);
    psychrethrow(lasterror);
end %try..catch.
