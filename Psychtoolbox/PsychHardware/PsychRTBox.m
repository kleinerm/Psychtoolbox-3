function varargout = PsychRTBox(varargin)
% Driver for the USTC reaction time button box (RTBox) by Xiangrui Li et al.
% PsychRTBox(varargin);
%
% PROTOTYPE, NOT READY FOR PRODUCTION USE!

% History:
% 08/01/2008 Initial implementation based on RTBox.m from Xiangrui Li (MK).
% 01/29/2009 "Close to beta" release. First checkin to SVN (MK).
% 01/30/2009 Improved syncClocks algorithm, option to spec a specific box
%            by port in the open call (MK).

    global rtbox_info;
    global nrOpen;
    global blocking;
    global eventcodes;
    global events4enable;
    global enableCode;
    global cmds;
    global esc;
    global rtbox_oldstylesync;

    % First time invocation?
    if isempty(rtbox_info)
        rtbox_info=struct('events',{{'1' '2' '3' '4' '1up' '2up' '3up' '4up' 'pulse' 'light' 'lightoff' 'serial'}},...
            'enabled',[], 'ID','','handle',[],'portname',[],'sync',[],'version',[],'clkRatio',1,'verbosity',3);
        eventcodes=[49:2:55 50:2:56 97 48 57 89]; % code for 12 events
        cmds={'close' 'closeall' 'clear' 'purge' 'start' 'test' 'buttondown' 'buttonnames' 'enable' 'disable' 'clockratio' 'syncclocks' ...
              'box2getsecs' 'box2secs' 'boxinfo' 'getcurrentboxtime','verbosity' };
        events4enable={'press' 'release' 'pulse' 'light' 'lightoff' 'all'};
        enableCode='DUPOFA'; % char to enable above events, lower case to disable
        %    rtbox_info(1).events = ;
        eval('GetSecs;KbCheck;WaitSecs(0.001);'); % initialize timing functions

        if IsWin || IsOSX
            % A blocking wait gives very good results on OS/X. On Windows
            % it gives the same results as a polling wait (setting 2), so
            % we prefer blocking for lower cpu load at same quality.
            blocking = 1;
        else
            % On Linux, a polling wait is of advantage, so we use that:
            % TODO: Double-check if this is really true.
            blocking = 2;
        end

        nrOpen = 0;

        KbName('UnifyKeyNames');
        esc = KbName('ESCAPE');
        
        rtbox_oldstylesync = 0;
    end

    if nargin < 1
        error('You must provide a command string to PsychRTBox!');
    end

    % Subcommand dispatch:
    cmd = lower(varargin{1});
    if isempty(cmd)
        error('You must provide a non-empty command string to PsychRTBox!');
    end
    
    % Open the connection to device, do initial setup and sync:
    if strcmp(cmd, 'open')
        % Assign deviceID identifier of device to open, or the default
        % 'Device1' if none specified:
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
    % default to the first open device:
    if isempty(id)
        id = 1;
    end
    
    if ~isscalar(id) || id < 1 || id > length(rtbox_info) || id > nrOpen
        error('Invalid device handle specified! Did you open the device already?');
    end

    if isempty(rtbox_info(id).handle)
        error('Invalid device handle specified! Maybe you closed this device already?');
    end

    % Build additional cell array of valid read commands:
    read=rtbox_info(id).events;
    read{end+1}='secs';
    read{end+1}='boxsecs';
    read{end+1}='getsecs';

    % Assign serial port handle:
    s = rtbox_info(id).handle;

    if ~any(strmatch(cmd,[cmds read],'exact')) % if invalid cmd, we won't open device
        if ~isempty(id)
            RTboxError('unknownCmd',cmd,cmds,rtbox_info(id).events); % invalid command
        else
            RTboxError('unknownCmd',cmd,cmds,{''}); % invalid command
        end
    end

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
            [nw tWritten]=IOPort('write',s,'Y'); % blocking write
            if nargout, varargout{1}=tWritten; end
            
        case read % 12 triggers, plus 'secs' 'boxsecs'
            cmdInd=strmatch(cmd,read,'exact'); % which command
            minbytes=7; % 1 event
            if cmdInd<13 % relative to trigger
                ind=[cmdInd<5 (cmdInd<9 && cmdInd>4) cmdInd==9:11];
                if ~rtbox_info(id).enabled(ind), RTboxError('triggerDisabled',events4enable{ind}); end
                minbytes=14; % at least 2 events
            end
            varargout={[] ''}; % return empty if no event detected
            isreading=false;
            byte=IOPort('bytesAvailable',s);
            [down tnow]=KbCheck; % return tnow
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
                timing = box2GetSecsTime(id, timing);
            elseif cmdInd==13 % secs: convert into computer time
                % Convert into computer time: Xiangrui-Style
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
            varargout={timing event};

        case 'boxinfo'
            varargout{1} = rtbox_info(id);
            
            if nIn > 1
                rtbox_info(id) = in2;
            end
            
        case 'box2getsecs'
            if nIn<2
                error('You must provide the boxtime to map!');
            end
            
            varargout{1} = box2GetSecsTime(id, varargin{3});
            
        case 'box2secs'
            if nIn<2
                error('You must provide the boxtime to map!');
            end
            
            varargout{1} = box2SecsTime(id, varargin{3});
            
        case 'purge'
            str=enableCode(4:5);
            enableEvent(id, str(rtbox_info(id).enabled(4:5))); % enable light if applicable
            purgeRTbox(id); % clear buffer

        case 'clear'
            if nIn<2, in2=1; end % block mode
            syncClocks(id, 1:5,in2); % clear buffer, sync clocks
            if nargout, varargout{1}=rtbox_info(id).sync; end

        case 'getcurrentboxtime'
            tmpsync = rtbox_info(id).sync;
            syncClocks(id, 1:5); % clear buffer, sync clocks
            varargout{1} = rtbox_info(id).sync;
            rtbox_info(id).sync = tmpsync;
            
        case 'syncclocks'
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
            if nIn<2, in2=30; end % default secs for clock test
            interval=1; % interval between trials
            ntrial=max(5,round(in2/interval)); % # of trials

            fprintf(' Measuring clock ratio. ESC to exit. Trials remaining:%4.f',ntrial);

            % Switch to realtime priority if not already there:
            oldPriority=Priority;
            if oldPriority < MaxPriority('GetSecs')
                Priority(MaxPriority('GetSecs'));
            end
            
            % Disable all event processing on box:
            %       enableEvent(id, 'a');

            [down tnow kcode]=KbCheck; t0=tnow;

            for i=1:ntrial
                % Update rtbox_info.sync:
                syncClocks(id, 1:3);
                t(i,:)=rtbox_info(id).sync; %#ok<AGROW>

                fprintf('\b\b\b\b%4.f',ntrial-i);

                while tnow<t0+interval*i && i<ntrial
                    [down tnow kcode]=KbCheck;
                    if kcode(esc), break; end
                    WaitSecs(0.01);
                end
                if kcode(esc), fprintf('\n User pressed ESC. Exiting'); break; end
            end

            % Restore priority
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

            %        rtbox_info(id).clkRatio = rtbox_info(id).clkRatio * (1+coef(2)); % update clock ratio
            rtbox_info(id).clkRatio = 0+ coef(2); % update clock ratio

            fprintf('\n Clock ratio (computer/box): %.7f\n',rtbox_info(id).clkRatio);
            fprintf(' Sigma (ms): %.2g\n',sd);
            
            if sd>0.5
                fprintf(2,' The ratio may not be reliable. Try longer time for ClockTest.\n');
            end

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
            
    end % end of switch.

% End of drivers "main" routine:
end


% Helper functions:
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

function syncClocks(id, enableInd)
    global rtbox_info;
    global blocking;
    global enableCode;
    global rtbox_oldstylesync;
    
    if rtbox_oldstylesync
        syncClocksOld(id, enableInd);
        return;
    end
    
    % Query level of verbosity to use:
    verbosity = rtbox_info(id).verbosity;
    
    % Perform multiple measurement trials per syncClocks run, take the best
    % one for final result. We use the "best" one because we have a good
    % criterion to find the best one.
    
    % We preallocate the sampling arrays for 250 samples at most. The
    % arrays will grow if 250 samples are not sufficient, at a small
    % performance penalty:
    ntrials = 250;
    
    % Maximum duration of a syncClocks calibration run:
    rtbox_maxDuration = 0.5;
    
    % Desired 'minwin' calibration accuracy:
    rtbox_optMinwinThreshold = 0.0002;
    
    % Maximum allowable (ie. worst) minwin for a sample:
    % On OS/X or Linux we could easily do with 2 msecs, as a 1.2 msecs
    % minwin is basically never exceeded. On MS-Windows however, 2.x
    % durations are not uncommon, so we need to slack this to 3 msecs.
    rtbox_maxMinwinThreshold = 0.003;
    
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
        WaitSecs(rand / 1000);
        
        % Take pre-Write timestamp in tpre - Sync command not emitted
        % before that time. Write sync command, wait 'blocking' for write
        % completion, store completion time in post-write timestamp tpost:
        [nw tpost, errmsg, tpre] = IOPort('Write', s, 'Y', blocking);

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
            fprintf('RTbox: Warning! Sync token send operation to box failed! [%s]\n', errmsg);
            continue;
        end
        
        if length(b7)~=7 || b7(1)~=89
            % Receive op failed!
            fprintf('RTbox: Warning! Corrupt data received from box! [%s]\n', errmsg);
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
            fprintf('PsychRTBox: Warning: On Box %s, Clock sync failed due to confidence interval of best sample %f secs > allowable maximum %f secs.\n', rtbox_info(id).ID, minwin, rtbox_maxMinwinThreshold);
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
    
    % Box timers time taken "as is":
    boxtime  = t(3,idx);
    
    % Recalculate upper bound on worst case error 'minwin' from this best
    % samples tpost - tpre:
    minwin = t(2,idx) - t(1,idx);

    if verbosity > 3
        fprintf('PsychRTBox: ClockSync: Box "%s": Got %i valid samples, maxconfidence interval = %f msecs, winner interval %f msecs.\n', rtbox_info(id).ID, ic, 1000 * rtbox_maxMinwinThreshold, 1000 * minwin);
        fprintf('PsychRTBox: Confidence windows in interval [%f - %f] msecs. Range of clock offset variation: %f msecs.\n', 1000 * min(t(2,:)-t(1,:)), 1000 * max(t(2,:)-t(1,:)), 1000 * range(t(2,:) - t(3,:)));
    end
    
    % Assign (host,box,confidence) sample to sync struct:
    rtbox_info(id).sync=[hosttime, boxtime, minwin];
end

function syncClocksOld(id, enableInd)
    global rtbox_info;
    global blocking;
    global enableCode;
    
    % Perform 25 trials per syncClocks run, take the best one for final
    % result. We use the "best" one because we have a good criterion to
    % find the best one. 25 is a tradeoff: The more samples we would use,
    % the more likely we would hit a very small "confidence window" and
    % therefore an especially accurate estimate of host<->box clock sync.
    % However, each trial takes about 20 msecs on some systems, so a large
    % number of trials would create long sync times. 25 amounts to about
    % 0.5 secs execution time, which is ok.
    ntrials = 25;
    
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
    
    % Perform ntrials trials:
    for ic=1:ntrials
        % Wait some random fraction of a millisecond. This will desync us
        % from the USB duty cycle and increase the chance of getting a very
        % small time window between scheduling, execution and acknowledge
        % of the send operation:
        WaitSecs(rand / 1000);
        
        % Take pre-Write timestamp in t(1,ic) - Sync command not emitted
        % before that time. Write sync command, wait 'blocking' for write
        % completion, store completion time in post-write timestamp t(2,ic):
        [nw t(2,ic), errmsg, t(1,ic)] = IOPort('Write', s, 'Y', blocking);

        % We know that sync command emission has happened at some time
        % after t(1,ic) and before t(2,ic). This by design of the USB
        % standard, host controllers and operating system USB stack. This
        % is the only thing we can take for granted wrt. timing, so the
        % "time window" between those two timestamps is our window of
        % uncertainty about the real host time when sync started. However,
        % on a well working system without massive system overload one can
        % be reasonably confident that the real emission of the sync
        % command happened no more than 1 msec before t(2,ic). That is a
        % soft constraint however - useful for computing the final estimate
        % for hosttime, but nothing to be taken 100% for granted.
        
        % Write op successfull?
        if nw==1
            % Retrieve 7-Byte packet with timestamp from box:
            [b7 dummy errmsg] = IOPort('Read', s, 1, 7);
        else
            % Send op failed!
            fprintf('RTbox: Warning! Sync token send operation to box failed! [%s]\n', errmsg);
            t(1,ic) = 0;
            t(2,ic) = inf;
            continue;
        end
        
        if length(b7)~=7 || b7(1)~=89
            % Receive op failed!
            fprintf('RTbox: Warning! Corrupt data received from box! [%s]\n', errmsg);
            t(1,ic) = 0;
            t(2,ic) = inf;
            continue;
        end
        
        % Decode boxtime into seconds (uncorrected for clock-drift etc.):
        t(3,ic) = bytes2secs(b7(2:7));
    end

    % Restore priority
    if Priority ~= oldPriority
        Priority(oldPriority);
    end
    
    % Restore event reporting:
    enableEvent(id, enableCode(find(rtbox_info(id).enabled(enableInd)>0))); %#ok<FNDSB>
    
    % For each measurement, the time window t(2,ic)-t(1,ic) defines kind of
    % a confidence interval for the "real" host system time when the sync
    % command was emitted. The measurement with the smallest time window is
    % the most accurate one. Find it and use it:
    confidencewindow = t(2,:) - t(1,:);
    [minwin idx] = min(confidencewindow);
    
    % On OS/X or Linux we could easily do with 2 msecs, as a 1.2 msecs
    % minwin is basically never exceeded. On MS-Windows however, 2.x
    % durations are not uncommon, so we need to slack this to 3.
    if minwin > 0.003
        RTboxWarn('poorSync', minwin)
    end
    
    % Subtract 0.5 times the length of the 'minwin' interval from the
    % postwrite timestamp t(2,idx) as best estimate for hosttime -
    % reasonable, assuming a uniform distribution of the "real" sync token
    % write time in the 'minwin' interval.    
    hosttime = t(2,idx) - (minwin/2);
    
    % Box timers time taken "as is":
    boxtime  = t(3,idx);
    
    % Assign (host,box,confidence) sample to sync struct:
    rtbox_info(id).sync=[hosttime, boxtime, minwin];
end

% function syncClocks(id, enableInd)
%     global rtbox_info;
%     global blocking;
%     global enableCode;
%     
%     if any(rtbox_info(id).enabled), enableEvent(id, 'a'); end % disable all
%     oldPriority=Priority(MaxPriority('GetSecs')); % raise priority
% 
%     s = rtbox_info(id).handle;
%     for ic=1:5
%         for ir=1:4 % unnecessary with disable
%             IOPort('purge', s);
%             tst=GetSecs;
%             [nw dt(ic)]=IOPort('write',s,'Y',blocking); %#ok block mode
%             b7=IOPort('read',s,1,7);
%             tend=GetSecs;
% %            ttotal = [ttotal , tend - tst];
%             if length(b7)==7 && b7(1)==89, break; end
%             if ir==4, RTboxError('notRespond'); end
%         end
%         boxt(ic)=bytes2secs(b7(2:7)); %#ok device time
%     end
%     Priority(oldPriority); % restore priority
%     dt=dt-boxt; % time diff between computer and device
%     [foo ic]=min(abs(dt-median(dt))); % median index
%     rtbox_info(id).sync=[dt(ic) boxt(ic)]; % for sync clocks
%     if range(dt)>0.002, RTboxWarn('poorSync',range(dt)); end
%     enableEvent(id, enableCode(find(rtbox_info(id).enabled(enableInd)>0))); % restore enable
% end


% send enable/disable str
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

% purge only when idle, prevent from leaving residual in buffer
% it would be ideal if we had IOPort('IsReceiving',s).
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

% Convert 6-byte x into secs. 1/115200 is the time unint of device clock
% clkRatio is measured by ClockTest. will be 1 if not measured.
function secs=bytes2secs(b6)
    secs=256.^(5:-1:0)*b6(:)/115200;
end

% Open first available (not yet opened) RT-Box if no specific 'deviceID' is
% given, otherwise try to open box with that 'deviceID'. Store info about
% opened box in slot 'handle' of the rtbox_info struct array:
function openRTBox(deviceID, handle)
    global rtbox_info;

    % Setup possible port list for different operating systems:
    if IsWin
        if ~isempty(strfind(deviceID, 'COM'))
            % COM port name provided: Use specified COM port:
            ports{1} = deviceID;
        else
            % Enumerate all possible ports as candidates for box:
            % Suppose user did not assign RTbox to COM1 or 2:
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
    deviceFound=0;
    rec=struct('avail','','busy',''); % for error record only
    verbo=IOPort('Verbosity', 0);

    trycount = 0;
    
    % Retry procedure up to a total of 2 times:
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
                % the FTDI driver level:
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

                % MK: TODO - What about other than EPERM EBUSY errors? They show
                % some other problem which should be told to the user.
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
    fprintf('RTBox "%s" opened at %s.\n', deviceID, port);

    % Disable all event reporting, except for button press:
    enableEvent(handle, 'aD');

    % Device open and initialized.
end

%
% put verbose error message here, to make main code cleaner
function RTboxError(err,varargin)
    global rtbox_info;
    global nrOpen;

    switch err
        case 'noUSBserial'
            str=' No USB-serial ports found. Is your device connected, or driver installed?';
        case 'noDevice'
            p=varargin{1};
            str=' No available port found.';

            if ~isempty(p.avail) % have available ports
                str=sprintf([' Port(s) available: %s, but failed to get identity.\n' ...
                    ' Is any of them the RT device? If yes, try again.\n' ...
                    ' It may help to unplug then plug the device.\n\n'],cell2str(p.avail));
            end

            if ~isempty(p.busy) % have busy ports
                str=sprintf(['%s Port(s) unavailable:\n%s\nprobably used by other program.\n' ...
                    'Is any of them the RT device? If yes, try ''clear all'' to close the port.\n'], str, cell2str(p.busy));
            end

            % Have opened RTbox?
            if nrOpen > 0
                str=sprintf('%s\nAlready opened RT device(s): ', str);
                for i=1:length(rtbox_info)
                    str=sprintf('%s %s at %s,\n',str, char(rtbox_info(i).ID), char(rtbox_info(i).portname));
                end
                str(end)='.';
            end
        case 'unknownCmd'
            str=sprintf(['Unknown command or trigger: ''%s''.\n'...
                'The first string input must be one of the commands or events:\n%s,\n%s.'],...
                varargin{1},cell2str(varargin{2}),cell2str(varargin{3}));
        case 'invalidButtonNames'
            str=sprintf('ButtonNames requires a cellstr containing four button names.');
        case 'invalidButtonName'
            str=sprintf('Invalid button name: %s.',varargin{1});
        case 'notRespond'
            str=sprintf('Failed to communicate with device. Try to close and re-connect the device.');
        case 'invalidEnable'
            str=sprintf('Valid events for enable/disable: %s.',cell2str(varargin{1}));
        case 'triggerDisabled'
            str=sprintf('Trigger is not enabled. You need to enable ''%s''.',varargin{1});
        otherwise, str=err;
    end
    error(sprintf('RTbox:%s %s', err, str)); %#ok<SPERR>
end

% Show warning message, but code will keep running
function RTboxWarn(err,varargin)
    switch err
        case 'invalidEvent'
            str=sprintf('%4i',varargin{1});
            str=sprintf('Events not recognized: %s.\nPlease do RTbox(''clear'') before showing stimulus.\nGetSecs = %.1f',str,GetSecs);
        case 'noTrigger'
            str=sprintf('Trigger ''%s'' not detected. GetSecs = %.1f', varargin{1}, GetSecs);
        case 'poorSync'
            str=sprintf('Unreliable synchronization detected (variation range %.1f ms).\nGetSecs = %.1f', varargin{1}*1000,GetSecs);
        case 'allDisabled'
            str=sprintf('All event detection has been disabled for %s.', varargin{1});
        otherwise
            str=sprintf('%s. GetSecs = %.1f',err,GetSecs);
    end
    warning(['RTbox:' err],str);
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
