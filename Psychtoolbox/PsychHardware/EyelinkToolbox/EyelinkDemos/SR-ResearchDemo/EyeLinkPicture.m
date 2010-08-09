function EyeLinkPicture

% Short MATLAB example that uses the Eyelink and Psychophysics Toolboxes
% This is the example as shown in the EyelinkToolbox article in BRMIC
% Cornelissen, Peters and Palmer 2002), but updated to also work on the
% PC version of the toolbox, and uses some new routines.
%
% Adapted after "Psychtoolbox\PsychHardware\EyelinkToolbox\EyelinkDemos\
% ShortDemos\EyelinkExample.m"
%
% HISTORY
%
% mm/dd/yy
% 07/01/08 js 	redone the structure of the experiment and added 
%		integration messages to the EyeLink Data Viewer software
% 07/14/08 js 	added code to set your own EDF file name before opening
%		the experiment graphics
% 07/13/10  fwc made to work with new toolbox with callback and updated to
%               enable eye image display, added "cleanup" function,
%               reenabled try-catch


clear all;
commandwindow;

% list of images used for the trial
imageList = {'town.jpg' 'town_blur.jpg' 'composite.jpg'};
dummymode=0;
try 
    % STEP 1
    % Added a dialog box to set your own EDF file name before opening 
    % experiment graphics. Make sure the entered EDF file name is 1 to 8 
    % characters in length and only numbers or letters are allowed.
    prompt = {'Enter tracker EDF file name (1 to 8 letters or numbers)'};
    dlg_title = 'Create EDF file';
    num_lines= 1;
    def     = {'DEMO'};
    answer  = inputdlg(prompt,dlg_title,num_lines,def);
    %edfFile= 'DEMO.EDF'
    edfFile = answer{1};
    fprintf('EDFFile: %s\n', edfFile );
    
    % STEP 2
    % Open a graphics window on the main screen
    % using the PsychToolbox's Screen function.
    screenNumber=max(Screen('Screens'));
    [window, wRect]=Screen('OpenWindow', screenNumber, 0,[],32,2);
    Screen(window,'BlendFunction',GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    % STEP 3
    % Provide Eyelink with details about the graphics environment
    % and perform some initializations. The information is returned
    % in a structure that also contains useful defaults
    % and control codes (e.g. tracker state bit and Eyelink key values).
    el=EyelinkInitDefaults(window);

    % STEP 4
    % Initialization of the connection with the Eyelink Gazetracker.
    % exit program if this fails.
    if ~EyelinkInit(dummymode)
        fprintf('Eyelink Init aborted.\n');
        cleanup;  % cleanup function
        return;
    end
    [v vs]=Eyelink('GetTrackerVersion');
    fprintf('Running experiment on a ''%s'' tracker.\n', vs );

    % open file to record data to
    i = Eyelink('Openfile', edfFile);
    if i~=0
        fprintf('Cannot create EDF file ''%s'' ', edffilename);
        cleanup;
%         Eyelink( 'Shutdown');
        return;
    end

    Eyelink('command', 'add_file_preamble_text ''Recorded by EyelinkToolbox demo-experiment''');
    [width, height]=Screen('WindowSize', screenNumber);


    % STEP 5    
    % SET UP TRACKER CONFIGURATION
    % Setting the proper recording resolution, proper calibration type, 
    % as well as the data file content;
    Eyelink('command','screen_pixel_coords = %ld %ld %ld %ld', 0, 0, width-1, height-1);
    Eyelink('message', 'DISPLAY_COORDS %ld %ld %ld %ld', 0, 0, width-1, height-1);                
    % set calibration type.
    Eyelink('command', 'calibration_type = HV9');
    % set parser (conservative saccade thresholds)
    Eyelink('command', 'saccade_velocity_threshold = 35');
    Eyelink('command', 'saccade_acceleration_threshold = 9500');
    % set EDF file contents
    Eyelink('command', 'file_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON');
    Eyelink('command', 'file_sample_data  = LEFT,RIGHT,GAZE,HREF,AREA,GAZERES,STATUS');
    % set link data (used for gaze cursor)
    Eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,SACCADE,BLINK,MESSAGE,BUTTON');
    Eyelink('command', 'link_sample_data  = LEFT,RIGHT,GAZE,GAZERES,AREA,STATUS');
    % allow to use the big button on the eyelink gamepad to accept the 
    % calibration/drift correction target
    Eyelink('command', 'button_function 5 "accept_target_fixation"');
   
    
    % make sure we're still connected.
    if Eyelink('IsConnected')~=1
        cleanup;
        return;
    end;
    
    
    % STEP 6
    % Calibrate the eye tracker  
    % setup the proper calibration foreground and background colors
    el.backgroundcolour = 128;
    el.foregroundcolour = 0;   
    % Hide the mouse cursor;
    Screen('HideCursorHelper', window);
    EyelinkDoTrackerSetup(el);


	
    % STEP 7
    % Now starts running individual trials; 
    % You can keep the rest of the code except for the implementation
    % of graphics and event monitoring 
    % Each trial should have a pair of "StartRecording" and "StopRecording" 
    % calls as well integration messages to the data file (message to mark 
    % the time of critical events and the image/interest area/condition 
    % information for the trial)
    
    for i=1:3
        % Now within the scope of each trial;
        imgfile= char(imageList(i));  
        
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
        % Must be offline to draw to EyeLink screen
        Eyelink('Command', 'set_idle_mode');
        % clear tracker display and draw box at center
        Eyelink('Command', 'clear_screen 0')
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
%         Eyelink('StartRecording', 1, 1, 1, 1);    
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
            [keyIsDown,secs,keyCode] = KbCheck;
            % if spacebar was pressed stop display
            if keyCode(stopkey)
                EyeLink('Message', 'Key pressed')
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
        Eyelink('Message', '!V IAREA ELLIPSE %d %d %d %d %d %s', 1, width/2-50, height/2-50, width/2+50, height/2+50,'center');
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 2, width/4-50, height/2-50, width/4+50, height/2+50,'left');
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 3, 3*width/4-50, height/2-50, 3*width/4+50, height/2+50,'right');
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 4, width/2-50, height/4-50, width/2+50, height/4+50,'up');
        Eyelink('Message', '!V IAREA RECTANGLE %d %d %d %d %d %s', 5, width/2-50, 3*height/4-50, width/2+50, 3*height/4+50,'down');
        
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
    
    % STEP 8
    % End of Experiment; close the file first   
    % close graphics window, close data file and shut down tracker
        
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
    catch
        fprintf('Problem receiving data file ''%s''\n', edfFile );
    end
    
    % STEP 9
    % run cleanup function (close the eye tracker and window).
    cleanup;
%     Eyelink('ShutDown');
%     Screen('CloseAll');

catch
     %this "catch" section executes in case of an error in the "try" section
     %above.  Importantly, it closes the onscreen window if its open.
     cleanup;
%      Eyelink('ShutDown');
%      Screen('CloseAll'); 
%      commandwindow;
%      rethrow(lasterr);
end %try..catch.


% Cleanup routine:
function cleanup
% Shutdown Eyelink:
Eyelink('Shutdown');

% Close window:
sca;
commandwindow;

% Restore keyboard output to Matlab:
% % ListenChar(0);

