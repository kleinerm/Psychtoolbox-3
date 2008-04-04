function varargout = CedrusResponseBox(cmd, varargin)
% CedrusResponseBox - Interface for Cedrus Response Boxes.
%
% This function provides an interface to response button boxes from Cedrus,
% specifically model RB 530 and compatible models supporting the XID protocol
% (see http://www.cedrus.com).
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
% "serial" command for communication -- therefore this function only works
% on Matlab with Java JVM enabled, and only on 32 bit MS-Windows and GNU/Linux,
% not on 64 bit operating systems, not on MacOS/X and not with GNU/Octave
% as runtime system.
%
%
% Subfunctions and their meaning:
%
% Functions for device init and shutdown: Call once at beginning/end of
% your script. These are slow!
%
% handle = CedrusResponseBox('Open', port);
% - Open a compatible response box which is connected to the given named
% serial 'port'. 'port'names differ accross operating systems. After the
% connection is established and some testing and initialization is done,
% returns a device 'handle', a unique identifier to use for all other
% subfunctions.
%
%
% CedrusResponseBox('Close', handle);
% - Close connection to response box. The 'handle' becomes invalid after
% that command.
%
%
% dev = CedrusResponseBox('GetDeviceInfo', handle);
% - Return queried information about the device in a struct 'dev'. 'dev'
% contains the following fields:
%
% dev.Name = Device name string.
% dev.VersionMajor and dev.VersionMinor = Major and minor firmware
% revision.
% dev.productId = Type of device.
% dev.modelId = Submodel of the device.
%
%
% Functions for use within script. These are as fast as possible:
%
%
% evt = CedrusResponseBox('GetButtons', handle);
% - Return next queued button-press or button-release event from the box.
% Each time a button on the box is pressed or released, and each time the
% state of the accessory connector changes, an "event" data packet is sent
% from the box to the computer. The packet is timestamped with the time of
% the triggering event, as measured by the boxes reaction time timer.
%
% This function checks if such an event is available and returns its
% describing 'evt' struct, if so. If no event is pending, it returns an
% empty 'evt', ie. isempty(evt) is true.
%
% 'evt' for a real fetched event is a struct with the following fields:
%
% evt.raw     = "raw" byte that describes the event. Only for debugging.
%
% evt.port    = Number of the device port on which the event occured. Push
%               buttons and scanner triggers are on port 0, the RJ-45
%               connector is on port 1, port 2 is the voice-key (if any).
%
% evt.action  = Action that triggered the event: 1 = Button press, 0 = Button release.
%
% evt.button  = Number of the button that was pressed or released (0 to 7).
%
% evt.rawtime = Time of the event in msecs since last reset of the reaction time timer.
%
% evt.ptbtime = Time of the event in secs, measured in PTBs "GetSecs"
%               timebase. This is easier to correlate with other
%               timestamps, e.g., Screen('Flip') timestamps, but its
%               reliability hasn't been tested yet for the current
%               software release.
%
%
% evt = CedrusResponseBox('WaitButtons', handle);
% - Queries and returns the same info as 'GetButtons', but waits for
% events. If there isn't any event available, will wait until one becomes
% available.
%
%
% CedrusResponseBox('FlushEvents', handle);
% - Empty/clear/flush the queue of pending events. Use this, to get rid of
% any stale button press or relese events before start of response
% collection in a trial. E.g., Assume you wait for a subjects keypress and
% finally receive that keypress via 'GetButtons' or 'WaitButtons'. You
% collected your response, the trial is done, but when the subject releases
% the button again, that will generate another event - a release event, in
% which you're not interested. Maybe the subject will accidentally hit the
% button as well. --> Good to clean the queue before a new trial.
%
% evt = CedrusResponseBox('GetBaseTimer', handle);
% - Query current time of base timer of the box. Returned values are in
% seconds, resolution is milliseconds. evt.basetimer is the timers time,
% evt.ptbtime is a timestamp taken via PTB's GetSecs() at time of receive
% of the timestamp. Note that this automatically discards all pending
% events in the queue before performing the query!
%
%
% CedrusResponseBox('ResetRTTimer', handle);
% - Reset reaction time timer of box to zero.
%
%

% History:
%
% 03/21/08 Written. Based on example code donated by Cambridge Research Systems. (MK)

% Cell array of device structs:
persistent devices;

% Subcommand dispatch:
if nargin < 1 || ~ischar(cmd)
    error('Must at least specify subcommand as textstring!')
end

% Following if-end blocks are roughly sorted by frequency of use. The most
% frequently used calls come first to achieve minimum dispatch
% latency in the trial loop.

% Wait for at least one button event available:
if strcmpi(cmd, 'WaitButtons')
    if nargin < 2
        error('You must provide the device "handle" for the box to wait for!');
    end

    % Map handle to device:
    handle = varargin{1};    
    if handle > length(devices) || isempty(devices{handle})
        error('Invalid handle - No such device open!');
    end
    
    dev = devices{handle};

    % Need at least 6 bytes. Only wait if not available:
    if dev.link.BytesAvailable < 6
        % Poll at 3 msecs intervals as long as input buffer is totally empty,
        % to allow the CPU to execute other tasks.
        while dev.link.BytesAvailable == 0
            % Choose 4 msecs, as PTB would not release the cpu for wait
            % times below 3 msecs (to account for MS-Windows miserable
            % process scheduler).
            WaitSecs(0.004);
        end

        % At least 1 byte available -- soon we'll have our required minimimum 6
        % bytes :-) -- Spin-Wait for the remaining few microseconds:
        while dev.link.BytesAvailable < 6; end
    end
    
    % Data should be ready. Rewrite cmd to a 'GetButtons' and let
    % 'GetButtons' handle the actual fetch & return below:
    cmd = 'GetButtons';
end

% Polling Button state query: Returns immediately if no events available.
if strcmpi(cmd, 'GetButtons')
    % Button state change event query:

    if nargin < 2
        error('You must provide the device "handle" for the box to query!');
    end

    % Map handle to device:
    handle = varargin{1};    
    if handle > length(devices) || isempty(devices{handle})
        error('Invalid handle - No such device open!');
    end
    
    dev = devices{handle};
    
    % XID devices send six bytes of information in the following format:
    % <"k"><key info><RT>:
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

    % Check if at least 6 bytes for a full event report are available:
    if dev.link.BytesAvailable < 6
        % Did not receive at least 6 bytes - No new event happened. Return an
        % empty evt result.
        evt = [];
        varargout{1} = evt;
        return;
    end

    % At least 6 bytes for one event available: Try to read them from box:
    response = fread(dev.link, 6);

    if length(response)~=6
        % Did not receive 6 bytes - This should not happen!
        error('In GetButtons: Received too short (or no) response packet from box!');
    end
    
    % Check byte 1 for correct value 'k':
    if char(response(1))~='k'
        % Failed!
        error('In GetButtons: Received invalid response packet [Not starting with a k] from box!');
    end
    
    % Extracts byte 2 to determine which button was pushed:
    evt.raw = (response(2));
    
    % Extract different bits into meaningful fields:
    
    % Port id: Bits 0-3
    evt.port = bitand(evt.raw, 15);
    
    % Button state: 1 = pressed, 0 = released. Bit 4
    evt.action = bitand(bitshift(evt.raw, -4), 1);
    
    % Button id: Which button? Bits 5-7
    evt.button = bitshift(evt.raw, -5);
    
    % Extracts bytes 3-6 and is the time elapsed in milliseconds since the
    % Reaction Time timer was last reset.
    %
    % For more information about the use of XID timers refer to
    % http://www.cedrus.com/xid/timing.htm
    evt.rawtime = (response(3)+(response(4)*256)+(response(5)*65536) +(response(6)*(65536*256)));
    
    % Map rawtime to ptbtime:
    evt.ptbtime = dev.basetime + evt.rawtime * 0.001;

    % Assign evt as output argument:
    varargout{1} = evt;
    
    return;
end

if strcmpi(cmd, 'FlushEvents')
    % Flush all pending events/data:

    if nargin < 2
        error('You must provide the device "handle" for the box to flush!');
    end

    % Map handle to device:
    handle = varargin{1};    
    if handle > length(devices) || isempty(devices{handle})
        error('Invalid handle - No such device open!');
    end
    
    dev = devices{handle};
    
    % Perform flush:
    FlushEvents(dev);
    
    return;
end

if strcmpi(cmd, 'ResetRTTimer')
    % RT timer reset request:
    if nargin < 2
        error('You must provide the device "handle" for the box!');
    end

    % Map handle to device:
    handle = varargin{1};    
    if handle > length(devices) || isempty(devices{handle})
        error('Invalid handle - No such device open!');
    end
    
    dev = devices{handle};

    % Reset reaction time timer of device and assign estimated time of reset
    % as basetime for all timing calculations:
    dev.basetime = ResetRTT(dev);
    
    return;
end

if strcmpi(cmd, 'GetBaseTimer')
    % Base Timer query:

    if nargin < 2
        error('You must provide the device "handle" for the box to query!');
    end

    % Map handle to device:
    handle = varargin{1};    
    if handle > length(devices) || isempty(devices{handle})
        error('Invalid handle - No such device open!');
    end
    
    dev = devices{handle};
    
    % Flush input buffer:
    FlushEvents(dev);
    
    % Send query code:
    fprintf(dev.link,['e3',char(13)]);

    % Read 6 bytes from box:
    response = fread(dev.link, 6);

    % Timestamp receive completion:
    evt.ptbtime = GetSecs;

    if length(response)~=6
        % Did not receive 6 bytes - This should not happen!
        error('In GetBaseTimer: Received too short (or no) response packet from box!');
    end
    
    % Check byte 1 for correct value 'e':
    if char(response(1))~='e' || char(response(2))~='3'
        % Failed!
        error('In GetBaseTimer: Received invalid response packet [Not starting with "e3"] from box!');
    end
        
    % Extracts bytes 3-6 and is the time elapsed in milliseconds since the
    % base timer was last reset.
    %
    % For more information about the use of XID timers refer to
    % http://www.cedrus.com/xid/timing.htm
    % Conver to seconds:
    evt.basetimer = 0.001 * (response(3)+(response(4)*256)+(response(5)*65536) +(response(6)*(65536*256)));
    
    % Assign evt as output argument:
    varargout{1} = evt;
    
    return;
end

if strcmpi(cmd, 'GetDeviceInfo')
    % Query info about device:
    
    if nargin < 2
        error('You must provide the device "handle" for the box to query!');
    end

    % Map handle to device:
    handle = varargin{1};    
    if handle > length(devices) || isempty(devices{handle})
        error('Invalid handle - No such device open!');
    end
    
    dev = devices{handle};
    varargout{1} = dev;
    
    return;
end

if strcmpi(cmd, 'Open')
    % Open a new connection to response box:
    
    if nargin < 2
        error('You must provide the "port" parameter for the serial port to which the box is connected!')
    end
    
    if IsOctave || IsOSX
        error('Sorry, the Apple-OS/X operating system and/or GNU Octave are not yet supported.');
    end
    
    % Create serial object for provided port, configure connection
    % properly:
    dev.port = varargin{1};
    dev.link = serial(dev.port, 'BaudRate', 115200, 'DataBits', 8, 'StopBits', 1, 'FlowControl', 'none', 'Parity', 'none', 'Terminator', 'CR', 'Timeout', 400, 'InputBufferSize', 16000);
    fopen(dev.link);
    
    % Set the device protocol to XID mode
    % Note: char(13) sends the Carriage Return terminator
    fprintf(dev.link,['c10',char(13)]);
    
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
    fprintf(dev.link, ['_d2',char(13)]);
    dev.productId = fread(dev.link, 1);
    
    % Get model ID: 0 = Unknown, 1 = RB-530, 2 = RB-730, 3 = RB-830, 4 = RB-834
    fprintf(dev.link, ['_d3',char(13)]);
    dev.modelId = fread(dev.link, 1);
    
    % Firmware revision:
    fprintf(dev.link, ['_d4',char(13)]);
    dev.VersionMajor = fread(dev.link, 1);
    fprintf(dev.link, ['_d5',char(13)]);
    dev.VersionMinor = fread(dev.link, 1);
    
    % Product name string:
    fprintf(dev.link,['_d1',char(13)]);
    dev.Name = fscanf(dev.link);
    
    % Reset base timer:
    fprintf(dev.link,['e1',char(13)]);
    
    % Reset reaction time timer of device and assign estimated time of reset
    % as basetime for all timing calculations:
    dev.basetime = ResetRTT(dev);
    
    % Create new entry in our struct array:
    if isempty(devices)
        devices = cell(1,1);
    else
        devices(end+1) = cell(1,1);
    end

    % Get a handle to it:
    handle = length(devices);

    % Assign device struct to array:
    devices{handle} = dev;
    
    % Return handle:
    varargout{1} = handle;
    
    return;
end

if strcmpi(cmd, 'Close')
    % Close device:
    if nargin < 2
        error('You must provide the device "handle" for the box to close!');
    end

    % Map handle to device:
    handle = varargin{1};    
    if handle > length(devices) || isempty(devices{handle})
        error('Invalid handle - No such device open!');
    end
    
    dev = devices{handle};
    
    % Clear out device struct:
    devices{handle} = [];
    
    % Close serial control link:
    fclose(dev.link);

    % Delete serial control link object:
    delete(dev.link);
    clear dev.link;
    
    return;
end

error('Invalid subcommand given. Read the help.');
% ---- End of main routine ----


% ---- Start of internal helper functions ----

function basetime = ResetRTT(dev)
    % Try to reset the reaction time timer to zero within a small time
    % window, so we can associate "time zero" of the RT timer with the
    % current GetSecs() time. This way, the RT timer will encode elapsed
    % time since that GetSecs basetime. We can then later on compute the
    % time of a keypress simply as sum of the GetSecs baseline time and the
    % reported event time (== value of RT timer at time of button press).
    % Retry up to 100 times if reset doesn't occur within 2 msecs.
    for i=1:100
        t1=GetSecs;
        fprintf(dev.link,['e5',char(13)]);
        t2=GetSecs;
        if t2 - t1 < 0.002
            break;
        end
    end

    if t2-t1 >=0.002
        fprintf('RT timer reset took over two milliseconds [%i repetitions, delay %f secs]! Reported evt.time values may be inaccurate!', i, t2-t1);
    end
    
    % Assume that RT timer reset took place in the middle between
    % acquisition of t1 and t2 -- Don't know if this is the case, but the
    % best assumption we can make.
    basetime = (t1+t2)/2;
return;

function FlushEvents(dev)
    % Read and discard all pending data from the input buffer:
    fread(dev.link, dev.link.BytesAvailable);
return;
