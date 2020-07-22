function HDRViewer(imfilepattern, scalefactor)
% HDRViewer([imfilepattern][, scalefactor]) -- Load and show high
% dynamic range images on a compatible HDR display setup.
%
% See "help PsychHDR" for system requirements and setup instructions for HDR
% display.
%
% The viewer allows to cycle through a sequence of images in the given
% folder, matching the given filename pattern. It allows to adjust the
% intensity online, as well as to zoom into regions of the image.
%
% 'imfilepattern' - Filename search pattern of the HDR images to load,
% e.g., 'myimages*.hdr' would load all images starting with 'myimages' and
% ending with extension '.hdr'. 
%
% Control keys:
%
% You can cycle through all images by pressing the space-bar. You can change
% image intensity scaling by pressing the cursor up- and down-keys. You can
% exit the viewer by pressing ESCape. By pressing the 'q' key, you can
% toggle between float precision HDR and 256 level 8 bit quantized output.
%
% Zoom mode:
% Pressing the left mouse button will enable zoom mode. Dragging the mouse
% while the button is depressed allows to change the region which should be
% zoomed. Releasing the mouse button will show the selected region zoomed
% up to fullscreen. A single mouse click will reset to the full image.
%
% Btw. you can find lot's of nice free HDR images by Googling on the
% internet!
%
% This is an extension of SimpleHDRDemo.m, see that file for basic usage of
% HDR displays with Psychtoolbox.
%
% History:
% Written 2006 by Mario Kleiner - MPI for Biological Cybernetics, Tuebingen, Germany
% and Oguz Ahmet Akyuz - Department of Computer Science, University of Central Florida.

% History:
%
% 21-Jul-2020   mk  Written. Derived from HDRViewer.m for the BrightSide HDR display,
%                   from 2006.

% Make sure we run on Psychtoolbox-3. Abort otherwise. Use unified key names for
% keyboard input across all supported operating systems. Use normalized color range,
% not the old 0-255 8 bit color convention:
PsychDefaultSetup(2);

% Startup with black screen to not hurt our eyes:
Screen('Preference', 'VisualDebugLevel', 3);

% Name of an image file passed? If not, we default to all *.hdr files in
% current working directory:
if nargin < 1 || isempty(imfilepattern)
    % No pattern provided. Search in current working directory.
    imfilepattern = '*.hdr';
end

if nargin < 2 || isempty(scalefactor)
    scalefactor = 1;
end

% Get list of all files matching the pattern:
imfilenames = dir(imfilepattern);

% No HDR files found?
if (isempty(imfilenames))
    if ~IsOctave
        % Matlab has some - actually one - sample files here:
        imfilenames = dir([matlabroot filesep 'toolbox/images/imdata/office.hdr']);
    else
        % Octave: But if Matlab is installed on the HDR test machine, it may be here:
        imfilenames = dir('/home/shared/MATLAB/R2019a/toolbox/images/imdata/office.hdr');
    end
end

try
    % Disable keypress output to Matlab window:
    ListenChar(2);

    % Find screen to display: We choose the one with the highest number,
    % assuming this is the HDR display:
    screenid = max(Screen('Screens'));

    % Open a double-buffered fullscreen onscreen window in HDR mode on the HDR
    % display, with black background color. Color values will be specified in
    % units of nits, the display is done according to HDR10 standard, ie.
    % Color space is BT2020, SMPTE ST-2084 PQ encoding is used to drive the
    % display, output color signals have 10 bpc precision.
    PsychImaging('PrepareConfiguration');
    PsychImaging('AddTask', 'General', 'EnableHDR', 'Nits', 'HDR10');
    [win, winrect] = PsychImaging('OpenWindow', screenid, 0);
    HideCursor(win);

    % Set text properties:
    Screen('TextSize', win, 24);
    Screen('TextStyle', win , 0);

    iteration = 0;
    abortit = 0;

    % Load and show image loop:
    while ~abortit
        iteration = iteration + 1;
        imagename = [imfilenames(mod(iteration, size(imfilenames,1))+1).folder filesep imfilenames(mod(iteration, size(imfilenames,1))+1).name];

        KbReleaseWait;

        % Show some status info:
        msg = ['Loading image: ' imagename];
        fprintf([msg, '\n']);
        DrawFormattedText(win, msg, 'center', 'center', [0 300 0]);
        Screen('Flip', win);

        % Read image into Matlab matrix. If file is not recognized, skip and
        % return an empty 'img', instead of aborting with an error:
        [img, hdrType] = HDRRead(imagename, 1);
        if isempty(img)
            if KbCheck
                break;
            end

            continue;
        end

        switch hdrType
            case 'rgbe'
                % HACK: Multiply by 180.0 as a crude approximation of Radiance units to nits:
                % This is not strictly correct, but will do to get a nice enough picture for
                % demo purpose:
                img = img * 180;

            case 'openexr'
                % HACK: Multiply by 1.0:
                img = img * 1;

            otherwise
                error('Unknown image format. Do not know how to convert into units of Nits.');
        end

        % Compute maximum and max mean luminance of the image:
        maxCLL = max(max(max(img)))
        maxFALL = mean(mean(mean(img)))

        % Set step size for change of scaling factor. We get the maximum luminance
        % of our HDR display, relate it to the maximum luminance of the current
        % image, and use steps in 1% increments of that:
        hdrProperties = PsychHDR('GetHDRProperties', win);
        sfstep = hdrProperties.MaxLuminance / maxCLL * 0.01;

        % Scale intensities by some factor. Here we set the default value:
        sf = scalefactor;

        % Tell the HDR display about maximum frame average light level and maximum
        % content light level of the image:
        PsychHDR('HDRMetadata', win, [], maxFALL, maxCLL);

        % Build a Psychtoolbox 16 bpc half-float texture from the image array
        % by setting the (optional) 'floatprecision' flag to 1.
        texid = Screen('MakeTexture', win, img, [], [], 1);

        % Build also a version of the image that is quantized to 8 Bit:
        quantimg = uint8((img / max(max(max(img)))) * 255);
        ldrtexid = Screen('MakeTexture', win, quantimg);

        needupdate = 1;
        zoomset = 0;
        xleft = 0;
        xtop = 0;
        xright = -1;
        xbottom = -1;
        hdrmode = 1;

        % Image display loop:
        while needupdate
            % Compute source and destination regions for zoom mode:
            srcleft = min(xleft, xright);
            srctop = min(xtop, xbottom);
            srcright = max(xleft, xright);
            srcbottom = max(xtop, xbottom);

            if zoomset == 2
                % Zoom set: Show selected region, expanded to full screen:
                texrect = CenterRect(Screen('Rect', texid), winrect);
                srcrect = [srcleft - texrect(1) srctop - texrect(2) srcright - texrect(1) srcbottom - texrect(2)];
                rscale = min(RectWidth(winrect) / RectWidth(srcrect), RectHeight(winrect) / RectHeight(srcrect));
                dstrect = CenterRect([0 0 RectWidth(srcrect)*rscale RectHeight(srcrect)*rscale], winrect);
            else
                % No zoom: Show image 1-1:
                srcrect = [];
                dstrect = [];
                rscale=1;
            end

            % Draw selected srcrect region of our HDR image to selected
            % dstrect region of the screen:
            if hdrmode
                % Draw HDR texture:
                Screen('DrawTexture', win, texid, srcrect, dstrect, [], [], [], [sf sf sf]);
            else
                % Draw quantized 8bpc version:
                Screen('DrawTexture', win, ldrtexid, srcrect, dstrect, [], [], [], [sf sf sf]);
            end

            % Some status text:
            if hdrmode
                quantized = 'Floating point HDR';
            else
                quantized = 'Quantized to 8 bpc LDR';
            end

            [~, ny] = DrawFormattedText(win, ['Image: ' imagename ' : Zoom=' num2str(rscale) ' : Intensity scaling: ' num2str(sf) ' : ' quantized '\n'], 0, 30, [0 200 0]);
            [~, ny] = DrawFormattedText(win, 'Cursor up/down = Intensity scale up/down , q = Switch 8 bpc / HDR, SPACE = Next image, ESCAPE = Quit\n', 0, ny, [0 0 200]);
            DrawFormattedText(win, 'Left mouse button + drag = Select zoom region. Left mouse button click = Reset to unzoomed.\n', 0, ny, [0 0 200]);

            % Show selected ROI during zoom selection, if any:
            if xright ~= -1 && xbottom ~= -1 && zoomset == 1
                Screen('FrameRect', win, [200 0 0], [srcleft srctop srcright srcbottom]);
            end

            % Keypress and what key?
            [pressed , ~, keycode] = KbCheck;
            if pressed
                % Key pressed: Dispatch...
                if keycode(KbName('space'))
                    % Load next image in folder:
                    needupdate = 0;

                    % Debounce key:
                    KbReleaseWait;
                end

                if keycode(KbName('q'))
                    % Switch between display of float32 HDR image and 8 bpc LDR image:
                    needupdate = 1;
                    hdrmode = 1 - hdrmode;

                    % Debounce key:
                    KbReleaseWait;
                end

                if keycode(KbName('ESCAPE'))
                    % Exit viewer:
                    needupdate = 0;
                    abortit = 1;
                end

                if keycode(KbName('UpArrow'))
                    % Increase intensity scaling factor:
                    needupdate = 1;
                    sf = sf + sfstep;
                end

                if keycode(KbName('DownArrow'))
                    % Decrease intensity scaling factor:
                    needupdate = 1;
                    sf = sf - sfstep;
                end
            else
                % No key pressed: Check for mouse actions...
                [xm, ym, buttons] = GetMouse(win);

                % Draw tiny yellow cursor dot:
                Screen('DrawDots', win, [xm, ym], 3, [400, 400, 0]);

                if buttons(1)
                    % Left button pressed:
                    if zoomset == 0
                        % Initial press: Reset and Setup zoom operation...
                        xleft = xm;
                        xtop = ym;
                        xright = -1;
                        xbottom = -1;
                        zoomset = 1;
                    else
                        if zoomset == 1
                            % Continued press: Update zoom area:
                            xright = xm;
                            xbottom = ym;
                        else
                            % Pressed while zoomed in: Reset...
                            while buttons(1)
                                [~, ~, buttons] = GetMouse(win);
                            end
                            zoomset = 0;
                        end
                    end
                else
                    % Left button released:
                    if zoomset == 1
                        zoomset = 2;
                    end
                end
            end

            % Show updated HDR framebuffer at next vertical retrace:
            Screen('Flip', win);
        end

        % Done with this image. Release its texture and next one...
        Screen('Close', texid);
        Screen('Close', ldrtexid);
        clear img;
        clear quantimg;
    end

    % Enable keypress output to Matlab window:
    ListenChar(0);

    % Release all textures, close all windows:
    sca;

    % Well done!
    fprintf('Bye bye!\n');
catch
    % Error handler: If something goes wrong between try and catch, we
    % close the window and abort.

    % Enable keypress output to Matlab window:
    ListenChar(0);

    % Release all textures, close all windows:
    sca;

    % Rethrow the error, so higher level routines can handle it:
    psychrethrow(psychlasterror);
end
