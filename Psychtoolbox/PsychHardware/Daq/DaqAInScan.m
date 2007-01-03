function [data,params]=DaqAInScan(device,options)
% [data,params]=DaqAInScan(device,options)
% USB-1208FS: Analog input scan. Clocked analog to digital conversion. This
% command samples several analog input channels at a specified rate and
% sends the readings to the host. Use DaqAInScan when you want to complete
% the whole operation in one call, tying up the computer until the sampling
% is done. Use DaqAInScanBegin, DaqAInScanContinue, and DaqAInScanEnd,
% instead, when you want to use the computer during sampling.
% 
% USE CHANNEL AND RANGE: You can specify the channels to be scanned in two
% different ways, "lowChannel" and "highChannel", or as an arbitrary
% "channel" list. We recommend the latter because it's much more general.
% With "channel" you also provide a corresponding list of the desired gain
% "range". If you use lowChannel and highChannel, then the sampling will
% use the existing gains that were last set on those channels (by AIn or
% ALoadQueue). However, DaqAInScan doesn't know what that gain range was,
% so it arbitrarily assumes a gain range of 3 in computing the scale factor
% to convert your readings into voltages. Thus every reason recommends that
% you supply channel and range when you call DaqAInScan.
% 
% "data" is an NxM matrix, with one column per channel. Each reading is a
%     double, based on a 16-bit value in the report. In the USB-1208FS
%     only the upper 12 of those 16 bits are significant.
% "params.fActual" is the actual sampling frequency, sample/channel/s. It
%     is as close as possible to the requested sampling frequency
%     options.f.
% "params.times" is the times (GetSecs) of receipt by PsychHID of the
%     reports in data.
% "device" is a small integer, the array index specifying which HID
%     device in the array returned by PsychHID('Devices') is interface 0
%     of the desired USB-1208FS box.
% "options.channel" is a vector of length 1 to 8. Each value (0 to 15)  
%     selects any of various single-ended or differential measurements
%     using the eight digitizers. By default, DaqAInScan sends this for you
%     to the device by calling DaqALoadQueue before issuing the AInScan
%     command.
%     "channel" Measurement
%      0        0-1 (differential)
%      1        2-3 (differential)
%      2        4-5 (differential)
%      3        6-7 (differential)
%      4        1-0 (differential)
%      5        3-2 (differential)
%      6        5-4 (differential)
%      7        7-6 (differential)
%      8          0 (single-ended)
%      9          1 (single-ended)
%     10          2 (single-ended)
%     11          3 (single-ended)
%     12          4 (single-ended)
%     13          5 (single-ended)
%     14          6 (single-ended)
%     15          7 (single-ended)
% "options.range" is a vector of the same length as options.channel, with  
%     values of 0 to 7, specifying the desired gain (and voltage range) for
%     the corresponding channel. By default, DaqAInScan sends this for you
%     to the device by calling DaqALoadQueue before issuing the AInScan
%     command. When options.range is not specified DaqAInScan assumes a
%     value of 3 in computing the scale factor applied to the results to
%     convert them into volts.
%     0 for 1x (+-20 V),  1 for 2x (+-20 V),
%     2 for 4x (+-20 V),  3 for 5x (+-20 V),
%     4 for 8x (+-20 V),  5 for 10x (+-20 V),
%     6 for 16x (+-20 V), 7 for 20x (+-1 V).
% "options.lowChannel" (0 to 15) is the first channel of the scan. NOT 
%     RECOMMENDED; use options.channel instead. If you specify lowChannel
%     and highChannel then don't specify channel and range, above.
% "options.highChannel" (0 to 15) is the last channel of the scan. The 
%     values lowChannel and highChannel specify the channel range for the
%     scan. If lowChannel is greater than highChannel, the scan will wrap
%     (i.e. if lowChannel is 14 and highChannel is 1, the scan will include
%     channels 14, 15, 0, and 1.) NOT RECOMMENDED; use options.channel
%     instead. If you specify lowChannel and highChannel then don't specify
%     channel and range, above.
% "options.count" is the desired number of samples per channel, either in
%     the 32-bit range 1 to 4e+09, or INF. The value INF invokes the
%     USB-1208FS's continuous data collection mode, which runs
%     indefinitely.
% "options.f" is desired sampling frequency, sample/channel/s, in the range
%     0.596/c to 10e6/c Hz, where c is the number of channels being
%     scanned.
% "options.immediate" is 1 = immediate-transfer mode, 0 (default) = 
%     block-transfer mode. At low sampling rates, immediate-transfer mode
%     will reduce the delay in receiving the sampled data. In
%     immediate-transfer mode, each 16-bit sample is sent immediately (a
%     2-byte report), rather than waiting for the buffer to fill (62
%     bytes). This mode should not be used if the aggregate sampling rate
%     is greater than 2,000 samples per second in order to avoid data loss.
%     The difference between the two modes will be more apparent if you
%     enable options.print, allowing you to see that you get one-sample
%     reports (2 bytes) in immediate mode, and less-frequent 31-sample
%     reports (62 bytes) in block-transfer mode. 
% "options.trigger" is 1 = use external trigger; 0 (default).
%     The external trigger may be used to start data collection
%     synchronously. If the bit is set, the device will wait until the
%     appropriate trigger edge is detected, then begin sampling data at
%     the specified rate.  No messages will be sent until the trigger is
%     detected.
% "options.retrigger" is 1 = retrigger mode, 0 (default) = normal trigger.
%     The retrigger mode option is only used if trigger is used.  This
%     option will cause the trigger to be rearmed after options.count
%     samples are acquired if in continuous mode.
% "options.sendChannelRange is 1 (default) to ask that the options.channel  
%     and options.range lists (if provided) be sent to the device by
%     calling DaqALoadQueue before the sending the AInScan command, or 0 to
%     not send them. Since those lists persist in the device you can save
%     time by skipping this transmission after you've already sent them
%     once (either through DaqAinScan or by explicitly calling
%     DaqALoadQueue yourself). Even if you set this option to 0 you should
%     still supply the options.channel and options.range arguments to
%     DaqAInScan because it needs to know how many channels you are using
%     in order to set up the count and frequency parameters in the AInScan
%     command to the USB-1208FS. This default is 0 if you supply
%     lowChannel and highChannel.
% "options.queue" is 1 = use channel gain queue (i.e. "channel" and
%     "range"), 0 = use options.lowChannel and options.highChannel
%     arguments. Don't bother setting this. It will be set for you, based
%     on which parameters you provide.
% "options.print" is 1 = enable diagnostic printing of the reports; 0 
%     (default) no diagnostics.
% "options.begin" is 1 (default) to begin a new scan.
% "options.continue" is 1 (default) to receive reports (inside PsychHID). 
% "options.end" is 1 (default) to wait until done and return the result.
% 
% LIMITATION: The literature from Measurement Computing mentions a speed of
% 50 kHz, but that's a theoretical limit. As of 17 April 2005, DaqAInScan 
% achieves 2000/c sample/channel/s, where c is the number of channels being
% sampled. 
%
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid, 
% DaqAInScanBegin, DaqAInScanContinue, DaqAInScanEnd,
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.

% 4/15/05 dgp Wrote it.
% 4/15/05 dgp Merged several of the arguments into the "options" struct.
% 4/25/05 dgp Added options.channel, options.range, and options.sendChannelRange.
% 4/27/05 dgp Fixed immediate-transfer mode once I realized that the report
%             contains only one sample, not one sample per channel.
% 8/26/05 dgp Fixed bug in extraction of serial number, as suggested by Steve 
%             Van Hooser, vanhooser@neuro.duke.edu
% 8/26/05 dgp Incorporated bug fix for compatibility with Mac OS X Tiger 
%             suggested by Maria Mckinley <parody@u.washington.edu>. The reported
%             number of outputs of the USB-1208FS has changed in Tiger.
%             http://groups.yahoo.com/group/psychtoolbox/message/3614
% 1/02/07 mk  Add 'persistent dinc' to line 245. This apparently fixes some
%             bug, bugfix proposed by Florian Stendel.

% These USB-1280FS parameters are computed from the user-supplied
% arguments.
% "timer_prescale" chooses the timer prescaler (0 - 8).
% 	0 = 1:1 prescale
% 	1 = 1:2 prescale
% 	2 = 1:4 prescale
% 	3 = 1:8 prescale
% 	4 = 1:16 prescale
% 	5 = 1:32 prescale
% 	6 = 1:64 prescale
% 	7 = 1:128 prescale
% 	8 = 1:256 prescale
% "timer_preload" is the 16-bit timer preload value.
% "options" controls various options
% 	bit 0: 	1 = single execution, 0 = continuous execution
% 	bit 1:	1 = immediate-transfer mode, 0 = block transfer mode
% 	bit 2: 	1 = use external trigger
% 	bit 3:	not used
% 	bit 4:	1 = use channel gain queue, 0 = use channel parameters specified
% 	bit 5:	1 = retrigger mode, 0 = normal trigger
% 	bits 6-7: 	not used
%
% The sample rate is set by the internal 16-bit incrementing timer running
% at a base rate of 10 MHz.  The timer is controlled by timer_prescale and
% timer_preload.  These values are only used if the device has been set to
% master the SYNC pin with the SetSync command. The timer will provide an
% internal interrupt when its value rolls over.  The timer is preloaded
% with the value specified in timer_preload, and counts down.  This allows
% for a lowest rate of 0.596 Hz (1:256 prescale, preload = 65535).  It is
% preferable to keep the prescaler to the lowest value that will achieve
% the desired rate. 

% Fill in defaults for any unspecified options.
% Parameters of the AInScan command sent to the USB-1208FS.
if ~isfield(options,'channel')
    options.channel=[];
end
if ~isfield(options,'range')
    options.range=[];
end
if length(options.channel)~=length(options.range)
    error('"options.channel" and "options.range" vectors must be of the same length.');
end
channelRangeOk=~isempty(options.channel) & ~isempty(options.range);
if ~isfield(options,'lowChannel')
    options.lowChannel=[];
end
if ~isfield(options,'highChannel')
    options.highChannel=[];
end
lowHighOk=~isempty(options.lowChannel) & ~isempty(options.highChannel);
if lowHighOk==channelRangeOk
    error('Please specify either options.channel and options.range OR options.lowChannel and options.highChannel.');
end
if length(options.lowChannel)>1 || length(options.highChannel)>1
    error('options.lowChannel and options.highChannel, if specified, must be scalars.');
end
if ~isfield(options,'count')
    options.count=1000;
end
options.count=round(options.count); % make it an integer.
if ~isfield(options,'f')
    options.f=1000;
end
if ~isfield(options,'immediate')
    options.immediate=0;
end
if ~isfield(options,'trigger')
    options.trigger=0;
end
if ~isfield(options,'queue')
    options.queue=channelRangeOk;
end
if options.queue~=channelRangeOk
    error('options.queue setting inconsistent with other parameters. Let us set it for you.');
end
if ~isfield(options,'retrigger')
    options.retrigger=0;
end
% User options.
if ~isfield(options,'sendChannelRange')
    options.sendChannelRange=channelRangeOk;
end
if options.sendChannelRange && ~channelRangeOk
    error('You are not supplying channel and range, so you can''t send them. Omit options.sendChannelRange.');
end
if ~isfield(options,'print')
    options.print=0;
end
if ~isfield(options,'begin')
    options.begin=1;
end
if ~isfield(options,'continue')
    options.continue=1;
end
if ~isfield(options,'end')
    options.end=1;
end
persistent start;
persistent dinc;

if options.begin
    % It might be running, so stop it.
    % hex2dec('12') is 18.
    err=PsychHID('SetReport',device,2,18,uint8(0)); % AInStop
    if err.n
        fprintf('AInStop SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
    end
    % The user supplies us only the device index "device" corresponding to
    % interface 0. The reports containing the samples arrive on interfaces
    % 1,2,3. As returned by PsychHID('Devices'), interface "i" is at
    % device-i, and we proceed on that basis after doing a quick check to
    % confirm our assumption. However, to be platform-independent, it would
    % be better to actually find all four device interfaces and confirm
    % their interface numbers. USB Probe reports interface numbers, but, as
    % far as I can tell, Apple's HID Explorer and our PsychHID do not
    % return this information. However, PsychHID('Devices') does report the
    % number of outputs: the USB-1208FS interface 0 has 229 (pre-Tiger) 70 (Tiger) outputs,
    % interface 1 has 65 (pre-Tiger) 1 (Tiger) outputs, and interfaces 2 and 3 have zero outputs.
    % I have no idea why the number of outputs changed with the arrival of Mac OS X Tiger.
    devices=PsychHID('Devices');
    for dinc=[-1 1]
        if device+dinc>=1 && device+dinc<=length(devices) && (devices(device+dinc).outputs==65 || devices(device+dinc).outputs==1)
            break
        else
            dinc=[];
        end
    end
    if devices(device).outputs<70 || isempty(dinc) || ~streq(devices(device).serialNumber,devices(device+dinc).serialNumber)
        error(sprintf('Invalid device, not the original USB-1208FS.'));
    end
    % Flush any stale reports.
    for d=(1:3)*dinc % Interfaces 1,2,3
        err=PsychHID('ReceiveReports',device+d);
    end
    for d=(1:3)*dinc % Interfaces 1,2,3
        [reports,err]=PsychHID('GiveMeReports',device+d);
        if ~isempty(reports) && options.print
            fprintf('Flushing %d stale reports from device %d.\n',length(reports),device+d);
        end
    end
end
if options.sendChannelRange
    err=DaqALoadQueue(device,options.channel,options.range);
    if err.n
        fprintf('DaqALoadQueue error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
    end
end
% How many channels?
if channelRangeOk
    c=length(options.channel);
else
    if ~ismember(options.highChannel,0:15) || ~ismember(options.lowChannel,0:15)
        error('options.lowChannel and options.highChannel must each be in the range 0:15.');
    end
    if options.lowChannel<=options.highChannel
        channel=options.lowChannel:options.highChannel;
    else
        channel=[options.lowChannel:15 0:options.highChannel]; % wrap around.
    end
    c=length(channel);
end
% Convert from user to USB-1208FS usage. "options.f" and "options.count" refer to
% samples per channel.
% USB-1208FS: pmd.f and pmd.count refer to
% samples.
pmd.f=c*options.f;
pmd.count=c*options.count;
if isinf(pmd.count)
    options.counted=0;
else
    if pmd.count<0 || pmd.count>intmax('uint32');
        error('pmd.count is out of 32-bit range, yet not INF.');
    end
    options.counted=1;
end
timer_prescale=ceil(log2(10e6/65535/pmd.f)); % Use smallest integer timer_prescale consistent with pmd.f.
timer_prescale=max(0,min(8,timer_prescale)); % Restrict to available range.
timer_preload=round(10e6/2^timer_prescale/pmd.f);
timer_preload=max(1,min(65535,timer_preload)); % Restrict to available range.
pmd.fActual=10e6/2^timer_prescale/timer_preload;
% Convert from USB-1208FS to user usage.
% "options.f" and "options.count" refer to samples/channel/s and
% samples/channel.
params.fActual=pmd.fActual/c;
if options.begin
    if abs(log10(params.fActual/options.f))>log10(1.1)
        error('Nearest attainable sampling frequency %.4f kHz is too far from requested %.4f kHz.',params.fActual/1000,options.f/1000);
    end
    report=uint8(zeros(1,11));
    report(1)=17;
    if ~options.queue
        report(2)=options.lowChannel;
        report(3)=options.highChannel;
    end
    if isfinite(pmd.count)
        countsave=pmd.count;
        report(4)=bitand(pmd.count,255); % count
        pmd.count=bitshift(pmd.count,-8);
        report(5)=bitand(pmd.count,255);
        pmd.count=bitshift(pmd.count,-8);
        report(6)=bitand(pmd.count,255);
        pmd.count=bitshift(pmd.count,-8);
        report(7)=bitand(pmd.count,255);
        pmd.count=countsave;
    end
    report(8)=timer_prescale; % timer_prescale
    report(9)=bitand(timer_preload,255); % timer_preload
    report(10)=bitshift(timer_preload,-8);
    report(11)=options.counted+2*options.immediate+4*options.trigger+16*options.queue+32*options.retrigger;
    % Tell the USB1208FS to start sampling and sending reports.
    % hex2dec('11') is 17.
    err=PsychHID('SetReport',device,2,17,report); % AInScan
    start=GetSecs;
    if err.n
        fprintf('AInScan SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
    end
end
if options.continue
    % AInScan responses from the USB-1208FS will be sent sequentially from
    % interrupt IN endpoints in interfaces 1-3 as input reports with a
    % report ID of 0. The last two bytes are a sequential index,
    % "reports.serial".
    data=[];
    if isfinite(options.count)
        while GetSecs<start+options.count/options.f+0.02;
            for d=(1:3)*dinc % interfaces 1,2,3
                % Tell PsychHID to receive reports, storing them internally.
                err=PsychHID('ReceiveReports',device+d,options); % options.print
                if err.n
                    fprintf('AInScan device %d, ReceiveReports error 0x%s. %s: %s\n',device+d,hexstr(err.n),err.name,err.description);
                end
            end
        end
    end
end
if options.end
    % Get and combine reports from all three interfaces.
    r=[];
    params.times=[];
    for d=(1:3)*dinc
        [reports,err]=PsychHID('GiveMeReports',device+d);
        if err.n
            fprintf('AInScan device %d, GiveMeReports error 0x%s. %s: %s\n',device+d,hexstr(err.n),err.name,err.description);
        end
        r=[r reports];
        err=PsychHID('ReceiveReportsStop',device+d);
        if err.n
            fprintf('AInScan device %d, ReceiveReportsStop error 0x%s. %s: %s\n',device+d,hexstr(err.n),err.name,err.description);
        end
    end
    % hex2dec('12') is 18.
    err=PsychHID('SetReport',device,2,18,uint8(0)); % AInStop
    if err.n
        fprintf('AInStop SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
    end
    for d=(1:3)*dinc
        err=PsychHID('ReceiveReports',device+d);
        [reports,err]=PsychHID('GiveMeReports',device+d);
        if ~isempty(reports)
            fprintf('WARNING: received %d post-deadline reports from device %d.\n',length(reports),device+d);
        end
        err=PsychHID('ReceiveReportsStop',device+d);
        if err.n
            fprintf('AInScan device %d, ReceiveReportsStop error 0x%s. %s: %s\n',device+d,hexstr(err.n),err.name,err.description);
        end
    end
    reports=r;
    if isempty(reports)
        data=[];
        fprintf('Nothing received.\n');
        return
    end
    % Extract the report serial number provided by the USB-1208FS
    for i=1:length(reports)
		reports(i).serial=double(reports(i).report(63))+double(reports(i).report(64))*256;
    end
    % Sort by serial number.
    [r,ri]=sort([reports.serial]);
    reports=reports(ri);
    % Keep only the consecutively numbered reports, from 0 on. Discard
    % everything after a gap.
    ri=[reports.serial]==0:length(reports)-1;
    reports=reports(ri);
    % Diagnostic print of the raw reports
    if options.immediate
        % report length
        bytes=2;
    else
        bytes=62;
    end
    if options.print
        fprintf('Got %d reports.\n',length(reports));
        fprintf('Serial number, Device, Time received (since AInScan command), Report rate, Report as 16-bit words.\n');
        for i=1:length(reports)
            fprintf('Report %3d, device %d, %4.0f ms, %4.0f report/s.',...
                reports(i).serial,reports(i).device,1000*(reports(i).time-start),reports(i).serial/(reports(i).time-start));
            r=double(reports(i).report);
            r=r(1:bytes);
            fprintf(' %5d',r(1:2:end)+256*r(2:2:end));
            fprintf('\n');
        end
    end
    % Return times.
    params.times=[reports.time];
    % Extract the data from the reports.
    data=[];
    for i=1:length(reports)
        data=[data double(reports(i).report(1:bytes))];
    end
    % Combine two bytes for each reading.
    data=(data(1:2:end)+data(2:2:end)*256)/65535;
    % Discard any extra 16-bit words at the end of the last report.
    if length(data)>c*options.count
        if 2*(length(data)-options.count*c)>60
            fprintf('Trimming off an extra %.1f sample/channel, %.0f bytes.\n',length(data)/c-options.count,2*(length(data)-options.count*c));
        end
        data=data(1:c*options.count);
    end
    if c*options.count>length(data)
        fprintf('Missing %.1f sample/channel, %.0f bytes.\n',options.count-length(data)/c,2*(options.count*c-length(data)));
        options.count=floor(length(data)/c);
        data=data(1:c*options.count);
    end
    data=reshape(data,c,options.count)';
    vmax=[20,10,5,4,2.5,2,1.25,1];
    range=ones([1 size(data,2)]);
    if length(options.range)>0
        if length(options.range)~=size(data,2)
            error('DaqAInScan line 475: wrong length of "options.range" vector.');
        end
        range(:)=options.range(:);
    else
        range=3*range;
    end
    range=vmax(range+1);
    data=repmat(range,size(data,1),1).*data;
else
    data=[];
    params.times=[];
end
