function OldNewRecogExp(subNo,hand)
%
% sample old/new recognition experiment
% 05/24/05
%
% subNo    subject number; use subNo>99 to skip check for existing file
% hand     response mapping for test phase
%
% e.g.: OldNewRecogExp(99,1);
%
% DESIGN:
%
% STUDY PHASE: study 3 objects
% TEST  PHASE: shown 6 objects, decide whether the object is old or new
%
% demonstrates:
%    - reading from files to get condition on each trial
%    - randomizing conditions (for the study and test phase)
%    - showing image/collecting response (response time, accuracy)
%    - writing data to file "OldNewRecogExp_<subNo>.dat
%
% please refer to other included demos for new OSX functions
%
% Requires PTB version 1.0.6 or later.
%
% Other than a few changes to the Screen function,
% and the try ... catch loop, this code
% replicates a typical OS9 experiment
%
% History:
% 
% 05/24/05  Quoc Vuong, PhD     wrote it. 


% NOTE: OSX is case sensitive!!!


%%%%%%%%%%%%%%%%%%%%%%%%%
% any preliminary stuff
%%%%%%%%%%%%%%%%%%%%%%%%%

clc;
rand('state',sum(100*clock)); % reseed the random-number generator for each expt.

% init keyboard responses (caps doesn't matter)
advancestudytrial=KbName('n');

if (hand==1) % use hand to determine response mapping
    oldresp=KbName('c'); % old response
    newresp=KbName('m'); % new response
else
    oldresp=KbName('m');
    newresp=KbName('c');
end

%%%%%%%%%%%%%%%%%%%%%%
% file handling
%%%%%%%%%%%%%%%%%%%%%%

% files
datafilename = strcat('OldNewRecogExp_',num2str(subNo),'.dat'); % name of data file to write to
studyfilename = 'studylist.txt';                           % study list
testfilename  = 'testlist.txt';                            % test list

% check for existing file (except for subject numbers > 99)
if subNo<99 & fopen(datafilename, 'rt')~=-1
    error('data files exist!');
else
    datafilepointer = fopen(datafilename,'wt'); % open ASCII file for writing
end

%%%%%%%%%%%%%%%%%%%%%%
% experiment
%%%%%%%%%%%%%%%%%%%%%%

% all OSX functions need to be embedded in try ... catch loop
try
    % check for opengl compatability
    AssertOpenGL;
    
    % get screen
    screens=Screen('Screens');
    screenNumber=max(screens);
    
    HideCursor;
    
    % Open a double buffered fullscreen window and draw a gray background 
    % to front and back buffers:
    [w, wRect]=Screen('OpenWindow',screenNumber, 0,[],32,2);
    
    % returns as default the mean gray value of screen
    gray=GrayIndex(screenNumber); 

    Screen('FillRect',w, gray);
    Screen('Flip', w);
    
    % set Text properties (all Screen functions must be called after screen
    % init
    Screen('TextSize', w, 32);
    
    % set priority - also set after Screen init
    priorityLevel=MaxPriority(w);
    Priority(priorityLevel);
    
    % run through study and test phase
    for phase=1:2 % 1 is study phase, 2 is test phase                

        if phase==1 % study phase
            
            % define variables for current phase
            phaselabel='study'
            duration=2.000; % sec
            trialfilename=studyfilename;
            
            % write message to subject
            Screen('DrawText', w, 'study phase ...', 10, 10, 255);
            Screen('DrawText', w, 'study each picture ... press _n_ when it disappears ...', 10, 40, 255);
            Screen('DrawText', w, '... press mouse to begin ...', 10, 70, 255);
        else        % test phase
            
            % define variables
            phaselabel='test'
            duration=0.500;  %sec
            trialfilename=testfilename;
            
            % write message to subject
            str=sprintf('Press _%s_ for OLD and _%s_ for NEW',KbName(oldresp),KbName(newresp));
            Screen('DrawText', w, 'test phase ...', 10, 10, 255);
            Screen('DrawText', w, str, 10, 40, 255);
            Screen('DrawText', w, '... press mouse to begin ...', 10, 70, 255);
        end

        Screen('Flip', w); % show text
        
        % wait for mouse press ( no GetClicks  :(  )
        buttons=0;
        while ~any(buttons) % wait for press
            [x,y,buttons] = GetMouse;
            % Wait 10 ms before checking the mouse again to prevent
            % overload of the machine at elevated Priority()
            WaitSecs(0.01);
        end
        
        % clear screen
        Screen('Flip', w);
        
        % wait a bit before starting trial
        WaitSecs(1.000);
        
        % read list -- textread() is a matlab function
        % objnumber  arbitrary number of stimulus
        % objname    stimulus filename
        % objtype    1=old stimulus, 2=new stimulus
        %            for study list, stimulus coded as "old"
        [ objnumber objname objtype ]=textread(trialfilename,'%d %s %d');
        
        % randomize order of list
        ntrials=length(objnumber);         % get number of trials
        randomorder=randperm(ntrials);     % randperm() is a matlab function
        objnumber=objnumber(randomorder);  % need to randomize each list!
        objname=objname(randomorder);      %
        objtype=objtype(randomorder);      %
        
        % loop through trials
        for trial=1:ntrials
                        
            % wait a bit between trials
            WaitSecs(0.500);
            
            % initialize key
            [KeyIsDown, endrt, KeyCode]=KbCheck;
            
            % read stimulus
            stimfilename=strcat('stims/',char(objname(trial))); % assume stims are in subfolder "stims"
            imdata=imread(char(stimfilename));
            
            % make texture
            tex=Screen('MakeTexture', w, imdata);
            tRect=Screen('Rect', tex);
            
            % draw texture to backbuffer
            Screen('DrawTexture', w, tex, [], CenterRect(tRect, wRect));
            
            % Show stimulus on screen & record onset time
            [VBLTimestamp startrt]=Screen('Flip', w);
            
            % while loop to show stimulus until subjects response or until
            % "duration" seconds elapsed.
            while (GetSecs-startrt)<=duration
                % poll for a resp
                % during test phase, subjects can response
                % before stimulus terminates
                if ( phase==2 ) % if test phase
                    if ( KeyCode(oldresp)==1 | KeyCode(newresp)==1 )
                        break;
                    end
                    [KeyIsDown, endrt, KeyCode]=KbCheck;
                end

                % Wait 1 ms before checking the keyboard again to prevent
                % overload of the machine at elevated Priority()
                WaitSecs(0.001);

            end

            % clear screen after fixed duration
            % or after subjects response (on test phase)
            Screen('Flip', w);
            
            % loop until valid key is pressed
            % if a response is made already, then this loop will be skipped
            if ( phase==1 ) % study phase
                while (KeyCode(advancestudytrial)==0)
                    [KeyIsDown, endrt, KeyCode]=KbCheck;
                    WaitSecs(0.001);
                end
            end
            
            if ( phase==2 ) % test phase
                while ( KeyCode(oldresp)==0 & KeyCode(newresp)==0 )
                    [KeyIsDown, endrt, KeyCode]=KbCheck;
                     WaitSecs(0.001);
                end
            end
            
            % compute response time
            rt=round(1000*(endrt-startrt));
            
            % compute accuracy
            if (phase==1 ) % study phase
                ac=1;
            else           % test phase
                ac=0;
                % code correct if old-response with old stimulus,
                % new-response with new stimulus, or study phase
                if ( (KeyCode(oldresp)==1 & objtype(trial)==1) | (KeyCode(newresp)==1 & objtype(trial)==2) )
                    ac=1;
                end
            end
            
            resp=KbName(KeyCode); % get key pressed by subject
            
            % print to file
            fprintf(datafilepointer,'%i %i %s %i %s %i %s %i %i %i\n', ...
                subNo, ...
                hand, ...
                phaselabel, ...
                trial, ...
                resp, ...
                objnumber(trial), ...
                char(objname(trial)), ...
                objtype(trial), ...
                ac, ...
                rt);
            
        end % for loop
        
    end % phase loop
    
    % cleanup at end of experiment
    Screen('CloseAll');
    ShowCursor;
    fclose('all');
    Priority(0);
    
catch
    % catch error
    Screen('CloseAll');
    ShowCursor;
    fclose('all');
    Priority(0);
    psychrethrow(psychlasterror);
end % try ... catch %
% only 277 lines of codes ...  :)

