function varargout = PsychRTBox(varargin)
% Driver for the USTC reaction time button box (RTBox) by Xiangrui Li et al.
% varargout = PsychRTBox(cmd, varargin);
%
% WORK IN PROGRESS, NOT FULLY READY FOR PRODUCTION USE!
%
% This driver allows to control all functions of the USTC RTBox response
% button box. The box is a device, connectable to the USB port. It provides
% 4 response buttons (pushbuttons) for subject responses and can report any
% button press- or release by the subject. Additionally it has an input for
% reporting of external TTL trigger signals and a photo-diode input for
% reporting of visual stimulus onset. The box uses a built-in
% high-resolution clock to timestamp all button- or trigger events,
% independent of the host computers clock in order to make it more reliable
% for response time measurements than most other response devices. It also
% buffers all events internally, so users experiment scripts can read back
% events when it is most convenient for the users code. Timestamps can be
% either reported in Psychtoolboxs standard GetSecs timebase for direct
% comparison with timestamps from GetSecs, WaitSecs, KbCheck et al.,
% Screen('Flip') and PsychPortAudio, etc. This simplifies reaction time
% calculations. Timestamps can also be reported in the boxes own timebase,
% e.g., time of a button press relative to the photo-diode light trigger
% signal or TTL trigger signal, if this is more convenient for a given
% experiment setup.
%
% See http://lobes.usc.edu/RTbox for up to date product information.
%
% The following subcommands are currently suppported:
%
% handle = PsychRTBox('Open' [, deviceID]);
% -- Try to open a connected RTBox, return a device handle 'handle' to it
% on success. The handle can be used in all further subcommands to refer to
% the box. By default, all USB ports (or rather USB-Serial ports) are scanned
% for a connected RTBox and the driver will connect to the first box found.
% Alternatively you can specify which box to use via the optional
% 'deviceID' namestring. This can be either the name of a box, or the name
% of the serial port to which the box is connected. This way you can avoid
% scanning of all ports and disambiguate in case multiple boxes are
% connected to your computer.
%
% Btw., if you only make use of one single RTBox, you don't need to specify
% the 'handle' parameter to all following subfunctions. Instead you can
% specify that parameter as [] or omit it and the driver will use the only
% open box connected.
%
% After opening the box, you usually will want to invoke this method:
%
%
% clockRatio = PsychRTBox('ClockRatio' [, handle] [, durationSecs]);
% -- Perform a clock drift calibration between the computers GetSecs host
% clock and the internal clock of the box 'handle'. Restrict calibration to
% a maximum of 'durationSecs' (default 60 seconds if omitted). Return the
% computed 'clockRatio' and use it for all further operations.
%
% Due to manufacturing imperfections and environmental factors, no two
% clocks ever run at exactly the same speed. Therefore the computer clock
% and box clock will slowly "drift out of sync" under normal conditions,
% rendering retrieved event timestamps inaccurate over the course of a long
% experiment session. This calibration routine will exercise the clocks and
% compute the clock drift due to this speed difference, then use the
% computed drift (= clockRatio) to correct all reported timestamps for this
% drift, thereby providing the accuracy needed for reaction time studies.
%
% The clockRatio value tells, how many seconds of GetSecs time elapse when
% the box clock measures 1 second elapsed time. Ideally this value would be
% 1, ie. box clocks run at the same speed. A more realistic value would be,
% e.g., 1.000009 -- The computer clock goes 9 microseconds faster than the
% box clock, so the drift will introduce an error of 9 microseconds during
% every elapsed second of your study.
%
% As every calibration, this routine involves some measurement/calibration
% error and is therefore not perfect, so even after a successfull
% 'ClockRatio' calibration, timestamps reported during your experiment will
% accumulate some error during the course of long experiment sessions.
%
% There are two ways to handle this:
%
% a) Use a long calibration time in this function for accurate results, and
% a reasonably short experiment duration.
%
% b) Repeat this procedure after every large block of trials, ie., every
% couple of minutes, e.g., while the subject is allowed to take a break in
% a long experiment session.
%
% c) Use the PsychRTBox('SyncClocks') function after each short block of
% trials, or even after each trial, for the highest accuracy.
%
%
% [syncResult, clockRatio] = PsychRTBox('SyncClocks' [, handle]);
% -- Synchronize or resynchronize the clocks of the host computer and the
% box, return result in 'syncResult' and the current clockRatio in
% 'clockRatio'. This routine is automatically carried out during invocation
% of PsychRTBox('ClockRatio'); but you can repeat the sync procedure
% anytime between trials via this subfunction for extra accuracy at the
% expense of about 0.5 - 1 second additional time for each invocation. You
% would typically execute this function at the start of each large block of
% trials, or before start of each trial if you are really picky about
% super-exact timing. The syncResult contains three values:
%
% syncResult(1) = Host time (GetSecs time) at time of clock sync.
%
% syncResult(2) = Box time at time of clock sync.
%
% syncResult(3) = Confidence interval for the accuracy of the sync. This
% value (in seconds) provides a reliable upper bound for the possible error
% introduced in all reported timestamps from the box. The real error may be
% significantly smaller, this is just an upper bound that you can check.
% Typical results on a well working system should be in the sub-millisecond
% range, e.g., 0.0003 seconds or 0.3 msecs. Typical results on a rather
% noisy system would be around 0.001 second or 1 msec. Results worse than 2
% msecs indicate some problem with your system setup that should be fixed
% before executing any experiment study which involves reaction time
% measurements. By default, the sync procedure will abort with an error if
% it can't calibrate to an accuracy with a maximum error of 2 msecs within
% a duration of 0.5 seconds. You can change these default constraints with
% a call to PsychRTBox('SyncConstraints').
%
%
% PsychRTBox('SyncConstraints'[, maxDurationSecs][, goodEnoughSecs][, requiredSecs]);
% -- Change the constraints to apply during calls to PsychRTBox('SyncClocks');
% 'maxDurationSecs' limits any call to 'SyncClocks' to a duration of at
% most the given number of seconds. Calibration aborts after at most that
% time, even if unsuccessfull - in that case with an error message. By
% default, the duration is limited to 0.5 seconds.
% 'goodEnoughSecs' Calibration will finish before 'maxDurationSecs' have
% elapsed, if the result is more accurate than an error of at most
% 'goodEnoughSecs'. By default, this is set to zero seconds, i.e.,
% calibration will always take 'maxDurationSecs'.
% 'requiredSecs' - The calibration will only use samples with an
% uncertainty of at most 'requiredSecs'. If not even a single sample of the
% required precision can be acquired within 'maxDurationSecs', the call
% will fail with an error, indicating that your system setup doesn't
% provide the required timing precision for your demands. By default, the
% minimum required precision is 0.002 seconds, ie., it will tolerate an
% error of at most 2 msecs.
%
%
% oldverbose = PsychRTBox('Verbosity' [, handle], verbosity);
% -- Set level of verbosity for driver: 0 = Shut up. 1 = Report errors
% only. 2 = Report warnings as well. 3 = Report additional status info. 4 =
% Be very verbose about what is going on. The default setting is 3 --
% Report moderate status output.
%
%
% devinfo = PsychRTBox('BoxInfo' [, handle] [, newdevinfo]);
% -- Return a struct 'devinfo' with all information about the current
% status and parameter settings for RTBox 'handle'. Optionally set a new
% struct with updated parameters via 'newdevinfo'. This function is mostly
% useful for debugging and benchmarking the driver itself. Most information
% contained in 'devinfo' will be useless for your purpose.
%
%
% PsychRTBox('Close', handle);
% -- Close connection to specific box 'handle'. Release all associated
% ressources.
%
%
% PsychRTBox('CloseAll');
% -- Close connections to all attached RTBox devices. Reset the PsychRTBox
% driver completely. You'll usually use this function at the end of your
% experiment script to clean up.
%
%
% oldeventspec = PsychRTBox('Enable' [,handle][, eventspec]);
% -- Enable specified type of event 'eventspec' on box 'handle'. This
% allows to enable detection and reporting of a specific type of event. By
% default, only reporting of push-button press is enabled, as this is the
% most common use of a response box.
%
% The following names are valid for the name string 'eventspec':
% 'press' = Report push-button press. This is the default setting.
% 'release' = Report push-button release.
% 'pulse' = Report TTL trigger events on external input port.
% 'light' = Report reception of light flashes by photo-diode on light port.
% 'lightoff' = Report offset of light on photo-diode.
% 'all' = Enable all events.
%
% If called without the 'eventspec' parameter, the function will return the
% names of all currently enabled events.
%
%
% oldeventspec = PsychRTBox('Disable' [,handle][, eventspec]);
% -- Disable specified type of event 'eventspec' on box 'handle'. This
% allows to disable detection and reporting of a specific type of event. By
% default, only reporting of push-button press is enabled, as this is the
% most common use of a response box.
%
% See 'Enable' call for help on parameters.
%
%
% Once you have setup and calibrated the box and selected the type of
% events to detect and report, you will want to actually retrieve
% information about events. For this you use this command:
%
% [time, event, boxtime] = PsychRTBox('GetSecs' [, handle]);
% -- Retrieve all recorded events from the box. If there aren't any pending
% events from the box, wait for up to 0.1 seconds for events to arrive.
%
% The function will return an array of timestamps in 'time', and an array
% of corresponding names of the events in 'event'. E.g., event(1) will
% report the identity of the first detected event, e.g., '1' if button 1
% was pressed, whereas time(1) will tell you when the event happened, ie.,
% when button 1 was pressed. Each call will return all currently available
% events. If no events are pending since last invocation of this function,
% empty vectors will be returned.
%
% By default, the following names are possible for 'event's:
%
% '1' = First button pressed. '1up' = First button released.
% '2' = 2nd button pressed, '2up' = 2nd button released.
% '3' = 3rd button pressed, '3up' = 3rd button released.
% '4' = 4th button pressed, '4up' = 4th button released.
% 'pulse' = TTL pulse received on TTL pulse input port.
% 'light' = Light pulse received by photo-diode connected to light input port.
% 'lightoff' = Light pulse offset on photo-diode connected to light input port.
% 'serial' = Trigger command received on USB-Serial port.
%
% However, you can assign arbitrary names to the buttons and events if you
% don't like this nomenclature via the PsychRTBox('ButtonNames') command.
%
% The reported timestamps are expressed in host clock time, ie., in the
% same units as the timestamps returned by GetSecs, Screen('Flip'),
% PsychPortAudio, KbCheck, KbWait, etc., so you can directly calculate
% reaction times to auditory stimuli, visual stimuli and other events.
%
% See the help for PsychRTBox('SyncClocks') and PsychRTBox('ClockRatio')
% for the accuracy of these timestamps.
%
% Additionally the event times are also returned in 'boxtime', but this
% time expressed in box time -- the time of the box internal clock.
% 
%
% You can also only retrieve time in box time without remapping to GetSecs
% time by calling:
%
% [boxtime, event] = PsychRTBox('BoxSecs' [, handle]);
% -- Timestamps are in raw box clock time, everything else is the same as
% in PsychRTBox('GetSecs' [, handle]).
%
% If you have the 'boxtime' timestamps from one of the previous functions
% around, you can map them later to GetSecs time with very high precision
% at the end of your experiment session via:
%
% [GetSecs, Stddev] = PsychRTBox('MapBoxSecsToGetSecsPostHoc' [, handle], boxTimes);
% -- Perform a post-hoc mapping of a vector of raw box timestamps
% 'boxTimes' into a vector of host clock 'GetSecs' timestamps. Return some
% error measure in 'Stddev' as well, if available.
%
% This method can be used to convert event timestamps expressed in the box
% clocks timebase into timestamps in Psychtoolbox GetSecs host clock
% timebase. It has the advantage of providing the highest possible accuracy
% in mapping, because it computes an optimal mapping function for this
% purpose, which is based on all the timing information collected
% throughout a whole experiment session. The disadvantage is that it will
% only provide meaningful results if you call it at the end of your
% experiment session, so you'll need to manage all your collected
% timestamps in a format that is suitable as input to this function.
%
%
% ... TODO ... FINISH THIS ...

% History:
% 08/01/2008 Initial implementation based on RTBox.m from Xiangrui Li (MK).
% 01/29/2009 "Close to beta" release. First checkin to SVN (MK).
% 01/30/2009 Improved syncClocks algorithm, option to spec a specific box
%            by port in the open call (MK).

% Global variables: Need to be persistent across driver invocation and
% shared with internal subfunctions:
global rtbox_info;
global nrOpen;
global blocking;
global eventcodes;
global events4enable;
global enableCode;
global cmds;
global rtbox_oldstylesync;
global rtbox_maxDuration;
global rtbox_optMinwinThreshold;
global rtbox_maxMinwinThreshold;

% Start of driver code -- Entry point:

    % First time invocation? Perform init of device arrays and global
    % settings:
    if isempty(rtbox_info)
        % Setup device info struct array, as well as per-device default
        % settings:
        % CAUTION: Same settings are reassigned in the openRTBox()
        % subfunction each time PsychRTBox('Open') is called! The settings
        % made there override the settings made here!!!
        rtbox_info=struct('events',{{'1' '2' '3' '4' '1up' '2up' '3up' '4up' 'pulse' 'light' 'lightoff' 'serial'}},...
            'enabled',[], 'ID','','handle',[],'portname',[],'sync',[],'version',[],'clkRatio',1,'verbosity',3, 'syncSamples', []);

        % Setup event codes:
        eventcodes=[49:2:55 50:2:56 97 48 57 89]; % code for 12 events
        
        % List of supported subcommands:
        cmds={'close' 'closeall' 'clear' 'purge' 'start' 'test' 'buttondown' 'buttonnames' 'enable' 'disable' 'clockratio' 'syncclocks' ...
              'box2getsecs' 'box2secs' 'boxinfo' 'getcurrentboxtime','verbosity','syncconstraints', 'mapboxsecstogetsecsposthoc'};
          
        % Names of events that can be enabled/disabled for reporting:
        events4enable={'press' 'release' 'pulse' 'light' 'lightoff' 'all'};
        
        % Low-level protocol codes corresponding to the events:
        enableCode='DUPOFA'; % char to enable above events, lower case to disable
        
        % Preload some functions of PTB we'll need:
        eval('GetSecs;WaitSecs(0.001);');
        
        % Selection of blocking strategy that the IOPort driver shall use
        % for blocking writes:
        if IsWin || IsOSX
            % A blocking wait gives very good results on OS/X. On Windows
            % it gives the same results as a polling wait (setting 2), so
            % we prefer blocking for lower cpu load at same quality on
            % Windows as well:
            blocking = 1;
        else
            % On Linux, a polling wait is of advantage, so we use that:
            blocking = 2;
        end

        % No devices open at first invocation:
        nrOpen = 0;
        
        % Default settings for the syncClocks() function:
        % -----------------------------------------------
        
        % Use new style syncClocks() method by default:
        rtbox_oldstylesync = 0;

        % Maximum duration of a syncClocks calibration run is 0.5 seconds:
        rtbox_maxDuration = 0.5;

        % Desired 'minwin' calibration accuracy is 0.0 msecs: If we manage
        % to get better than that, we abort sampling. We also abort
        % sampling of the rtbox_maxDuration is reached:
        rtbox_optMinwinThreshold = 0.0;

        % Maximum allowable (ie. worst) acceptable minwin for a sample:
        % On OS/X or Linux we could easily do with 2 msecs, as a 1.2 msecs
        % minwin is basically never exceeded. On MS-Windows however, 2.x
        % durations happen occassionally...
        rtbox_maxMinwinThreshold = 0.002;
    end

    if nargin < 1
        error('You must provide a command string to PsychRTBox!');
    end

    % Command dispatch:
    cmd = lower(varargin{1});
    if isempty(cmd)
        error('You must provide a non-empty command string to PsychRTBox!');
    end
    
    % Open the connection to device, do initial setup and sync:
    if strcmp(cmd, 'open')
        % Assign deviceID identifier of device to open, or the default
        % name 'Default' if none specified: User can also specify a serial
        % port device name for a device:
        if nargin < 2
            deviceID=[];
        else
            deviceID=varargin{2};
        end

        if isempty(deviceID)
            deviceID = 'Default';
        end

        % Open and initialize box:
        openRTBox(deviceID, nrOpen+1);

        % Increment count of open boxes:
        nrOpen = nrOpen + 1;

        % Return as handle:
        varargout{1} = nrOpen;

        % Perform initial mandatory clock sync:
        syncClocks(nrOpen, 1:5);

        return;
    end

    if strcmp(cmd, 'syncconstraints')
        % Return current constraint settings:
        varargout{1} = rtbox_maxDuration;
        varargout{2} = rtbox_optMinwinThreshold;
        varargout{3} = rtbox_maxMinwinThreshold;
        
        % Set constraints for syncClocks:
        if nargin > 1 && ~isempty(varargin{2})
            rtbox_maxDuration = varargin{2};
        end

        if nargin > 2 && ~isempty(varargin{3})
            rtbox_optMinwinThreshold = varargin{3};
        end

        if nargin > 3 && ~isempty(varargin{4})
            rtbox_maxMinwinThreshold = varargin{4};
        end
        
        return;
    end
    
    % Deal with variable number of inputs:
    nIn = max(nargin - 1, 0);
    if nIn > 1
        in2=varargin{3};
    end

    % Device handle provided?
    if nargin > 1
        % Yes: Store it in 'id':
        id = varargin{2};
    else
        % Nope.
        id = [];
    end

    % If no device handle - or empty default handle - provided, just
    % default to the first open RTBox device for convenience in
    % setups/scripts that only use one RTBox -- which is probably the
    % common case:
    if isempty(id)
        id = 1;
    end
    
    % Child protection:
    if ~isscalar(id) || id < 1 || id > length(rtbox_info) || id > nrOpen
        error('Invalid device handle specified! Did you open the device already?');
    end

    if isempty(rtbox_info(id).handle)
        error('Invalid device handle specified! Maybe you closed this device already?');
    end

    % Build additional cell array of valid read commands:
    read=rtbox_info(id).events;
    read{end+1}='secs';    % Obsolete: like GetSecs but with old method.
    read{end+1}='boxsecs'; % All events measured in absolute box time.
    read{end+1}='getsecs'; % All events measured in absolute GetSecs time.

    % Assign serial port handle:
    s = rtbox_info(id).handle;

    % Check for invalid commands:
    if ~any(strmatch(cmd,[cmds read],'exact')) % if invalid cmd, we won't open device
        if ~isempty(id)
            RTboxError('unknownCmd',cmd,cmds,rtbox_info(id).events); % invalid command
        else
            RTboxError('unknownCmd',cmd,cmds,{''}); % invalid command
        end
    end

    % Subcommand dispatch:
    switch cmd
        case 'verbosity'
            if nIn<2
                error('You must provide the new level of "verbosity" to assign!');
            end
            
            % Return old level of verbosity:
            varargout{1} = rtbox_info(id).verbosity;

            % Assign new level of verbosity to device:
            rtbox_info(id).verbosity = in2;
            
        case 'start' % send serial trigger to device
            [nw tWritten]=IOPort('Write',s, 'Y'); % blocking write
            if nargout, varargout{1}=tWritten; end
            
        % Retrieve all pending events from the box, aka the serial port
        % receive buffers, parse them, filter/postprocess them, optionally
        % return mapped event timestamps in GetSecs timebase:
        case read % 12 triggers, plus 'secs' 'boxsecs' 'getsecs'
            cmdInd=strmatch(cmd,read,'exact'); % which command
            minbytes=7; % 1 event
            if cmdInd<13 % relative to trigger
                ind=[cmdInd<5 (cmdInd<9 && cmdInd>4) cmdInd==9:11];
                if ~rtbox_info(id).enabled(ind), RTboxError('triggerDisabled',events4enable{ind}); end
                minbytes=14; % at least 2 events
            end
            varargout={[] '' []}; % return empty if no event detected
            isreading=false;
            byte=IOPort('bytesAvailable',s);
            tnow = GetSecs; % return tnow
            if ~exist('in2','var'), in2=0.1; end % default timeout
            tout=tnow+in2; % stop time
            while (tnow<tout && byte<minbytes || isreading)
                tnow = WaitSecs(0.01); % don't hang processor
                byte1=IOPort('bytesAvailable',s);
                isreading= byte1>byte; % wait if is reading
                byte=byte1;
            end
            nevent=floor(byte/7);
            if nevent==0, return; end  % return if nothing
            b7=IOPort('read',s,1,nevent*7);
            b7=reshape(b7,[7 nevent]); % each event contains 7 bytes
            timing=[];
            for i=1:nevent % extract each event and time
                ind=min(find(b7(1,i)==eventcodes)); %#ok<MXFND> % which event
                if isempty(ind)
                    RTboxWarn('invalidEvent',b7(:,i));
                    break; % not continue, rest must be messed up
                end
                event{i}=rtbox_info(id).events{ind}; %#ok event name
                timing(i)=bytes2secs(b7(2:7,i)); %#ok
            end
            if isempty(timing), return; end

            if cmdInd==15
                % Convert into computer time: MK-Style

                % First return optional "raw" array with boxtimes:
                varargout{3} = timing;
                
                % Then remap to GetSecs host timebase:
                timing = box2GetSecsTime(id, timing);
                
            elseif cmdInd==13 % secs: convert into computer time
                % Convert into computer time: Xiangrui-Style

                % First return optional "raw" array with boxtimes:
                varargout{3} = timing;

                % Then remap to GetSecs host timebase via Xiangrui's method:
                timing = box2SecsTime(id, timing);
                
            elseif cmdInd<13 % trigger: relative to trigger
                ind=strmatch(cmd,lower(event),'exact'); % trigger index
                if isempty(ind), RTboxWarn('noTrigger',cmd); return; end
                ind=ind(1); % use the 1st in case of more than 1 triggers
                trigT=timing(ind); % time of trigger event
                event(ind)=[]; timing(ind)=[]; % omit trigger and its time from output
                if isempty(event), return; end % if only trigger event, return empty
                timing=timing-trigT;   % relative to trigger time
            end
            if length(event)==1, event=event{1}; end % if only 1 event, use string
            varargout{1} = timing;
            varargout{2} = event;

        case 'boxinfo'
            % Return complete device info struct:
            varargout{1} = rtbox_info(id);
            
            % Optionally set a new one -- Only for debugging!!
            if nIn > 1
                rtbox_info(id) = in2;
            end
            
        case 'box2getsecs'
            % Map boxtime to GetSecs time with the recommended method:
            if nIn<2
                error('You must provide the boxtime to map!');
            end
            
            varargout{1} = box2GetSecsTime(id, varargin{3});
            
        case 'mapboxsecstogetsecsposthoc'
            % Map boxtime to GetSecs time post-hoc style:
            % We compute an optimal least-squares fit linear mapping
            % function of boxtime to hosttime, using all collected
            % syncClocks samples from the whole experiment session. Then we
            % remap all given input boxsecs samples to getsecs time via
            % lookup in that linear best-fit. This automatically corrects
            % for clock-drift and should provide the least possible error
            % on the mapping procedure, because it makes use of all
            % available sync information of a whole session:
            if nIn<2
                error('You must provide the boxtimes to map!');
            end
            
            [remapped, sd, clockratio] = box2GetSecsTimePostHoc(id, varargin{3}); 
            varargout = { remapped, sd , clockratio};
                        
        case 'box2secs'
            % Map boxtime to GetSecs time according to Xiangruis method:
            % (Obsolete, just left for debugging):
            if nIn<2
                error('You must provide the boxtime to map!');
            end
            
            varargout{1} = box2SecsTime(id, varargin{3});
            
        case 'purge'
            % Clear send/receive buffers of box:
            str=enableCode(4:5);
            enableEvent(id, str(rtbox_info(id).enabled(4:5))); % enable light if applicable
            purgeRTbox(id); % clear buffer

        case 'getcurrentboxtime'
            % Retrieve current time of box clock.
            
            % We do so by performing a syncClocks call, but retaining the
            % current .sync results, so we just (mis-)use the function for
            % our purpose:
            tmpsync = rtbox_info(id).sync;
            syncClocks(id, 1:5); % clear buffer, sync clocks
            varargout{1} = rtbox_info(id).sync;
            rtbox_info(id).sync = tmpsync;

        case 'clear'
        case 'syncclocks'
            % Synchronize host clock and box clock, i.e., establish mapping
            % between both:
            syncClocks(id, 1:5); % clear buffer, sync clocks
            if nargout
                varargout{1}=rtbox_info(id).sync;
                varargout{2}=rtbox_info(id).clkRatio;
            end

        case 'buttondown'
            for i=1:5 % try several times, since button event may mess up '?'
                purgeRTbox(id); % clear buffer
                IOPort('write',s,'?'); % ask button state: '4321'*16 63
                b2=IOPort('read',s,1,2); % ? returns 2 bytes
                if length(b2)==2 && b2(2)==63 && mod(b2(1),16)==0, break; end
                if i==5, RTboxError('notRespond'); end
            end

            b2=dec2bin(b2(1)/16,4); % first 4 bits are button states
            if nIn<2, in2=read(1:4); end % not specified which button

            in2=cellstr(in2); % convert it to cellstr if it isn't
            for i=1:length(in2)
                ind=strmatch(lower(in2{i}),read(1:4),'exact');
                if isempty(ind), RTboxError('invalidButtonName',in2{i}); end
                bState(i)=str2num(b2(5-ind)); %#ok
            end
            varargout{1}=bState;

        case 'buttonnames' % set or query button names
            oldNames=rtbox_info(id).events(1:4);
            if nIn<2, varargout{1}=oldNames; return; end
            if length(in2)~=4 || ~iscellstr(in2), RTboxError('invalidButtonNames'); end
            for i=1:length(in2)
                in2{i} = lower(in2{i});
            end
            rtbox_info(id).events(1:4)=in2;
            for i=5:8
                rtbox_info(id).events(i)=[char(rtbox_info(id).events(i-4)) 'up'];
            end
            if nargout, varargout{1}=oldNames; end

        case {'enable' 'disable'} % enable/disable event detection
            if nIn<2 % no event, return current state
                varargout{1}=events4enable(rtbox_info(id).enabled);
                return;
            end
            
            isEnable=strcmp(cmd,'enable');
            str=enableCode; % upper case to enable
            if ~isEnable, str=lower(str); end % lower case to disable
            in2=lower(cellstr(in2));
            for i=1:length(in2)
                ind=strmatch(in2{i},events4enable,'exact');
                if isempty(ind), RTboxError('invalidEnable',events4enable); end
                enableEvent(id, str(ind));
                if ind==6, ind=1:5; end % all
                rtbox_info(id).enabled(ind)=isEnable; % update state
            end
            if nargout, varargout{1}=events4enable(rtbox_info(id).enabled); end
            if ~any(rtbox_info(id).enabled), RTboxWarn('allDisabled',rtbox_info(id).ID); end

        case 'clockratio' % measure clock ratio computer/box
            % Default to 60 seconds for clock ratio calibration, unless
            % specified otherwise:
            if nIn<2, in2=60; end;

            % Interval between trials
            interval = 1.5 * rtbox_maxDuration;
            
            % Number of trials to perform:
            ntrial=max(5,round(in2/interval));

            if rtbox_info(id).verbosity > 2
                fprintf('PsychRTBox: Measuring clock ratio on box %s. Trials remaining:%4.f', rtbox_info(id).ID, ntrial);
            end
            
            % Switch to realtime priority if not already there:
            oldPriority=Priority;
            if oldPriority < MaxPriority('GetSecs')
                Priority(MaxPriority('GetSecs'));
            end
            

            % Perform ntrial calibration trials:
            tnow = GetSecs;
            for i=1:ntrial
                % Update rtbox_info.sync via a syncClocks() operation:
                syncClocks(id, 1:3);
                
                % Store new syncClocks sample in array:
                t(i,:)=rtbox_info(id).sync; %#ok<AGROW>

                % Give status output:
                if rtbox_info(id).verbosity > 2
                    fprintf('\b\b\b\b%4.f',ntrial-i);
                end

                % Make sure that trials are at least 'interval' secs apart:
                tnow = WaitSecs('UntilTime', tnow + interval);
            end

            % Restore priority to old value:
            if Priority ~= oldPriority
                Priority(oldPriority);
            end
            
            % Octave and older versions of Matlab don't have 'robustfit',
            % so we fall back to 'regress' if this function is lacking:
            if exist('robustfit') %#ok<EXIST>
                [coef st]=robustfit(t(:,2)-t(1,2),t(:,1)-t(1,1));  % fit a line
                sd=st.robust_s*1000; % std in ms
            else
                coef =regress(t(:,1)-t(1,1), [ones(size(t,1), 1) t(:,2)-t(1,2)]);  % fit a line
                sd=0;
            end

            % Assign new clock ratio for use by the timestamp mapping
            % routines later on:
            rtbox_info(id).clkRatio = coef(2);

            if rtbox_info(id).verbosity > 2
                fprintf('\n Clock ratio (computer/box): %.7f\n', rtbox_info(id).clkRatio);
                fprintf(' Sigma (ms): %.2g\n',sd);
            end
            
            if (sd > 0.5) & rtbox_info(id).verbosity > 1 %#ok<AND2>
                fprintf(2,' The ratio may not be reliable. Try longer time for ClockTest.\n');
            end

            % Optionally return new ratio:
            if nargout
                varargout{1} = rtbox_info(id).clkRatio;
            end

        case 'test' % for device test, no complain on error
            byte=IOPort('bytesAvailable',s);
            nevent=floor(byte/7);
            data=IOPort('read',s,1,nevent*7); % read data
            data=reshape(data,[7 nevent]);
            enableEvent(id, 'a'); % avoid interference to the following commands
            purgeRTbox(id); % clear buffer
            fprintf(' ID(%g): %s, %s\n',id,rtbox_info(id).ID,rtbox_info(id).version);
            fprintf(' Serial port: %s (handle = %g)\n',rtbox_info(id).portname, s);
            IOPort('write',s,'?'); % ask button state
            b2=IOPort('read',s,1,2);
            b2=dec2bin(b2(1)/16,4);
            b2=sprintf('%s ',b2(4:-1:1));
            fprintf(' Button down: %s\n',b2);
            fprintf(' Events enabled: %s\n',cell2str(events4enable(find(rtbox_info(id).enabled)>0)));
            syncClocks(id, 1:3); % sync clocks, restore detection
            fprintf(' ComputerClock/BoxClock: %.7f\n',rtbox_info(id).clkRatio);
            fprintf(' GetSecs-BoxSecs: %.4f\n',rtbox_info(id).sync(1));
            fprintf(' Number of events: %g\n',nevent);
            if nevent==0, return; end
            t0=bytes2secs(data(2:7,1)); % first event time
            fprintf(' BoxSecs of event 1 (t0): %.3f s\n\n',t0);
            fprintf('  Data (7 bytes each)%19s  BoxSecs-t0\n','Events');
            for i=1:nevent  % disp each event and time
                ind=find(data(1,i)==eventcodes); % which event
                if isempty(ind)
                    event=''; t=nan; % no error complain
                else
                    event=rtbox_info(id).events{ind}; % event ID
                    t=bytes2secs(data(2:7,i)); % device time
                end
                fprintf('%4g',data(:,i));
                fprintf('%12s%8.3f\n',event,t-t0); % relative to first event
            end

        case 'close' % Close given device
            % Close port
            if ~isempty(rtbox_info(id))
                if s>=0
                    IOPort('Close', s);
                end

                % Mark corresponding struct slot as dead:
                rtbox_info(id).handle = [];

                % Decrease count of open devices:
                nrOpen = nrOpen - 1;
            end

        case 'closeall' % Close all devices
            % Only close our devices, not other devices that may be opened
            % via IOPort but unrelated to us:
            for i=1:length(rtbox_info)
                s=rtbox_info(i).handle;
                if s>=0
                    IOPort('Close', s);
                end
            end

            nrOpen = 0;       % Reset count of open devices to zero.
            clear rtbox_info; % clear main device info struct array.

        otherwise
            % Unknown command:
            RTboxError('unknownCmd',cmd,cmds,rtbox_info(id),events);
            
    end % End of subfunction dispatch.

% End of drivers "main" routine:
end


% Helper functions:
% -----------------

% Obsolete method for mapping of box timestamps to GetSecs timestamps from
% original RTBox driver. MK is not fully convinced that this is correct, so
% we use the box2GetSecsTime() below and keep this for documentation and
% benchmarking...
function timing = box2SecsTime(id, timing)
    global rtbox_info;

    tdiff=rtbox_info(id).sync(1);
    if isempty(tdiff) % sync never done?
        syncClocks(id, 1:3); tdiff=rtbox_info(id).sync(1);
    elseif any(timing-rtbox_info(id).sync(2)>5) % sync done too long before?
        sync=rtbox_info(id).sync; % remember last sync for interpolation
%        syncClocks(id, 1:3); % update sync
        sync(2,:)=rtbox_info(id).sync; % append current sync
        % get tdiff by linear interpolation for all timing
        tdiff=interp1(sync(:,2),sync(:,1),timing * rtbox_info(id).clkRatio);
    end
    timing=(timing * rtbox_info(id).clkRatio) + tdiff - rtbox_info(id).sync(2); % computer time
end

% Map box timestamps to host clock (aka GetSecs) timestamps, based on clock
% sync results from syncClocks, and clock drift correction from clock ratio
% calibration:
function timing = box2GetSecsTime(id, timing)
    global rtbox_info;

    thostbase = rtbox_info(id).sync(1);
    tboxbase  = rtbox_info(id).sync(2);

    % Compute delta in box seconds from time when calibration was
    % done, wrt. box timer, then multiply with clockRatio
    % conversion factor to translate that delta into a delta wrt.
    % host clock, so timing will be the delta in host clock seconds
    % wrt. calibration time:
    timing = (timing - tboxbase) * rtbox_info(id).clkRatio;

    % Add basetime on host - this should give us correctly mapped
    % time in the GetSecs timebase of the computer:
    timing = timing + thostbase;
end

% Map box timestamps to host clock (aka GetSecs) timestamps, based on clock
% sync results from all syncClocks samples from a session:
function [timing, sd, clockratio] = box2GetSecsTimePostHoc(id, timing)
    global rtbox_info;
    global rtbox_maxMinwinThreshold;

    % Check if the latest syncClocks sample is older than 30 seconds. If
    % so, then we acquire a new final sample. We also resample if the last
    % sample is of too low accuracy, or if there are less than 3 samples in
    % total, as the fitting procedure needs at least 3 samples to work:
    while (size(rtbox_info(id).syncSamples, 1) < 3) || ...
          ((GetSecs - rtbox_info(id).syncSamples(end, 1)) > 30) || ...
          (rtbox_info(id).syncSamples(end, 3) > rtbox_maxMinwinThreshold)

      % Perform a syncClocks to get a fresh sample to finalize the sampleset:
      syncClocks(id, 1:5);
    end

    % Extract samples for line fit:
    tbox  = rtbox_info(id).syncSamples(:, 2);
    thost = rtbox_info(id).syncSamples(:, 1);
    
    % Octave and older versions of Matlab don't have 'robustfit',
    % so we fall back to 'regress' if this function is lacking:
    if exist('robustfit') %#ok<EXIST>
        [coef st]=robustfit(tbox,thost);  % fit a line
        sd=st.robust_s; % stddev. in seconds.
    else
        coef =regress(thost, [ones(size(thost,1), 1), tbox ]);  % fit a line
        sd=0; % stddev. undefined with regress().
    end
    
    % Ok, got mapping equation getsecst = timing * coef(2) + coef(1);
    % Apply it to our input timestamps:
    clockratio = coef(2);
    timing = timing * clockratio + coef(1);

    % Ready.
end

% Clock sync routine: Synchronizes host clock (aka GetSecs time) to box
% internal clock via a sampling and calibration procedure:
function syncClocks(id, enableInd)
    global rtbox_info;
    global blocking;
    global enableCode;
    global rtbox_oldstylesync;
    global rtbox_maxDuration;
    global rtbox_optMinwinThreshold;
    global rtbox_maxMinwinThreshold;
    
    % Query level of verbosity to use:
    verbosity = rtbox_info(id).verbosity;
    
    % Perform multiple measurement trials per syncClocks run, take the best
    % one for final result. We use the "best" one because we have a good
    % criterion to find the best one.
    
    % We preallocate the sampling arrays for 250 samples at most. The
    % arrays will grow if 250 samples are not sufficient, at a small
    % performance penalty:
    ntrials = 250;
    
    % Any event reporting active?
    if any(rtbox_info(id).enabled)
        % Disable all events on box. This will also clear all buffers:
        enableEvent(id, 'a');
    else
        % Clear buffers "manually":
        purgeRTbox(id);
    end

    % Switch to realtime priority if not already there:
    oldPriority=Priority;
    if oldPriority < MaxPriority('GetSecs')
        Priority(MaxPriority('GetSecs'));
    end
    
    % Get porthandle:
    s = rtbox_info(id).handle;
    t = zeros(3,ntrials);

    minwin = inf;
    tdeadline = GetSecs + rtbox_maxDuration;
    ic = 0;
    
    % Perform measurement trials until success criterion satisfied:
    % Either a sample with a maximum error 'minwin' less than desired
    % threshold, or maximum allowable calibration time reached:
    while (minwin > rtbox_optMinwinThreshold) && (GetSecs < tdeadline)
        % Wait some random fraction of a millisecond. This will desync us
        % from the USB duty cycle and increase the chance of getting a very
        % small time window between scheduling, execution and acknowledge
        % of the send operation:
        t0=WaitSecs(rand / 1000);
        
        % Take pre-Write timestamp in tpre - Sync command not emitted
        % before that time. Write sync command, wait 'blocking' for write
        % completion, store completion time in post-write timestamp tpost:
        [nw tpost, errmsg, tpre] = IOPort('Write', s, 'Y', blocking);

        % tel = 1000 * (tpre - t0)
        
        % We know that sync command emission has happened at some time
        % after tpre and before tpost. This by design of the USB
        % standard, host controllers and operating system USB stack. This
        % is the only thing we can take for granted wrt. timing, so the
        % "time window" between those two timestamps is our window of
        % uncertainty about the real host time when sync started. However,
        % on a well working system without massive system overload one can
        % be reasonably confident that the real emission of the sync
        % command happened no more than 1 msec before tpost. That is a
        % soft constraint however - useful for computing the final estimate
        % for hosttime, but nothing to be taken 100% for granted.
        
        % Write op successfull?
        if nw==1
            % Retrieve 7-Byte packet with timestamp from box:
            [b7 dummy errmsg] = IOPort('Read', s, 1, 7);
        else
            % Send op failed!
            fprintf('PsychRTBox: Warning! Sync token send operation to box failed! [%s]\n', errmsg);
            continue;
        end
        
        if length(b7)~=7 || b7(1)~=89
            % Receive op failed!
            fprintf('PsychRTBox: Warning! Corrupt data received from box! [%s]\n', errmsg);
            continue;
        end
        
        % Decode boxtime into seconds (uncorrected for clock-drift etc.):
        tbox = bytes2secs(b7(2:7));
        
        % Compute confidence interval for this sample:
        % For each measurement, the time window tpost - tpre defines a
        % worst case confidence interval for the "real" host system time
        % when the sync command was emitted.
        confidencewindow = tpost - tpre;
        
        % If the confidencewindow is greater than the maximum acceptable
        % window, then we reject this sample, else we accept it:
        if confidencewindow <= rtbox_maxMinwinThreshold
            % Within acceptable range. Accept this sample and check if it
            % is the best wrt. to window size so far:
            if confidencewindow < minwin
               % Best confidencewindow so far. Update minwin, as this is one
               % of the abortion critera:
               minwin = confidencewindow;
            end
            
            % Increase sample index to permanently accept this sample for
            % final set of competitors:
            ic = ic + 1;
            
            % Assign values:
            t(1,ic) = tpre;
            t(2,ic) = tpost;
            t(3,ic) = tbox;
        else
            % Inacceptably large error confidencewindow. Reject this sample:
            continue;
        end

        % Next sample pass:
    end

    % Done with sampling: We have up to 'ic' valid samples, unless minwin
    % is still == inf.
    
    % Restore priority to state pre syncClocks:
    if Priority ~= oldPriority
        Priority(oldPriority);
    end
    
    % Restore event reporting:
    enableEvent(id, enableCode(find(rtbox_info(id).enabled(enableInd)>0))); %#ok<FNDSB>
    
    % At least one sample with acceptable precision acquired?
    if (minwin > rtbox_maxMinwinThreshold) | (ic < 1) %#ok<OR2>
        % No, not even a single one! Set all results to "invalid"
        rtbox_info(id).sync=[inf, inf, inf];
        
        if verbosity > 1
            fprintf('PsychRTBox: Warning: On Box "%s", Clock sync failed due to confidence interval of best sample %f secs > allowable maximum %f secs.\n', rtbox_info(id).ID, minwin, rtbox_maxMinwinThreshold);
            fprintf('PsychRTBox: Warning: Likely your system is massively overloaded or misconfigured!\n');
        end
        
        % Warn user:
        RTboxWarn('poorSync', minwin);
        
        % That's it:
        return;
    end

    % Ok, we have 'ic' > 0 samples with acceptable precision, according to
    % user specified constraints. Prune result array to valid samples 1 to ic:
    t = t(:, 1:ic);

    % We have two different methods for final sample selection. Each has
    % its favorable cases, but none of them is better by a large margin.
    % Both can't guarantee accurate results on highly overloaded systems,
    % but only provide the best under given circumstances.
    %
    % Under low-load conditions, the new style method may be slightly more
    % accurate under some conditions, but the difference is usually in the
    % sub-millisecond range, so either choice is ok. The options are mostly
    % here for benchmarking and stress testing of the driver...
    if rtbox_oldstylesync
        % Old style method:
        
        % Select sample with smallest confidence interval [tpre; tpost]:
        [mintdiff, idx] = min(t(2,:) - t(1,:));

        % Host time corresponds to midpoint of the confidence interval,
        % assuming a uniform distribution of likelyhood of the true write
        % time in the interval [tpre ; tpost]:
        hosttime = (t(1,idx) + t(2,idx)) / 2;
    else
        % New style method:
        
        % Choose the most accurate sample from the set of candidates. This is
        % the sample with the smallest difference between the postwrite
        % timestamp and the associated box timestamp, ie., with the smallest
        % offset between postwrite host clock time and box clock time at
        % receive of sync command. The reasoning behind this goes like this:
        %
        % 1) The time offset between host clock and box clock is a constant -
        % at least within a significant multi-second time interval between
        % successive syncClocks calls (due to only small clock drift), but
        % certainly within a syncClocks run of a few hundred milliseconds
        % (error due to clock drift in this interval is negligible).
        %
        % 2) Computed box clock time t(3,:) is "close to perfect", as this
        % timestamp is taken by box microprocessor and firmware with a very
        % small and basically constant delay after sync token receive, ie.,
        % write completion. (Maximum theoretical error is smaller than 0.1 msecs).
        %
        % 3) The correct and optimal clock offset between host and box would be
        % tdiff = tsend - t(3,:) iff tsend would be host time at true write
        % completion.
        %
        % 4) The measured host time at write completion t(2,:) is always later
        % (and therefore numerically greater) than the true host time tsend at
        % write completion due to an unknown, random, greater than zero delay
        % tdelta, i.e., t(2,:) = tsend + tdelta, tdelta > 0. tdelta is the sum
        % of:
        %
        % a) The unknown delay of up to 1 msec between USB write-URB completion
        % by the USB host controller (which would be the real completion time
        % tsend) and detection of completion due to USB IOC (Interrupt-On-
        % Completion) due to invocation of the host controllers hardware
        % interrupt handler and host controller schedule scan and URB
        % retirement inside the interrupt handler.
        %
        % b) Random (and theoretically unbounded) scheduling delay / execution
        % delay between status update of the serial port data structures by the
        % interrupt handler and detection of write completion + timestamping by
        % the IOPort driver in polling mode, or scheduling delay between
        % wakeup-operation caused by the interrupt handler and start of
        % execution of the timestamping in the IOPort driver in blocking mode.
        %
        % The syncClocks error is therefore directly proportional to the size
        % of tdelta. Therefore:
        %
        % tdiff(:) = t(2,:) - t(3,:) by definition of clock offset host vs. box.
        % t(2,:) = tsend(:) + tdelta(:) by unknown scheduling/execution noise tdelta.
        %
        % It follows that by above definitions:
        %
        % tdiff(:) = tsend(:) - t(3,:) + tdelta(:);
        %
        % --> As we defined tsend(:) to be the unknown, but perfect and
        % noise-free, true send timestamp, and t(3,:) to be the perfect receive
        % timestamp by the box, it follows that by selecting the sample 'idx'
        % with the minimal tdiff(idx) from the set tdiff(:), we will select the
        % sample with the unknown, but minimal tdelta(idx). As tdelta accounts
        % for all the remaining calibration error, minimizing tdelta will
        % maximize the accuracy of the clock sync.
        %
        % ==> Select sample with minimum t(2,:) - t(3,:) as final best result:
        [mintdiff, idx] = min(t(2,:) - t(3,:));

        % mintdiff is our best estimate of clock offset host vs. box, and
        % t(:,idx) is the associated best sample. Unfortunately there isn't any
        % way to compute the exact residual calibration error tdelta(idx). The
        % only thing we know is that the error is bounded by the length of the
        % associated 'minwin' confidence interval of this sample, so we will
        % return 'minwin' as an upper bound on the calibration error. As
        % 'minwin' was used as a threshold in the sample loop for outlier
        % rejection, we can be certain that our estimate carries no greater
        % error than 'rtbox_maxMinwinThreshold'.

        % Extract all relevant values for the final sample:

        % Host time corresponds to tpost write timestamp, which should be as
        % close as possible to real host send timestamp:
        hosttime = t(2,idx);
    end
    
    % Box timers time taken "as is":
    boxtime  = t(3,idx);
    
    % Recalculate upper bound on worst case error 'minwin' from this best
    % samples tpost - tpre:
    minwin = t(2,idx) - t(1,idx);

    if verbosity > 3
        fprintf('PsychRTBox: ClockSync(%i): Box "%s": Got %i valid samples, maxconfidence interval = %f msecs, winner interval %f msecs.\n', rtbox_oldstylesync, rtbox_info(id).ID, ic, 1000 * rtbox_maxMinwinThreshold, 1000 * minwin);
        fprintf('PsychRTBox: Confidence windows in interval [%f - %f] msecs. Range of clock offset variation: %f msecs.\n', 1000 * min(t(2,:)-t(1,:)), 1000 * max(t(2,:)-t(1,:)), 1000 * range(t(2,:) - t(3,:)));
    end
    
    % Assign (host,box,confidence) sample to sync struct:
    rtbox_info(id).sync=[hosttime, boxtime, minwin];
    
    % Also store the sample in the syncSamples history:
    rtbox_info(id).syncSamples(end+1, :) = [hosttime, boxtime, minwin];
end

% Send enable/disable string to tell box to enable or disable detection and
% reporting of certain events. Wait for acknowledge from box that the
% requested event enable/disable was carried out properly. Certain
% characters encode certain types of events. A capital character enables
% that event type, a minor character disables that event type. The special
% letter 'A' or 'a' enables or disables all events. See the init routine
% for a mapping table of button/TTLtrigger,lightTrigger,serialTrigger
% events to control characters.
%
% It is useful to disable event detection/reporting both to filter out
% unwanted events for a given experiment, but also during clock calibration
% and clockSync to prevent unwanted events from messing up the
% communication protocol between the box and the driver and from
% introducing unwanted processing delays during this time critical
% procedure.
function enableEvent(handle, str)
    global rtbox_info;

    s = rtbox_info(handle).handle;
    for ie=1:length(str)
        % Try 4 times in case of failure:
        for ir=1:4
            % Clear buffers:
            purgeRTbox(handle);

            % Send control character for event enable/disable:
            IOPort('Write', s, str(ie));

            % Wait blocking for acknowledge from box:
            if IOPort('Read', s, 1, 1) ==str(ie)
                % Acknowledged:
                break;
            end

            if ir==4, RTboxError('notRespond'); end
        end
    end
end

% Purge send-/receive buffers from stale data:
function purgeRTbox(handle)
    global rtbox_info;

    % Get IOPort handle:
    s = rtbox_info(handle).handle;

    % Set timeout of purge to 1 second:
    tout=GetSecs+1;

    byte=IOPort('BytesAvailable', s);
    while 1
        % Sleep 100 msecs between polls:
        if WaitSecs(0.1) > tout
            RTboxError('notRespond');
        end

        byte1=IOPort('BytesAvailable', s);
        if byte1==byte
            % No
            break;
        end
        byte=byte1;
    end

    % Clear buffers again:
    IOPort('Purge', s);
end

% Convert 6-byte raw box timestamp x into seconds: 1/115200 is the time
% unit of the device clock, i.e., the device clock increments with a
% granularity of 1/115200 of a second:
function secs=bytes2secs(b6)
    secs=256.^(5:-1:0)*b6(:)/115200;
end

% Open first available (not yet opened) RT-Box if no specific 'deviceID' is
% given, otherwise try to open box with that 'deviceID'. Store info about
% opened box in slot 'handle' of the rtbox_info struct array:
function openRTBox(deviceID, handle)
    global rtbox_info;

    % Setup possible port probe list for different operating systems:
    if IsWin
        if ~isempty(strfind(deviceID, 'COM'))
            % COM port name provided: Use specified COM port:
            ports{1} = deviceID;
        else
            % Enumerate all possible ports as candidates for box:
            % Suppose RTBox not assigned to COM1 or COM2, as these are
            % usually the native serial ports, if any:
            ports=cellstr(num2str((3:256)','\\\\.\\COM%i'));
        end
        
        nPorts=length(ports);
    end

    if IsOSX || IsLinux
        if ~isempty(strfind(deviceID, '/dev/'))
            % Explicit port name provided: Use it.
            ports{1} = deviceID;
            nPorts = 1;
        else
            % Enumerate all available USB-Serial ports as candidates for box:
            if IsOSX
                candidateports=dir('/dev/cu.usbserial*');
            else
                candidateports=dir('/dev/ttyUSB*');
            end

            % Different handling for Octave, as its dir() function returns
            % results in slightly different format than Matlab:
            if ~IsOctave
                for i=1:length(candidateports)
                    ports{i} = ['/dev/', candidateports(i).name];
                end
            else
                for i=1:length(candidateports)
                    ports{i} = candidateports(i).name;
                end
            end

            nPorts = length(candidateports);
        end
        
    end

    % Any found?
    if nPorts==0
        RTboxError('noUSBserial');
    end

    % Silence IOPorts output to prevent visual clutter:
    verbo=IOPort('Verbosity', 0);

    % Prepare device detection loop:
    deviceFound=0;
    rec=struct('avail','','busy',''); % for error record only
    trycount = 0;
    
    % Retry device detection procedure up to a total of 2 times:
    % Sometimes a freshly connected RTBox has a little hickup at first try,
    % but it reliably works at the 2nd iteration:
    while (trycount < 2) & (~deviceFound)     %#ok<AND2>
        % Scan each possible port if it is the response box:
        for ic=1:nPorts
            % Device file name of port to probe:
            port = ports{ic};

            % Already opened?
            if strmatch(port, {rtbox_info(1:handle-1).portname},'exact')
                % Yes. Skip this candidate:
                continue;
            end

            % Try to open port: We open at maximum supported baud rate of 115200
            % bits, use a timeout for blocking reads of 1 second, and request
            % low-latency polling for write completion if the IOPort('Write')
            % command uses a polling method for waiting for write completion. Set
            % the "sleep time" between consecutive polls to 0.0001 seconds = 0.1
            % msecs. That is good enough for our purpose and still prevents system
            % overload on OS/X and Linux:
            [s errmsg]=IOPort('OpenSerialPort', port, 'BaudRate=115200 ReceiveTimeout=1.0 PollLatency=0.0001');

            % Worked?
            if s>=0
                % Device open succeeded. Test if it is really our box and not some
                % other serial-USB device:

                % Flush all transmit/receive queues:
                % N.B. This flushes receive and send buffers on Windows and OS/X,
                % but doesn't flush the send buffers on Linux -- not implemented at
                % the FTDI driver level as of Linux 2.6.22:
                IOPort('Purge', s);

                % Read out whatever junk maybe in the input buffer:
                IOPort('Read', s, 0);

                % Write the 'X' command code to ask box for its identity:
                IOPort('Write', s, 'X');

                % Wait blocking with 1 sec timeout (see above) for id string response from box:
                idn=char(IOPort('Read', s, 1, 21));
                
                % Expected response is a 21-Bytes string of format 'USTCRTBOX,115200,v?.?'
                % with ?.? being the major and minor firmware/box revision.
                if strfind(idn,'USTCRTBOX')
                    % Found device:
                    deviceFound=1;
                    break;
                end

                % Not our box :-(
                IOPort('Close', s);

                % Store port as existent but not used by us:
                rec.avail{end+1}=port; %#ok

            elseif isempty(strfind(errmsg,'ENOENT'))
                % Failed to open port, but not with error code ENOENT.
                
                % Open failed, but port exists. That means it is busy - used by
                % ourselves or some other process:
                rec.busy{end+1}=port; %#ok

                if isempty(strfind(errmsg,'EPERM')) & isempty(strfind(errmsg,'EBUSY')) %#ok<AND2>
                    % No permission or busy error. Something else went
                    % wrong. Tell user:
                    RTboxWarn('openError', errmsg);
                end
            end
            % Scan next candidate port:
        end
       
        % Tried trycount times to perform open operation:
        trycount = trycount + 1;
        
        % Retry, if neccessary:
    end
    
    % Done with scan. Restore normal level of debug output of IOPort, as
    % selected by external usercode or defaults:
    IOPort('Verbosity',verbo);

    % Found a suitable RTBox within number of retries?
    if ~deviceFound
        % Nope. Bail out:
        RTboxError('noDevice', rec, rtbox_info);
    end

    % Ok, found a box and opened connection. Setup its device info structure:
    
    % First the default settings...
    rtbox_info(handle)=struct('events',{{'1' '2' '3' '4' '1up' '2up' '3up' '4up' 'pulse' 'light' 'lightoff' 'serial'}},...
        'enabled',[], 'ID','','handle',[],'portname',[],'sync',[],'version',[],'clkRatio',1,'verbosity',3, 'syncSamples', []);

    % Enabled events at start:
    rtbox_info(handle).enabled=logical([1 0 0 0 0]);
    % IOPort serial port handle:
    rtbox_info(handle).handle=s;
    % DeviceID:
    rtbox_info(handle).ID=deviceID; % store handle and deviceID
    % Portname:
    rtbox_info(handle).portname=port;
    % Box version:
    rtbox_info(handle).version=char(idn(18:21));
    % Init clock-ratio to an uncalibrated 1.0:
    rtbox_info(handle).clkRatio=1;

    % Disable all event reporting, except for button presses:
    enableEvent(handle, 'aD');

    % Device open and initialized.
    fprintf('PsychRTBox: RTBox device "%s" opened on serial port device %s.\n', deviceID, port);
end

% put verbose error message here, to make main code cleaner
function RTboxError(err,varargin)
    global rtbox_info;
    global nrOpen;

    switch err
        case 'noUSBserial'
            str=' No connected USB-Serial devices found. Is your device connected and the USB-Serial driver properly installed?';
        case 'noDevice'
            p=varargin{1};
            str=sprintf(' No RTBox on available ports found or no USB-Serial port found at all, or no port with the given name exists.');

            if ~isempty(p.avail) % have available ports
                str=sprintf([' USB-Serial Port(s) available: %s, but failed to identify any of them as an RTBox device.\n' ...
                    ' Is any of them the RTBox device? If yes, try again.\n' ...
                    ' It may help to unplug, then replug the device.\n\n'],cell2str(p.avail));
            end

            if ~isempty(p.busy) % have busy ports
                str=sprintf(['%s USB-Serial Port(s) unavailable:\n%s\nProbably already in use by another program.\n' ...
                    'Is any of the given busy ports the RTBox device? If yes, try ''clear all'' to close the ports and then retry.\n'], str, cell2str(p.busy));
            end

            % Have opened RTbox?
            if nrOpen > 0
                str=sprintf('%s\nAlready opened RTBox device(s): ', str);
                for i=1:length(rtbox_info)
                    str=sprintf('%s %s at %s,\n',str, char(rtbox_info(i).ID), char(rtbox_info(i).portname));
                end
                str(end)='.';
            end
        case 'unknownCmd'
            str=sprintf(['Unknown subcommand or trigger: ''%s''.\n'...
                'The first string input must be one of the commands or events:\n%s,\n%s.'],...
                varargin{1},cell2str(varargin{2}),cell2str(varargin{3}));
        case 'invalidButtonNames'
            str=sprintf('ButtonNames requires a cellstr containing four button names.');
        case 'invalidButtonName'
            str=sprintf('Invalid button name: %s.',varargin{1});
        case 'notRespond'
            str=sprintf('Failed to communicate with the device due to communication error. Try to close and re-connect the device.');
        case 'invalidEnable'
            str=sprintf('Valid events for enable/disable: %s.',cell2str(varargin{1}));
        case 'triggerDisabled'
            str=sprintf('Trigger is not enabled. You need to enable ''%s''.',varargin{1});
        otherwise, str=err;
    end
    
    error(sprintf('PsychRTBox:%s %s', err, str)); %#ok<SPERR>
end

% Show warning message, but code will keep running
function RTboxWarn(err,varargin)
    switch err
        case 'invalidEvent'
            str=sprintf('%4i',varargin{1});
            str=sprintf('Events not recognized: %s.\nPlease do PsychRTBox(''clear'') before showing stimulus.\nGetSecs = %.1f',str,GetSecs);
        case 'noTrigger'
            str=sprintf('Trigger ''%s'' not detected. GetSecs = %.1f', varargin{1}, GetSecs);
        case 'poorSync'
            str=sprintf('Unreliable synchronization detected (variation range %.1f ms).\nGetSecs = %.1f', varargin{1}*1000,GetSecs);
        case 'allDisabled'
            str=sprintf('All event detection has been disabled for %s.', varargin{1});
        case 'openError'
            str=sprintf('Some unknown error occured while trying to open port. The IOPOrt driver says: %s', varargin{1});            
        otherwise
            str=sprintf('%s. GetSecs = %.1f',err,GetSecs);
    end
    warning(['PsychRTBox:' err], str);
end

% return str from cellstr for printing, also remove port path
function str=cell2str(Cstr)
    if isempty(Cstr)
        str='';
        return;
    end

    str='';
    for i=1:length(Cstr)
        str = [str Cstr{i} ' ']; %#ok<AGROW>
    end
end
