function varargout = CedrusResponseBox(cmd, varargin)
% CedrusResponseBox - Interface to Cedrus Response Boxes.
%
% This function provides an interface to response button boxes from Cedrus,
% specifically model RB 530,...,830 and compatible models supporting the
% XID protocol (see http://www.cedrus.com).
%
% These response boxes connect via a serial line link interface, or a USB
% interface which emulates a serial link interface. They support the XID
% protocol for communication. See http://www.cedrus.com/xid for details.
%
% This function allows to establish a connection to the box, control a few
% of its parameters and most importantly query its button state and
% associated button press timestamps.
%
% It supports multiple subcommands, which accept and return different
% arguments, as listed below.
%
% Limitations: As PTB-3 currently lacks a unified portable serial port
% driver and Matlab lacks such a driver as well, we use Matlab's
% "serial" command for communication on Windows and Linux, and
% "SerialComm" on OS/X -- therefore this function currently only works
% on Matlab, not with GNU/Octave as runtime system. Also, the Java JVM
% must be enabled, and we only support 32 bit MS-Windows and GNU/Linux,
% not 64 bit operating systems.
%
% Functionality is currently limited mostly to button queries (and RJ-45
% connector state queries), including timestamps, as well as control of
% built-in timers of the box. We don't support configuration of TTL ports
% yet, neither other settings like e.g., button debounce time. Adding such
% calls is straightforward and simple. If you need such functions, either
% add them yourself and contribute your extensions to PTB, or ask us for
% implementing the missing calls.
%
%
% Subfunctions and their meaning:
%
% Functions for device init and shutdown: Call once at beginning/end of
% your script. These are slow!
%
% handle = CedrusResponseBox('Open', port [,doCalibrate=1]);
% - Open a compatible response box which is connected to the given named
% serial 'port'. 'port'names differ accross operating systems. A typical
% port name for Windows would be 'COM2', whereas a typical port name on OS/X
% would be a simple number, starting with 1 for the first serial port in the
% system (The FindSerialPort() function might be useful for OS/X users for
% finding their ports, or for mapping port names to numbers). After the
% connection is established and some testing and initialization is done,
% returns a device 'handle', a unique identifier to use for all other
% subfunctions.
%
% If you don't specify the optional 'doCalibrate' flag, or
% leave it at its default setting of 1, a couple of lengthy (multiple
% seconds) timing calibrations and tests are performed. These allow to
% assess the delays in communication between box and Matlab. They will also
% allow to return all times of events (as detected by the box) in PTB's
% standard GetSecs() time reference system -- Timestamps of button press
% events and TTL input events can be directly compared with timestamps
% delivered by other PTB functions like GetSecs, KbCheck, KbWait,
% Screen('Flip') etc.
%
% If you set the 'doCalibrate' flag to zero, all timing calibrations will
% be skipped: Startup time is drastically reduced. However there isn't any
% simple and straightforward way of comparing timestamps or timer readings
% delivered by the box with other timestamps of PTB functions. This only
% makes sense if you use some external triggering mechanism to reset the
% built-in reaction time timer via some external TTL input trigger signals
% and want to use raw timer measurements.
%
%
% CedrusResponseBox('Close', handle);
% - Close connection to response box. The 'handle' becomes invalid after
% that command.
%
%
% CedrusResponseBox('CloseAll');
% - Close all connections to all response boxes. This is a convenience
% function for quick shutdown.
%
%
% dev = CedrusResponseBox('GetDeviceInfo', handle);
% - Return queried information about the device in a struct 'dev'. 'dev'
% contains (amongst other) the following fields:
%
% General information:
%  dev.Name = Device name string.
%  dev.VersionMajor and dev.VersionMinor = Major and Minor firmware revision.
%  dev.productId = Type of device, e.g., 'Lumina', 'VoiceKey' or 'RB response pad'.
%  dev.modelId   = Submodel of the device if the device is a RB response pad,
%                  e.g., 'RB-530', 'RB-730', 'RB-830' or 'RB-834'.
%
%  dev.port      = Portname of serial port, as passed to the open function.
%
% Diagnostic information for timing:
%  dev.roundtriptime   = Median of estimated roundtrip latency for
%                        communication with the box - in seconds.
%
%  dev.roundtripstddev = Standard deviation from mean of roundtrip latency
%  measurements in seconds. Large numbers mean that your operating system
%  has bad scheduling and that reported event timestamps may be uncertain by
%  that amount.
%
%  dev.rttresetdelay   = Duration (in seconds) of a reaction time timer reset sequence
%  Values of more than 3 msecs indicate some problems with the box itself or
%  the communication link -- Measured event times or reaction times may not
%  be trustworthy!
%
%
% Functions for use within script. These are as fast as possible:
%
% [status = ] CedrusResponseBox('FlushEvents', handle);
% - Empty/clear/flush the queue of pending events. Use this to get rid of
% any stale button press or release events before start of response
% collection in a trial. E.g., Assume you wait for a subjects keypress and
% finally receive that keypress via 'GetButtons' or 'WaitButtons'. You
% collected your response, the trial is done, but when the subject releases
% the button again, that will generate another event - a release event, in
% which you're not interested. Maybe the subject will accidentally hit the
% button as well. --> Good to clean the queue before a new trial.
%
% This function has a second use as well. It has an optional output
% argument, 'status', which will return the current status of all buttons
% (i.e. whether they are currently being pressed or not).
% Status is a 3 row by 8 column matrix: Row 1 describes the status of the
% up to eight pushbuttons of the box. Row 2 describes the status of the TTL
% lines of the RJ-45 accessory connector. Row 3 describes the status of the
% VoiceKey if any. Columns 1 to 8 of each row correspond to buttons 1-8,
% TTL lines 1-8 or inputs 1-8 of the VoiceKey.
%
% The mapping for the CB-530 for row 1 of 'status' status(1,:) is as follows:
%
% [top ??? left middle right bottom] -- the 2nd entry has no associated
% button, but it may be the scanner trigger input. The mapping on other boxes
% may be different.
%
% This is useful if you just want to know whether the subject is currently
% pressing any buttons before you proceed, but are not fussed about timing.
%
% E.g. I often find myself doing the following:
%   buttons = 1;
%   while any(buttons(1,:))
%     buttons = CedrusResponseBox('FlushEvents', mybox);
%   end
%
% ...to wait for the subject to release any buttons which might currently be down.
%
% evt = CedrusResponseBox('GetButtons', handle);
% - Return next queued button-press or button-release event from the box.
% Each time a button on the box is pressed or released, and each time the
% state of the accessory connector changes, an "event" data packet is sent
% from the box to the computer. The packet is timestamped with the time of
% the triggering event, as measured by the boxes reaction time timer.
%
% This function checks if such an event is available and returns its
% description in a 'evt' struct, if so. If no event is pending, it returns an
% empty 'evt', ie. isempty(evt) is true.
%
% 'evt' for a real fetched event is a struct with the following fields:
%
% evt.raw     = "raw" byte that describes the event. Only for debugging.
%
% evt.port    = Number of the device port on which the event occured. Push
%               buttons and scanner triggers are on port 0, the RJ-45 TTL
%               connector is on port 1, port 2 is the voice-key (if any).
%
% evt.action  = Action that triggered the event:
%               1 = Button press, 0 = Button release for pushbuttons.
%               1 = TTL line high, 0 = TTL line low for RJ-45 I/O lines.
%               1 = Voice onse, 0 = Voice offset/silence for Voicekey.
%
% evt.button  = Number of the button that was pressed or released (1 to 8)
%               or the TTL line that was going high/low. Numbers vary by
%               response box.
%
% evt.buttonID= Descriptive name string for pressed button, e.g., 'top' or
%               'left'. Please note that this mapping is only meaningful
%               for the RB-530 response box.
%
% evt.rawtime = Time of the event in secs since last reset of the reaction
%               time timer, measured in msecs resolution. This value is
%               always valid, but not directly comparable to any other
%               timestamps or time measurements within Psychtoolbox.
%
% evt.ptbtime = Time of the event in secs, measured in PTBs "GetSecs"
%               timebase. This is easier to correlate with other
%               timestamps, e.g., Screen('Flip') timestamps, but its
%               reliability hasn't been tested yet for the current
%               software release. When opening a connection to a response
%               box, we perform timing calibrations to establish the
%               mapping of time values as measured by the hardware timers
%               of your response pad to time values in PTB's reference
%               system. If you skipped that calibrations by setting the
%               optional 'doCalibrate' flag to zero at device open time,
%               then the evt.ptbtime field will not be available and you
%               have to cope with evt.rawtime values only.
%
%
% evt = CedrusResponseBox('WaitButtons', handle);
% - Queries and returns the same info as 'GetButtons', but waits for
% events. If there isn't any event available, will wait until one becomes
% available.
%
% evt = CedrusResponseBox('WaitButtonPress', handle);
% - Like WaitButtons, but will wait until the subject /presses/ a key -- the
% signal that a key has been released is not acceptable -- Button release
% events are simply discarded.
%
%
% evt = CedrusResponseBox('GetBaseTimer', handle [, nSamples=1]);
% - Query current time of base timer of the box. Returned values are in
% seconds, resolution is milliseconds. evt.basetimer is the timers time,
% corrected for serial link receive latency. evt.ptbreceivetime is a
% timestamp taken via PTB's GetSecs() at time of receive of the data.
% evt.ptbtime is the basetimers time mapped into PTB GetSecs time if such a
% mapping is possible, otherwise this field doesn't exist:
% evt.ptbreceivetime and evt.ptbtime shouldn't be significantly different
% if everything is good. Large differences indicate some timing problems
% with the connection to the box, or a timer problem - either with your
% computers timer or the hardware timer of the tox, or significant
% clock-drift between the computers timer and the boxes timer. In any case,
% reaction timer measurements and such will be problematic.
%
% Note that this automatically discards all pending events in the queue before
% performing the timer query!
%
% The optional argument 'nSamples' allows to specify if multiple samples of
% PTB timer vs. the response boxes timer should be measured. If 'nSamples'
% is set to a value greater than one, a cell array with nSamples elements
% will be returned, each corresponding to one measurement. This allows,
% e.g., to check if PTBs timer and the boxes timer drift against each
% other.
%
%
% CedrusResponseBox('ResetRTTimer', handle);
% - Reset reaction time timer of box to zero. This should not be neccessary
% if you use the evt.ptbtime timestamps for time measurements or reaction
% time measurements. If you however use uncalibrated mode and the
% evt.rawtime values directly, this function may be useful to establish a
% zero baseline for reaction time measurements. However, as the communication
% delay for sending the reset command can't be reliably measured, using
% such a software triggered timer reset may not be the most reliable way of
% resetting the timer. 
%
% Note that this automatically discards all pending
% events in the queue before performing the query!
%
%
% roundtrip = CedrusResponseBox('RoundTripTest', handle);
% - Initiate 100 trials of the roundtrip test of the box. Data is echoed
% forth and back 100 times between PTB and the box, and the latency is
% measured (in seconds, with msecs resolution). The vector of all samples
% is returned in 'roundtrip' for evaluation and debugging. The measured
% latency is also used for delay correction for the 'GetBaseTimer'
% subfunction. However, a roundtrip test is performed automatically when
% opening the response box connection, so this is rarely needed.
%
% Note that this automatically discards all pending
% events in the queue before performing the query!
%
%


% History:
%
% 03/21/08 Written. Based on example code donated by Cambridge Research Systems. (MK)
% 03/28/08 Altered by Jenny Read.
% 04/03/08 Refined and added MacOS/X support via SerialComm driver. (MK)
% 04/06/08 Improved timing code for mapping of box timers --> GetSecs time. (MK)

% Cell array of device structs. Globally available for main function and
% all subfunctions in this file, persistent across invocation:
global ptb_cedrus_devices;

% Subcommand dispatch:
if nargin < 1 || ~ischar(cmd)
    error('Must at least specify subcommand as textstring!')
end

% Following if-end blocks are roughly sorted by frequency of use. The most
% frequently used calls come first to achieve minimum dispatch
% latency in the trial loop.

if strcmpi(cmd, 'FlushEvents')
    % Flush all pending events/data:

    if nargin < 2
        error('You must provide the device "handle" for the box to flush!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});

    % Perform flush:
    CedrusStatus = FlushEvents(handle);

    if nargout>0
        varargout{1} = CedrusStatus;
    end
    return
end

% Wait until a key-pressed signal is detected:
if strcmpi(cmd, 'WaitButtonPress')
    if nargin < 2
        error('You must provide the device "handle" for the box to wait for!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});

    % Wait until the box reports that a key has been pressed (NOT released)
    keypress = 0;
    while ~keypress

        % Need at least 6 bytes. Only wait if not available:
        if BytesAvailable(handle) < 6
            % Poll at 4 msecs intervals as long as input buffer is totally empty,
            % to allow the CPU to execute other tasks.
            while BytesAvailable(handle) == 0
                % Choose 4 msecs, as PTB would not release the cpu for wait
                % times below 3 msecs (to account for MS-Windows miserable
                % process scheduler).
                WaitSecs(0.004);
            end

            % At least 1 byte available -- soon we'll have our required minimimum 6
            % bytes :-) -- Spin-Wait for the remaining few microseconds:
            while BytesAvailable(handle) < 6; end
        end

        % At least 6 bytes for one event available: Try to read them from box:
        response = ReadDev(handle, 6);
        
        % Timestamp receive completion in PTB's timeframe. Allows to get a
        % feeling on how much time elapses between keypress and data receive:
        ptbfetchtime = GetSecs;

        % Unpack this binary data into a more readable form:
        evt = ExtractKeyPressData(handle,response);        
        evt.ptbfetchtime = ptbfetchtime;
        
        keypress = evt.action;
        % This is 0 if the key was released, 1 if it was pressed down,
        % which is what we are waiting for.
    end
    % Assign evt as output argument:
    varargout{1} = evt;

    return % JCAR added
end

% Wait for at least one button event available:
if strcmpi(cmd, 'WaitButtons')
    if nargin < 2
        error('You must provide the device "handle" for the box to wait for!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});

    % Need at least 6 bytes. Only wait if not available:
    if BytesAvailable(handle) < 6
        % Poll at 4 msecs intervals as long as input buffer is totally empty,
        % to allow the CPU to execute other tasks.
        while BytesAvailable(handle) == 0
            % Choose 4 msecs, as PTB would not release the cpu for wait
            % times below 3 msecs (to account for MS-Windows miserable
            % process scheduler).
            WaitSecs(0.004);
        end

        % At least 1 byte available -- soon we'll have our required minimimum 6
        % bytes :-) -- Spin-Wait for the remaining few microseconds:
        while BytesAvailable(handle) < 6; end
    end

    % At least 6 bytes for one event available: Try to read them from box:
    response = ReadDev(handle, 6);

    % Timestamp receive completion in PTB's timeframe. Allows to get a
    % feeling on how much time elapses between keypress and data receive:
    ptbfetchtime = GetSecs;

    % Unpack this binary data into a more readable form:
    evt = ExtractKeyPressData(handle,response);
    evt.ptbfetchtime = ptbfetchtime;

    % Assign evt as output argument:
    varargout{1} = evt;

    return % JCAR added
end

% Polling Button state query: Returns immediately if no events available.
if strcmpi(cmd, 'GetButtons')
    % Button state change event query:

    if nargin < 2
        error('You must provide the device "handle" for the box to query!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});

    % XID devices send six bytes of information.
    % Check if at least 6 bytes for a full event report are available:
    if BytesAvailable(handle) < 6
        % Did not receive at least 6 bytes - No new event happened. Return an
        % empty evt result.
        evt = [];
        varargout{1} = evt;
        return;
    end

    % At least 6 bytes for one event available: Try to read them from box:
    response = ReadDev(handle, 6);

    % Timestamp receive completion in PTB's timeframe. Allows to get a
    % feeling on how much time elapses between keypress and data receive:
    ptbfetchtime = GetSecs;
    
    % Unpack this binary data into a more readable form:
    evt = ExtractKeyPressData(handle,response);
    evt.ptbfetchtime = ptbfetchtime;

    % Assign evt as output argument:
    varargout{1} = evt;

    return;
end

if strcmpi(cmd, 'RoundTripTest')
    % Initiate roundtrip-test procedure: Will receive data from device,
    % echo it back, then receive a roundtrip timestamp:

    if nargin < 2
        error('You must provide the device "handle" for the box to query!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});
        
    % Start roundtrip test and return results: They are also stored in the
    % device struct of 'handle':
    varargout{1} = RoundTripTestDev(handle);
    return;
end

if strcmpi(cmd, 'ResetRTTimer')
    % RT timer reset request:
    if nargin < 2
        error('You must provide the device "handle" for the box!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});

    % Reset reaction time timer of device and assign estimated time of reset
    % as basetime for all timing calculations:
    ResetRTT(handle);

    return;
end

if strcmpi(cmd, 'GetBaseTimer')
    % Base Timer query:

    if nargin < 2
        error('You must provide the device "handle" for the box to query!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});

    if nargin >=3
        nQueries = varargin{2};
    else
        nQueries = 1;
    end
    
    % Preallocate output cell array:
    evts = cell(nQueries, 1);
    
    % Flush input buffer:
    FlushEvents(handle);

    for i=1:nQueries
        % Send basetimer query code:
        evt.roundtriptime = GetSecs;
        WriteDev(handle, 'e3');

        % Spin-Wait for first byte:
        while BytesAvailable(handle) < 1; end;

        % Timestamp receive completion of first byte. This is closest to the
        % real time when the transmitted timer values was actually generated on
        % the device:
        evt.ptbreceivetime = GetSecs;

        % Receive packet, then parse into raw timer value (in seconds):
        evt.basetimer = receiveAndParseTimePacket(handle);

        % Store roundtrip-time of query:
        evt.roundtriptime = evt.ptbreceivetime - evt.roundtriptime;
        
        % Correct reported time value of basetimer by half roundtrip delay
        % of serial link: We assume that transmission took half the total
        % measured roundtrip time, so we need to add that delay to the
        % basetimer value to get an estimate of the "real" basetimer time
        % at time of response packet receive "ptbtime":
        evt.basetimer = evt.basetimer + ptb_cedrus_devices{handle}.roundtriptime/2;

        % Assign mapped PTB GetSecs time if mapping possible:
        if ptb_cedrus_devices{handle}.baseToPtbSlope ~= 0
            % Simple linear equation mapping:
            evt.ptbtime = ptb_cedrus_devices{handle}.baseToPtbOffset + ptb_cedrus_devices{handle}.baseToPtbSlope * basetime;
        end
        
        % Assign i'th measurement event:
        evts{i} = evt;
    end
    
    % Assign evts as output argument:
    if nQueries > 1
        varargout{1} = evts;
    else
        varargout{1} = evt;
    end
    
    return;
end

if strcmpi(cmd, 'GetDeviceInfo')
    % Query info about device:

    if nargin < 2
        error('You must provide the device "handle" for the box to query!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});

    dev = ptb_cedrus_devices{handle};
    
    varargout{1} = dev;

    return; % JCAR added
end

if strcmpi(cmd, 'Open')

    % Open a new connection to response box:

    if nargin < 2
        error('You must provide the "port" parameter for the serial port to which the box is connected!')
    end

    if IsOctave
        error('Sorry, GNU Octave are not yet supported by this driver.');
    end

    % Create serial object for provided port, configure connection
    % properly:
    port = varargin{1};

    if nargin < 3
        % Assume user wants time calibration:
        doCalibrate = 1;
    else
        doCalibrate = varargin{2};
    end
    
    % Open device link, return 'dev' struct:
    dev = OpenDev(port);
    
    % Create new entry in our struct array:
    if isempty(ptb_cedrus_devices)
        ptb_cedrus_devices = cell(1,1);
    else
        ptb_cedrus_devices(end+1) = cell(1,1);
    end

    % Get a handle to it:
    handle = length(ptb_cedrus_devices);

    % Assign device struct to array:
    ptb_cedrus_devices{handle} = dev;
    clear dev;

    % This is for keeping track of what buttons are currently up or
    % down. I assume that all buttons are up when the device is opened.
    ptb_cedrus_devices{handle}.CedrusStatus = zeros(3,8);
    
    % Put this in a try-catch loop so that if it doesn't work for any
    % reason, I can then close the link and you can try again. Otherwise,
    % the COM port is permanently busy and I have to restart Matlab.
%    try
        % Set the device protocol to XID mode
        WriteDev(handle, 'c10'); %JCAR removed cr

        % % Debug information from http://www.cedrus.com/xid/properties.htm
        % %
        % % Note: 0 is ASCII value 48, 1 is ASCII value 49, and so forth.
        %
        % fprintf(s1,['_d3',char(13)]);
        % fread(s1,1)
        % fprintf(s1,['_d1',char(13)]);
        % fscanf(s1)

        % Query a few device properties:

        % Get product ID: 0 = Lumina, 1 = VoiceKey, 2 = RB response pad:

        % I have to put this in a while loop, because sometimes '_d2' fails to
        % evoke a response:
        bytes = 0;
        while bytes==0
            WriteDev(handle, '_d2');
            WaitSecs(0.25); % I also have to wait, because even when it does evoke a response,
            % there can be a long delay - tens of milliseconds. Jon Peirce confirms
            % this.
            bytes = BytesAvailable(handle);
        end

        response=ReadDev(handle, bytes);
        switch response
            case 48
                ptb_cedrus_devices{handle}.productID = 'Lumina';
            case 49
                ptb_cedrus_devices{handle}.productID = 'VoiceKey';
            case 50
                ptb_cedrus_devices{handle}.productID = 'RB response pad';
            otherwise
                ptb_cedrus_devices{handle}.productID = 'Unknown';
        end

        % Get model ID: 0 = Unknown, 1 = RB-530, 2 = RB-730, 3 = RB-830, 4 = RB-834
        bytes = BytesAvailable(handle);
        % I have to put this in a while loop, because sometimes '_d3' fails to
        % evoke a response:
        while bytes==0
            WriteDev(handle, '_d3');
            WaitSecs(0.1); % I also have to wait, because even when it does evoke a response,
            % there can be a long delay - tens of milliseconds. Jon Peirce confirms
            % same behaviour on his system.
            bytes = BytesAvailable(handle);
        end
        
        response=ReadDev(handle, bytes);
        
        if response==48
            ptb_cedrus_devices{handle}.modelID = 'Unknown';
        else if strcmp(ptb_cedrus_devices{handle}.productID,'RB response pad')
                switch response
                    case 49
                        ptb_cedrus_devices{handle}.modelID = 'RB-530';
                    case 50
                        ptb_cedrus_devices{handle}.modelID = 'RB-730';
                    case 51
                        ptb_cedrus_devices{handle}.modelID = 'RB-830';
                    case 52
                        ptb_cedrus_devices{handle}.modelID = 'RB-834';
                    otherwise
                        ptb_cedrus_devices{handle}.modelID = sprintf('Unknown id %i', response);

                end
            else
                ptb_cedrus_devices{handle}.modelID = 'Unknown';
            end
        end

        % Firmware revision:
        bytes = BytesAvailable(handle);
        while bytes==0
            WriteDev(handle, '_d4');
            WaitSecs(0.1);
            bytes = BytesAvailable(handle);
        end
        ptb_cedrus_devices{handle}.VersionMajor = ReadDev(handle, bytes) - 48;

        bytes = BytesAvailable(handle);
        while bytes==0
            WriteDev(handle, '_d5');
            WaitSecs(0.1);
            bytes = BytesAvailable(handle);
        end
        ptb_cedrus_devices{handle}.VersionMinor = ReadDev(handle, bytes) - 48;

        % Product name string:
        bytes = BytesAvailable(handle);
        while bytes==0
            WriteDev(handle, '_d1');
            WaitSecs(0.1);
            bytes = BytesAvailable(handle);
        end
        ptb_cedrus_devices{handle}.Name = char(ReadDev(handle, bytes));

        % Reset base timer:
        WriteDev(handle, 'e1');
      
        % Calibration of PTB's timebase vs. Boxes timebase wanted?
        if doCalibrate
            % Yep. First perform estimation of roundtrip delay time of
            % serial link, so we know for what transmission latency to
            % account for:

            RoundTripTestDev(handle);
            % From here on, all basetimer readouts will be corrected for
            % transmit latency.

            % Now perform calibration of mapping between box time
            % values of box timer(s) and PTB's GetSecs timebase:
            calibrateBaseTimer(handle);
        else
            % Uncalibrated mode requested. Saves a few seconds of startup
            % time, but doesn't allow mapping of boxes time measurements
            % into GetSecs() timebase of PTB:
            
            % No link roundtrip time estimates available:
            ptb_cedrus_devices{handle}.roundtriptime = 0;
            ptb_cedrus_devices{handle}.roundtripstddev = 0;

            % No mapping of box time to PTB time available:
            ptb_cedrus_devices{handle}.baseToPtbSlope  = 0;
            ptb_cedrus_devices{handle}.baseToPtbOffset = 0;
        end
        
        % Reset reaction time timer of device: If calibration was
        % requested, this will also estimate the offset between RTT values
        % and basetimer values, which is needed for later mapping of RTT to
        % GetSecs time. In uncalibrated mode, this will just send out the
        % reset code.
        ResetRTT(handle);

        % Return handle:
        varargout{1} = handle;
%    catch
        % Close serial control link:
%        CloseDev(handle);
%    end
    return;
end

if strcmpi(cmd, 'Close')
    % Close device:
    if nargin < 2
        error('You must provide the device "handle" for the box to close!');
    end

    % Retrieve handle and check if valid:
    handle = checkHandle(varargin{1});

    CloseDev(handle);
    
    return;
end

if strcmpi(cmd, 'CloseAll')
    % Close all open devices:

    if exist('ptb_cedrus_devices', 'var') 
        for handle = 1:length(ptb_cedrus_devices)
            if ~isempty(ptb_cedrus_devices{handle})
                CloseDev(handle);
            end
        end
        
        % All handles closed: Release the device array itself:
        clear ptb_cedrus_devices;
    end
    
    return;
end

error('Invalid subcommand given. Read the help.');
% ---- End of main routine ----


% ---- Start of internal helper functions ----

function ResetRTT(handle)
% Try to reset the reaction time timer to zero within a small time
% window, so we can associate "time zero" of the RT timer with the
% current GetSecs() time. This way, the RT timer will encode elapsed
% time since that GetSecs basetime. We can then later on compute the
% time of a keypress simply as sum of the GetSecs baseline time and the
% reported event time (== value of RT timer at time of button press).
% Retry up to 100 times if reset doesn't occur within 10ms.

global ptb_cedrus_devices;

% Calibrated reset?
if ptb_cedrus_devices{handle}.baseToPtbSlope ~= 0
    % Calibrated reset:
    
    % Flush input buffer:
    WaitSecs(0.1);
    FlushEvents(handle);
    WaitSecs(0.1);
    
    % Retry timer-reset up to 100 times if needed:
    for i=1:1

        % Send basetimer query code:
        dx1=GetSecs;
        WriteDev(handle, 'e3');

        % Send reaction time timer reset code:
        WriteDev(handle, 'e5');

        % Send second basetimer query code:
        WriteDev(handle, 'e3');
        dx2=GetSecs;

        % Receive both basetimer packets:
        t1 = receiveAndParseTimePacket(handle);
        t2 = receiveAndParseTimePacket(handle);

        % Ok, the RTT reset command was "sandwiched" inbetween two basetimer
        % queries, so RT reset must have happened inbetween processing of those
        % two queries. The timestamps of the basetimer values of those two
        % queries give us some time window in which RT reset must have
        % happened. We consider this a successfull reset if the timewindow is
        % small (less than 3 msecs), so the point in time of reset is well
        % bounded/defined:
        if t2 - t1 < 0.003
            % Good enough! Abort reset-retry loop:
            break;
        end
    end

    % Any success?
    if t2-t1 >=0.003
        fprintf('RT timer reset took over 3 ms [%i repetitions, box delay %f secs]! Reported evt.ptbtime values may be unreliable!\n', i, t2-t1);
        fprintf('Send delay on computer was %f secs. btw.\n', dx2 - dx1);
    end

    WaitSecs(0.2);
    FlushEvents(handle);
    WaitSecs(0.2);
    
    % Assume that RT timer reset took place in the middle between
    % acquisition of t1 and t2 -- The best assumption we can make.
    % We can now map RTT values (as reported by timestamped response box event
    % packets) to basetimer values by simply adding the RTTimerToBasetimerOffset
    % to each RTT time value.
    ptb_cedrus_devices{handle}.RTTimerToBasetimerOffset = (t1+t2)/2;
    ptb_cedrus_devices{handle}.rttresetdelay = t2 - t1;
else
    % Only uncalibrated fast reset requested:

    % Reset offset fields to invalid values:
    ptb_cedrus_devices{handle}.RTTimerToBasetimerOffset = 0;
    ptb_cedrus_devices{handle}.rttresetdelay = -1;
    
    % Send reaction time timer reset code:
    WriteDev(handle, 'e5');
end

return;

function CedrusStatus=FlushEvents(handle)
% JCAR: I modified this because I don't want to just throw away information
% about key-presses. If I do, I lose track of what the current button
% status is. So, I will read the information and check whether any of it
% is key-presses. If so, I will use this information to update the current
% status.
global ptb_cedrus_devices;

CedrusStatus = ptb_cedrus_devices{handle}.CedrusStatus;

while BytesAvailable(handle)>0
    % Read 1 byte
    response=ReadDev(handle, 1);

    % See if this is "k", indicating that a key press is following
    if char(response)=='k'
        % Seems to be an event packet: Read remaining 5 more bytes
        last5=ReadDev(handle, 5);
        response(1:6) = [response(1) last5];
        [evt,CedrusStatus] = ExtractKeyPressData(handle,response);
    else
        fprintf('CedrusResponseBox:FlushEvents: Warning invalid value %s instead of "k" received!\n', char(response));
    end
end

return;

function label = findbuttonlabel(numbr)
% The response box labels buttons by rather arbitrary numbers.
% I thought it might be helpful to have something more descriptive.
% THese descriptions assume the box is postioned with its cables/ports
% on the back edge furthest from the user.
switch (numbr - 1)
    case 1
        label = 'top';
    case 6
        label = 'bottom';
    case 3
        label = 'left';
    case 5
        label = 'right';
    case 4
        label = 'middle';
    otherwise
        label = 'unknown';
end
return;

function [evt,CedrusStatus] = ExtractKeyPressData(handle,response)
%The XID device sends six bytes of information in the following format:
%<“k”><key info><RT>:
%
% The first parameter is simply the letter "k", lower case.
%
% The second parameter consists of one byte, divided into the following
% bits
%
% Bits 0-3 store the port number. For Lumina LP-400, the push buttons and
% scanner trigger are on port 0; the RJ45 I/O lines are on port 1.
% For SV?1, voice key is on port 2 and the RJ45 is on port 1 ? there is
% no port 0. For the RB-x30 response pads, the push buttons are on port 0
% and the RJ45 port is on port 1.
%
% Bit 4 stores an action flag. If set, the button has been pressed. If
% cleared, the button has been released.
%
% Bits 5-7 indicate which push button was pressed.
%
% The reaction time consists of four bytes and is the time elapsed since
% the Reaction Time timer was last reset. See description of command "e5".
%
% Information taken from http://www.cedrus.com/xid/protocols.htm

global ptb_cedrus_devices;

if length(response)~=6
    % Did not receive 6 bytes - This should not happen!
    error('In GetButtons: Received too short (or no) response packet from box!');
end

% According to cedrus, http://www.cedrus.com/xid/protocols.htm,
% The XID device sends six bytes of information in the following
% format: <“k”><key info><RT>:
% So the first byte is 107, ie the letter “k”, lower case

% Check byte 1 for correct value 'k':
if char(response(1))~='k'
    % Failed!
    error('Received invalid event packet [Not starting with a k] from box!');
end

% Extracts byte 2 to determine which button was pushed:
evt.raw = (response(2));

% Extract different bits into meaningful fields:
% According to Cedrus, the second parameter consists of one byte, divided into the following bits:
% Bits 0-3 store the port number.
% For Lumina LP-400, the push buttons and scanner trigger are on port 0;
% the RJ45 I/O lines are on port 1. For SV?1, voice key is on port 2 and
% the RJ45 is on port 1 – there is no port 0.
% For the RB-x30 response pads, the push buttons are on port 0 and the
% RJ45 port is on port 1.

% Port id: Bits 0-3
evt.port = bitand(evt.raw, 15);
% 15 is 1111, so this extracts the rightmost 4 bits from evt.raw, ie bits 0-3

% Button state: 1 = pressed, 0 = released. Bit 4
evt.action = bitand(bitshift(evt.raw, -4), 1);

% Button id: Which button? Bits 5-7
evt.button = bitshift(evt.raw, -5) + 1;
% This chops off the rightmost 5 bits, i.e. bits 0-4, leaving only bits
% 5-7

% Map to a more descriptive label: 
evt.buttonID = findbuttonlabel(evt.button);

% Extracts bytes 3-6 and is the time elapsed in milliseconds since the
% Reaction Time timer was last reset.
%
% For more information about the use of XID timers refer to
% http://www.cedrus.com/xid/timing.htm
response = double(response);
evt.rawtime = 0.001 * (response(3)+(response(4)*256)+(response(5)*65536) +(response(6)*(65536*256)));

% Map rawtime to ptbtime if possible:
ptbTime = mapRTTimerToPTBTime(evt.rawtime, handle);

% Valid mapping? Assign if so. If mapping is impossible due to skipped
% timecalibration, we don't return the 'ptbtime' field. This way, usercode
% that relies on it without performing the mandatory calibration will die
% with a nice error message.
if ~isnan(ptbTime)
    evt.ptbtime = ptbTime;
end

% Try and keep track of which buttons are currently down and up, based on
% what bytes have been read in.
ptb_cedrus_devices{handle}.CedrusStatus(evt.port + 1, evt.button) = evt.action;

% CedrusStatus will tell you what buttons are currently up or down,
% based on the last time the device was read.
CedrusStatus = ptb_cedrus_devices{handle}.CedrusStatus;

return;

% Check if 'handle' is a valid handle into our struct array of devices,
% return it if it is valid, abort with error otherwise.
function retHandle = checkHandle(handle)
    global ptb_cedrus_devices;
    
    if handle > length(ptb_cedrus_devices) || isempty(ptb_cedrus_devices{handle})
        error('Invalid response box handle %i passed: No such response box device open!', handle);
    end
    retHandle = handle;
return;

function dev = OpenDev(port)
    
    if ~IsOctave
        % Matlab: Let's see:
        if ~IsOSX
            % Not OS/X, so Matlab supports serial() object in JVM mode:
            if ~psychusejava('desktop')
                error('You must run Matlab in JVM mode (JAVA enabled) for Cedrus response box to work!');
            end

            % Ok, Matlab with JVM on Windows or Linux: Let's do it!
            dev.link = serial(port, 'BaudRate', 115200, 'DataBits', 8, 'StopBits', 1,...
                'FlowControl', 'none', 'Parity', 'none', 'Terminator', 'CR', 'Timeout', 400,...
                'InputBufferSize', 16000);

            fopen(dev.link);
            dev.driver = 0;
        else
            % OS/X and Matlab: Use the SerialComm driver for driving the
            % serial link:
            try
                % Open 'port' with 115200 baud, no parity, 8 data bits, 1
                % stopbit.
                SerialComm('open', port, '115200,n,8,1');
%                SerialComm('open', port, '9600,n,8,1');

                % Disable handshaking 'n' == none:
                SerialComm('hshake', port, 'n');
                
                % Wait a bit...
                WaitSecs(0.5);
                
                % And flush all send- and receivebuffers:
                purgedata = SerialComm('read', port);
                
                if ~isempty(purgedata)
                    fprintf('CedrusResponseBox: Open: Purged some trash data...\n');
                end
                
                % Assign and init stuff:
                dev.link = port;
                dev.driver = 1;
                dev.recvQueue = [];
            catch
                error('Failed to open port %s on OS/X for Cedrus response box.', port);
            end
        end
    else
        % Octave: No go for now due to lack for serial port driver:
        error('Sorry, GNU/Octave support for Cedrus response box not yet implemented!');
    end

    % Assign output port:
    dev.port = port;
        
    % Ready.
return;

function CloseDev(handle)
    global ptb_cedrus_devices;

    if ptb_cedrus_devices{handle}.driver == 0
        % Matlabs serial() driver:

        % Close serial control link:
        dev = ptb_cedrus_devices{handle};

        % Close data link:
        fclose(dev.link);

        % Delete serial control link object:
        delete(dev.link);
        clear dev.link;
    else
        if ptb_cedrus_devices{handle}.driver == 1
            % OS/X + Matlab + SerialComm driver:
            SerialComm('purge', ptb_cedrus_devices{handle}.link);
            SerialComm('close', ptb_cedrus_devices{handle}.link);
        end
    end
    
    % Clear out device struct:
    ptb_cedrus_devices{handle} = [];

return;

function nrAvail = BytesAvailable(handle)
    global ptb_cedrus_devices;

    if ptb_cedrus_devices{handle}.driver == 0
        % Matlabs serial() driver:
        
        % Readout BytesAvailable subfield of device link object:
        nrAvail = ptb_cedrus_devices{handle}.link.BytesAvailable;
    else
        if ptb_cedrus_devices{handle}.driver == 1
            % OS/X + Matlab + SerialComm driver:
            
            % All reads are non-blocking and there isn't any BytesAvailable
            % command. We fetch all data that's currently available via
            % non-blocking read and attach it to our own queue, then return
            % the total number of bytes in the queue:
            data = transpose(SerialComm('read', ptb_cedrus_devices{handle}.link));
            ptb_cedrus_devices{handle}.recvQueue = [ptb_cedrus_devices{handle}.recvQueue data];
            nrAvail = length(ptb_cedrus_devices{handle}.recvQueue);
        end
    end
    
    if nrAvail > 0
        % Store timestamp when queue was not empty:
        ptb_cedrus_devices{handle}.lastTimeQueueNonEmpty = GetSecs;
    end
    
return;

function data = ReadDev(handle, nwanted)
    global ptb_cedrus_devices;

    if ptb_cedrus_devices{handle}.driver == 0
        % Matlabs serial() driver:
        
        % Read via fread 'nwanted' bytes from link. Block until we get the
        % wanted 'nwanted' bytes or until timeout / error:
        data = transpose(fread(ptb_cedrus_devices{handle}.link, nwanted));
    else
        if ptb_cedrus_devices{handle}.driver == 1
            % OS/X + Matlab + SerialComm driver:
            
            % Call BytesAvailable to trigger read-in of data from serial
            % port to our internal queue and to update the available stats,
            % until at least the 'nwanted' bytes are available, or until
            % the read operation times out after 2 seconds:
            currtime = GetSecs;
            timeout = currtime + 2;
            while (BytesAvailable(handle) < nwanted) && (currtime < timeout)
                % We are on OS/X, so waiting for 1 msec should suffice, no
                % need to wait 4 msecs as on that other deficient OS:
                currtime = WaitSecs(0.001);
            end;
            
            if currtime >= timeout
                fprintf('Timed out: nwanted = %i, got %i bytes: %s\n', nwanted, BytesAvailable(handle), char(ptb_cedrus_devices{handle}.recvQueue));
                fprintf('Read operation on response box timed out after 2 secs!\n');
                data = [];
                return;
            end
            
            % Have at least the nwanted bytes, so fetch the first nwanted
            % bytes from queue:
            data = ptb_cedrus_devices{handle}.recvQueue(1:nwanted);
            
            % Dequeue them from queue:
            if length(ptb_cedrus_devices{handle}.recvQueue) > nwanted
                % Keep tail of queue:
                ptb_cedrus_devices{handle}.recvQueue = ptb_cedrus_devices{handle}.recvQueue(nwanted+1:end);
            else
                % Nothing more in queue: Delete it.
                ptb_cedrus_devices{handle}.recvQueue = [];
            end            
        end
    end
return;

function WriteDev(handle, data)
    global ptb_cedrus_devices;

    if ptb_cedrus_devices{handle}.driver == 0
        % Matlabs serial() driver:
        
        % Write data via fwrite: We provide our own '%s' formatting string
        % to make sure that data is passed as-is, without any terminators
        % (CR or LF or CR+LF) attached. This will block until send
        % completion:
        fwrite(ptb_cedrus_devices{handle}.link, '%s', char(data));
    else
        if ptb_cedrus_devices{handle}.driver == 1
            % OS/X + Matlab + SerialComm driver:
            
            % Write data - without terminator - via SerialComm:
            SerialComm('write', ptb_cedrus_devices{handle}.link, double(data));
        end
    end
return;

function roundtrip = RoundTripTestDev(handle)
    global ptb_cedrus_devices;

    % Flush the queue:
    FlushEvents(handle);

    % Perform 100 measurement trials:
    roundtrip = zeros(1,100);
    for i=0:100
        % Wait a bit between each trial:
        WaitSecs(0.100);

        % Send 'e4' code to initiate procedure:
        WriteDev(handle, 'e4');

        % Wait for receive completion:
        while BytesAvailable(handle) < 1
        end;

        % Send echo, optimistically assuming we received a 'X':
        WriteDev(handle, 'X');

        % Get the really received byte and check:
        if char(ReadDev(handle, 1))~='X'
            error('Roundtrip test did not receive "X" char as expected!');
        end

        % Wait for receipt of timestamp:
        response = ReadDev(handle, 4);

        if response(1)~='P' || response(2)~='T'
            error('Roundtrip test did not receive "PT" marker as expected!');
        end

        response = double(response);
        
        % We throw away the first trial:
        if i > 0
            roundtrip(i) = 0.001 * (response(3) + 256 * response(4));
        end
    end

    % Store median and stddev of roundtrip time in device struct:
    ptb_cedrus_devices{handle}.roundtriptime = median(roundtrip);
    ptb_cedrus_devices{handle}.roundtripstddev = std(roundtrip);

return;

function calibrateBaseTimer(handle)
    global ptb_cedrus_devices;

    % Flush the queue:
    WaitSecs(0.1);
    FlushEvents(handle);
    WaitSecs(0.1);
    
    ptbtimes  = zeros(100,1);
    basetimes = zeros(100,1);
    
    % Perform 100 measurement trials and one warmup trial:
    for i=0:100
        % Send basetimer query code:
        WriteDev(handle, 'e3');

        % Spin-Wait for receive of first byte:
        while BytesAvailable(handle) < 1; end;

        % Timestamp receive completion of first byte. This is closest to the
        % real time when the transmitted timer values were received:
        ptbtime = GetSecs;

        % Receive packet, then parse into raw timer value (in seconds):
        basetimer = receiveAndParseTimePacket(handle);
        
        % Correct reported time value of basetimer by half roundtrip delay
        % of serial link: We assume that transmission took half the total
        % measured roundtrip time, so we need to add that delay to the
        % basetimer value to get an estimate of the "real" basetimer time
        % at time of response packet receive "ptbtime":
        basetimer = basetimer + ptb_cedrus_devices{handle}.roundtriptime/2;
        
        % Store i'th measurement: Throw away first one, just to be safe:
        if i>0
            ptbtimes(i) = ptbtime;
            basetimes(i) = basetimer;
        end
    end

    % Perform linear least squares fit on values to find linear mapping:
    coeff = polyfit(basetimes, ptbtimes, 1);

    % Store regression coefficients for later mapping of basetimer times to
    % ptb GetSecs times:
    ptb_cedrus_devices{handle}.baseToPtbSlope  = coeff(1);
    ptb_cedrus_devices{handle}.baseToPtbOffset = coeff(2);

return;

% Reads raw basetimer response packet from box, converted to seconds, but
% not corrected for receive latency etc. Query command must have been sent
% by calling code!
function rawBaseTime = receiveAndParseTimePacket(handle)

    % Read all 6 bytes of basetimer response packet from box:
    response = ReadDev(handle, 6);

    if length(response)~=6
        % Did not receive 6 bytes - This should not happen!
        error('In receiveAndParseTimePacket: Received too short (or no) response packet from box!');
    end

    % Check bytes 1:2 for correct values 'e3':
    if char(response(1))~='e' || char(response(2))~='3'
        % Failed!
        error('In receiveAndParseTimePacket: Received invalid response packet [Not starting with "e3"] from box!');
    end

    % Extracts bytes 3-6 and is the time elapsed in milliseconds since the
    % base timer was last reset.
    %
    % For more information about the use of XID timers refer to
    % http://www.cedrus.com/xid/timing.htm
    % Conver to seconds:
    response = double(response);
    rawBaseTime = 0.001 * (response(3)+(response(4)*256)+(response(5)*65536) +(response(6)*(65536*256)));

return;

function ptbTime = mapRTTimerToPTBTime(rtt, handle)
    global ptb_cedrus_devices;

    % rtt is the parsed timevalue (already mapped from msecs to seconds),
    % as received in a event packet from the box. First we convert it to
    % box-basetimer time by adding the calibrated offset between both, as
    % estimated by last calibrated RTTReset():
    basetime = rtt + ptb_cedrus_devices{handle}.RTTimerToBasetimerOffset;
    
    % Now we map the 'basetime' into PTB GetSecs time by use of the linear
    % mapping function created at init time via calibrateBaseTimer():
    if ptb_cedrus_devices{handle}.baseToPtbSlope ~= 0
        % Simple linear equation mapping:
        ptbTime = ptb_cedrus_devices{handle}.baseToPtbOffset + ptb_cedrus_devices{handle}.baseToPtbSlope * basetime;
    else
        % Not calibrated! Can't map:
        ptbTime = NaN;
    end
return;
