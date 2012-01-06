function PsychRTBoxDemo
% A demonstration of basic usage and functionality of the RTBox reaction time box.
%
% Usage: PsychRTBoxDemo
%
% This demo shows the basics of using the PsychRTBox function to control
% and use the "USTC - RTBox" reaction time box, a response button box for
% response collection from subjects with high timing accuracy.
%
% You must have a compatible box attached to one of the USB ports of your
% computer and the FTDI Serial-Over-USB drivers must be properly installed
% and configured on your machine. Otherwise the demo will fail to start.
%
% Only the most basic operations of the box are demonstrated with the most
% basic parameter settings to get you started. Other demos will provide
% more detailed introduction into more advanced features.
%
% First the demo tests & shows basic live button queries.
% Then a fake reaction time experiment is run for four trials. You have to
% press a button on the box in response to a beep sound as fast as possible
% and your reaction time is measured. A post-hoc method for retrieving the
% timestamps of the button events is shown. This method is fast and
% accurate, but only provides results after a session is finished, so
% programming may appear a bit awkward to some users.
%
% Then the same reaction time experiment is run again, this time
% implementing a different method to retrieve button timestamps "live"
% during the experiment session within each trial. This method is also
% accurate, but involves a calibration procedure which takes about 0.5
% seconds in each trial and therefore slows down the progress of each trial
% a bit.
%
% See the "help PsychRTBox" online help of the PsychRTBox driver for
% detailed explanation of all commands and more commands.
%

% History:
% 02/15/09  mk  Initial version written.

% Psychtoolbox properly installed?
AssertOpenGL;

% Open connection to the first RTBox connected to any USB port on the
% computer. Find the box by scanning all serial-over-usb ports for a
% compatible device. Initialize the driver and return a unique device
% handle 'rtbox' for the box:
rtbox = PsychRTBox('Open');

% After the box is opened, it will immediately start recording button
% responses and other external trigger events and store them in its
% internal event buffer, so your code can retrieve those timestamped events
% later on at a time that is convenient for you. By default the box will
% only report pushbutton presses, not release of buttons or external
% trigger signals on the light- or TTL trigger input.
% If you wanted to enable additional events to be reported, you could
% select the events to be reported via PsychRTBox('Enable', rtbox, eventname);
%
% E.g., this...
% PsychRTBox('Enable', rtbox, 'pulse');
% ... would enable reporting of TTL input pulse trigger signals as well.
%
% this...
% PsychRTBox('Enable', rtbox, 'release');
% ... would enable reporting of button release actions as well.
%
% You can disable reporting of selected events via PsychRTBox('Disable');
%
% E.g., this...
% PsychRTBox('Disable', rtbox, 'press');
% ... would disable reporting of button press actions.
%
% You can also set multiple events at once, e.g.:
% PsychRTBox('Enable', rtbox,{'release', 'pulse'});
%
% Anyway, for now we are happy with the default press events only...


% Btw. as long as your script only uses one RTBox device at a time, you
% don't really need to pass the 'rtbox' handle to all PsychRTBox functions.
% Instead of the handle you could either leave the argument away, or pass
% in the default argument []. In that case, the driver will assume that you
% want to operate on the first opened RTBox. E.g., as long as you only want
% to access the first open RTBox, PsychRTBox('GetBoxInfo'); is the same as
% PsychRTBox('GetBoxInfo', []); and PsychRTBox('GetBoxInfo', rtbox);
%
% However, we love clean coding for the sake of clean coding, so we will
% pass the 'rtbox' handle here just to demonstrate good coding style ;-)

% Query and print all box settings inside the returned struct 'boxinfo':
boxinfo = PsychRTBox('BoxInfo', rtbox);
disp(boxinfo);

% Wait for at least one button to be pressed:
fprintf('\n\nPlease press any button on the box.\n');
PsychRTBox('WaitButtonDown', rtbox);

% Test live button queries: This is the equivalent to KbCheck for the
% keyboard. It queries the current state of the box buttons. This method is
% simple, but not very exact in timing, as some delay is involved in live
% query of button state over the USB bus:

% Wait for all buttons to be released:
fprintf('Please release all buttons on the box.\n');
PsychRTBox('WaitButtonUp', rtbox);

% This loop will show the state of all buttons in an infinite loop until
% all buttons are pressed simultaneously:
while 1
    % Get 4-element bState button state vector: Each element corresponds to
    % a button on the box. A 1 means button pressed, a 0 means button
    % released:
    bState = PsychRTBox('ButtonDown', rtbox);
    clc;
    fprintf('Press and release buttons randomly. I will report buttonstate,\nuntil you press all 4 buttons simultaneously.\n');
    fprintf('ButtonState is '); disp(bState);
    
    if all(bState)
        % All buttons pressed. Break out of loop.
        break;
    end
end

drawnow;

% Wait for all button to be released:
fprintf('\n\nPlease release all buttons on the box.\n');
PsychRTBox('WaitButtonUp', rtbox);

fprintf('Thanks!\n\n');

% Now for a little fake RT experiment to demonstrate how to collect button
% responses with accurate timestamps:
%
% This is a very minimalistic experiment. In a loop the suject has to press
% a button on the box as fast as possible as soon as she hears a beep
% sound. For the sake of simplicity we will use the simple Beeper()
% function for sound generation. Albeit simple, this is not how you'd do it
% in a real auditory RT study! For a real study you'd use our
% PsychPortAudio sound driver (help PsychPortAudio for more info), which is
% optimized for high quality sound with accurate and reliable timing.
% However, we don't want to distract you here with sound related stuff, so
% we go for the simple solution.

% Start response collection and recording by the box:
% This is not strictly needed here, as the box is already "started" after
% opening the connection, but it doesn't hurt to use it. If the box is
% already started, this command will get automatically ignored:
PsychRTBox('Start', rtbox);

tBeep = [];
t = [];
b = [];

% Run 4 trials:
for trial = 1:4
    % Clear Matlab window:
    clc;
    
    % Give a "attention" signal:
    fprintf('Ready? Push a button on the box as fast as possible when you hear the beep noise.\n');
    drawnow;
    WaitSecs(2);
    
    % Now we clear the input buffer of the RTBox to get rid of any button
    % press events from previous trials or activities:
    PsychRTBox('Clear', rtbox);
    
    % From now on, the box is waiting for responses and recording them into
    % its internal buffer for later retrieval...
    
    % Wait a random amount of time within the interval 0 - 3 seconds. Take
    % a tOnset timestamp after waiting. We define tOnset as stimulus onset
    % time for the sake of simplicity here:
    tOnset = WaitSecs(rand * 3);
    
    % Emit standardized beep noise: Normally one would use the
    % PsychPortAudio driver here for good timing precision:
    Beeper(400, 0.8, 0.15);
    
    % Now we wait for some button response from the box, but not forever.
    % We wait until either exactly one button response has been received
    % (that is the 1 in the function), or until 5 seconds of time have
    % elapsed, with or without a reponse. The function will return 'tPress'
    % and 'evt', which are either empty (i.e. == []) if no button was
    % pressed within the 5 seconds, or they contain the id of the button in
    % 'evt' and the timestamp in seconds of the button press in 'tPress'.
    % Please note that 'tPress' is expressed in time of the builtin clock
    % of the box, not in GetSecs time! That means you can't directly
    % compare the tOnset timestamp from above with the tPress timestamp
    % from here. For a way to do that we'll show you examples further down
    % the code...
    [tPress, evt] = PsychRTBox('BoxSecs', rtbox, 5, [], 1);
    
    % Store result of this trial:
    if isempty(evt)
        fprintf('No response given within 5 seconds :-( Invalid trial.\n');
        WaitSecs(5);
    else
        fprintf('Good boy!\n');
        WaitSecs(3);
        t(end+1) = tPress; %#ok<AGROW>
        b(end+1) = evt; %#ok<AGROW>
        tBeep(end+1) = tOnset; %#ok<AGROW>
    end
    
    % Next trial...
end

% Ok, our experiment is finished. We'd like to have our button timestamps
% (stored in vector 't') mapped to normal computer GetSecs time, so we can
% calculate RT's post-hoc. This is accomplished by the following mapping
% function. It will perform some calibration and then convert the
% timestamps in 't' to computer timestamps and return them in the vector
% 'tResponse'. This function will stop response collection on the box, so
% you'll need to call the 'Start' method again. This function should be
% only called once during an experiment session at the end of all response
% collection for best accuracy:
tResponse = PsychRTBox('BoxsecsToGetsecs', rtbox, t);

% Calculate RT's:
RT = tResponse - tBeep;

% Show results for all trials:
for i = 1:length(RT)
    fprintf('In trial %i: Button %s pressed. RT = %f msecs.\n', i, char(b(i)), RT(i) * 1000);
end

% Close down connections to all open boxes, ie., to our only one ;-)
% Shutdown driver as well. A call to PsychRTBox('Close', rtbox); would have
% closed only the connection to the box 'rtbox' if there would have been
% multiple boxes open for uses:
PsychRTBox('CloseAll');

fprintf('Experiment I Done.\n\n');


% Ok, now we'll show you a second approach to button timestamp collection.
% This time we will retrieve timestamps in the computers GetSecs clock time
% already during the experiment. This has the advantage that you can
% immediately work with timestamps. The disadvantage is that the driver has
% to perform a clock synchronization procedure frequently - in this example
% at the start of each new trial - in order to achieve the same accuracy as
% in the example above. Therefore each trial will last about 0.5 seconds
% longer. Also, an initial calibration procedure is needed at start of the
% experiment, which takes about 60 seconds.

% Reopen the box:
rtbox = PsychRTBox('Open');

% Perform the special clock calibration procedure. This will take about 60
% seconds. You could select a different duration, e.g., 30 seconds via the
% call PsychRTBox('ClockRatio', rtbox, 30); if you wanted. However, shorter
% calibration duration means less accurate timestamps...
PsychRTBox('ClockRatio', rtbox);

% Wait for all button to be released:
clc;
fprintf('\n\nPlease release all buttons on the box.\n');
PsychRTBox('WaitButtonUp', rtbox);

tBeep = [];
t = [];
b = [];

% Run 4 trials:
for trial = 1:4
    % Clear Matlab window:
    clc;
    
    % Give a "attention" signal:
    fprintf('Ready? Push a button on the box as fast as possible when you hear the beep noise.\n');
    drawnow;
    WaitSecs(2);
    
    % Now we clear the input buffer of the RTBox to get rid of any button
    % press events from previous trials or activities. Additionally we set
    % the optional syncClocks flag to 1. This will resynchronize the clock
    % of the computer and the box for optimum accuracy of timestamps. The
    % procedure takes about 0.5 seconds extra time.
    PsychRTBox('Clear', rtbox, 1);
    
    % From now on, the box is waiting for responses and recording them into
    % its internal buffer for later retrieval...
    
    % Wait a random amount of time within the interval 0 - 3 seconds. Take
    % a tOnset timestamp after waiting. We define tOnset as stimulus onset
    % time for the sake of simplicity here:
    tOnset = WaitSecs(rand * 3);
    
    % Emit standardized beep noise: Normally one would use the
    % PsychPortAudio driver here for good timing precision:
    Beeper(400, 0.8, 0.15);
    
    % Now we wait for some button response from the box, but not forever.
    % We wait until either exactly one button response has been received
    % (that is the 1 in the function), or until 5 seconds of time have
    % elapsed, with or without a reponse. The function will return 'tResponse'
    % and 'evt', which are either empty (i.e. == []) if no button was
    % pressed within the 5 seconds, or they contain the id of the button in
    % 'evt' and the timestamp in seconds of the button press in 'tPress'.
    % Please note that 'tPress' is expressed this time in GetSecs time
    % units. That means you can directly compare the tOnset timestamp from
    % above with the tPress timestamp from here.
    % For the fun of it, we also return the times in box clock times in the
    % optional return argument 'tBox', so we can later remap the tBox times
    % into GetSecs times post-hoc, although this isn't neccessary here.
    % Anyway, just to show you it is possible to do that.
    [tResponse, evt, tBox] = PsychRTBox('GetSecs', rtbox, 5, [], 1);
    
    % Store result of this trial:
    if isempty(evt)
        fprintf('No response given within 5 seconds :-( Invalid trial.\n');
        WaitSecs(5);
    else        
        % We calculate the RT immediately this time:
        RT = tResponse - tOnset;
        fprintf('In trial %i: Button %s pressed. RT = %f msecs.\n', trial, char(evt), RT * 1000);
        WaitSecs(3);
        t(end+1) = tBox; %#ok<AGROW>
        b(end+1) = evt; %#ok<AGROW>
        tBeep(end+1) = tOnset; %#ok<AGROW>
    end
    
    % Next trial...
end

% Ok, our experiment is finished. We can perform a post-hoc remapping of
% box clock timestamps to computer timestamps again if we want, although
% this wouldn't be neccessary here, as it has been done already "live"
% within the trial loop via the PsychRTBox('GetSecs') function...
tResponse = PsychRTBox('BoxsecsToGetsecs', rtbox, t);

% Calculate RT's:
RT = tResponse - tBeep;

clc;

% Show results for all trials:
for i = 1:length(RT)
    fprintf('In trial %i: Button %s pressed. RT = %f msecs.\n', i, char(b(i)), RT(i) * 1000);
end

% Close down connections to all open boxes, ie., to our only one ;-)
% Shutdown driver as well. A call to PsychRTBox('Close', rtbox); would have
% closed only the connection to the box 'rtbox' if there would have been
% multiple boxes open for uses:
PsychRTBox('CloseAll');

fprintf('\nExperiment II Done. Bye!\n\n');

return;
