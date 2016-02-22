function EyelinkPictureCustomCalibration

%
% ___________________________________________________________________
%
% Demo implemetation of EyelinkPicture with custom calbration
% based on EyelinkPicture by JS
%
%
% stimulus: 3 pictures
%
% This task demonstrates stimuli presentation with Eyelink and Data Viewer
% integration as well as a few functionalities:
% 1. how to perform a custom calibration by modifying
%    calibration target positions 
% 2. how to change camera setup parameters
%    on the hostfrom within ETB
% 3. how to query host pc for variable values such as mount type
%
% ___________________________________________________________________

% HISTORY
% mm/dd/yy
%
% 01/28/11  NJ  created
% 12/20/13  LJ  changed isoctave to IsOctave, case sensitive for the latest matlab
                fixed issue with non integer arguments for Eyelink('message' ...)



clear;
if ~IsOctave
    commandwindow;
else
    more off;
end
% list of images used for the trial. Octave cares about capitalization 
imageList = {'town.bmp' 'town_blur.bmp' 'composite.bmp'};
dummymode=0;
try
    %%%%%%%%%%
    % STEP 1 %
    %%%%%%%%%%
    
    % Added a dialog box to set your own EDF file name before opening
    % experiment graphics. Make sure the entered EDF file name is 1 to 8
    % characters in length and only numbers or letters are allowed.
    if IsOctave
        edfFile = 'DEMO';
    else  
        prompt = {'Enter tracker EDF file name (1 to 8 letters or numbers)'};
        dlg_title = 'Create EDF file';
        num_lines= 1;
        def     = {'DEMO'};
        answer  = inputdlg(prompt,dlg_title,num_lines,def);
        %edfFile= 'DEMO.EDF'
        edfFile = answer{1};
        fprintf('EDFFile: %s\n', edfFile );
    end
    %%%%%%%%%%
    % STEP 2 %
    %%%%%%%%%%
    
    % Open a graphics window on the main screen
    % using the PsychToolbox's Screen function.
    screenNumber=max(Screen('Screens'));
    [window, wRect]=Screen('OpenWindow', screenNumber, 0,[],32,2); %#ok<*NASGU>
    Screen(window,'BlendFunction',GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    [wW, wH] = WindowSize(window);
    
    %%%%%%%%%%
    % STEP 3 %
    %%%%%%%%%%
    
    % Provide Eyelink with details about the graphics environment
    % and perform some initializations. The information is returned
    % in a structure that also contains useful defaults
    % and control codes (e.g. tracker state bit and Eyelink key values).
    
    el=EyelinkInitDefaults(window);
    
    el.backgroundcolour = BlackIndex(el.window);
    el.msgfontcolour    = WhiteIndex(el.window);
    el.imgtitlecolour = WhiteIndex(el.window);
    el.targetbeep = 0;
    el.calibrationtargetcolour= WhiteIndex(el.window);
    el.calibrationtargetsize= 1;
    el.calibrationtargetwidth=0.5;
    el.displayCalResults = 1;
    el.eyeimgsize=50;
    EyelinkUpdateDefaults(el);
    
    %%%%%%%%%%
    % STEP 4 %
    %%%%%%%%%%
    
    % Initialization of the connection with the Eyelink Gazetracker.
    % exit program if this fails.
    if ~EyelinkInit(dummymode)
        fprintf('Eyelink Init aborted.\n');
        cleanup;  % cleanup function
        return;
    end
        
    % open file to record data to
    res = Eyelink('Openfile', edfFile);
    if res~=0
        fprintf('Cannot create EDF file ''%s'' ', edffilename);
        cleanup;
        %         Eyelink( 'Shutdown');
        return;
    end
    
    Eyelink('command', 'add_file_preamble_text ''Recorded by EyelinkToolbox demo-experiment''');
    [width, height]=Screen('WindowSize', screenNumber);
    
    
    % make sure we're still connected.
    if Eyelink('IsConnected')~=1 && ~dummymode
        fprintf('Not connected. exiting');
        cleanup;
        return;
    end    
    
    %%%%%%%%%%
    % STEP 5 %
    %%%%%%%%%%
    
    % SET UP TRACKER CONFIGURATION
    % Setting the proper recording resolution, proper calibration type,
    % as well as the data file content;
    
    % it's location here is overridded by EyelinkDoTracker which resets it
    % with display PC coordinates
    Eyelink('command','screen_pixel_coords = %ld %ld %ld %ld', 0, 0, width-1, height-1);
    Eyelink('message', 'DISPLAY_COORDS %ld %ld %ld %ld', 0, 0, width-1, height-1);
    % set calibration type.
    Eyelink('command', 'calibration_type = HV5');
    % you must send this command with value NO for custom calibration
    % you must also reset it to YES for subsequent experiments
    Eyelink('command', 'generate_default_targets = NO');
    
    % STEP 5.1 modify calibration and validation target locations
    Eyelink('command','calibration_samples = 6');
    Eyelink('command','calibration_sequence = 0,1,2,3,4,5');
    Eyelink('command','calibration_targets = %d,%d %d,%d %d,%d %d,%d %d,%d',...
        width/2,height/2,  width/2,height*0.2,  width/2,height - height*0.2,  width*0.2,height/2,  width - width*0.2,height/2 );
    Eyelink('command','validation_samples = 5');
    Eyelink('command','validation_sequence = 0,1,2,3,4,5');
    Eyelink('command','validation_targets = %d,%d %d,%d %d,%d %d,%d %d,%d',...
        width/2,height/2,  width/2,height*0.2,  width/2,height - height*0.2,  width*0.2,height/2,  width - width*0.2,height/2 );
    
    
    % set parser (conservative saccade thresholds)
    Eyelink('command', 'saccade_velocity_threshold = 35');
    Eyelink('command', 'saccade_acceleration_threshold = 9500');
    % set EDF file contents
    % STEP 5.2 retrieve tracker version and tracker software version
    [v,vs] = Eyelink('GetTrackerVersion');
    fprintf('Running experiment on a ''%s'' tracker.\n', vs );
    vsn = regexp(vs,'\d','match'); % wont work on EL I
    if isempty(vsn)
        eyelinkI = 1;
    else
        eyelinkI = 0;
    end
    
    if v == 3 && str2double(vsn{1}) == 4 % if EL 1000 and tracker version 4.xx
        
        % remote mode possible add HTARGET ( head target)
        Eyelink('command', 'file_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,INPUT');
        Eyelink('command', 'file_sample_data  = LEFT,RIGHT,GAZE,HREF,AREA,GAZERES,STATUS,INPUT,HTARGET');
        % set link data (used for gaze cursor)
        Eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,FIXUPDATE,INPUT');
        Eyelink('command', 'link_sample_data  = LEFT,RIGHT,GAZE,GAZERES,AREA,STATUS,INPUT,HTARGET');
    else
        Eyelink('command', 'file_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,INPUT');
        Eyelink('command', 'file_sample_data  = LEFT,RIGHT,GAZE,HREF,AREA,GAZERES,STATUS,INPUT');
        % set link data (used for gaze cursor)
        Eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,FIXUPDATE,INPUT');
        Eyelink('command', 'link_sample_data  = LEFT,RIGHT,GAZE,GAZERES,AREA,STATUS,INPUT');
    end
    % allow to use the big button on the eyelink gamepad to accept the
    % calibration/drift correction target
    Eyelink('command', 'button_function 5 "accept_target_fixation"');
    
    
    % STEP 5.3 change camera setup options
    if v == 3
        % set pupil Tracking model in camera setup screen
        % no = centroid. yes = ellipse
        Eyelink('command', 'use_ellipse_fitter = no');
        % set sample rate in camera setup screen
        Eyelink('command', 'sample_rate = %d',1000);
    end
   
    % if desktop mount and tracker version is 4.2 or later change
    % illumination
    twropts = {'TOWER','MPRIM','MIRROR','BLRR','MLRR'};
    % query tracker for mount type using elcl_select_configuration variable
    [result,reply]=Eyelink('ReadFromTracker','elcl_select_configuration');
    
    if ~eyelinkI && ~dummymode && ~result && ~any(strcmp(reply,twropts)) && str2double(vsn{1}) == 4 && str2double(vsn{2}) >= 2
        %set illumination power in camera setup screen
        % 1 = 100%, 2 = 75%, 3 = 50%
        
        Eyelink('command', 'elcl_tt_power = %d',2);
    else 
        disp('failed to change illumination. possible causes: DummyMode, EL not desktop mount, EL not 1000, EL version number pre 4.2, EL disconnected');
    end
    
    % query host to see if automatic calibration sequencing is enabled.
    % ReadFromTracker needs to have 2 outputs.
    % variables querable are listed in the .ini files in the host
    % directories. Note that not all variables are querable.
    [result, reply]=Eyelink('ReadFromTracker','enable_automatic_calibration');
    
    if reply % reply = 1
        fprintf('Automatic sequencing ON');
    else
        fprintf('Automatic sequencing OFF');
    end
    
    %%%%%%%%%%
    % STEP 6 %
    %%%%%%%%%%
    disp('pre cal')
    % Hide the mouse cursor;
    Screen('HideCursorHelper', window);
    % enter Eyetracker camera setup mode, calibration and validation
    EyelinkDoTrackerSetup(el);
    disp('post cal')
    %%%%%%%%%%
    % STEP 7 %
    %%%%%%%%%%
    
    % Now starts running individual trials;
    % You can keep the rest of the code except for the implementation
    % of graphics and event monitoring
    % Each trial should have a pair of "StartRecording" and "StopRecording"
    % calls as well integration messages to the data file (message to mark
    % the time of critical events and the image/interest area/condition
    % information for the trial)
    
    for i=1:3
        
        imgfile = char(imageList(i));
        
        % STEP 7.1
        % Sending a 'TRIALID' message to mark the start of a trial in Data
        % Viewer.  This is different than the start of recording message
        % START that is logged when the trial recording begins. The viewer
        % will not parse any messages, events, or samples, that exist in
        % the data file prior to this message.
        Eyelink('Message', 'TRIALID %d', i);
        
        % This supplies the title at the bottom of the eyetracker display
        Eyelink('command', 'record_status_message "TRIAL %d/%d  %s"', i, 3, imgfile);
        % Before recording, we place reference graphics on the host display
        % Must be in offline mode to transfer image to Host PC
        Eyelink('Command', 'set_idle_mode');
        % clear tracker display and draw box at center
        Eyelink('Command', 'clear_screen %d', 0);
        
        finfo = imfinfo(imgfile);
        finfo.Filename 
          Status = Eyelink('ImageTransfer', finfo.Filename ,0,0,0,0,round(wW/2 - finfo.Width/2) ,round(wH/2 - finfo.Height/2),4);
        if transferStatus ~= 0
            fprintf('Image to host transfer failed\n');
        end
        WaitSecs(0.1);
        Eyelink('command', 'draw_box %d %d %d %d 15', width/2-50, height/2-50, width/2+50, height/2+50);
        
        % STEP 7.2
        % Do a drift correction at the beginning of each trial
        % Performing drift correction (checking) is optional for
        % EyeLink 1000 eye trackers.
        EyelinkDoDriftCorrection(el);
        
        % STEP 7.3
        % start recording eye position (preceded by a short pause so that
        % the tracker can finish the mode transition)
        % The paramerters for the 'StartRecording' call controls the
        % file_samples, file_events, link_samples, link_events availability
        Eyelink('Command', 'set_idle_mode');
        WaitSecs(0.05);
        Eyelink('StartRecording');
        % record a few samples before we actually start displaying
        % otherwise you may lose a few msec of data
        WaitSecs(0.1);
        
        % STEP 7.4
        % Prepare and show the screen.
        Screen('FillRect', window, el.backgroundcolour);
        imdata=imread(imgfile);
        imageTexture=Screen('MakeTexture',window, imdata);
        Screen('DrawTexture', window, imageTexture);
        Screen('DrawText', window, 'Press the SPACEBAR to end the recording of the trial.', width/5, height/2, 0);
        Screen('Flip', window);
        % mark zero-plot time in data file
        Eyelink('Message', 'SYNCTIME');
        
        % Send an integration message so that an image can be loaded as
        % overlay backgound when performing Data Viewer analysis.  This
        % message can be placed anywhere within the scope of a trial (i.e.,
        % after the 'TRIALID' message and before 'TRIAL_RESULT')
        % See "Protocol for EyeLink Data to Viewer Integration -> Image
        % Commands" section of the EyeLink Data Viewer User Manual.
        Eyelink('Message', '!V IMGLOAD CENTER %s %d %d', imgfile, width/2, height/2);
        
        stopkey=KbName('space');
        
        % STEP 7.5
        % Monitor the trial events;
        while 1 % loop till error or space bar is pressed
            % Check recording status, stop display if error
            error=Eyelink('CheckRecording');
            if(error~=0)
                break;
            end
            % check for keyboard press
            [keyIsDown,secs,keyCode] = KbCheck; %#ok<*ASGLU>
            % if spacebar was pressed stop display
            if keyCode(stopkey)
                Eyelink('Message', 'Key pressed')
                break;
            end
        end % main loop
        
        
        % STEP 7.6
        % Clear the display
        Screen('FillRect', window, el.backgroundcolour);
        Screen('Flip', window);
        Eyelink('Message', 'BLANK_SCREEN');
        % adds 100 msec of data to catch final events
        WaitSecs(0.1);
        % stop the recording of eye-movements for the current trial
        Eyelink('StopRecording');
        
        
        % STEP 7.7
        % Send out necessary integration messages for data analysis
        % Send out interest area information for the trial
        % See "Protocol for EyeLink Data to Viewer Integration-> Interest
        % Area Commands" section of the EyeLink Data Viewer User Manual
        % IMPORTANT! Don't send too many messages in a very short period of
        % time or the EyeLink tracker may not be able to write them all
        % to the EDF file.
        % Consider adding a short delay every few messages.
        WaitSecs(0.001);
        Eyelink('Message', '!V IAREA ELLIPSE %d %d %d %d %d %s', 1, floor(width/2-50), floor(height/2-50), floor(width/2+50), floor(height/2+50),'center');
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 2, floor(width/4-50), floor(height/2-50), floor(width/4+50), floor(height/2+50),'left');
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 3, floor(3*width/4-50), floor(height/2-50), floor(3*width/4+50), floor(height/2+50),'right');
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 4, floor(width/2-50), floor(height/4-50), floor(width/2+50), floor(height/4+50),'up');
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 5, floor(width/2-50), floor(3*height/4-50), floor(width/2+50), floor(3*height/4+50),'down');
        

        % Send messages to report trial condition information
        % Each message may be a pair of trial condition variable and its
        % corresponding value follwing the '!V TRIAL_VAR' token message
        % See "Protocol for EyeLink Data to Viewer Integration-> Trial
        % Message Commands" section of the EyeLink Data Viewer User Manual
        WaitSecs(0.001);
        Eyelink('Message', '!V TRIAL_VAR index %d', i)
        Eyelink('Message', '!V TRIAL_VAR imgfile %s', imgfile)
        
        % STEP 7.8
        % Sending a 'TRIAL_RESULT' message to mark the end of a trial in
        % Data Viewer. This is different than the end of recording message
        % END that is logged when the trial recording ends. The viewer will
        % not parse any messages, events, or samples that exist in the data
        % file after this message.
        Eyelink('Message', 'TRIAL_RESULT 0')
    end
    
    
    %%%%%%%%%%
    % STEP 8 %
    %%%%%%%%%%
    
    % End of Experiment; close the file first
    % close graphics window, close data file and shut down tracker
    
    % reset so tracker uses defaults calibration for other experiemnts
    Eyelink('command', 'generate_default_targets = YES')
    Eyelink('Command', 'set_idle_mode');
    WaitSecs(0.5);
    Eyelink('CloseFile');
    
    % download data file
    try
        fprintf('Receiving data file ''%s''\n', edfFile );
        status=Eyelink('ReceiveFile');
        if status > 0
            fprintf('ReceiveFile status %d\n', status);
        end
        if 2==exist(edfFile, 'file')
            fprintf('Data file ''%s'' can be found in ''%s''\n', edfFile, pwd );
        end
    catch %#ok<*CTCH>
        fprintf('Problem receiving data file ''%s''\n', edfFile );
    end
    
    %%%%%%%%%%
    % STEP 9 %
    %%%%%%%%%%
    
    % run cleanup function (close the eye tracker and window).
    cleanup
    
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    cleanup;
    
end %try..catch.


% Cleanup routine:
    function cleanup
        % Shutdown Eyelink:        
        Eyelink('Shutdown');
        Screen('CloseAll');
        
    end
end
