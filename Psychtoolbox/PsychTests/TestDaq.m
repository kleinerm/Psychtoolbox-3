function TestDaq
% TestDaq
% TestDaq assesses the Daq Toolbox, which provides communication with a
% particular USB data acquisition device (daq): the USB-1208FS made by
% Measurement Computing (see URL below). This daq costs $150 and offers "50
% kHz" input and output 12-bit sampling of analog voltages (8 in, 2 out)
% and 16 digital i/o lines, with signals brought out to screw terminals.
% ("50 kHz" is a theoretical upper limit: as of 18 April 2005 we attain 2
% kHz.) The USB-1208FS is the size of a wallet and is powered through its
% USB cable. The Daq Toolbox gives you complete control of it from within
% Matlab, via the PsychHID extension.
%
% TestDaq assesses all the functions provided by the USB-1208FS firmware.
% (Some risky tests, below, have been turned off, but can be re-enabled by
% the adventurous user.) DaqFunctions gives a brief description of every
% function.
%
% NOT RESPONDING? If PsychHID is not responding, e.g. after unplugging and 
% re-plugging the USB connector of your device, try quitting and restarting
% MATLAB. We find that this reliably restores normal communication. 
%
% web http://www.http://www.measurementcomputing.com/cbicatalog/directory.asp?dept_id=403 -browser;
% web http://psychtoolbox.org/daq.html -browser;
% See also: Daq, DaqFunctions, DaqPins,TestPsychHID,PsychHID,PsychHardware,
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan, DaqAOutScan

% 3/21/05 dgp Denis Pelli wrote it.
% 3/28/05 dgp Corrected reportID and reportBytes.
% 3/30/05 dgp More error checking.
% 3/31/05 dgp Added the rest of the functions.
% 4/2/05  dgp The input functions now work, for the first time.
% 4/15/05 dgp Polished.
% 4/15/05 dgp Merged arguments in calls to DaqAOutScan and DaqAInScan.
% 4/25/05 dgp Nearly eliminated crash associated with CLEAR MEX.
% 4/26/05 dgp Fixed DaqAInScan to return sensible results.
% 8/26/05 dgp Incorporated bug fix for compatibility with Mac OS X Tiger 
%             suggested by Maria Mckinley <parody@u.washington.edu>. The reported
%             number of outputs of the USB-1208FS has changed in Tiger.
%             http://groups.yahoo.com/group/psychtoolbox/message/3614

debugging=0; % Used by dgp to track down a crash in PsychHID.

% Do we have a USB-1208FS daq? Each such box will present itself as four
% HID "devices" sharing the same serial number.
daq=DaqDeviceIndex;
fprintf('\n');
switch length(daq)
    case 0,
        fprintf('Sorry. Couldn''t find a USB-1208FS box connected to your computer.\n');
        return
    case 1,
        fprintf('Yay. You have a USB-1208FS daq: \n');
    case 2,
        fprintf('Yay. You have two USB-1208FS daqs: \n');
    otherwise,
        fprintf('Yay. You have %d USB-1208FS daqs: \n',length(daq));
end
devices=PsychHID('Devices');
for i=1:length(daq)
    d=devices(daq(i));
    fprintf('device %d, serialNumber %s\n',d.index,d.serialNumber);
end
if debugging
    TestClearMex(daq);
end
if length(daq)>1
    fprintf(['We''re only going to test the first of your daqs, but rest assured\n'...
        'that PsychHID can deal with many daqs.\n']);
end
if debugging
% 	fprintf('TestClearMex once crashed at line 67.\n');
    TestClearMex(daq);
end

fprintf(['\n** NOT RESPONDING? If your USB-1208FS is not responding, try quitting and\n'...
    '** restarting MATLAB. We find that this reliably restores normal\n'...
    '** communication.\n']);
fprintf(['\n'...
    '** WIRING THE PINS OF THE USB-1208FS\n'...
    '**    For the analog i/o test, please connect both channels of analog\n'...
    '** output to the first two channels of differential analog input:\n'...
    '** Connect pin 1 to 13, 2 to 12, 4 to 14, and 5 to 12.\n'...
    '**    If you''ve got one, you may want to attach a speaker or oscilloscope\n'...
    '** to analog output channel 0 (pin 13) and analog ground (pin 12). \n'...
    '**    Or you may want to connect to bit 0 of digital output port A (pin 21) \n'...
    '** and digital ground (pin 29).\n']);

% The USB-1208FS is a composite device. It has four interfaces (0,1,2,3)
% which are each represented as a device in the list returned by
% PsychHID('Devices'). In calling the Daq functions, the user supplies us
% only the device index "device" corresponding to interface 0 of the
% desired USB-1208FS. However, the reports containing the samples arrive on
% interfaces 1,2,3. As returned by PsychHID('Devices'), interface i is at
% device-i, and we proceed on that basis after doing a quick check to
% confirm our assumption. However, to be platform-independent, it would be
% better to actually find all four device interfaces and confirm their
% interface numbers. USB Probe reports interface numbers, but, as far as I
% can tell, Apple's HID Explorer and our PsychHID do not return this
% return this information. However, PsychHID('Devices') does report the
% number of outputs: the USB-1208FS interface 0 has 229 (pre-Tiger) 70 (Tiger) outputs,
% interface 1 has 65 (pre-Tiger) 1 (Tiger) outputs, and interfaces 2 and 3 have zero outputs.
% I have no idea why the number of outputs changed with the arrival of Mac OS X Tiger.
devices=PsychHID('Devices');
for d=daq
    ok= (d-3>=1 && (devices(d-1).outputs==65 || devices(d-1).outputs==1)) || (d+3<=length(devices) && (devices(d+1).outputs==65 || devices(d+1).outputs==1));
    if devices(d).outputs<70 && ok
        error(sprintf('Invalid device, not the original USB-1208FS.'));
    end
end

if debugging
    TestClearMex(daq);
end

fprintf('\nThe following tests assess all the USB-1208FS commands, reporting any errors.\n');
fprintf('\n** DIGITAL COMMANDS \n** DaqDConfigPort, DaqDIn, DaqDOut, DaqGetAll\n');

% DaqDConfigPort
fprintf('\nDaqDConfigPort: configuring digital ports for output.\n');
err=DaqDConfigPort(daq(1),0,0); % configuring digital port A for output
err=DaqDConfigPort(daq(1),1,0); % configuring digital port B for output

% Make sure the USB-1208FS is "attached".
if streq(err.name,'kIOReturnNotAttached')
    fprintf(['\nkIOReturnNotAttached: Mac OS error message says USB-1208FS is "not attached".\n'...
        'But we know it is. We suggest that you quit and restart MATLAB. In our experience\n'...
        'that always clears up the problem.\n']);
    error('USB-1208FS not attached.');
end

% DaqDOut
fprintf(['\nDaqDOut: random test patterns on ports A and B. This is a basic functionality\n'...
    'test, showing that we can indeed read back what we wrote.\n']);
a=Randi(255);
err=DaqDOut(daq(1),0,a);
b=Randi(255);
err=DaqDOut(daq(1),1,b);
fprintf('DaqDOut: %3d %3d.\n',a,b);

if debugging
    TestClearMex(daq);
end

% DaqDIn
fprintf('\nDaqDIn: reading digital ports.\n');
data=DaqDIn(daq(1));
if ~isempty(data)
    fprintf('DaqDIn: %3d %3d.\n',data);
else
    fprintf('Nothing received.\n');
end

if debugging
    TestClearMex(daq);
end

% DaqGetAll
fprintf('\nDaqGetAll: reading all analog and digital inputs.\n');
data=DaqGetAll(daq(1));
if ~isempty(data)
    fprintf('data.digital: %3d %3d.\n',data.digital);
else
    fprintf('Nothing received.\n');
end

if debugging
    TestClearMex(daq);
end

fprintf('\nDaqDOut & DaqDIn: write & read back one hundred random values as fast as\n');
fprintf(  'possible. This test assesses reliability and latency, the time for data to \n');
fprintf(  'go from host to device and back.\n');
wrong=0;
missing=0;
t=GetSecs;
for trials=1:100
    % DaqDOut
    a=Randi(255);
    err=DaqDOut(daq(1),0,a);
    % DaqDIn
    data=DaqDIn(daq(1));
    if ~isempty(data)
        if a~=data(1)
            wrong=wrong+1;
        end
    else
        missing=missing+1;
    end
    if err.n
        break;
    end
end
t=GetSecs-t;
if missing==trials
    fprintf('Nothing received.\n');
else
    fprintf('Achieved %.0f Hz, i.e. %.0f ms cycle time. %.0f errors and %.0f missing in %.0f trials.\n',...
        trials/t,1000*t/trials,wrong,missing,trials);
end
if debugging
    TestClearMex(daq);
end

% DaqDOut
fprintf('\nDaqDOut: producing fast squarewave digital output on port A.\n');
DaqDConfigPort(daq(1),0,0); % output
for i=1:500
    err=DaqDOut(daq(1),0,255);
    err=DaqDOut(daq(1),0,0);
    if err.n
        break;
    end
end

if debugging
    TestClearMex(daq);
end

fprintf('\n** ANALOG COMMANDS \n** DaqAIn, DaqAOut, DaqAInScan, DaqAOutScan, DaqALoadQueue\n');

% DaqAOut: 1 kHz squarewave
outTick=0.0005; % s
duration=0.5; % s
fprintf('\nDaqAOut: producing %.0f kHz squarewave output voltage for %.1f s.\n',0.5/outTick/1000,duration);
until=GetSecs;
for i=1:duration/outTick
    until=until+outTick;
    WaitSecs(until-GetSecs);
    err=DaqAOut(daq(1),0,mod(i,2)); % D/A 0
    err=DaqAOut(daq(1),1,mod(i,2)); % D/A 1
    if err.n
        break;
    end
end

% DaqAOut & DaqAIn: 4 Hz sinewave
f=4;
fprintf([
    '\nDaqAOut & DaqAIn: producing %.0f Hz sinewave and triangle wave output voltages,\n'...
    'on two channels, and reading them back in through the analog inputs. We plot \n'...
    'the readings in Figure 1. (It''s just noise unless you''ve installed wires\n'...
    'connecting output to input.)\n'],f);
start=GetSecs;
time=0:0.01:0.5;
x=f*2*pi*time;
sineWave=0.5+0.5*sin(x);       % Raised sine. Positive.
triangleWave=0.5+0.5*trin(x);  % Raised triangle. Positive.
v0=[];
v1=[];
for i=1:length(time)
    WaitSecs(time(i)+start-GetSecs);
    err=DaqAOut(daq(1),0,sineWave(i));
    err=DaqAOut(daq(1),1,triangleWave(i));
    if err.n
        break;
    end
    v0(i)=DaqAIn(daq(1),0,3);
    v1(i)=DaqAIn(daq(1),1,3);
end
if ~isempty(v0)
    figure(1);
    plot(time,v0,'r',time,v1,'g');
    ylabel('Volts');
    xlabel('Time (s)');
    title('Two channels (sine and triangle waves) put out by DaqAOut and read back in by DaqAIn.');
    text(0.15,1.9,'This graph will be meaningless noise unless you previously');
    text(0.15,1.8,'connected output to input. This test requires 4 wires ');
    text(0.15,1.7,'connecting analog output to input. Connect pin 1 to 13, ');
    text(0.15,1.6,'2 to 12, 4 to 14, and 5 to 12.');
    figure(1);
else
    fprintf('Nothing received.\n');
end
if debugging
% 	fprintf('TestClearMex once crashed at line 269.\n');
    TestClearMex(daq);
end

% DaqAInScan
fprintf('\nDaqAInScan: sampling analog input.\n');
options.lowChannel=[];
options.highChannel=[];
options.count=1000;
options.f=2000;
options.immediate=0;
options.trigger=0;
options.print=0;
options.channel=0:0;
options.range=3*ones(size(options.channel));
c=length(options.channel);
% n is sample/channel/report
if options.immediate
    n=1;
else
    n=31/c;
end
start=GetSecs;
[data,params]=DaqAInScan(daq(1),options);
stop=GetSecs;
fprintf('Got %d (of %d) sample/channel on %d channels at %.0f Hz.\n',...
    size(data,1),options.count,size(data,2),params.fActual);
if length(params.times)>1
    t=params.times;
    fprintf('Received %.1f report/s, %.1f sample/channel/s.\n',...
        (length(t)-1)/(t(end)-t(1)),(size(data,1)-n)/(t(end)-t(1)));
end
if debugging
% 	fprintf('TestClearMex once crashed at line 302.\n');
    TestClearMex(daq);
end

% DaqAInScanBegin, DaqAInScanContinue, DaqAInScanEnd
% Works well for f up to 2048/c sample/channel/s, where c is number of
% channels. Currently (18 April 2005) unreliable at higher rates.
c=length(options.channel);
for log2f=11
    options.f=2^log2f/c;
    fprintf('\nDaqAInScanBegin, DaqAInScanContinue, DaqAInScanEnd: sampling analog input. \n');
    duration=1;
    options.count=duration*options.f;
    options.immediate=0;
    params=DaqAInScanBegin(daq(1),options);
    params=DaqAInScanContinue(daq(1),options);
    params=DaqAInScanContinue(daq(1),options);
    [data,params]=DaqAInScanEnd(daq(1),options);
    fprintf('Got %d (of %d) sample/channel on %d channels at %.0f Hz.\n',size(data,1),options.count,size(data,2),params.fActual);
    if length(params.times)>1
        t=params.times;
        c=size(data,2);
        % n is sample/channel/report
        if options.immediate
            n=1;
        else
            n=31/c;
        end
        fprintf('Received %.1f report/s, %.1f sample/channel/s.\n',(length(t)-1)/(t(end)-t(1)),(size(data,1)-n)/(t(end)-t(1)));
    end
end
if debugging
% 	fprintf('TestClearMex once crashed at line 390.\n');
    TestClearMex(daq);
end

% DaqAOut & DaqAInScan: 4 Hz sinewave
% This runs well at slow sampling rates (100 Hz). Attempting to run at high
% rates (e.g. 1000 Hz) results in losing most of the data. This isn't a
% bug. It's asking the USB-1208FS to do two things at once (AOut and
% AInScan) and clearly they interfere with each other. None of the
% Measurement Computing literature that I've read mentions the issue of
% doing two things at once, so there's no promise to live up to. This
% simultaneous in and out is an excellent way to test the capabilities of
% the USB-1208FS without any extra equipment, but is probably not a useful
% model for experimental work since it limits you to low sampling rates. If
% you need to sample in and out at the same time you may want to buy two
% USB-1208FS units and use one for input and the other for output.
f=1;
fprintf([
    '\nDaqAOut & DaqAInScan: producing %.0f Hz sinewave and square wave output\n'...
    'voltages, on two channels, and reading them back in through the analog inputs. \n'...
    'We plot the readings in Figure 2. (It''s just noise unless you''ve installed\n'...
    'wires connecting output to input.)\n'],f);
time=0:0.02:5;
x=f*2*pi*time;
sineWave=0.5+0.5*sin(x);       % Raised sine. Positive.
triangleWave=0.5+0.5*trin(x);  % Raised triangle. Positive.
squareWave=round(sineWave);
% In
v0=[];
v1=[];
options.f=100;
options.channel=[];
options.range=[];
options.lowChannel=0;
options.highChannel=0;
options.count=round(max(time)*options.f);
options.immediate=0;
channel=options.lowChannel:options.highChannel;
range=3*ones(size(channel));
for i=channel
    DaqAIn(daq(1),i,3);% set gain range
end
start=GetSecs;
params=DaqAInScanBegin(daq(1),options);
for i=1:length(time)
    WaitSecs(time(i)+start-GetSecs);
    err=DaqAOut(daq(1),0,sineWave(i));
    err=DaqAOut(daq(1),1,squareWave(i));
    if err.n
        break;
    end
    tOut(i)=GetSecs-start;
end
params=DaqAInScanContinue(daq(1),options);
[data,params]=DaqAInScanEnd(daq(1),options);
fprintf('Got %.0f (of %.0f) sample/channel on %d channels at %.0f Hz.\n',...
    size(data,1),options.count,size(data,2),params.fActual);
if ~isempty(data)
    t=(1:size(data,1))/params.fActual;
    v0=data(:,1);
    figure(2);
    plot(t,v0,'r');
    if size(data,2)>=2
        v1=data(:,2);
        hold on
        plot(t,v1,'g');
        hold off
        title(sprintf('%.0f Hz sine & square waves put out by DaqAOut and read back in by DaqAInScan.',f));
    else
        title(sprintf('%.0f Hz sine wave put out by DaqAOut and read back in by DaqAInScan.',f));
    end
    ylabel('Volts');
    xlabel('Time (s)');
end
if debugging
    TestClearMex(daq);
end

% DaqAOutScan. Analog output scan.
% Works well for f up to 1024 sample/channel/s on one channel, and 256
% sample/channel/s on two channels. Currently (28 April 2005) unreliable at
% higher rates.
for log2f=10
    options.f=2^log2f; % sampling frequency in Hz
    duration=0.999; % s
    fSine=10; % Hz
    options.lowChannel=0;
    options.print=0;
    outTick=1/options.f; % s
    wave=0.5+0.5*sin(fSine*2*pi*(0:outTick:duration)); % Raised sine. Positive.
    if 1
        % works for f up to 1024
        options.highChannel=0;
    else
        % works for f up to 256
        options.highChannel=1;
        wave=[wave' wave'];
    end
    options.getReports=1;
    c=1+options.highChannel-options.lowChannel;
    fprintf('\nDaqAOutScan: producing %.0f Hz sine for %.0f s sampled at %.0f Hz on %d channels.\n',...
        fSine,duration,options.f,c);
    params=DaqAOutScan(daq(1),wave,options);
    if isempty(params.start)
        params.start=nan;
    end
    if isempty(params.end)
        params.end=nan;
    end
    fprintf('Sent %.0f (out of %.0f) samples/channel on %d channels in %.0f (out of %.0f) ms.\n',...
        params.countActual,length(wave),c,1000*(params.end-params.start),1000*length(wave)/options.f);
    r=(params.numberOfReportsSent-1)/(params.end-params.start);
    fprintf('Sending speed was %.0f report/s, %.0f sample/channel/s.\n',r,r*32/c);
end
if debugging
% 	fprintf('TestClearMex once crashed at line 499.\n');
    TestClearMex(daq);
end

% DaqALoadQueue.
fprintf('\nDaqALoadQueue: setting channel 0 to gain range 0.\n');
err=DaqALoadQueue(daq(1),0,0);
if debugging
    TestClearMex(daq);
end

fprintf('\n** COUNTER COMMANDS \n** DaqCIn, DaqCInit\n');

% DaqCIn. Reading counter
fprintf('\nDaqCIn: reading the counter.\n');
count=DaqCIn(daq(1));
if ~isempty(count)
    fprintf('Count is %d.\n',count);
else
    fprintf('Nothing received.\n');
end
if debugging
    TestClearMex(daq);
end

% DaqCInit. Resetting counter to zero.
fprintf('\nDaqCInit: resetting counter to zero.\n');
err=DaqCInit(daq(1));

fprintf(['\n** MISCELLANEOUS COMMANDS \n** DaqBlinkLED, DaqGetStatus, DaqSetCal, DaqSetSync, DaqSetTrigger\n']);

% DaqBlinkLED
fprintf('\nDaqBlinkLED: blinking the LED.\n');
err=DaqBlinkLED(daq(1));

if 0
    % DaqReset. Optional reset. Make sure the USB-1208FS is "attached". In
    % theory it's nice to detect anomalous behavior and fix it, by
    % resetting the device. However, this requires a call to CLEAR PsychHID to
    % re-enumerate, which may result in a crash when you next ask PsychHID to 
    % receive reports. Don't use DaqReset. Quitting and restarting MATLAB is a 
    % reliable way of reestablishing normal communication with the USB-1208FS.
    err=PsychHID('SetReport',daq(1),2,64,uint8(64)); % Blink LED
    if streq(err.name,'kIOReturnNotAttached')
        fprintf('\nMac OS error message says USB-1208FS is "not attached". But we know it is.\n');
        fprintf('We will attempt to reestablish communication by re-enumerating, resetting the USB, and re-enumerating again. ');
        err=DaqReset(daq(1));
        fprintf('End of reset. Hopefully your USB-1208FS is working again.\n');
    end
end

for i=[1 0]
    % DaqSetSync
    fprintf('\nDaqSetSync: configure synchronization to %d. (0 for master, 1 or 2 for slave)\n',i);
    err=DaqSetSync(daq(1),i);

    % DaqGetStatus
    fprintf('\nDaqGetStatus: reading the status bits.\n');
    status=DaqGetStatus(daq(1));
    if ~isempty(status)
        if status.master
            fprintf('master.\n');
        else
            fprintf('slave.\n');
        end
    else
        fprintf('Nothing received.\n');
    end
end
if debugging
    TestClearMex(daq);
end

% DaqSetCal
fprintf(['\nDaqSetCal: setting calibration voltage output to +2.5 V for 1 s. \n'...
    'CAL is pin 16; pin 15 is ground.\n']);
err=DaqSetCal(daq(1),1); % SetCal +2.5 V
WaitSecs(1);
err=DaqSetCal(daq(1),0); % SetCal 0 V

% DaqSetTrigger
fprintf('\nDaqSetTrigger: setting to trigger on falling edge.\n');
err=DaqSetTrigger(daq(1),0);

fprintf(['\n** MEMORY COMMANDS\n'...
    '** DaqMemRead, DaqMemWrite, DaqReadCode, DaqPrepareDownload, DaqWriteCode,\n'...
    '** DaqWriteSerialNumber\n']);
fprintf(['\nWe only test the two read commands, since the write commands could\n'...
    'potentially screw up the firmware, and we have no immediate need for\n'...
    'writing to memory.\n']);
if debugging
    TestClearMex(daq);
end

% DaqMemRead
fprintf('\nDaqMemRead: Reading memory.\n');
data=DaqMemRead(daq(1),hex2dec('100'),62);
if isempty(data)
    fprintf('Nothing received.\n');
end

% DaqReadCode
fprintf('\nDaqReadCode: Reading program memory.\n');
data=DaqReadCode(daq(1),0,62);
if isempty(data)
    fprintf('Nothing received.\n');
end
if debugging
    TestClearMex(daq);
end

if 0
    % NOT TESTED. I don't need these functions, which write to memory, and
    % I don't want to risk screwing up my USB-1208FS firmware. If you decide to
    % try this, pick your addresses carefully, after consulting the table
    % of memory types: "help daqwritecode".

    % DaqMemWrite
    fprintf('\nDaqMemWrite: Writing memory.\n');
    err=DaqMemWrite(daq(1),0,1:2);

    % DaqPrepareDownload
    fprintf('\nDaqPrepareDownload: Preparing for program memory download.\n');
    err=DaqPrepareDownload(daq(1));

    % DaqWriteCode
    fprintf('\nDaqWriteCode: Writing program memory.\n');
    err=DaqWriteCode(daq(1),0,1:2);
end


if 0
    % WARNING: you probably do NOT want to run this command, because it
    % will over-write your factory-assigned serial number. I tested this
    % on my USB-1208FS and it works fine. Note that the new serial number
    % won't show up until you unplug and reconnect your device.

    % DaqWriteSerialNumber
    serialString='00000123';
    err=DaqWriteSerialNumber(daq(1),serialString);
    fprintf('\nDaqWriteSerial: setting the serial number to "%s".\n',serialString);
end

fprintf('\nDone.\n');

function y=trin(x)
% y=trin(x)
% Triangle wave. Has same peaks as SIN, but is linear between those peaks.
y=4*mod(x/2/pi,1);
fy=floor(y);
i=find(fy==1 | fy==2);
y(i)=2-y(i);
i=find(fy==3);
y(i)=y(i)-4;
return

function TestClearMex(daq)
% TestClearMex is being used by dgp to track down a crash of PsychHID associated
% with CLEAR MEX. 
options.printCrashers=1;
% PsychHID('ReceiveReports',daq(1),options);
% 
% % DaqDIn
% fprintf('\nDaqDIn: reading digital ports.\n');
% data=DaqDIn(daq(1));
% if ~isempty(data)
%     fprintf('DaqDIn: %3d %3d.\n',data);
% else
%     fprintf('DaqDIn: Nothing received.\n');
% end

fprintf('CLEAR MEX\n');
clear mex;

PsychHID('ReceiveReports',daq(1),options);

% DaqDIn
fprintf('DaqDIn: reading digital ports.\n');
data=DaqDIn(daq(1));
if ~isempty(data)
    fprintf('DaqDIn: %3d %3d.\n',data);
else
    fprintf('DaqDIn: Nothing received.\n');
end
return