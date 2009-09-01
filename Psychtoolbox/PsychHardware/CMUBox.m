function varargout = CMUBox(cmd, handle, varargin)
% CMUBox - Access CMU response button box or PST serial response button box.
%
% Commands and their syntax:
% --------------------------
%
% handle = CMUBox('Open', boxtype [, portName] [, options]);
% - Open response box connected to serial port 'portName', or the first
% serial port found, if 'portName' is omitted. Initialize it, return a
% 'handle' to it. You'll have to pass 'handle' to all following functions
% to access the box.
%
% If your system has multiple devices connected to multiple serial ports
% then you should explicitely specify the 'portName', otherwise the driver
% may connect to the wrong port and choke!
%
% The optional string parameter 'options' allows to tweak the behaviour of
% the driver for certain configurations. It supports the following options:
%
% 'ftdi' - Tells the driver that it is connecting to a Serial-over-USB port
% and that the converter/driver is from FTDI Inc., or a compatible device.
% Allows for certain optimizations in timing accuracy.
%
%
% The mandatory parameter 'boxtype' is a name string defining the
% type/model of box to connnect to. Supported settings are:
%
% 'bitwhacker' - Connect to UBW32/Bitwhacker with StickOS Firmware,
% reconfigure it to impersonate a response box.
%
% 'cmu' - Connect to CMU serial port response button box, assuming 19.2
% KBaud datarate, 8 databits, 1 stop bit, odd parity.
%
% 'pst' - Connect to PST serial port response button box in E-Prime
% configuration with 800 streaming samples per second at 19200 Baud, 8
% databits, 1 stopbit, no parity. Please note this is our expectation: If
% the box is configured for something else than 19200 Baud, we will fail or
% hang. If 1600 samples/sec are configured instead of 800 sampes/sec, we
% won't care, but timing might be less accurate due to the too high USB load,
% unless we're running on a real serial port which shouldn't have a
% problem with 1600 samples/sec. Serial-over-USB however will likely choke!
% Verify that the DIP switches or jumpers inside the box are properly set
% up.
%
% 'forpserial-1' - Connect to a fORP interface unit type FIU-005 on the
% serial port, configured with a program switch setting of 1. This is
% exactly the same as a PST box running with 800 Samples/Sec streaming
% rate.
%
% CMUBox('Close', handle);
% - Close connection to response box 'handle'. The 'handle' is invalid
% thereafter.
%
%
% evt = CMUBox('GetEvent', handle [, waitForEvent=0]);
% - Retrieve next queued event received from the box in the struct 'evt'.
% If no new events are available and the optional 'waitForEvent' is set to
% 1, then the function will wait until at least one event becomes
% available, or until a timeout of 10 seconds elapses. Otherwise it will
% return an empty struct, ie., evt = [].
%
% The following subfields are available in 'evt' if 'evt' is non-empty:
%
% evt.state = 1 Byte value which encodes the new status of the response
% buttons and input lines of the box. A 1-bit means button pressed/signal
% active. A 0-bit means button released/signal inactive. See the
% documentation of your box for meaning of the single bits.
%
% evt.time  = Psychtoolbox GetSecs() timestamp of the time when the event
% was received from the box.
%
% evt.trouble = If zero, then evt is probably valid and good. If non-zero,
% then the timestamp is likely screwed and useless, as are probably all
% following timestamps!
%
%
% status = CMUBox('Status', handle);
% - Retrieve internal status of response box 'handle'.
%
%

% How to setup latency timer and eventChar under Linux for FTDI
% USB-Serial converters?
%
% Example for Ubuntu Linux 7.1 with first serial converter device ttyUSB0:
%
% su + Superuser password.
% cd /sys/bus/usb-serial/devices/ttyUSB0
%
% Set event character:
%
% echo X > event_char
% with X being 0 to disable event character, or 256 + ASCII code of
% event character, e.g., echo 256 > event_char for ASCII code 0, or
% echo 266 > event_char for ASCII code 10, because 266 = 256 + 10.
%
% Set latency timer to 1 millisecond: (Default is 16 msecs)
% echo 1 > latency_timer
%
% Query current latency timer setting:
% cat latency_timer
%
% History:
% 9.08.2009  mk  Written. Initial prototype.

% Cell array of structs for our boxes: One cell for each open box.
persistent boxes;

if nargin < 1
    help CMUBox;
    return;
end

if nargin < 2
    handle = [];
end

if strcmpi(cmd, 'GetEvent')
    if isempty(handle)
        error('CMUBox: GetEvent: No "handle" for box to close provided!');
    end

    if length(handle) ~= 1
        error('CMUBox: GetEvent: Passed argument is not a valid response box "handle"!');
    end
    
    if length(boxes) < handle
        error('CMUBox: GetEvent: Passed argument is not a valid response box "handle"!');
    end

    % Retrieve box struct:
    box = boxes{handle};
    
    if isempty(box.useBitwhacker)
        error('CMUBox: GetEvent: Passed argument is not a valid response box "handle"!');
    end
    
    
    if length(varargin) < 1
        waitEvent = [];
    else
        waitEvent = varargin{1};
    end
    
    if isempty(waitEvent)
        waitEvent = 0;
    end
    
    % Start with empty event 'evt':
    evt = [];
    
    % Reset timestamp trouble flag:
    tTrouble = 0;
    skipped = 0;
    
    % Repeat until forceful abortion via break as long as either waiting
    % for new events is requested, or - in non-blocking mode - new status
    % bytes from the box are available to parse:
    while (waitEvent > 0) | (IOPort('BytesAvailable', box.port) >= 9) %#ok<OR2>

        % Wait blocking for at least one status packet of 9 bytes from box:
        [inpkt, t, err] = IOPort('Read', box.port, 1, 9);

        % Error condition?
        if ~isempty(err)
            % Game over!
            error('CMUBox: GetEvent: I/O ERROR!! System says: %s\n', err);
        end

        % Box status byte:
        data = inpkt(1);
        
        % Serial number of byte in stream:
        serNumber = inpkt(2) * 256^0 + inpkt(3) * 256^1 + inpkt(4) * 256^2 + inpkt(5) * 256^3;
        refTime   = (serNumber * box.dt) + box.baseTime;
        
        % Time delta between consecutive stream scans in seconds:
        % Effectively the sampling interval and therefore uncertainty of
        % the time measurement: If this deviates significantly from the
        % expected per-byte transmission interval then the timestamps are
        % not to be trusted!
        box.deltaScan = (inpkt(6) * 256^0 + inpkt(7) * 256^1 + inpkt(8) * 256^2 + inpkt(9) * 256^3) / 1e6;

        % Special case for Bitwhacker emulation: Filter out codes 10 and
        % 13, they're an artifact of the emulation:
        if box.useBitwhacker & ismember(data, [10, 13]) %#ok<AND2>
            continue;
        end
        
        % Timestamps at least 0.5 msecs apart and no more than 2 msecs
        % apart? This window should be sufficient for the CMU and PST box
        % in all streaming modes:
        if (t - box.oldTime < 0.0005) | (box.deltaScan < 0.0005) | (box.deltaScan > 0.002) %#ok<OR2>
            % Too close to each other! Timestamp is not reliable!
            tTrouble = 1;
            fprintf('CMUBox: GetEvent: Timestamp trouble!! Delta %f msecs, ScanInterval %f msecs.\n', 1000 * (t - box.oldTime), 1000 * box.deltaScan); %#ok<WNTAG>
        end

        % Keep track of last events timestamp:
        box.oldTime = t;
        
        % Update last status state of box:
        oldState = box.oldState;
        box.oldState = data;

        % Update trouble counter:
        box.tTrouble = box.tTrouble + tTrouble;

        % Box status changed since last query?
        if data ~= oldState
            % Yes. We have a new event. Store it and break out of loop:
            
            % USB-Serial converter type?
            if box.ftdiusbserialtype > 0
                % Special timestamp correction for some transitions possible:
                if oldState == 0
                    % Zero to non-zero transition: As zeros are reported with 1
                    % msec internal latency, and the latency timer is reset
                    % at each reported zero, the first non-zero will be
                    % delayed by the latency of the FTDI latencytimer,
                    % i.e., with 2 msecs delay. Therefore we can subtract 2
                    % msecs to compensate for this case:
                    t = t - 0.002;
                else
                    if data == 0
                        % Non-zero to zero transition: Transmitted with
                        % only 1 msec delay due to FTDI special event
                        % character set to trigger on zeros, as requested
                        % by setting the 'Terminator' to 0. We subtract 1
                        % msecs to take this into account:
                        t = t - 0.001;
                    end
                end
            end
            
            evt.time = t;
            evt.streamTime = refTime;
            evt.state = data;
            evt.trouble = tTrouble;
            evt.deltaScan = box.deltaScan;
            break;
        end
        
        % Repeat scanloop until we find a new event or no new data
        % available from box in non-blocking mode...
        skipped = skipped + 1;
    end

    box.skipped = skipped;
    
    % Update box status:
    boxes{handle} = box;
    
    % Return evt if any:
    varargout{1} = evt;
    
    return;
end

% Open connection to box:
if strcmpi(cmd, 'Open')
    if isempty(handle)
        error('CMUBox: Open: Mandatory "boxtype" parameter missing!');
    else
        boxtype = handle;
    end

    %IOPort('Verbosity', 9);

    switch(lower(boxtype))
        case {'bitwhacker'},
            % No special options for UBW32/Bitwhacker:
            pString = '';
            box.useBitwhacker = 1;
            box.type = 1;
            fprintf('CMUBox: Using Bitwhacker/StickOS emulated box!\n');
            
        case {'cmu'},
            % BaudRate is 19.2 KiloBaud, 8-Odd-1 config without flow control:
            % This is valid for at least hardware type mark 3, and mark 4,
            % Firmware V.1.x.
            pString = 'BaudRate=19200 Parity=Odd ReceiveTimeout=10.0';
            box.useBitwhacker = 0;
            box.type = 2;
            fprintf('CMUBox: Using CMU serial response button box!\n');
            
        case {'pst'},
            % BaudRate is 19.2 KiloBaud, 8-N-1 config without flow control:
            pString = 'BaudRate=19200 ReceiveTimeout=10.0';
            box.useBitwhacker = 0;
            box.type = 3;
            fprintf('CMUBox: Using PST serial response button box!\n');
            
        case {'forpserial-1'},
            % BaudRate is 19.2 KiloBaud, 8-N-1 config without flow control:
            pString = 'BaudRate=19200 ReceiveTimeout=10.0';
            box.useBitwhacker = 0;
            box.type = 3;
            fprintf('CMUBox: Using fORP interface program 1 as serial response button box!\n');

        otherwise,
            error('CMUBox: Open: Unknown "boxtype" specified! Typo?');
    end
    
    if length(varargin) < 1
        portName = [];
    else
        portName = varargin{1};
    end
    
    if isempty(portName)
        % No portName provided: Try to auto-detect:
        portName = FindSerialPort([], 1);
    end
    
    if length(varargin) >= 2
        specialOptions = varargin{2};
    else
        specialOptions = '';
    end
    
    if ~isempty(findstr(specialOptions, 'ftdi'))
        % USB-Serial port via FTDI converter chip:
        box.ftdiusbserialtype = 1;
    else
        % Default to non-FTDI:
        box.ftdiusbserialtype = 0;
    end
    
    % Try to open connection: Allocate an input buffer of a size of
    % 1600 * 1 * 3600 = 5760000 Bytes. This is sufficient for 1 hour of
    % uninterrupted box operation without ever reading out events from the
    % queue at highest box operating speed.
    % On MS-Windows, we set the driver receivebuffer seize to 32 kB.
    %
    % The Terminator=0 setting is interesting for USB-Serial converters of
    % FTDI Inc. It will configure the converter to transmit zero bytes,
    % ie., "button resting position" immediately at each USB work-cycle
    % with at most 1 msec converter-internal latency. Other states, ie.,
    % "at least one button pressed", will be reported with the latency set
    % in the latency timer - at least 2 msecs delay. This however means
    % that a zero -> non-zero transition is known to have 2 msecs delay and
    % we can account for that, and that a non-zero -> zero transition has 1
    % msec delay and we can account for that. Net result is that we get a
    % low-latency for button transitions, as long as they are from none to
    % some and some to none.
    box.portName = portName;
    box.port = IOPort('OpenSerialPort', portName, ['InputBufferSize=5760000 HardwareBufferSizes=32768,32768 Terminator=0 ' pString]);
    
    
    % Is this a testrun with an emulated CMU/PST box by use of the
    % UBW32-Bitwhacker device with StickOS?
    if box.useBitwhacker
        % Upload StickOS Basic program to turn the BitWhacker into a CMU
        % box look-alike:

        % Upload simple program:
        % ----------------------

        % Disable input echoing from StickOS:
        Command(box.port, 'echo off');
        % Delete current program, upload a fresh one:
        Command(box.port, 'new');
        % Infinite while-loop, runs until program termination:
        Command(box.port, '5 dim t');
        Command(box.port, '10 while 1 do');
        % Print a character - Send 1 Byte to host computer:
        Command(box.port, '15 let t = seconds % 10');
        Command(box.port, '20 print t');
        % Sleep for 500 microseconds...
        %        Command(box.port, '30 sleep 1 ms');
        % Print a character - Send 1 Byte to host computer:
        %        Command(box.port, '35 let t = seconds % 10');
        %        Command(box.port, '40 print t');
        % Sleep for 500 microseconds...
        %        Command(box.port, '50 sleep 1 ms');
        % Then repeat.
        Command(box.port, '60 endwhile');

        Command(box.port, 'list');

        % Wait a second, then drain the input buffers:
        WaitSecs(1);
        while IOPort('BytesAvailable', box.port) > 0
            char(IOPort('Read', box.port))
        end
        
        % Box is programmed and ready. Start the program and thereby data
        % streaming.
        Command(box.port, 'run');
        
        % Let a second of data junk accumulate to simulate the real
        % situation where the box streams uncontrollably after power-up...
        WaitSecs(2);
        
        % Ok, this is as realistic as it gets for an emulated CMU box.
        box.useBitwhacker = 1;
        
        % Set input filter to discard CR and LF characters as well as
        % redundant data:
        IOPort('ConfigureSerialPort', box.port, 'ReadFilterFlags=3');
    else
        % Set input filter to discard redundant data and attach a total
        % streamcount tag and dT tag of 32 bit size to each read datum,
        % i.e., 2 * 32 bit = 8 bytes:
        IOPort('ConfigureSerialPort', box.port, 'ReadFilterFlags=1');
        box.useBitwhacker = 0;
    end
    
    if box.type == 3
        % PST box: Disable streaming from box and switch off all lamps:
        % Bit 7 = 128 -> Enable/Disable streaming.
        % Bit 6 =  64 -> Lower bits control lamp state.
        % Bit 5 =  32 -> Enable/Disable button queries.
        % Bit 0-4 = 1-16 = Enable/Disable Lamp 0-4.
        IOPort('Write', box.port, uint8(64));
        
        % Wait extra 250 msecs for box to calm down:
        WaitSecs(0.25);
        
        % Now we can reasonably assume that no new data will arrive from
        % box and no data is in-flight. Whatever's there should be in the
        % OS receive buffers.
    end

    if box.type == 2
        % CMU box: Disable serial port receiver to stop the data stream
        % from box at the receiving side, as this braindead box can't be
        % instructed to stop streaming by itself. This call will only work
        % on OS/X and Linux and even there only on a small subset of serial
        % port hardware :-(
        IOPort('ConfigureSerialPort', box.port, 'ReceiverEnable=0');

        % Wait extra 250 msecs for box to calm down:
        WaitSecs(0.25);
        
        % Now we can at least pray that no new data will arrive from
        % box and no data is in-flight. Whatever's there should be in the
        % OS receive buffers.
    end

    % Preheat GetSecs:
    GetSecs;

    % Purge all input and output buffers:
    IOPort('Purge', box.port);
        
    % Now try to get rid of all of it:
    while IOPort('BytesAvailable', box.port) > 0
        IOPort('Read', box.port, 0);
    end
    
    if box.type == 3 | box.type == 2 %#ok<OR2>
        % Calibrate inter-byte-interval:
        if box.type == 3
            % PST box: Enable streaming:
            IOPort('Write', box.port, uint8(128+64+32+1+4+16), 0);
        else
            % CMU box: Enable receiver:
            IOPort('ConfigureSerialPort', box.port, 'ReceiverEnable=1');
        end

        % Wait until 8000 bytes have arrived, read them and timestamp:
        when1 = GetSecs;
        while IOPort('BytesAvailable', box.port) == 0
            % Still waiting for 1st byte...
            when1 = GetSecs;
        end
        
        nscanned = 0;
        while nscanned < 8000        
            % Wait for final byte: We must not request more than 1000 Bytes
            % per request, otherwise we'll get in trouble on OS/X!
            [dummy, when2] = IOPort('Read', box.port, 1, 1000);
            nscanned = nscanned + length(dummy);
        end
        
        if box.type == 3
            % PST: Stop streaming from box:
            IOPort('Write', box.port, uint8(64+2+8));
        else
            % CMU: Disable receiver:
            IOPort('ConfigureSerialPort', box.port, 'ReceiverEnable=0');            
        end
        
        % Wait extra 250 msecs for box to calm down:
        WaitSecs(0.25);

        % Purge all input and output buffers:
        IOPort('Purge', box.port);

        % Now try to get rid of all of it:
        while IOPort('BytesAvailable', box.port) > 0
            IOPort('Read', box.port, 0);
        end
        
        % All buffer drained, no more data. Compute real byteduration of a
        % single samplebyte:
        box.dt = (when2 - when1) / nscanned;

        if box.type == 2
            % If this is Windows, or if the results seem implausible for the
            % CMU box, ie., more than 0.1 msecs off the expected 1.0 msec, then
            % we simply hard-code dt to 1.0 msec.
            if IsWin | (box.dt < 0.0009) | (box.dt > 0.0011) %#ok<OR2>
                % CMU Box: Assume 1 msec per sample:
                box.dt = 1/1000;
            end
        end
    end

    % Setup Async-Reads with blocking background read behaviour. This also
    % affects regular Sync-Reads by avoiding polling for the first byte!
    % This will cause the background async reader thread to block in the
    % serial port read function until data is available, then fetch,
    % process, timestamp and enqueue it, then repeat the cycle. No cpu
    % resources are wasted polling and the accuracy of the timestamps is
    % only limited by the scheduling latency of the operating system:
    IOPort('ConfigureSerialPort', box.port, 'BlockingBackgroundRead=1');

    if box.type == 3
        % PST box: Enable streaming from box:
        % Technically only bit 7 (128) controls streaming and its speed, the
        % rest controls lamp state (1 = Lamp dark, 0 = Lamp on) as
        % described above. A setting of 255 will enable streaming (128) and will
        % switch on all lamps due to 64 + 32 + 16 + 8 + 4 + 2 + 1:
        IOPort('Write', box.port, uint8(255), 0);
    end
    
    if box.type == 2
        % CMU box: Enable serial port receiver to start the data stream
        % from box at the receiving side, as this braindead box can't be
        % instructed to start streaming by itself. This call will only work
        % on OS/X and Linux and even there only on a small subset of serial
        % port hardware :-(
        IOPort('ConfigureSerialPort', box.port, 'ReceiverEnable=1');
    end
    
    % Fetch 1st sample synchronously, so we don't get a skewed box.baseTime
    % due to startup latencies of the async reader thread:
    [box.oldState, box.baseTime, box.olderr] = IOPort('Read', box.port, 1, 1);
    
    % Start background read operation, try to fetch and timestamp data at a
    % granularity of 9 Byte -- Each single status byte from the box gets
    % timestamped individually, and a 4 Byte streamcount of read bytes gets
    % attached, as well as a 4 byte timedelta in microseconds, resulting in
    % 9 Bytes of data for each single non-redundant byte of data from the
    % box:
    IOPort('ConfigureSerialPort', box.port, 'StartBackgroundRead=9');
    
    % CAUTION: As soon as StartBackgroundRead has been called, we should
    % avoid calling IOPort('Write') or IOPort('Purge') etc. on MS-Windows.
    % Apparently access to serial ports is not thread-safe and we can get
    % into race-conditions if trying to access the port while our
    % background thread accesses as well. Only 'Read', 'BytesAvailable',
    % and a subset of 'ConfigureSerialPort' commands are safe on Windoze.
    
    % baseTime is first oldTime:
    box.oldTime = box.baseTime;
    
    % Reset trouble counter:
    box.tTrouble = 0;
    
    % Assign box struct to internal boxes struct array at slot box.port+1:
    boxes{box.port + 1} = box;
    
    % Return box handle:
    varargout{1} = box.port + 1;
    
    % Perform one dummy read to discard first event: It is always invalid.
    IOPort('Read', box.port, 1, 9);
    
    return;
end

% Close box:
if strcmpi(cmd, 'Close')
    if isempty(handle)
        error('CMUBox: Close: No "handle" for box to close provided!');
    end
    
    if length(handle) ~= 1
        error('CMUBox: Close: Passed argument is not a valid response box "handle"!');
    end
    
    if length(boxes) < handle
        error('CMUBox: Close: Passed argument is not a valid response box "handle"!');
    end

    % Retrieve box struct:
    box = boxes{handle};
    
    if isempty(box.useBitwhacker)
        error('CMUBox: Close: Passed argument is not a valid response box "handle"!');
    end

    %    IOPort('Verbosity', 10);
    
    % Stop async read, release queues:
    IOPort('ConfigureSerialPort', box.port, 'StopBackgroundRead');

    % Emulated box?
    if box.useBitwhacker
        % Stop streaming: Send a CTRL+C control character (ascii code 3).
        % This will abort the program running on the Bitwhacker:
        IOPort('Write', box.port, char(3));
        WaitSecs(0.2);

        % Send reset signal to StickOS:
        Command(box.port, 'reset');
    end
    
    if box.type == 3
        % PST box: Disable streaming from box and all lamps:
        IOPort('Write', box.port, uint8(64));
        
        % Wait extra 250 msecs for box to calm down:
        WaitSecs(0.25);
        
        % For the fun of it, a little scan of the lights as a goodbye:
        for j=1:4
            i = 1;
            while i < 32
                IOPort('Write', box.port, uint8(64 + i));
                i = i * 2;
                WaitSecs(0.1);
            end

            i = 16;
            while i >= 1
                IOPort('Write', box.port, uint8(64 + i));
                i = i / 2;
                WaitSecs(0.1);
            end
        end
        
        % Flush lights a last time:
        IOPort('Write', box.port, uint8(64 + 32 + 16 + 8 + 4 + 2 + 1));
        WaitSecs(0.4);
        
        % Turn off lights:
        IOPort('Write', box.port, uint8(64));
        WaitSecs(0.4);

        % Everything off:
        IOPort('Write', box.port, uint8(0));
        WaitSecs(0.25);

        % Now we can reasonably assume that no new data will arrive from
        % box and no data is in-flight. Whatever is there should be in the
        % OS receive buffers.
    end
    
    % Close down driver and port:
    IOPort('Close', box.port);
    
    % Invalidate entry in boxes() cell array to make handle invalid:
    box.useBitwhacker = [];
    boxes{handle} = box;
    
    % Done.
    return;
end

if strcmpi(cmd, 'Status')
    if isempty(handle)
        error('CMUBox: Status: No "handle" for box to close provided!');
    end
    
    if length(handle) ~= 1
        error('CMUBox: Status: Passed argument is not a valid response box "handle"!');
    end
    
    if length(boxes) < handle
        error('CMUBox: Status: Passed argument is not a valid response box "handle"!');
    end

    % Retrieve box struct:
    box = boxes{handle};
    
    if isempty(box.useBitwhacker)
        error('CMUBox: Status: Passed argument is not a valid response box "handle"!');
    end
    
    % Return box status struct:
    varargout{1} = box;
    
    return;
end

% Invalid command!
error('CMUBox: Invalid or unknown command specified!');

% End of main function:
end

% Helper function for sending commands to Bitwhacker:
function Command(p, cmd)
    IOPort('Write', p, [cmd char(13)]);
end
