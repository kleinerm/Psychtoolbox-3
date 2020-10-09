function HDRDebugViewer(imfilepattern, scalefactor, gpudebug)
% HDRDebugViewer([imfilepattern][, scalefactor=autoselect][gpudebug=1]) -- Load and show high
% dynamic range images on a compatible HDR display setup.
%
% See "help PsychHDR" for system requirements and setup instructions for HDR
% display. See "help HDRRead" for supported HDR image file formats, ie. the
% formats that HDRRead() can load.
%
% The viewer allows to cycle through a sequence of images in the given
% folder, matching the given filename pattern. It allows to adjust the
% intensity online, as well as to zoom into regions of the image. A simple
% "color picker" displays the RGB color values in units of nits under the cursor
% position.
%
% 'imfilepattern' - Filename search pattern of the HDR images to load,
% e.g., 'myimages*.hdr' would load all images in the current folder that
% are starting with 'myimages' and ending with extension '.hdr'.
%
% If 'imfilepattern' is omitted or empty then image files bundled with
% Psychtoolbox and Matlab are loaded. Additionally the viewer will offer to
% download some freely useable sample files from the MPI for Informatics in
% Saarbruecken, Germany. Another bunch of OpenEXR sample images can be found
% on the OpenEXR website under:
%
% https://www.openexr.com/downloads.html
%
%
% Limitations:
%
% The viewers formulas for converting pixel color values from the units in the
% image file to the required linear unit in Nits are not neccessarily correct, as
% there seems to be not much agreement about what the units in the image files are
% supposed to reflect. In case of Radiance files (.hdr) we multiply by some fixed
% number, when we should actually convert from units of Radiance to units of
% Luminance. For the OpenEXR format and others, we just use values as they are,
% assuming they are already in Nits. This seems to be the case for some, but not all,
% sample images.
%
% Similarly the formula for computing frame average light level and max content
% light level may or may not be 100% according to spec. At least they give reasonable
% results...
%
% Control keys:
%
% You can cycle through all images by pressing the space-bar. You can change
% image intensity scaling by pressing the cursor up- and down-keys. You can
% exit the viewer by pressing ESCape. By pressing the 'q' key, you can
% toggle between float precision HDR and 256 level 8 bit quantized output.
%
% Zoom mode:
%
% Pressing the left mouse button will enable zoom mode. Dragging the mouse
% while the button is depressed allows to change the region which should be
% zoomed. Releasing the mouse button will show the selected region zoomed
% up to fullscreen. A single mouse click will reset to the full image.
%
%
% This is an extension of SimpleHDRDemo.m, see that file for the most basic usage
% of HDR displays with Psychtoolbox.
%

% History:
%
% 21-Jul-2020   mk  Written. Derived from HDRViewer.m for the BrightSide HDR display,
%                   from 2006.

% Make sure we run on Psychtoolbox-3. Abort otherwise. Use unified key names for
% keyboard input across all supported operating systems. Use normalized color range,
% not the old 0-255 8 bit color convention:
PsychDefaultSetup(2);

% Startup with black screen to not hurt our eyes:
oldVis = Screen('Preference', 'VisualDebugLevel', 3);

% Name of an image file passed? If not, we default to all *.hdr files in
% current working directory:
if nargin < 1 || isempty(imfilepattern)
    % No pattern provided. Set up so nothing is found:
    imfilepattern = 'NONONONO';
end

% No intensity scaling by default:
if nargin < 2 || isempty(scalefactor)
    scalefactor = 1;
end

% No time-consuming debug mode by default:
if nargin < 3 || isempty(gpudebug)
    gpudebug = 1;
end

% Get list of all files matching the pattern:
imfilenames = dir(imfilepattern);

% No HDR files found at user-provided search location/pattern?
% Use our own bundled demo images then:
if isempty(imfilenames)
    % Try to load our own OpenEXR ".exr" sample images:
    imfilenames = FileFromFolder([PsychtoolboxRoot 'PsychDemos/OpenEXRImages/'], 'ssilent', {'exr', 'hdr', 'dpx'});

    % Check for sample collection of MPI for informatics:
    if ~exist([PsychHomeDir 'ThirdPartyHDRSampleImages/'], 'dir')
        % Nope. Ask if we should fetch those images?
        if TwoStateQuery('Did not find the freely downloadable and useable OpenEXR HDR image samples from MPI for Informatics Saarbruecken (http://resources.mpi-inf.mpg.de/hdr/gallery.html). Should i download them to your Home directory?') == 1
            % Yes:
            fprintf('Trying to download files to folder %s ...\n', [PsychHomeDir 'ThirdPartyHDRSampleImages']);
            fprintf('See the MPI for Informatics Saarbruecken webpage for credits, copyright and license info, as well as more\n');
            fprintf('background info about these files:\nhttp://resources.mpi-inf.mpg.de/hdr/gallery.html\n\n');
            % Define path to target folder, create folder if it does not yet exist:
            target = PsychHomeDir('ThirdPartyHDRSampleImages');
            % Get'em:
            mpiimnames = {'Iwate.exr', 'AtriumMorning.exr', 'AtriumNight.exr', 'snow.exr', 'nancy_cathedral_1.exr', 'nancy_cathedral_2.exr', 'mpi_atrium_1.exr'};
            for imname=mpiimnames
                tname = char(imname);
                [fname, rc] = urlwrite(['http://resources.mpi-inf.mpg.de/hdr/img_hdr/' tname], [target tname]); %#ok<URLWR>
                fprintf('Tried to download sample file to: ''%s'' [Status: %i]\n', fname, rc);
            end
        end
    end

    % Add MPI sample files if they exist in the users home directory:
    if exist([PsychHomeDir 'ThirdPartyHDRSampleImages/'], 'dir')
        imfilenames1 = FileFromFolder([PsychHomeDir 'ThirdPartyHDRSampleImages/'], 'ssilent', {'exr', 'hdr', 'dpx'});
        imfilenames = [imfilenames ; imfilenames1];
    end

    if ~IsOctave && exist([matlabroot filesep 'toolbox/images/imdata/office.hdr'], 'file')
        % Matlab has one Radiance .hdr sample file here, so load it if we are on Matlab:
        imfilenames1 = FileFromFolder([matlabroot filesep 'toolbox/images/imdata/office.hdr']);
        imfilenames = [imfilenames ; imfilenames1];
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
    [xm, ym] = RectCenter(winrect);
    SetMouse(xm, ym, win);
    HideCursor(win);
    Screen('Hookfunction', win, 'Disable', 'FinalOutputFormattingBlit');
    % Set text properties:
    Screen('TextSize', win, 18);
    Screen('TextStyle', win , 0);

    iteration = 0;
    abortit = 0;

    % Load and show image loop:
    while ~abortit
        iteration = iteration + 1;
        imagename = [imfilenames(mod(iteration, size(imfilenames,1))+1).folder filesep imfilenames(mod(iteration, size(imfilenames,1))+1).name];

        % Show some status info:
        msg = ['Loading image: ' imagename];
        disp(msg);
        DrawFormattedText(win, msg, 'center', 'center', [0 300 0]);
        Screen('Flip', win);

        % Read image into Matlab matrix. If file is not recognized, skip and
        % return an empty 'img', instead of aborting with an error:
        [img, info, errmsg] = HDRRead(imagename, 1);
        if isempty(img)
            % Not recognized as HDR file. Try if imread() can handle it as
            % standard SDR image file:
            try
                img = imread(imagename);
                info.format = 'sdr';
            catch
                msg = ['Could not load file ' imagename ' as either HDR or SDR image file, skipping.\n' errmsg];
                fprintf([msg, '\n']);
                DrawFormattedText(win, [msg '\nPress ESCAPE key to abort viewer in case of endless loops.'], 'center', 'center', [300 0 0]);
                Screen('Flip', win);

                [~, keyCode] = KbStrokeWait(-1, GetSecs + 20);
                if keyCode(KbName('ESCAPE'))
                    break;
                end

                continue;
            end
        end

        switch info.format
            case 'rgbe'
                % HACK: Multiply by 180.0 as a crude approximation of Radiance units to nits:
                % This is not strictly correct, but will do to get a nice enough picture for
                % demo purpose:
                img = img * 180;

            case 'openexr'
                % HACK: Multiply by 1.0:
                img = img * 1;

            case 'sdr'
                % Standard - supposedly SDR - file read via imread():
                img = img * 1;

            otherwise
                error('Unknown image format. Do not know how to convert into units of Nits.');
        end

        KbReleaseWait;

        % Compute maximum and max mean luminance of the image: Do not abort
        % on errors (flag 1), but just return an error message in 'errmsg',
        % and maxFALL = maxCLL = 0 for "don't know". The returned maxFALL
        % and maxCLL are clamped to the safe range of 0 - 65535 nits:
        [maxFALL, maxCLL, ~, ~, errmsg] = ComputeHDRStaticMetadataType1ContentLightLevels(img, [], [], 1);
        if ~isempty(errmsg)
            % Something is wrong with this image 'img'. Lets skip it:
            msg = ['Could not compute content light levels for image file ' imagename '. Image may display funny. Error was:\n' errmsg];
            fprintf([msg, '\n']);
            DrawFormattedText(win, [msg '\nPress ESCAPE key to abort viewer in case of endless loops.'], 'center', 'center', [300 0 0]);
            Screen('Flip', win);
            [~, keyCode] = KbStrokeWait(-1, GetSecs + 5);
            if keyCode(KbName('ESCAPE'))
                break;
            end
        end

        % Set step size for change of scaling factor. We get the maximum luminance
        % of our HDR display, relate it to the maximum luminance of the current
        % image, and use steps in 1% increments of that:
        hdrProperties = PsychHDR('GetHDRProperties', win);
        sfstep = hdrProperties.MaxLuminance / maxCLL * 0.1;

        % Scale intensities by some factor. Here we set the default value:
        sf = scalefactor;

        % Tell the HDR display about maximum frame average light level and maximum
        % content light level of the image:
        imgpropmsg = sprintf('Setting image maxFALL %f nits, maxCLL %f nits\n', maxFALL, maxCLL);
        if maxCLL > 10000
            imgpropmsg = [imgpropmsg sprintf('-> maxCLL %f exceeds 10000 nits! Rescaling to clamp to 10000 nits.\n', maxCLL)]; %#ok<AGROW>

            nrhigh = length(find(img > 10000));
            if nrhigh < 0.005 * numel(img)
                % Less than 0.5% of all pixels are above threshold. Just
                % clamp them to threshold and avoid rescaling:
                img(img > 10000) = 10000;
                maxCLL = 10000;
                imgpropmsg = [imgpropmsg sprintf('-> Only %i pixels [%f %% of all pixels] exceed 10000 nits! Just clamping those to 10000 nits.\n', nrhigh, nrhigh / numel(img) * 100)]; %#ok<AGROW>
            end

            scaledown = 10000 / maxCLL;
            img = img * scaledown;

            % Recompute maximum and max mean luminance of the image:
            [maxFALL, maxCLL] = ComputeHDRStaticMetadataType1ContentLightLevels(img);

            imgpropmsg = [imgpropmsg sprintf('-> Clamping: Setting image maxFALL %f nits, maxCLL %f nits\n', maxFALL, maxCLL)]; %#ok<AGROW>
        end
        disp(imgpropmsg);

        if maxFALL > hdrProperties.MaxFrameAverageLightLevel
            imgpropmsg = [imgpropmsg sprintf('-> maxFALL %f exceeds sustainable MaxFrameAverageLightLevel %f nits of display! Rescaling to clamp maxFALL to sustainable range.\n', maxFALL, hdrProperties.MaxFrameAverageLightLevel)]; %#ok<AGROW>

            scaledown = hdrProperties.MaxFrameAverageLightLevel / maxFALL;
            img = img * scaledown;

            % Recompute maximum and max mean luminance of the image:
            [maxFALL, maxCLL] = ComputeHDRStaticMetadataType1ContentLightLevels(img);

            imgpropmsg = [imgpropmsg sprintf('-> Clamping: Setting image maxFALL %f nits, maxCLL %f nits\n', maxFALL, maxCLL)]; %#ok<AGROW>
        end
        disp(imgpropmsg);

        % Build a 32 bpc single-precision float texture from the image
        % array by setting the (optional) 'floatprecision' flag to 2.
        % texid = Screen('MakeTexture', win, img, [], [], 2);
        %
        % Or simply don't, because the 'floatprecision' flag defaults to 2
        % in HDR display mode anyway if omitted, for your convenience:
        texid = Screen('MakeTexture', win, img);

        % Build also a version of the image that is quantized to 8 Bit:
        % Note: Difficult to say, if the following conversion formula is the best
        % one. In tonemapping, there are many ways to slice the cat, and this is
        % the most naive method:
        quantimg = uint8((img * 255 / max(max(max(img)))));

        % This will take the uint8 256 discrete level matrix and turn it
        % back into a "HDR" texture of fp16 precision, by mapping the input
        % value range 0 - 255 to the range 0.0 nits - 80.0 nits, because 0
        % - 80 nits is considered to be the SDR "standard dynamic range" of
        % a regular display or image. Note the floatprecision flag being
        % specified as 1. If omitted, this would create a fp32 texture by
        % default - a safe choice, but maybe a bit overkill for just
        % storing 8 bpc content:
        ldrtexid = Screen('MakeTexture', win, quantimg, [], [], 1);

        % Send current content light level properties to display:
        PsychHDR('HDRMetadata', win, 0, maxFALL * sf, maxCLL * sf);

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

            % Send current content light level properties to display, clamped to 10k nits:
            % Or don't! While this would be strictly the right thing to do
            % in my opinion, it creates a seriously disrupting experience
            % for users of NVidia graphics cards on MS-Windows, at least as
            % of NVidia driver version 445 in August 2020: Each time you
            % set new HDR metadata, the driver will perform a full modeset
            % sequence on the monitor, with a signal dropout and flicker
            % for about 1 second. This totally disrupts the viewing
            % experience if you actually try to adjust the scaling factor
            % 'sf' and thereby defeats the purpose. The only solution for
            % this would be a driver fix by NVidia. Note that PsychHDR will
            % detect redundant updates of metadata and skip such update
            % calls, so the performance impact is already as much reduced
            % as possible from Psychtoolbox side.
            % AMD's Windows driver is not quite as bad, but not great
            % either, taking over 120 msecs for such a setup call...
            % PsychHDR('HDRMetadata', win, 0, min(maxFALL * sf, 10000), min(maxCLL * sf, 10000));

            % Some status text:
            if hdrmode
                quantized = 'Floating point HDR';
            else
                quantized = 'Quantized to 8 bpc LDR';
            end

            [~, ny] = DrawFormattedText(win, ['Image: ' imagename ' : Zoom=' num2str(rscale) ' : Intensity scaling: ' num2str(sf) ' : ' quantized '\n'], 0, 30, [0 200 0]);
            [~, ny] = DrawFormattedText(win, [imgpropmsg '\n'], 0, ny, [0 200 0]);
            [~, ny] = DrawFormattedText(win, 'Cursor up/down = Intensity scale up/down , q = Switch 8 bpc / HDR, SPACE = Next image, ESCAPE = Quit\n', 0, ny, [0 0 200]);
            [~, ny] = DrawFormattedText(win, 'Left mouse button + drag = Select zoom region. Left mouse button click = Reset to unzoomed.\n', 0, ny, [0 0 200]);

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
                    sf = max(sf - sfstep, 0);
                end
            else
                % No key pressed: Check for mouse actions...
                [xm, ym, buttons] = GetMouse(win);

                % Take a screenshot of the pixel below the mouse:
                mouseposrgb = Screen('GetImage', win, OffsetRect([0 0 1 1], xm, ym), 'drawBuffer', 1);
                msg = sprintf('RGB at cursor position (%f, %f): (%f, %f, %f) nits.\n', xm, ym, mouseposrgb);
                [~, ny] = DrawFormattedText(win, msg, 0, ny, [100 100 0]); %#ok<ASGLU>

                % Draw tiny yellow cursor dot:
                [~, mi] = max(mouseposrgb);
                switch (mi)
                    case 1
                        cursorcolor = [0, 200, 200];
                    case 2
                        cursorcolor = [200, 0, 200];
                    case 3
                        cursorcolor = [200, 200, 0];
                end

                % Low level debugging enabled?
                if gpudebug
                    % Print out color sample from user-facing virtual framebuffer - values in nits:
                    fprintf(msg);
                    % Make sure our cursor does not occlude the sample location for debug readback:
                    Screen('FrameRect', win, cursorcolor, CenterRectOnPoint([0 0 4 4], xm, ym));
                else
                    Screen('DrawDots', win, [xm, ym], 3, cursorcolor);
                end

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

            % Low level debugging enabled?
            if gpudebug
                % Take a sample from the actual framebuffer, which contains the unorm range OETF encoded content:
                gpupqvalue = (Screen('GetImage', win, OffsetRect([0 0 1 1], xm, ym), 'backBuffer', 1, 3));
                fprintf('PQ unorm at cursor position (%f, %f): (%f, %f, %f).\n\n', xm, ym, gpupqvalue);
            end
        end

        % Done with this image. Release its texture and next one...
        Screen('Close', texid);
        Screen('Close', ldrtexid);
        clear img;
        clear quantimg;
    end

    % Restore old startup screen display mode:
    Screen('Preference', 'VisualDebugLevel', oldVis);

    % Enable keypress output to Matlab window:
    ListenChar(0);

    % Release all textures, close all windows:
    sca;

    % Well done!
    fprintf('Bye bye!\n');
catch
    % Error handler: If something goes wrong between try and catch, we
    % close the window and abort.

    % Restore old startup screen display mode:
    Screen('Preference', 'VisualDebugLevel', oldVis);

    % Enable keypress output to Matlab window:
    ListenChar(0);

    % Release all textures, close all windows:
    sca;

    % Rethrow the error, so higher level routines can handle it:
    psychrethrow(psychlasterror);
end
