function change
%
% ___________________________________________________________________
%
% Demo implemetation of detecting eye in fixation window on display
% computer through fixation update events from host pc
%
% stimulus: A changing checker board image
%
% This task demonstrates stimuli presentation with Eyelink and Data Viewer
% integration.
% 
% NOTE: Please make sure you are in the same directory where the change
% images are located. 
%
% Press space to exit change
% ___________________________________________________________________

% HISTORY
% mm/dd/yy
%
% 01/28/11  NJ  created
% 12/20/13  LJ  changed isoctave to IsOctave and EyeLink to Eyelink, case sensitive for the latest matlab
%                added parameters for function infixationWindow, in order to run with octave
%                fixed issue with non integer arguments for Eyelink('message' ...)
%

if ~IsOctave
    commandwindow;
else
    more off;
end

dummymode = 0;

% set trial times
TIMER_DISPLAY = 150;
TIMER_BLANK = 50;
TRIAL_TIMEOUT = 60000;

% images to use and locations of interest areas
% Only 24 and 32 bit images are supported. Octave cares about case
imageListA = {'REG1A.bmp' 'REG2A.bmp' 'RND1A.bmp' 'RND2A.bmp'};
imageListB = {'REG1B.bmp' 'REG2B.bmp' 'RND1B.bmp' 'RND2B.bmp'};
trigLocs = [ [149 ; 365] [279;176] [404;365] [404;176]];

stopkey=KbName('space');

try
    
    %%%%%%%%%
    % STEP 1%
    %%%%%%%%%
    
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
        edfFile = answer{1};
        fprintf('EDFFile: %s\n', edfFile );
     end
    %%%%%%%%%%
    % STEP 2 %
    %%%%%%%%%%
    
    % Open a graphics window on the main screen
    % using the PsychToolbox's Screen function.
    screenNumber=max(Screen('Screens'));
    window=Screen('OpenWindow', screenNumber, 0,[],32,2); 
    Screen(window,'BlendFunction',GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    [winWidth, winHeight] = WindowSize(window);
    
    %%%%%%%%%%
    % STEP 3 %
    %%%%%%%%%%
    
    % Provide Eyelink with details about the graphics environment
    % and perform some initializations. The information is returned
    % in a structure that also contains useful defaults
    % and control codes (e.g. tracker state bit and Eyelink key values).
    el=EyelinkInitDefaults(window);
    
    % We are changing calibration to a black background with white targets,
    % no sound and smaller targets
    el.backgroundcolour = WhiteIndex(el.window);
    el.msgfontcolour  = BlackIndex(el.window);
    el.targetbeep = 0;
    el.calibrationtargetcolour= BlackIndex(el.window);
    % for lower resolutions you might have to play around with these values
    % a little. If you would like to draw larger targets on lower res
    % settings please edit PsychEyelinkDispatchCallback.m and see comments
    % in the EyelinkDrawCalibrationTarget function
    el.calibrationtargetsize= 1;
    el.calibrationtargetwidth=0.5;
    % call this function for changes to the calibration structure to take
    % affect
    EyelinkUpdateDefaults(el);
    
    %%%%%%%%%%
    % STEP 4 %
    %%%%%%%%%%
    
    % Initialization of the connection with the Eyelink tracker
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
        return;
    end
    
    % make sure we're still connected.
    if Eyelink('IsConnected')~=1 && ~dummymode
        cleanup;
        return;
    end
    
    %%%%%%%%%%
    % STEP 5 %
    %%%%%%%%%%
    
    % SET UP TRACKER CONFIGURATION
    % Setting the proper recording resolution, proper calibration type,
    % as well as the data file content;
    Eyelink('command', 'add_file_preamble_text ''Recorded by EyelinkToolbox demo-experiment''');
    
    % This command is crucial to map the gaze positions from the tracker to
    % screen pixel positions to determine fixation
    Eyelink('command','screen_pixel_coords = %ld %ld %ld %ld', 0, 0, winWidth-1, winHeight-1);
    
    Eyelink('message', 'DISPLAY_COORDS %ld %ld %ld %ld', 0, 0, winWidth-1, winHeight-1);
    % set calibration type.
    Eyelink('command', 'calibration_type = HV9');
    Eyelink('command', 'generate_default_targets = YES');
    % set parser (conservative saccade thresholds)
    Eyelink('command', 'saccade_velocity_threshold = 35');
    Eyelink('command', 'saccade_acceleration_threshold = 9500');
    
    
    % 5.1 retrieve tracker version and tracker software version
    [v,vs] = Eyelink('GetTrackerVersion');
    fprintf('Running experiment on a ''%s'' tracker.\n', vs );
    vsn = regexp(vs,'\d','match');
    
    % set EDF file contents. Note the FIXUPDATE event for fixation update
    if v ==3 && str2double(vsn{1}) == 4 % if EL 1000 and tracker version 4.xx
        % remote mode possible add HTARGET ( head target)
        Eyelink('command', 'file_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,INPUT');
        Eyelink('command', 'file_sample_data  = LEFT,RIGHT,GAZE,HREF,AREA,GAZERES,STATUS,INPUT,HTARGET');
        % set link data (used for gaze cursor)
        Eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,INPUT,FIXUPDATE');
        Eyelink('command', 'link_sample_data  = LEFT,RIGHT,GAZE,GAZERES,AREA,STATUS,INPUT,HTARGET');
    else
        Eyelink('command', 'file_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,INPUT,FIXUPDATE');
        Eyelink('command', 'file_sample_data  = LEFT,RIGHT,GAZE,HREF,AREA,GAZERES,STATUS,INPUT');
        % set link data (used for gaze cursor)
        Eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON,INPUT,FIXUPDATE ');
        Eyelink('command', 'link_sample_data  = LEFT,RIGHT,GAZE,GAZERES,AREA,STATUS,INPUT');
    end
    
    % allow to use the big button on the eyelink gamepad to accept the
    % calibration/drift correction target
    Eyelink('command', 'button_function 5 "accept_target_fixation"');
    
    % Tell the Eyelink to send a fixation update every 50 ms
    Eyelink('command', 'fixation_update_interval = %d', 50);
    Eyelink('command', 'fixation_update_accumulate = %d', 50);
    
    %%%%%%%%%%
    % STEP 6 %
    %%%%%%%%%%
    
    if ~dummymode
        % Hide the mouse cursor and Calibrate the eye tracker
        Screen('HideCursorHelper', window);
    end
    
    % enter Eyetracker camera setup mode, calibration and validation
    EyelinkDoTrackerSetup(el);
    
    %%%%%%%%%%
    % STEP 7 %
    %%%%%%%%%%
    
    % Now starts running individual trials
    % You can keep the rest of the code except for the implementation
    % of graphics and event monitoring
    % Each trial should have a pair of "StartRecording" and "StopRecording"
    % calls as well integration messages to the data file (message to mark
    % the time of critical events and the image/interest area/condition
    % information for the trial)
    
    for i = 1 : 4
        stopKeyPressed = 0;
        % select the two images to be displayed in the trial
        imgfileA = char(imageListA(i));
        imgfileB = char(imageListB(i));
        
        % STEP 7.1
        % Sending a 'TRIALID' message to mark the start of a trial in Data
        % Viewer.  This is different than the start of recording message
        % START that is logged when the trial recording begins. The viewer
        % will not parse any messages, events, or samples, that exist in
        % the data file prior to this message.
        Eyelink('Message', 'TRIALID %d', i);
        % This supplies the title at the bottom of the eyetracker display
        Eyelink('command', 'record_status_message "* TRIAL %d/%d %s"', i,4, imgfileA);
        % Before recording, we place reference graphics on the host display
        % Must be in offline mode to transfer image to Host PC
        Eyelink('Command', 'set_idle_mode');
        % clear tracker display and draw box at center
        Eyelink('Command', 'clear_screen %d', 0);
        
        % pause between trials
        WaitSecs(0.1);
        
        % transfer image A to host pc
        imgArray = imread(imgfileA);
        finfo = imfinfo(imgfileA);
        transferStatus = Eyelink('ImageTransfer', finfo.Filename ,0,0,0,0,round(winWidth/2 - finfo.Width/2) ,round(winHeight/2 - finfo.Height/2),4);
        if transferStatus ~= 0
            fprintf('Image to host transfer failed\n');
        end
        % give image transfer time to finish
        WaitSecs(0.1);
        
        % find interest area locations for fixation window
        trigLocs(1,i) = trigLocs(1,i) + (winWidth/2 - finfo.Width/2);
        trigLocs(2,i) = trigLocs(2,i) + (winHeight/2 - finfo.Height/2);
        fixationWindow = [trigLocs(1,i) trigLocs(2,i) trigLocs(1,i)+64 trigLocs(2,i)+64];
        % draw filled box after image transfer so it is drawn on top of
        % image on Host PC
        Eyelink('command', 'draw_filled_box %d %d %d %d 2', fixationWindow(1), fixationWindow(2), fixationWindow(3), fixationWindow(4));
        
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
        % we are disabling samples transfered to the display but we're still
        % getting events. This speeds things up a little since in this
        % example we do not care about samples but about events from the
        % tracker
        Eyelink('StartRecording',1,1,0,1);
        
        % record a few samples before we actually start displaying
        % otherwise you may lose a few msec of data
        WaitSecs(0.1);
        
        % get eye that's tracked
        eye_used = Eyelink('EyeAvailable');
        % returns 0 (LEFT_EYE), 1 (RIGHT_EYE) or 2 (BINOCULAR) depending on what data is
        if eye_used == 2
            eye_used = 1; % use the right_eye data
        end
        
                
        % STEP 7.4
        % Prepare and show the screen.
        Screen('FillRect', window, el.backgroundcolour);
        Screen('Flip', window);
        % create trial textures
        tex(1) = Screen('MakeTexture', window,imgArray);
        imgArray = imread(imgfileB);
        tex(2) = Screen('MakeTexture', window,imgArray);
        
        % variables to determine what to display in each iteration
        counter = 0;
        counter2 = 0;
        correct = 0;
        trialtime = GetSecs + TRIAL_TIMEOUT/1000;
        totalFixTime = 0;
        mouseTimer = GetSecs;
        
        
        % loop until either trial time over or subject fixates changing
        % square
        while GetSecs < trialtime
            
            if correct
                break;
            end
            
            display = mod(counter,2) + 1;
            if display < 2
                tex2show = mod(counter2, 2) + 1;
                counter2 = counter2 + 1;
                displayTimer = GetSecs + TIMER_DISPLAY/1000;
            else
                displayTimer = GetSecs + TIMER_BLANK/1000;
            end
            
            
            while GetSecs < displayTimer
                
                [keyIsDown,secs,keyCode] = KbCheck; %#ok<*ASGLU>
                % if spacebar was pressed stop display
                if keyCode(stopkey )
                    sprintf('Space pressed, exiting trial\n');
                    Eyelink('Message', 'Stop Key pressed');
                    stopKeyPressed = 1;
                    break;
                end
                
                
                if display < 2
                    % display
                    Screen('FillRect', window, el.backgroundcolour);
                    Screen('DrawTexture', window, tex(tex2show));
                    Screen('Flip',window);
                    % send a msg to host with display 1 or 2 ( A or B)
                    Eyelink('Message', 'DISPLAY %d', tex2show);
                    
                else
                    % blank screen
                    Screen('FillRect', window, el.backgroundcolour);
                    Screen('Flip',window);
                    Eyelink('Message', 'BLANK_SCREEN');
                end
                
                if dummymode==0
                    error=Eyelink('CheckRecording');
                    if(error~=0)
                        disp('Error in Recording');
                        break;
                    end
                    % we need to loop over this a few times ( 30 is
                    % randomly chosen) so that we do not miss any events
                    % and to prevent any buffer overflow
                    for j=1:30
                        evtype = Eyelink('GetNextDataType');
                        if evtype == el.FIXUPDATE
                            if Eyelink('isconnected') == el.connected % if we're really measuring eye-movements
                                evt = Eyelink('getfloatdata', evtype);% get data
                                
                                % only process if its the desired eye
                                if evt.eye == eye_used
                                    % send msg with details of fixation
                                    % update event
                                    Eyelink('message', 'Fixupdate: avg_x %d, y %d, dur %d',floor(evt.gavx), floor(evt.gavy), floor(evt.entime)-floor(evt.sttime));

                                    % determine if gaze values are within
                                    % interest region and if gaze has been
                                    % maintained over 300 ms. This method
                                    % allows for saccades as long as they
                                    % are withing interest area
                                    if infixationWindow(fixationWindow, evt.gavx,evt.gavy)

                                        totalFixTime = totalFixTime + 50;
                                        if totalFixTime >= 300
                                            break;
                                        end
                                    else % broke fixation reset time
                                        totalFixTime = 0;
                                    end
                                end
                            else
                                disp('Eyelink disconnected!');
                            end
                        end
                    end %end for
                else
                    % using display PC mouse
                    [x,y] = GetMouse(window); %#ok<*NASGU>
                    evt.type=el.FIXUPDATE;
                    evt.gavx=x;
                    evt.gavy=y;                    
                    
                    if infixationWindow(fixationWindow, evt.gavx,evt.gavy)

                        if GetSecs - mouseTimer >= 0.300
                            disp('in fixation window');
                            correct = 1;
                            break;
                        end
                    else % reset
                        mouseTimer = GetSecs;
                    end
                end
                
                % has the subject completed 300 ms of fixation within iA?
                if totalFixTime >= 300
                    Beeper(el.calibration_success_beep(1), el.calibration_success_beep(2), el.calibration_success_beep(3));
                    correct = 1;
                    break;
                end
            end
            
            
            
            if stopKeyPressed
                break;
            end
            
            counter = counter + 1;
            
            [keyIsDown,secs,keyCode] = KbCheck;
            % if spacebar was pressed stop display
            if keyCode(stopkey )
                sprintf('Space pressed, exiting trial\n');
                Eyelink('Message', 'Key pressed');
                break;
            end
        end
        
        
        % STEP 7.6
        % add 100 msec of data to catch final events and blank display
        WaitSecs(0.1);
        Eyelink('StopRecording');
        Screen('FillRect', window, el.backgroundcolour);
        Screen('Flip', window);
        
        % did the trial time out without user fixating? send a message to
        % edf
        if GetSecs > trialtime
            disp('Trial time out');
            Eyelink('Message', 'Trial time out');
        end
        
        % STEP 7.7
        % Send out necessary integration messages for data analysis
        % See "Protocol for EyeLink Data to Viewer Integration-> Interest
        % Area Commands" section of the EyeLink Data Viewer User Manual
        % IMPORTANT! Don't send too many messages in a very short period of
        % time or the EyeLink tracker may not be able to write them all
        % to the EDF file.
        % Consider adding a short delay every few messages.
        WaitSecs(0.001);
        % Send an integration message so that an image can be loaded as
        % overlay backgound when performing Data Viewer analysis.  This
        % message can be placed anywhere within the scope of a trial (i.e.,
        % after the 'TRIALID' message and before 'TRIAL_RESULT')
        % See "Protocol for EyeLink Data to Viewer Integration -> Image
        % Commands" section of the EyeLink Data Viewer User Manual.
        Eyelink('Message', '!V IMGLOAD CENTER %s %d %d', imgfileA, winWidth/2, winHeight/2);
        % Send out interest area information for the trial
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 2, fixationWindow(1), fixationWindow(2), fixationWindow(3), fixationWindow(4),'change');
        % Send messages to report trial condition information
        % Each message may be a pair of trial condition variable and its
        % corresponding value follwing the '!V TRIAL_VAR' token message
        % See "Protocol for EyeLink Data to Viewer Integration-> Trial
        % Message Commands" section of the EyeLink Data Viewer User Manual
        WaitSecs(0.001);
        Eyelink('Message', '!V TRIAL_VAR index %d', i);
        Eyelink('Message', '!V TRIAL_VAR imgfile %s', imgfileA);
        % STEP 7.8
        % Sending a 'TRIAL_RESULT' message to mark the end of a trial in
        % Data Viewer. This is different than the end of recording message
        % END that is logged when the trial recording ends. The viewer will
        % not parse any messages, events, or samples that exist in the data
        % file after this message.
        Eyelink('Message', 'TRIAL_RESULT 0');
        
    end
    
    %%%%%%%%%%
    % STEP 8 %
    %%%%%%%%%%
    
    % End of Experiment; close the file first
    % close graphics window, close data file and shut down tracker
    Eyelink('Command', 'set_idle_mode');
    WaitSecs(0.5);
    Eyelink('CloseFile');
    
    try
        fprintf('Receiving data file ''%s''\n', edfFile );
        status=Eyelink('ReceiveFile');
        if status > 0
            fprintf('ReceiveFile status %d\n', status);
        end
        if 2==exist(edfFile, 'file')
            fprintf('Data file ''%s'' can be found in ''%s''\n', edfFile, pwd );
        end
    catch 
        fprintf('Problem receiving data file ''%s''\n', edfFile );
    end
    
    %%%%%%%%%%
    % STEP 9 %
    %%%%%%%%%%
    
    % run cleanup function (close the eye tracker and window).
    cleanup;
    
catch %#ok<*CTCH>
    cleanup;
    fprintf('%s: some error occured\n', mfilename);
    psychrethrow(lasterror); %#ok<LERR>
    
end

    function cleanup
        % Shutdown Eyelink:
        Eyelink('Shutdown');
        Screen('CloseAll');
    end

        function fix = infixationWindow(fixationWindow, mx,my)

        % determine if gx and gy are within fixation window
        fix = mx > fixationWindow(1) &&  mx <  fixationWindow(3) && ...
            my > fixationWindow(2) && my < fixationWindow(4) ;
    end


end
