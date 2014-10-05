function results = MullerLyerIllusion(subID)
% results = MullerLyerIllusion(subID)
%
% Presenting the Muller-Lyer Illusion with the Psychtoolbox, complete experiment.
%
% Input:
%
%   subID = subject Id (scalar), defaults to 66
%
% Output:
%
%   'results' is a numerical matrix with one row per trial and columns:
%   colHeaders = {'subID', 'trial no', 'trial ID', 'length l1', 'headdir l1',...
%   'length l2', 'headdir l2', 'length ratio', 'correct', 'rt PTB', 'rt ML'}
%
% In this experiment two lines (l1 and l2) are presented, where each line can have 
% inward or outward pointing arrow heads at its ends.  
% subjects are asked to judge the relative length of the basic lines
% and to respond by pressing 's' for same or 'd' for different
%
% correctness and reaction times are recorded, the latter by two
% different methods in order to compare precision:
%   rt PTB relies on PTB commands which are supposedly more precise
%   rt ML uses the Matlab's native tic/toc stopwatch
%
% This experiment is a counterbalanced 2x2x2x2 design with
% the factors line length (l1, l2) and head direction (l1, l2)
%   The 16 test trials are presented in randomized order, following 3
% randomly drawn training trials
%   The exact design is defined in condtable (see below), where each line
% stands for one trial and the columns for the four factors
%
% line length can be either 1 or 2, these values will be taken as
% indices into the 1 x 2 vector hll (half line length, difference of 10%)
% using indices allows the stimuli to be scaled by a random factor in each trial
%
% head direction can be -1 (outward pointing), 0 (no head) or 1
% (inward pointing); 0 is not used in the present design
%
%   
% Written by N. Ruh, 15/02/2008.
%
% Exemplary reference:
%
%   Restle F & Decker J (1977) Size of the Mueller-Lyer illusion as a
%   function of its dimensions: Theory and data. Perception & Psychophysics 21:489 503
%
% History:
% 3/3/2008  Included in Psychtoolbox (MK).



% Make sure the script is running on Psychtoolbox-3:
AssertOpenGL;

%set default values for input arguments
if ~exist('subID','var')
    subID=66;
end

%warn if duplicate sub ID
fileName=['MLIexpSubj' num2str(subID) '.txt'];
if exist(fileName,'file')
    if ~IsOctave
        resp=questdlg({['the file ' fileName 'already exists']; 'do you want to overwrite it?'},...
            'duplicate warning','cancel','ok','ok');
    else
        resp=input(['the file ' fileName ' already exists. do you want to overwrite it? [Type ok for overwrite]'], 's');
    end
    
    if ~strcmp(resp,'ok') %abort experiment if overwriting was not confirmed
        disp('experiment aborted')
        return
    end
end

%prepare conditions;
%the first two elements in each row code for linelength,
%the last two for head orientation
condtable=[ 1     1     1     1;
            1     2     1     1;
            2     1     1     1;
            2     2     1     1;
            1     1    -1     1;
            1     2    -1     1;
            2     1    -1     1;
            2     2    -1     1;
            1     1     1    -1;
            1     2     1    -1;
            2     1     1    -1;
            2     2     1    -1;
            1     1    -1    -1;
            1     2    -1    -1;
            2     1    -1    -1;
            2     2    -1    -1];

ntrials=size(condtable,1);

%re-order the 16 trials randomly
shuffeldTrialIDs=randperm(ntrials);

%number of training trials
ntrain=3;

%attach a randomly drawn subsample as training trials
order=[ceil(rand(1,ntrain)*ntrials), shuffeldTrialIDs];

%Prepare output
colHeaders = {'subID', 'trial no', 'trial ID', 'length l1', 'headdir l1',...
    'length l2', 'headdir l2', 'length ratio', 'correct', 'rt PTB', 'rt ML'};
results=NaN * ones(length(order),length(colHeaders)); %preallocate results matrix

%head=[-1 0 1]; %we are currently using only +/- 1, see condtable
lw=3; %line width of stimuli

%when working with the PTB it is a good idea to enclose the whole body of your program
%in a try ... catch ... end construct. This will often prevent you from getting stuck
%in the PTB full screen mode
try
    % Enable unified mode of KbName, so KbName accepts identical key names on
    % all operating systems (not absolutely necessary, but good practice):
    KbName('UnifyKeyNames');

    %funnily enough, the very first call to KbCheck takes itself some
    %time - after this it is in the cache and very fast
    %to make absolutely sure, we thus call it here once for no other
    %reason than to get it cached. This btw. is true for all major
    %functions in Matlab, so calling each of them once before entering the
    %trial loop will make sure that the 1st trial goes smooth wrt. timing.
    KbCheck;

    %disable output of keypresses to Matlab. !!!use with care!!!!!!
    %if the program gets stuck you might end up with a dead keyboard
    %if this happens, press CTRL-C to reenable keyboard handling -- it is
    %the only key still recognized.
    ListenChar(2);

    %Set higher DebugLevel, so that you don't get all kinds of messages flashed
    %at you each time you start the experiment:
    olddebuglevel=Screen('Preference', 'VisualDebuglevel', 3);

    %Choosing the display with the highest display number is
    %a best guess about where you want the stimulus displayed.
    %usually there will be only one screen with id = 0, unless you use a
    %multi-display setup:
    screens=Screen('Screens');
    screenNumber=max(screens);

    %open an (the only) onscreen Window, if you give only two input arguments
    %this will make the full screen white (=default)
    [expWin,rect]=Screen('OpenWindow',screenNumber);

    %alternative: replace the above with smaller window for testing
    %   [expWin,rect]=Screen('OpenWindow',screenNumber,[],[10 20 1200 700]);
    %NOTE that smaller windows can induce synchronisation problems
    %and other issues, so they're not suitable for running real experiment
    %sessions. See >> help SyncTrouble

    %get the midpoint (mx, my) of this window, x and y
    [mx, my] = RectCenter(rect);

    %get rid of the mouse cursor, we don't have anything to click at anyway
    HideCursor;

    %         %Syntax for querying user input, e.g. subject initials:
    %         reply=Ask(expWin,'Enter subject initials: ',[],[],'GetChar',RectLeft,RectTop,20);

    %Preparing and displaying the welcome screen
    % We choose a text size of 24 pixels - Well readable on most screens:
    Screen('TextSize', expWin, 24);
    
    % This is our intro text. The '\n' sequence creates a line-feed:
    myText = ['In this experiment you are asked to judge\n' ...
              'the relative length of two horizontal lines\n' ...
              '  Press  s  if the lines have the same length \n' ...
              '  Press  d  if the lines have different length\n' ...
              'You will begin with ' num2str(ntrain) ' training trials\n' ...
              '      (Press any key to start training)\n' ];
    
    % Draw 'myText', centered in the display window:
    DrawFormattedText(expWin, myText, 'center', 'center');

    % This chunk of code would do roughly the same: It uses the lower-level
    % 'DrawText' subcommand to draw text. DrawFormattedText is a
    % convenience wrapper that provides basic text formatting functions.
    % See 'help DrawFormattedTextDemo' for a demo of its capabilities...
    %
    %     lm=150; %left margin, adjust to suit the size of your screen
    %     Screen('DrawText', expWin, 'In this experiment you are asked to judge', lm, 50);
    %     Screen('DrawText', expWin, 'the relative length of two horizontal lines', lm, 80);
    %     Screen('DrawText', expWin, '  press  s  if the lines have the same length', lm, 110);
    %     Screen('DrawText', expWin, '  press  d  if the lines have different lengths', lm, 140);
    %     Screen('DrawText', expWin, ['you will begin with ' num2str(ntrain) ' training trials'], lm, 170);
    %     Screen('DrawText', expWin, '      (press any key to start training)', lm, 250);

    % Show the drawn text at next display refresh cycle:
    Screen('Flip', expWin);
    
     % Wait for key stroke. This will first make sure all keys are
     % released, then wait for a keypress and release:
    KbWait([], 3);

    %         %an example of preparing an offScreenwindow (for repeated use and fast drawing)
    %         [fixcross,rect2]=Screen('OpenOffscreenWindow',screenNumber,[],[0 0 20 20]);
    %         Screen('drawline',fixcross,[0 0 0],10,0,10,20,2);%mx-10,my,mx+10,my
    %         Screen('drawline',fixcross,[0 0 0],0,10,20,10,2);%mx,my-10,mx,my+10

    % Another way to create a fixation cross: Doing the above with textures,
    % by preparing a little Matlab matrix with the image of a fixation
    % cross:  --> Choose whatever you like more.
    FixCr=ones(20,20)*255;
    FixCr(10:11,:)=0;
    FixCr(:,10:11)=0;  %try imagesc(FixCr) to display the result in Matlab
    fixcross = Screen('MakeTexture',expWin,FixCr);

    %start trials loop (over training and test trials)
    for i=1:length(order)

        %prepare and display end of training/start experiment screen
        if i==ntrain+1  %before the first test trial
            DrawFormattedText(expWin, 'Are you ready for the experiment?\n(Press any key to start experiment)', 'center', 'center');
            Screen('Flip', expWin);
            KbWait([], 3);
        end

        % Copy the content of the previously prepared texture or offscreenWindow
        % into the backbuffer of the onscreen window, then flip it to the front
        % NOTE that offscreen windows and textures are almost the same
        % thing. Btw. although we specify the target region, this is not
        % strictly neccessary. A Screen('DrawTexture', expWin, fixcross);
        % would yield exactly the same result, as all textures are centered
        % in the target window by default.
        Screen('DrawTexture', expWin, fixcross,[],[mx-10,my-10,mx+10,my+10]);

        % We show the fixation cross at next display refresh cycle and
        % store the onset time of the fixation cross in 'tfixation'. Later
        % on we will use that as baseline to make sure the actual Mueller
        % Lyer test stim is shown 0.5 secs after onset of fixation:
        tfixation = Screen('Flip', expWin);

        %Prepare stimulus characteristics, make all aspects of the stimuli
        %proportional to stimsize so it can be dynamically changed
        stimsize=rand*100+50;   %between 50 and 150 pixels
        hll=[stimsize, stimsize*0.9]; %halflinelength, difference of 10%
        hos=stimsize*.8; %headoffset
        voff=stimsize*1.5; %vertical distance between lines

        %look up the index in the correct line of current trial and find the
        %corresponding item in hll (half line length)
        %the identity of the current line is governed by the scrambled indices in order
        l_hll = hll(condtable(order(i),(1)));
        
        %look up the direction of the header in the current line of condtable
        l_head = condtable(order(i),(3));
        
        %find horizontal offset according to this line's hearer direction
        l_hos=hos*l_head;

        %same again for the second stimulus/line
        u_hll = hll(condtable(order(i),(2)));
        u_head = condtable(order(i),(4));
        u_hos=hos*u_head;

        %draw stimuli into backbuffer
        Screen('DrawLine', expWin , 0, mx-l_hll, my-voff, mx+l_hll, my-voff, lw);
        if l_hos~=0
            Screen('DrawLine', expWin , 0, mx-l_hll, my-voff, mx-l_hll+l_hos, my-voff+hos/2, lw);
            Screen('DrawLine', expWin , 0, mx-l_hll, my-voff, mx-l_hll+l_hos, my-voff-hos/2, lw);
            Screen('DrawLine', expWin , 0, mx+l_hll, my-voff, mx+l_hll-l_hos, my-voff+hos/2, lw);
            Screen('DrawLine', expWin , 0, mx+l_hll, my-voff, mx+l_hll-l_hos, my-voff-hos/2, lw);
        end

        Screen('DrawLine', expWin , 0, mx-u_hll, my+voff, mx+u_hll, my+voff, lw);
        if u_hos~=0
            Screen('DrawLine', expWin , 0, mx-u_hll, my+voff, mx-u_hll+u_hos, my+voff+hos/2, lw);
            Screen('DrawLine', expWin , 0, mx-u_hll, my+voff, mx-u_hll+u_hos, my+voff-hos/2, lw);
            Screen('DrawLine', expWin , 0, mx+u_hll, my+voff, mx+u_hll-u_hos, my+voff+hos/2, lw);
            Screen('DrawLine', expWin , 0, mx+u_hll, my+voff, mx+u_hll-u_hos, my+voff-hos/2, lw);
        end

        %this would tell PTB that no further drawing commands will occur
        %before the next screen 'flip'. Apparently this can improve performance
        %telapsed is the time since the last flip command; use this if
        %you want to test how long it takes to draw into the backbuffer
        %   telapsed = Screen('DrawingFinished', expWin, [], 1);
        %However, its not needed here...

        %display stimuli and get onset time (two alternative ways). We ask
        %to show the stim 0.5 seconds after onset 'tfixation' of the
        %fixation cross:
        [VBLTimestamp, StimulusOnsetTime, FlipTimestamp]=Screen('Flip', expWin, tfixation + 0.5);
        tic;
        %these different timestamps are not exactly the same, e.g.:
        %   plot([VBLTimestamp StimulusOnsetTime FlipTimestamp tic])
        %the difference is negligible for most experiments

        %record response time, two methods again
        %this is just to compare between Matlab and PTB timing.
        %In your experiment, you should settle for one method --> 
        %the Psychtoolbox method of using 'StimulusOnsetTime' seems to be 
        %the more reliable solution, specifically on varying hardware
        %setups or under suboptimal conditions 
        [resptime, keyCode] = KbWait;
        MLrt=toc;
        rt=resptime-StimulusOnsetTime;

        %find out which key was pressed
        cc=KbName(keyCode);  %translate code into letter (string)

        %calculate performance or detect forced exit
        if isempty(cc) || strcmp(cc,'ESCAPE')
            break;   %break out of trials loop, but perform all the cleanup things
                     %and give back results collected so far
        elseif ~any(strcmp(cc,'s') || strcmp(cc,'d'))
            anscorrect = 66;
        elseif u_hll==l_hll && strcmp(cc,'s')
            anscorrect = 1;
        elseif u_hll~=l_hll && strcmp(cc,'d')
            anscorrect = 1;
        else
            anscorrect = 0;
        end

        %enter results in matrix
        results(i,:) = [subID, i-ntrain, order(i), l_hll*2, l_head, u_hll*2, u_head, u_hll/l_hll, anscorrect, rt, MLrt];

        %beep if the response was incorrect
        if anscorrect ~=1
            beep;
        end

        %show between trial prompt and wait for button press
        DrawFormattedText(expWin, 'Press any key to start next trial', 'center', 'center');
        Screen('Flip', expWin);
        KbWait([], 3); %wait for keystroke

    end %of trials loop

    %write results to comma delimited text file (use '\t' for tabs)
    dlmwrite(fileName, results, 'delimiter', ',', 'precision', 6);

    %         %alternative: write to excel format
    %         xlswrite(['MLIexpSubj' num2str(subID) '.xls'],[colHeaders; num2cell(results)]);

    %calculate and display performance feedback
    performance_errors=mean(results(results(:,9)~=66,9));
    DrawFormattedText(expWin, ['You were correct in ' num2str(performance_errors*100,2) '% of trials.\nThank you for participating!'], 'center', 'center');
    Screen('Flip', expWin);
    KbWait([], 2); %wait for keystroke

    %clean up before exit
    ShowCursor;
    sca; %or Screen('CloseAll');
    ListenChar(0);
    %return to olddebuglevel
    Screen('Preference', 'VisualDebuglevel', olddebuglevel);

catch
    % This section is executed only in case an error happens in the
    % experiment code implemented between try and catch...
    ShowCursor;
    Screen('CloseAll'); %or sca
    ListenChar(0);
    Screen('Preference', 'VisualDebuglevel', olddebuglevel);
    %output the error message
    psychrethrow(psychlasterror);
end
