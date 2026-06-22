function EyeLink_GaussianGCDemo(screenNumber)

% An EyeLink gaze-contingent demo based on the EyeLink_GCFastSamples.m demo showing how to retrieve fast online samples to
% apply a gaze-contingent gaussian weight mask. 
%
% In each trial a background image is presented with a second foreground image
% overlaid on top. The background image is a foveated image i.e., it is seen through a gaze-contingent
% gaussian window within the foreground image. The foreground image is
% visible at the periphery, outside the gaze-contingent gaussian window or whenever gaze position is not detected by the EyeLink. 
% The location of the gaussian window is updated online based on the x y coordinates of the latest gaze sample retrieved online.
% The demo presents 4 trials. Each trial ends when the space bar is pressed.
%
% Usage:
% EyeLink_GaussianGCDemo(screenNumber)
%
% screenNumber is an optional parameter which can be used to pass a specific value to Screen('OpenWindow', ...)
% If screenNumber is not specified, or if isempty(screenNumber) then the default:
% screenNumber = max(Screen('Screens'));
% will be used.
%
% The demo checks if a new sample is available online via the link. This is the most recent sample, which is faster than buffered data.
% This is equivalent to eyeLink_newest_float_sample() in C API. See EyeLink Programmers Guide manual > Function Lists > Message and Command Sending/Receiving > Functions
% It allows access to the following sample properties:
%
%       time (sample time)
%       type (SAMPLE=200)
%       gx ([left gaze x, right gaze x])
%       gy ([left gaze y, right gaze y])
%       pa ([lef eye pupil size, right eye pupil size])
%       rx (x 'pixel per degree' value)
%       ry (y 'pixel per degree' value)
%       buttons (button state and changes)
%       hdata (contains a list of 8 fields. Only the first 4 values are important:
%             [uncalibrated target sticker x, uncalibrated target sticker y, target sticker distance in mm, target flags)
%
% HISTORY
%
% mm/dd/yy
%
% 07/23/05 mk   Derived it from Frans Cornelissen's AlphaImageDemoOSX.
% 29/06/06 fwc  Derived from Mario Kleiner's GazeContingentDemoOSX ;-)
% 07/13/10 fwc  Made to work with new toolbox with callback and updated to enable eye image display, added "cleanup" function,
% 11/08/23 kd   Adapted to be similar to the official SR-Research demo EyeLink_GCFastSamples.m


% Bring the Command Window to the front if it is already open
if ~IsOctave; commandwindow; end
PsychDefaultSetup(1);

% Initialize PsychSound for calibration/validation audio feedback
InitializePsychSound();

% Use default screenNumber if none specified
if (nargin < 1)
    screenNumber = [];
end

try
    %% STEP 1: INITIALIZE EYELINK CONNECTION; OPEN EDF FILE; GET EYELINK TRACKER VERSION

    % Initialize EyeLink connection (dummymode = 0) or run in "Dummy Mode" without an EyeLink connection (dummymode = 1);
    dummymode = 0;
    EyelinkInit(dummymode); % Initialize EyeLink connection
    status = Eyelink('IsConnected');
    if status < 1 % If EyeLink not connected
        dummymode = 1;
    end

    % Open dialog box for EyeLink Data file name entry. File name up to 8 characters
    prompt = {'Enter EDF file name (up to 8 characters)'};
    dlg_title = 'Create EDF file';
    def = {'demo'}; % Create a default edf file name
    answer = inputdlg(prompt, dlg_title, 1, def); % Prompt for new EDF file name
    % Print some text in Matlab's Command Window if a file name has not been entered
    if  isempty(answer)
        fprintf('Session cancelled by user\n')
        cleanup; % Abort experiment (see cleanup function below)
        return
    end
    edfFile = answer{1}; % Save file name to a variable
    % Print some text in Matlab's Command Window if file name is longer than 8 characters
    if length(edfFile) > 8
        fprintf('Filename needs to be no more than 8 characters long (letters, numbers and underscores only)\n');
        cleanup; % Abort experiment (see cleanup function below)
        return
    end

    % Open an EDF file and name it
    failOpen = Eyelink('OpenFile', edfFile);
    if failOpen ~= 0 % Abort if it fails to open
        fprintf('Cannot create EDF file %s', edfFile); % Print some text in Matlab's Command Window
        cleanup; %see cleanup function below
        return
    end

    % Get EyeLink tracker and software version
    % <ver> returns 0 if not connected
    % <versionstring> returns 'EYELINK I', 'EYELINK II x.xx', 'EYELINK CL x.xx' where 'x.xx' is the software version
    ELsoftwareVersion = 0; % Default EyeLink version in dummy mode
    [ver, versionstring] = Eyelink('GetTrackerVersion');
    if dummymode == 0 % If connected to EyeLink
        % Extract software version number.
        [~, vnumcell] = regexp(versionstring,'.*?(\d)\.\d*?','Match','Tokens'); % Extract EL version before decimal point
        ELsoftwareVersion = str2double(vnumcell{1}{1}); % Returns 1 for EyeLink I, 2 for EyeLink II, 3/4 for EyeLink 1K, 5 for EyeLink 1KPlus, 6 for Portable Duo
        % Print some text in Matlab's Command Window
        fprintf('Running experiment on %s version %d\n', versionstring, ver );
    end
    % Add a line of text in the EDF file to identify the current experimemt name and session. This is optional.
    % If your text starts with "RECORDED BY " it will be available in DataViewer's Inspector window by clicking
    % the EDF session node in the top panel and looking for the "Recorded By:" field in the bottom panel of the Inspector.
    preambleText = sprintf('RECORDED BY Psychtoolbox demo %s session name: %s', mfilename, edfFile);
    Eyelink('Command', 'add_file_preamble_text "%s"', preambleText);


    %% STEP 2: SELECT AVAILABLE SAMPLE/EVENT DATA
    % See EyeLinkProgrammers Guide manual > Useful EyeLink Commands > File Data Control & Link Data Control

    % Select which events are saved in the EDF file. Include everything just in case
    Eyelink('Command', 'file_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,INPUT');
    % Select which events are available online for gaze-contingent experiments. Include everything just in case
    Eyelink('Command', 'link_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,BUTTON,FIXUPDATE,INPUT');
    % Select which sample data is saved in EDF file or available online. Include everything just in case
    if ELsoftwareVersion > 3  % Check tracker version and include 'HTARGET' to save head target sticker data for supported eye trackers
        Eyelink('Command', 'file_sample_data  = LEFT,RIGHT,GAZE,HREF,PUPIL,AREA,HTARGET,GAZERES,BUTTON,STATUS,INPUT');
        Eyelink('Command', 'link_sample_data  = LEFT,RIGHT,GAZE,GAZERES,AREA,HTARGET,STATUS,INPUT');
    else
        Eyelink('Command', 'file_sample_data  = LEFT,RIGHT,GAZE,HREF,PUPIL,AREA,GAZERES,BUTTON,STATUS,INPUT');
        Eyelink('Command', 'link_sample_data  = LEFT,RIGHT,GAZE,GAZERES,AREA,STATUS,INPUT');
    end


    %% STEP 3: OPEN GRAPHICS WINDOW

    % Get the list of screens and choose the one with the highest screen number.
    % Screen 0 is, by definition, the display with the menu bar. Often when
    % two monitors are connected the one without the menu bar is used as
    % the stimulus display.  Chosing the display with the highest display number is
    % a best guess about where you want the stimulus displayed.

    % Open experiment graphics on the specified screen
    if isempty(screenNumber)
        screenNumber = max(Screen('Screens')); % Use default screen if none specified
    end
    % Open a fullscreen window.
    [window, wRect]=Screen('OpenWindow',screenNumber, [128 128 128]);
    Screen('Flip', window);
    % Return width and height of the graphics window/screen in pixels
    [width, height] = Screen('WindowSize', window);


    %% STEP 4: SET CALIBRATION SCREEN COLOUR; PROVIDE WINDOW SIZE TO EYELINK HOST & DATAVIEWER; SET CALIBRATION PARAMETERS; CALIBRATE

    % Provide EyeLink with some defaults, which are returned in the structure "el".
    el = EyelinkInitDefaults(window);
    % set calibration/validation/drift-check(or drift-correct) size as well as background and target colors.
    % It is important that this background colour is similar to that of the stimuli to prevent large luminance-based
    % pupil size changes (which can cause a drift in the eye movement data)
    el.calibrationtargetsize = 3;% Outer target size as percentage of the screen
    el.calibrationtargetwidth = 0.7;% Inner target size as percentage of the screen
    el.backgroundcolour = [128 128 128];% RGB grey
    el.calibrationtargetcolour = [0 0 0];% RGB black
    % set "Camera Setup" instructions text colour so it is different from background colour
    el.msgfontcolour = [0 0 0];% RGB black
    % You must call this function to apply the changes made to the el structure above
    EyelinkUpdateDefaults(el);

    % Set display coordinates for EyeLink data by entering left, top, right and bottom coordinates in screen pixels
    Eyelink('Command','screen_pixel_coords = %ld %ld %ld %ld', 0, 0, width-1, height-1);
    % Write DISPLAY_COORDS message to EDF file: sets display coordinates in DataViewer
    % See DataViewer manual section: Protocol for EyeLink Data to Viewer Integration > Pre-trial Message Commands
    Eyelink('Message', 'DISPLAY_COORDS %ld %ld %ld %ld', 0, 0, width-1, height-1);
    % Set number of calibration/validation dots and spread: horizontal-only(H) or horizontal-vertical(HV) as H3, HV3, HV5, HV9 or HV13
    Eyelink('Command', 'calibration_type = HV9'); % horizontal-vertical 9-points
    % Allow a supported EyeLink Host PC button box to accept calibration or drift-check/correction targets via button 5
    Eyelink('Command', 'button_function 5 "accept_target_fixation"');
    % Hide mouse cursor
    HideCursor(screenNumber);
    % Start listening for keyboard input. Suppress keypresses to Matlab windows.
    ListenChar(-1);
    Eyelink('Command', 'clear_screen 0'); % Clear Host PC display from any previus drawing
    % Put EyeLink Host PC in Camera Setup mode for participant setup/calibration
    EyelinkDoTrackerSetup(el);


    %% STEP 5: TRIAL LOOP.

    spaceBar=KbName('space'); % Identify keyboard key code for space bar to end each trial later on
    ms=200; % Setup default aperture size to 2*200+1 x 2*200+1 pixels.
    hurryup=0; % Set hurryup = 1 for benchmarking - Syncing to retrace is disabled in that case so we'll get the maximum refresh rate.
    backgroundColor=GrayIndex(window); % Set background color to gray

    % Set trial images.
    fovImgList = {'fovImg1.jpeg' 'fovImg1.jpeg' 'fovImg2.jpeg' 'fovImg2.jpeg'}; % List of background (foveated) images for 4 trials
    perImgList = {'perImg1a.jpeg' 'perImg1b.jpeg' 'perImg2a.jpeg' 'perImg2b.jpeg'}; % List of peripheral (non-foveated) images for 2 trials

    for i = 1:length(fovImgList)

        % STEP 5.1: PREPARE FOVEATED (BACK) AND PERIPHERAL (FRONT) IMAGES
        % Get image names
        fovImg = char(fovImgList(i));
        perImg = char(perImgList(i));

        % Load image files:
        imdataFov = imread(fovImg);
        imdataPer = imread(perImg);

        imgInfo = imfinfo(fovImg);  % Get foveated image size info for later interest area creation

        % Build texture for foveated region:
        foveatex=Screen('MakeTexture', window, imdataFov);
        tRect=Screen('Rect', foveatex);
        Screen('DrawTexture', window, foveatex); % Prepare image texture on backbuffer

        % Save complete backbuffer as trial*.bmp to be used as Host PC & DataViewer backdrop
        stimName = ['trial' num2str(i) '.bmp']; % Prepare stimulus file name
        stimArray = Screen('GetImage', window, [], 'drawBuffer'); % Copy backbuffer to be used as stimulus
        imwrite(stimArray, stimName); % Save .bmp stimulus file in experment folder

        % Build texture for peripheral (non-foveated) region:
        nonfoveatex=Screen('MakeTexture', window, imdataPer);
        [ctRect, dx, dy]=CenterRect(tRect, wRect);

        % Create a Luminance+Alpha matrix for use as transparency mask: Layer 1 (Luminance) is filled with 'backgroundColor'.
        transLayer=2;
        [x,y]=meshgrid(-ms:ms, -ms:ms);
        maskBlob=ones(2*ms+1, 2*ms+1, transLayer) * backgroundColor;

        % Layer 2 (Transparency/Alpha) is filled with gaussian transparency mask.
        xsd=ms/2.2;
        ysd=ms/2.2;
        maskBlob(:,:,transLayer)=round(255 - exp(-((x/xsd).^2)-((y/ysd).^2))*255);

        % Build a single transparency mask texture:
        masktex=Screen('MakeTexture', window, maskBlob);

        % STEP 5.2: START TRIAL; SHOW TRIAL INFO ON HOST PC; SHOW BACKDROP IMAGE AND/OR DRAW FEEDBACK GRAPHICS ON HOST PC; DRIFT-CHECK/CORRECTION

        % Write TRIALID message to EDF file: marks the start of a trial for DataViewer
        % See DataViewer manual section: Protocol for EyeLink Data to Viewer Integration > Defining the Start and End of a Trial
        Eyelink('Message', 'TRIALID %d', i);
        % Write !V CLEAR message to EDF file: creates blank backdrop for DataViewer
        % See DataViewer manual section: Protocol for EyeLink Data to Viewer Integration > Simple Drawing
        Eyelink('Message', '!V CLEAR %d %d %d', el.backgroundcolour(1), el.backgroundcolour(2), el.backgroundcolour(3));
        % Supply the trial number as a line of text on Host PC screen
        Eyelink('Command', 'record_status_message "TRIAL %d/%d"', i, length(fovImgList));

        % Draw graphics on the EyeLink Host PC display. See COMMANDS.INI in the Host PC's exe folder for a list of commands
        Eyelink('SetOfflineMode'); % Put tracker in idle/offline mode before drawing Host PC graphics and before recording
        Eyelink('Command', 'clear_screen 0'); % Clear Host PC display from any previus drawing
        % Optional: Send an image to the Host PC to be displayed as the backdrop image over which
        % the gaze-cursor is overlayed during trial recordings.
        % See Eyelink('ImageTransfer?') for information about supported syntax and compatible image formats.
        % Below, we use the new option to pass image data from imread() as the imageArray parameter, which
        % enables the use of many image formats.
        % [status] = Eyelink('ImageTransfer', imageArray, xs, ys, width, height, xd, yd, options);
        % xs, ys: top-left corner of the region to be transferred within the source image
        % width, height: size of region to be transferred within the source image (note, values of 0 will include the entire width/height)
        % xd, yd: location (top-left) where image region to be transferred will be presented on the Host PC
        % This image transfer function works for non-resized image presentation only. If you need to resize images and use this function please resize
        % the original image files beforehand
        transferStatus = Eyelink('ImageTransfer', stimArray, 0, 0, 0, 0, 0, 0);
        if dummymode == 0 && transferStatus ~= 0 % If connected to EyeLink and image transfer fails
            fprintf('Image transfer Failed\n'); % Print some text in Matlab's Command Window
        end

        % Optional: draw feedback box and lines on Host PC interface instead of (or on top of) backdrop image.
        % See section 25.7 'Drawing Commands' in the EyeLink Programmers Guide manual
        Eyelink('Command', 'draw_box %d %d %d %d 15', round(width/2-imgInfo.Width/2), round(height/2-imgInfo.Height/2), round(width/2+imgInfo.Width/2), round(height/2+imgInfo.Height/2));

        % Perform a drift check/correction.
        % Optionally provide x y target location, otherwise target is presented on screen centre
        EyelinkDoDriftCorrection(el, round(width/2), round(height/2));

        %STEP 5.3: START RECORDING

        % Put tracker in idle/offline mode before recording. Eyelink('SetOfflineMode') is recommended
        % however if Eyelink('Command', 'set_idle_mode') is used allow 50ms before recording as shown in the commented code:
        % Eyelink('Command', 'set_idle_mode');% Put tracker in idle/offline mode before recording
        % WaitSecs(0.05); % Allow some time for transition
        Eyelink('SetOfflineMode');% Put tracker in idle/offline mode before recording
        Eyelink('StartRecording'); % Start tracker recording
        WaitSecs(0.1); % Allow some time to record a few samples before presenting first stimulus

        % Check which eye is available for gaze-contingent drawing. Returns 0 (left), 1 (right) or 2 (binocular)
        eyeUsed = Eyelink('EyeAvailable');
        % Get samples from right eye if binocular
        if eyeUsed == 2
            eyeUsed = 1;
        end

        % STEP 5.4: PRESENT STIMULUS; CREATE DATAVIEWER BACKDROP AND INTEREST AREA

        % Present peripheral (non-foveated) region
        Screen('BlendFunction', window, GL_ONE, GL_ZERO);
        Screen('DrawTexture', window, nonfoveatex);
        [~, RtStart] = Screen ('Flip', window);
        % Write message to EDF file to mark the start time of stimulus presentation.
        Eyelink('Message', 'STIM_ONSET');

        % Write !V IMGLOAD message to EDF file: creates backdrop image for DataViewer
        % See DataViewer manual section: Protocol for EyeLink Data to Viewer Integration > Image Commands
        Eyelink('Message', '!V IMGLOAD CENTER %s %d %d', stimName, width/2, height/2);
        % Write !V IAREA message to EDF file: creates interest area around image in DataViewer
        % See DataViewer manual section: Protocol for EyeLink Data to Viewer Integration > Interest Area Commands
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 1, round(width/2-imgInfo.Width/2), round(height/2-imgInfo.Height/2), round(width/2+imgInfo.Width/2), round(height/2+imgInfo.Height/2),'IMAGE_IA');

        % STEP 5.5: DRAW GAZE-CONTINGENT GAUSSIAN APERTURE; WAIT FOR KEYPRESS; SHOW BLANK SCREEN; STOP RECORDING

        % Clear some variables which keep track of previous gaze position
        xOld=0; yOld=0;

        while 1 % loop until error, space bar press
            % Check that eye tracker is  still recording. Otherwise close and transfer copy of EDF file to Display PC
            err = Eyelink('CheckRecording');
            if(err ~= 0)
                fprintf('EyeLink Recording stopped!\n');
                % Transfer a copy of the EDF file to Display PC
                Eyelink('SetOfflineMode'); % Put tracker in idle/offline mode
                Eyelink('CloseFile'); % Close EDF file on Host PC
                Eyelink('Command', 'clear_screen 0'); % Clear trial image on Host PC at the end of the experiment
                WaitSecs(0.1); % Allow some time for screen drawing
                % Transfer a copy of the EDF file to Display PC
                transferFile; % See transferFile function below
                cleanup; % Abort experiment (see cleanup function below)
                return
            end
            % Check if a new sample is available online via the link. This is the most recent sample, which is faster than buffered data
            % This is equivalent to eyeLink_newest_float_sample() in C API. See EyeLink Programmers Guide manual > Function Lists > Message and Command Sending/Receiving > Functions
            if Eyelink('NewFloatSampleAvailable') > 0
                % Get sample data in a Matlab structure
                % This is equivalent to eyeLink_newest_float_sample() in C API. See EyeLink Programmers Guide manual > Function Lists > Message and Command Sending/Receiving > Functions
                evt = Eyelink('NewestFloatSample');

                % Save sample properties as variables. See EyeLink Programmers Guide manual > Data Structures > FSAMPLE
                x = evt.gx(eyeUsed+1); % [left eye gaze x, right eye gaze x] +1 as we're accessing a Matlab array
                y = evt.gy(eyeUsed+1); % [left eye gaze y, right eye gaze y]

                % The following sample properties are also available online but are not used in this demo:
                % evt.time; % Sample EDF time
                % evt.type; % Event type (SAMPLE =200)
                % evt.pa; %[left eye pupil size, right eye pupil size]
                % evt.rx; % Gaze x 'pixel per deggree' value
                % evt.ry; % Gaze y 'pixel per degree' value
                % evt.hdata; % [uncalibrated target sticker x, uncalibrated target sticker y, target sticker distance in mm, target flags ...]

                % We only redraw if gaze position has changed:
                if (x~=xOld || y~=yOld)
                    % Compute position and size of source and destination rect and clip it, if necessary...
                    myrect=[x-ms y-ms x+ms+1 y+ms+1]; % center dRect on current mouseposition
                    dRect = ClipRect(myrect,ctRect);
                    sRect=OffsetRect(dRect, -dx, -dy);

                    % Check for a valid destination rectangle and draw image for current frame
                    if ~IsEmptyRect(dRect)

                        % Step 1: Draw alpha-mask into the backbuffer. It
                        % defines the aperture for foveation: Gaze x y location has zero alpha value. Alpha values increase with distance from 
                        % gaze according to a gaussian function and approach 255 at the border of the aperture
                        Screen('BlendFunction', window, GL_ONE, GL_ZERO);
                        Screen('DrawTexture', window, masktex, [], myrect);

                        % Step 2: Draw peripheral image where the alpha-value in the backbuffer is 255 or high, leaving
                        % the foveated area (low or zero alpha values) alone: This is done by weighting each color value of each pixel
                        % with the corresponding alpha-value in the backbuffer (GL_DST_ALPHA).
                        Screen('BlendFunction', window, GL_DST_ALPHA, GL_ZERO);
                        Screen('DrawTexture', window, nonfoveatex, [], ctRect);

                        % Step 3: Draw foveated image only where the alpha-value in the backbuffer is zero or low: This is
                        % done by weighting each color value with one minus the corresponding alpha-value in the backbuffer (GL_ONE_MINUS_DST_ALPHA).
                        Screen('BlendFunction', window, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
                        Screen('DrawTexture', window, foveatex, sRect, dRect);

                        % Clear the drawing surface. Use clearmode=2: This doesn't clear the backbuffer, which is not needed for this kind
                        % of stimulus and gives 2 msecs extra headroom for higher refresh rates. For benchmark purpose, disable
                        % syncing to retrace if hurryup is == 1.
                        Screen('Flip', window, 0, 2, 2*hurryup);
                    end
                end

                % Keep track of last gaze position. Wait 1 ms each loop iteration so system in realtime-priority is not overloaded:
                xOld=x; yOld=y;                
                WaitSecs(0.001);
            end

            % Check for keyboard press to break loop and end trial
            [~, RtEnd, keyCode] = KbCheck;
            if keyCode(spaceBar)
                % Write message to EDF file to mark the spacebar press time
                Eyelink('Message', 'KEY_PRESSED');
                reactionTime = round((RtEnd-RtStart)*1000); % Calculate RT from stimulus onset
                break;
            end
        end % End while loop

        % Draw blank screen at end of trial
        Screen('BlendFunction', window, GL_ONE, GL_ZERO);
        Screen('FillRect', window, backgroundColor);
        Screen('Flip', window);
        % Write message to EDF file to mark time when blank screen is presented
        Eyelink('Message', 'BLANK_SCREEN');
        % Write !V CLEAR message to EDF file: creates blank backdrop for DataViewer
        % See DataViewer manual section: Protocol for EyeLink Data to Viewer Integration > Simple Drawing
        Eyelink('Message', '!V CLEAR %d %d %d', el.backgroundcolour(1), el.backgroundcolour(2), el.backgroundcolour(3));

        % Stop recording eye movements at the end of each trial
        WaitSecs(0.1); % Add 100 msec of data to catch final events before stopping
        Eyelink('StopRecording'); % Stop tracker recording

        % STEP 5.6: CREATE VARIABLES FOR DATAVIEWER; END TRIAL

        % Write !V TRIAL_VAR messages to EDF file: creates trial variables in DataViewer
        % See DataViewer manual section: Protocol for EyeLink Data to Viewer Integration > Trial Message Commands
        Eyelink('Message', '!V TRIAL_VAR iteration %d', i); % Trial iteration
        Eyelink('Message', '!V TRIAL_VAR rt %d', reactionTime); % Reaction time
        % Write TRIAL_RESULT message to EDF file: marks the end of a trial for DataViewer
        % See DataViewer manual section: Protocol for EyeLink Data to Viewer Integration > Defining the Start and End of a Trial
        Eyelink('Message', 'TRIAL_RESULT 0');
        WaitSecs(0.01); % Allow some time before ending the trial
    end % End trial loop


    %% STEP 6: CLOSE EDF FILE. TRANSFER EDF COPY TO DISPLAY PC. CLOSE EYELINK CONNECTION. FINISH UP

    % Put tracker in idle/offline mode before closing file. Eyelink('SetOfflineMode') is recommended.
    % However if Eyelink('Command', 'set_idle_mode') is used, allow 50ms before closing the file as shown in the commented code:
    % Eyelink('Command', 'set_idle_mode');% Put tracker in idle/offline mode
    % WaitSecs(0.05); % Allow some time for transition
    Eyelink('SetOfflineMode'); % Put tracker in idle/offline mode
    Eyelink('Command', 'clear_screen 0'); % Clear Host PC backdrop graphics at the end of the experiment
    WaitSecs(0.5); % Allow some time before closing and transferring file
    Eyelink('CloseFile'); % Close EDF file on Host PC
    % Transfer a copy of the EDF file to Display PC
    transferFile; % See transferFile function below
catch % If syntax error is detected
    cleanup;
    % Print error message and line number in Matlab's Command Window
    fprintf('%s: some error occured\n', mfilename);
    psychrethrow(psychlasterror);
end

% Cleanup function used throughout the script above
    function cleanup
        try
            Screen('CloseAll'); % Close window if it is open
        catch
            fprintf('%s: some error occured\n', mfilename);
        end
        Eyelink('Shutdown'); % Close EyeLink connection
        ListenChar(0); % Restore keyboard output to Matlab
        ShowCursor; % Restore mouse cursor
        if ~IsOctave
            commandwindow;
        end % Bring Command Window to front
    end

% Function for transferring copy of EDF file to the experiment folder on Display PC.
% Allows for optional destination path which is different from experiment folder
    function transferFile
        try
            if dummymode ==0 % If connected to EyeLink
                % Show 'Receiving data file...' text until file transfer is complete
                Screen('FillRect', window, el.backgroundcolour); % Prepare background on backbuffer
                Screen('DrawText', window, 'Receiving data file...', 5, height-35, 0); % Prepare text
                Screen('Flip', window); % Present text
                fprintf('Receiving data file ''%s.edf''\n', edfFile); % Print some text in Matlab's Command Window

                % Transfer EDF file to Host PC
                % [status =] Eyelink('ReceiveFile',['src'], ['dest'], ['dest_is_path'])
                status = Eyelink('ReceiveFile');

                % Check if EDF file has been transferred successfully and print file size in Matlab's Command Window
                if status > 0
                    fprintf('EDF file size: %.1f KB\n', status/1024); % Divide file size by 1024 to convert bytes to KB
                end
                % Print transferred EDF file path in Matlab's Command Window
                fprintf('Data file ''%s.edf'' can be found in ''%s''\n', edfFile, pwd);
            else
                fprintf('No EDF file saved in Dummy mode\n');
            end
            cleanup;
        catch % Catch a file-transfer error and print some text in Matlab's Command Window
            fprintf('Problem receiving data file ''%s''\n', edfFile);
            cleanup;
            psychrethrow(psychlasterror);
        end
    end
end

 