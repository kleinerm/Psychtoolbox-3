function ReceivingTriggerFromSerialPortDemo(sampleFreq, baudRate, portSpec, specialSettings)
% Template for asynchronous trigger collection and timestamping from serial port.
%
% ReceivingTriggerFromSerialPortDemo([sampleFreq=120][, baudRate=115200][, portSpec=auto-detect][, specialSettings=None])
%
% This demo shows how to perform efficient trigger recording from an
% external trigger device which is connected to the serial port or a USB-Serial
% converter. The device is assumed to send trigger bytes at a rate of at most
% 'sampleFreq' Hz. Typical devices would be fMRI / TMS / MEG / EEG systems
% or other research equipment.
%
% The demo connects to the first found serial port, or optionally the port
% given by the 'portSpec' parameter, e.g., 'COM5'. It connects at a
% baudrate of 'baudRate' Baud, by default without flow-control, with 8
% databits, 1 stopbit and no parity, but you can set arbitrary settings via
% the optional 'specialSettings' string (see IOPort OpenSerialPort?
% online-help for possible parameters).
%
% Then it allocates receivebufferspace for up to 1 hour of uninterrupted
% recording, then starts background recording of data.
%
% Triggers can be read out in realtime, as demonstrated in the main
% while loop here, or offline at end of a session.
% Each trigger gets a GetSecs() timestamp of when it was received.
%
% Of course you'll need to understand the code of this demo and then
% customize it for your needs.
%
% For accurate timestamping and data reception with low latency, make sure
% that you've configured your serial ports properly. Search the
% Psychtoolbox forum for posts on that topic, e.g., message 9873.
%

% History:
% 17.4.12  mk  Written. Derived from DataRecordingFromSerialPortDemo().

% joker variable: Ignore this! Only for MK's internal testing:
joker = '';

%joker = 'Lenient';

% sampleFreq provided?
if ~exist('sampleFreq', 'var')
    sampleFreq = [];
end

if isempty(sampleFreq)
    % Choose an expected sampling frequency for incoming trigger packets of
    % 120 Hz:
    sampleFreq = 120;
end

% Baudrate provided?
if ~exist('baudRate', 'var')
    baudRate = [];
end

if isempty(baudRate)
    % Choose an optimistic default of 115.2 KBaud:
    baudRate = 115200;
end

% Any serial port specification provided?
if ~exist('portSpec', 'var')
    portSpec = [];
end

if isempty(portSpec)
    % Try to auto-detect a suitable serial port:
    portSpec = FindSerialPort([], 1);
end

% Any serial specialSettings provided?
if ~exist('specialSettings', 'var')
    % Set to empty aka "use defaults" which is 8-N-1 encoding, no flow
    % control...
    specialSettings = [];
end

% Compute maximum input buffer size for 1 hour worth of triggers coming
% in at a expected sampleFreq Hz with a size of at most 1 Bytes each:
InputBufferSize = sampleFreq * 3600;

% Assign an interbyte readtimeout which is either 15 seconds, or 10 times
% the expected time between consecutive datapackets at the given sampleFreq
% sampling frequency, whatever's higher. Could go higher or lower than
% this, but this seems a reasonable starter: Will give code and devices
% time to start streaming, but will prevent script from hanging longer than
% 15 seconds if something goes wrong with the connection:
readTimeout = max(10 * 1/sampleFreq, 15);

% HACK: Restrict maximum timeout to 21 seconds. This is needed on Macintosh
% computers, because at least OS/X 10.4.11 seems to have a bug which can
% cause the driver to hang when trying to stop at the end of a session if
% the timeout value is set higher than 21 seconds!
readTimeout = min(readTimeout, 21);

% Assemble initial configuration string for opening the port with
% reasonable settings: Given special settings, baudrate, inputbuffersize.
% Also set the special delimiter character code 'lineTerminator' that
% signals the end of a valid data packet:
portSettings = sprintf('%s %s BaudRate=%i InputBufferSize=%i Terminator=0 ReceiveTimeout=%f ReceiveLatency=0.0001', joker, specialSettings, baudRate, InputBufferSize, readTimeout);

% Open port portSpec with portSettings, return handle:
myport = IOPort('OpenSerialPort', portSpec, portSettings);

fprintf('Link online: Hit a key on keyboard to start trigger recording, after that hit any key to finish trigger collection.\n');
KbStrokeWait;

% ---- Here you'd put any IOPort setup calls, e.g., write and read commands
% to setup your device, enable streaming of data etc...


% ---- End of device specific setup ----

% Start asynchronous background trigger collection and timestamping. Use
% blocking mode for reading data -- easier on the system:
asyncSetup = sprintf('%s BlockingBackgroundRead=1 StartBackgroundRead=1', joker);
IOPort('ConfigureSerialPort', myport, asyncSetup);

% Trigger reception started: From now on, the driver will read data from
% the serial port, byte by byte. Whenever 1 Byte has been received from the
% trigger mechanism, the driver assumes a trigger pulse is complete.
% Reception of the first byte of a new packet is timestamped in GetSecs()
% time, and the later IOPort('Read') calls will return those timestamps...

% ---- From here on, only a limited set of IOPOrt commands is allowed if
% you are working on MS-Windows. On OS/X and Linux no such restrictions
% apply afaik.


% Alloc timestamp array for 1 hour worth of samples at 'sampleFreq' Hz:
tpkt = zeros(1, sampleFreq * 3600);
count = 0;

% Fake experiment loop: Run until any key is pressed:
while ~KbCheck
    % Wait blocking for a new data packet of 1 trigger byte from
    % the serial port, then return the packet data as uint8's plus the
    % GetSecs receive timestamp 'treceived' of the start of each packet:
    [pktdata, treceived] = IOPort('Read', myport, 1, 1);

    % Now 'pktdata' will most likely be empty, or some random number, because
    % a typical trigger received via serial port has no meaning - the only
    % information it carries is the fact that it arrived, and the time when
    % it arrived:
    fprintf('ReceiveTime: %f seconds GetSecs time.\n', treceived);
    
    % Store timestamp for the fun of it:
    count = count + 1;
    tpkt(count) = treceived;
    
    % Next loop iteration...
end

% We'd like to stop trigger collection here. However there may be still data
% queued up in the receive buffers, so we will now try to get all data that
% has been received up to this point in time:
tEnd = GetSecs;

fprintf('TRIAL LOOP STOPPED AT t = %f seconds. Now fetching pending triggers up to that point...\n', tEnd);

% Fetch all pending data that has been received up to systemtime tEnd:
while (treceived < tEnd) && (IOPort('BytesAvailable', myport) > 0)
    % Same as above, but now a non-blocking read (flag == 0):
    [pktdata, treceived] = IOPort('Read', myport, 0, 1);
    
    fprintf('ReceiveTime: %f seconds GetSecs time.\n', treceived);
    
    % Store timestamp for the fun of it:
    count = count + 1;
    tpkt(count) = treceived;

    % Next iteration...
end
    
% All data up to time tEnd fetched. There may be more data lingering in the
% queue but we don't care about those orphaned little triggers which arrived
% after the "official" end of your experiment session...


% End of data recording. Stop background read operation. This will discard
% any remaining data in the receive buffers. Stopping is a bit tricky if
% the source device doesn't transmit anymore. Then this may hang until the
% ReceiveTimeout timeout value expires, ie., up to 'readTimeout' seconds.
%
% Therefore it is better to only stop data transmission from the sending
% device after we have executed the following stop commands.
fprintf('STOPPING DATA COLLECTION: This may take up to %f seconds...\n', readTimeout);

% This line may or may not help if you experience hangs at this place: IOPort('ConfigureSerialPort', myport, [joker ' BlockingBackgroundRead=0']);
IOPort('ConfigureSerialPort', myport, [joker ' StopBackgroundRead']);

% Now the driver has discarded the data and is in synchronous manual mode
% of operation again. You could add any kind of IOPort commands to shut
% down your serial device now...
fprintf('SHUTTING DOWN DEVICE\n');


% Close port and driver:
fprintf('CLOSING SERIAL PORT\n');
IOPort('Close', myport);
fprintf('DONE.\n');

% Plot delta between consecutive received packets for the fun of it:
tpkt = tpkt(1:count);

% Deltas in msecs:
tpkt = 1000 * diff(tpkt);

% Print mean delay and sample frequency:
fprintf('Mean delay between triggers is %f msecs, i.e., %f Hz real datarate vs. expected %f Hz.\n', mean(tpkt), 1000/mean(tpkt), sampleFreq);

% Plot it:
close all;
plot(tpkt);

fprintf('Done. Bye!\n');

return;
