function [status, error] = NetStation(varargin)
%
% NetStation - Basic control of the EGI/NetStation EEG recording system via
% TCP/IP network connection. (See http://www.egi.com)
%
% This function was developed and contributed to Psychtoolbox by Gergely Csibra, 2006-2008.
% Code is based on Rick Gilmore's routines, 2005. Thanks!
% Code adapted to PCs (and Macs with Intel architecture) by Zhao Fan, 2008.
%
%
% General syntax
%
%   [status, error] = NetStation('command', ...)
%
%   if status == 0, the command has been succesfully executed
%   otherwise see string "error" for error message
%
% Commands
%
%   NetStation('Connect', host [, port])
%
%           Establishes TCP/IP connection to the NetStation host computer.
%           "host" is the hostname as a string (e.g., 'anything.com' or '187.14.176.12')
%           "port" is the ethernet port to be used. Default is 55513.
%
%   NetStation('Disconnect')
%
%           Disconnects from NetStation host.
%
%   NetStation('Synchronize' [, SynchLimit])
%
%           Synchronize to the connected host. "SynchLimit" could specify the maximum allowed difference
%           IN MILLISECONDS. Default is 2.5 ms.
%
%   NetStation('StartRecording')
%
%           Instructs NetStation to start recording.
%
%   NetStation('StopRecording')
%
%           Instructs NetStation to stop recording.
%
%   NetStation('Event' [,code] [,starttime] [,duration] [,keycode1] [,keyvalue1] [...])
%   NetStation('EventNoAck' [,code] [,starttime] [,duration] [,keycode1] [,keyvalue1] [...])
%
%           Send an event to the NetStation host. The 'EventNoAck' variant doesn't
%           wait for acknowledgement of reception of the event, whereas the 'Event'
%           version does. Note that the 'EventNoAck' command is included for
%           completeness. It is not a good practice to skip waiting for
%           acknowledgements. If time is an important factor, Event commands can be sent
%           at a later opportunity within a session (e.g., in blocks at the end
%           of experiments).
%
%           "code"      The 4-char event code (e.g., 'STIM')
%                       Default: 'EVEN'
%           "starttime" The time IN SECONDS when the event started. The VBL time
%                       returned by Screen('Flip') can be passed here as a parameter.
%                       Default: current time.
%           "duration"  The duration of the event IN SECONDS.
%                       Default: 0.001.
%           "keycode"   The 4-char code of a key (e.g., 'tria').
%           "keyvalue"  The value of the key (any number or string)
%                       The keycode-keyvalue pairs can be repeated arbitrary times.
%
%   NetStation('FlushReadbuffer');
%
%            Flushes the read buffer.
%
%   This also contains an experimental implementation of NTP clock drift corrected
%   synchronization with EGI, under the subfunction NetStation('ntpsynchronize'), but
%   this code is so far completely untested! It may or may not work, and its behavior
%   may change drastically in future releases, or the subfunction may disappear again.
%   Only for alpha-testing by dedicated testers, not to be relied on in any way fo real
%   data collection!
%
%   Uses a modified version of the TCP/UDP/IP Toolbox 2.0.5, a third party GPL'ed
%   open source toolbox, which is included in Psychtoolbox,
%   but also available from the Mathworks website:
%   http://www.mathworks.com/matlabcentral/fileexchange/loadFile.do?objectId=345
%
%   The toolbox has been modified for lower communication latency.
%
%   Created by Gergely Csibra, 2006-2008
%   Based on Rick Gilmore's routines, 2005
%   Adapted to PC by Zhao Fan, 2008
%   This function was modified by Matt Mollison to accommodate sending more than just int16s to Net Station
%   Improved by Justin Ales 2014.
%   Consolidated by Gergely Csibra, 2015
%   Improved by Justin Ales 2017.
%%

persistent NSIDENTIFIER;
persistent NSSTATUS;
persistent NSRECORDING;

%Originally this was to fix the bug that large epochs (like unix) overflows
%the int32 value. Now it's also being used to implement NTP synching--JMA
persistent SYNCHEPOCH;
persistent NTPSYNCED;

DefaultSynchLimit = 2.5;  % The accuracy of synchronization in milliseconds

%Switch for implementing debug code.
%Set to true if no netstation is connected.
noNetstationAvailable = false;
netstationAvailable   = ~noNetstationAvailable; %Cause double negatives annoy me.

%Debug code
if noNetstationAvailable
    NSIDENTIFIER = 1;
    SYNCHEPOCH = 0;
    NTPSYNCED = 0;
end

if nargin < 1
    if(isempty(NSSTATUS))
        status = -1;
    else
        status = NSSTATUS;
    end
else
    switch lower(varargin{1})
        case 'connect'
            if(nargin < 2)
                status = 2;
            else
                NetStationPCHostName = varargin{2};
                if (~isempty(NSIDENTIFIER)) && (NSIDENTIFIER > 0)
                    send(NSIDENTIFIER,'X');
                    rep = receive(NSIDENTIFIER,1);
                    pnet( NSIDENTIFIER, 'close' );
                    NSIDENTIFIER = 0;
                end
                port=55513;
                if nargin > 2
                    port = varargin{3};
                end
                c = pnet( 'tcpconnect', NetStationPCHostName, port );
                if(c < 0)
                    status = 3;
                else
                    NSIDENTIFIER = c;
                    NSRECORDING = 0;
                    send(NSIDENTIFIER,'QMAC-');
                    rep = receive(NSIDENTIFIER,1);
                    switch char(rep)
                        case 'F'
                            status = 4;
                        case 'I'
                            vers = receive(NSIDENTIFIER,1);
                            if(int8(vers) ~= 1)
                                status = 5;
                            else
                                status = 0;
                            end
                    end
                    if status ~= 0
                        pnet( NSIDENTIFIER, 'close' );
                        NSIDENTIFIER = 0;
                    end
                    SYNCHEPOCH = 0;
                    NTPSYNCED = 0;
                end
            end
        case 'disconnect'
            if isempty(NSIDENTIFIER) || (NSIDENTIFIER < 0)
                status = 1;
            else
                if NSRECORDING
                    WaitSecs(.5);
                    send(NSIDENTIFIER,'E');
                    rep=receive(NSIDENTIFIER,1);
                    NSRECORDING = 0;
                end
                WaitSecs(1.);
                send( NSIDENTIFIER,'X');
                rep=receive(NSIDENTIFIER,1);
                WaitSecs(.5);
                pnet( NSIDENTIFIER, 'close' );
                NSIDENTIFIER = -1;
                status = 0;
            end
        case 'synchronize'
            if isempty(NSIDENTIFIER) || (NSIDENTIFIER < 0)
                status = 1;
            else
                NSSynchLimit = DefaultSynchLimit;
                SYNCHEPOCH = GetSecs();
                if nargin > 1
                    NSSynchLimit = varargin{2};
                end
                if NSSynchLimit < .5 || NSSynchLimit > 50,
                    NSSynchLimit = DefaultSynchLimit;
                end
                df = 10000;
                n = 1;
                while df > NSSynchLimit && n < 100
                    send(NSIDENTIFIER,'A');
                    receive(NSIDENTIFIER,1);
                    now = GetSecs()-SYNCHEPOCH;
                    send(NSIDENTIFIER,'T',int32(now*1000));
                    receive(NSIDENTIFIER,1);
                    ack = GetSecs()-SYNCHEPOCH;
                    df = 1000*(ack-now);
                    status = 0;
                    n = n + 1;
                end
                if n >= 100
                    warning('\nNetStationPC synchronization did not succeed within %.1f ms\nSynchronizatoin accuracy is %.1f ms\n',NSSynchLimit,df);
                end
                NTPSYNCED = 0;
            end

        case 'ntpsynchronize'
            if isempty(NSIDENTIFIER) || (NSIDENTIFIER < 0)
                status = 1;
            else
                if netstationAvailable
                    %Tell netstation to get ready for synchronization.
                    send(NSIDENTIFIER,'A');
                    receive(NSIDENTIFIER,1);
                    %get the timebase from the netstation.
                    send(NSIDENTIFIER,'S');
                end

                % Get current time in EGI's NTP adjusted timebase (seconds since 1.1.1900):
                [ntpTimestamp] = receiveNtpTimestamp(NSIDENTIFIER);

                % Get current PTB GetSecs time and NTP adjusted UTC time.
                % ptbRefTime on Linux/OSX/Unix is time in seconds since 1.1.1970, NTP synced, microseconds resolution.
                % ptbRefTime on Windows is time in seconds since 1.1.1601, NTP synced, milliseconds resolution.
                [ptbTimestamp, ptbRefTime] = GetSecs('AllClocks');

                %For debugging let's print what EGI gave us.
                rawDataString = dec2hex(ntpTimestamp);
                disp('Raw NETSTATION NTP timestamp bytes:')
                disp(rawDataString)

                %The first 32 bits is the seconds since 1900
                netstationSyncEpochSeconds = ntpTimestamp(1);

                %THe next 32 bits is fractions of a second
                %With one tick equal to 2^-32 seconds.
                %We want to convert this to a more convenient number.
                %I'm using microseconds. To get these first divide the
                %fraction by 2^-32 to convert to Seconds than multiply
                %by 10^6 to convert seconds to microseconds.
                %I'm using microseconds because ideally we want sub
                %millisecond precision. Also, because I want all
                %conversions as explicit as possible for documentation.
                netstationSynchEpochMicroSeconds = ntpTimestamp(2) * 1e6 / 2 ^ 32;

                % NetStation system time at sync, seconds since 1.1.1900, NTP synced:
                netStationSyncFraction = netstationSyncEpochSeconds + netstationSynchEpochMicroSeconds / 1e6;

                % Now we need to convert the ntp sync time to the
                % equivalent timebase used by PTB. On Linux and OSX the
                % epoch is 1970, on Windows the epoch is the year 1601.
                % So first we're going to convert the netstation epoch
                % to these:
                if IsWin
                    secondsFromNtpEpochToSystemEpoch = -9435484800;
                else
                    secondsFromNtpEpochToSystemEpoch = 2208988800;
                end

                %Let's create a human readable string for help
                %debugging, first we make a single number for the numnber
                %of seconds. Then we create a matlab date vector
                %corresponding to the ntp epoch. Then we use the matlab
                %datestr to create a human readable date.

                matlabDateNum = datenum([1900 1 1 0 0  netStationSyncFraction]);
                netstationSynchString = datestr(matlabDateNum,'yyyy-mm-dd HH:MM:SS.FFF');
                disp('Human readable NETSTATION NTP timestamp bytes:')
                disp(netstationSynchString)
                rawPtbTimestamp = num2hex(ptbTimestamp);
                disp(['Raw ptb timestamp bytes: ' rawPtbTimestamp])

                %On osx the timer is in boot time not unix epoch time. We
                %don't know when the boot happened in unix/ntp time. So we
                %need to figure this out to have a common timebase. What
                %I'm doing here is using an undocumented feature of GetSecs
                %to return more outputs when called with a non-zero input.
                %The second return value is gettimeofday() time on OSX
                %Gettimofday() is referenced to the unix epoch.  So we can
                %use that to figure out when the boot time was in unix
                %epoch.
                %Warning: I'm relying on the latency between the call to
                %mach_absolute_time() and gettimeofday() to be shorter than
                %a millisecond.  On my quick test of GetSecs time it was
                %taking around .1-.3 milliseconds for the total time for
                %GetSecs. That's ok as an upper bound, but it's
                %uncomfortably long. There must be a better/more accurate
                %way to find get information. It either exists somewhere to
                %query, or we can write specific c-code for this and test
                %it a bit more rigorously
                %

                % Difference PTB GetSecs time and ptbRefTime at this point in time,
                % drifting over time:
                unixEpochToCurrentEpoch = ptbRefTime - ptbTimestamp;

                % Set EGI baseline NTP time, mapped to OS specific PTB wall clock time as SYNCHEPOCH:
                SYNCHEPOCH = netStationSyncFraction - secondsFromNtpEpochToSystemEpoch;
                NTPSYNCED = 1;

                fprintf('NetStation: Clock offset GetSecs() - NTP adjusted time: %f secs.\n', unixEpochToCurrentEpoch);
                if IsWin
                    fprintf('NetStation: Difference between 1. January 1601 and 1. January 1900: %f secs.\n', ...
                            -secondsFromNtpEpochToSystemEpoch);
                else
                    fprintf('NetStation: Difference between 1. January 1970 and 1. January 1900: %f secs.\n', ...
                            secondsFromNtpEpochToSystemEpoch);
                end
                fprintf('NetStation: Events will be labeled with time difference to EGI baseline %f secs.\n', SYNCHEPOCH);

                status=0;
            end
        case 'startrecording'
            if isempty(NSIDENTIFIER) || (NSIDENTIFIER < 0)
                status = 1;
            else
                if ~NSRECORDING
                    send(NSIDENTIFIER,'B');
                    rep = receive(NSIDENTIFIER,1);
                    NSRECORDING = 1;
                end
                status=0;
            end
        case 'stoprecording'
            if isempty(NSIDENTIFIER) || (NSIDENTIFIER < 0)
                status = 1;
            else
                if NSRECORDING
                    WaitSecs(.5);
                    send(NSIDENTIFIER,'E');
                    rep = receive(NSIDENTIFIER,1);
                    NSRECORDING=0;
                end
                status = 0;
            end
        case { 'event' 'eventnoack' }
            if isempty(NSIDENTIFIER) || (NSIDENTIFIER < 0)
                status = 1;
            else
                if nargin < 2 || isempty(varargin{2})
                    event = 'EVEN';
                else
                    event = [char(varargin{2}) '    '];
                end

                if nargin < 3
                    start = [];
                else
                    start = varargin{3};
                end

                % Are Psychtoolbox client computer and NetStation host computer NTP synchronized?
                if NTPSYNCED
                    % Yes: Translate GetSecs timestamp into PTB NTP synchronized time via local clock-sync,
                    % then by subtracting SYNCHEPOCH, translate that into time delta since EGI's NTP baseline
                    % time. This would be NTP drift corrected for synchronized machines, so we don't accumulate
                    % timestamp error, even for long experiment runtimes:
                    [ptbGetSecsTime, ptbRefTime] = GetSecs('AllClocks');
                    if isempty(start)
                        % Can skip translation and use current ptbRefTime directly,
                        % just get delta to EGI baseline. This is a tad more accurate:
                        start = ptbRefTime - SYNCHEPOCH;
                    else
                        % Translate, map, get delta:
                        GetSecsToNTPTime = ptbGetSecsTime - ptbRefTime;
                        start = start - GetSecsToNTPTime - SYNCHEPOCH;
                    end
                else
                    % No: Just transmit elapsed time since event GetSecs timestamp and
                    % EGI base timestamp from last 'synchronize'.
                    if isempty(start)
                        start = GetSecs();
                    end
                    start = start - SYNCHEPOCH;
                end

                if nargin < 4 || isempty(varargin{4})
                    duration=.001;
                else
                    duration=varargin{4};
                end
                if isnumeric(duration)
                    if duration > 3600
                        duration=.001;
                    end
                else
                    duration = .001;
                end

                if nargin > 4
                    keyn = floor((nargin - 4) / 2);
                    keylength = 0;
                    realkeyn = 0;
                    for k = 1:keyn
                        len = keycodedata(varargin{k*2+4});
                        if len > 0
                            keylength = keylength + len + 10;
                            realkeyn = realkeyn + 1;
                        end
                    end

                    send(NSIDENTIFIER,'D',uint16(15+keylength),int32(start*1000),uint32(duration*1000),event(1:4),int16(0),uint8(realkeyn));

                    for k = 1 : keyn
                        id = [char(varargin{(k-1)*2+5}) '    '];
                        [len, code, val] = keycodedata(varargin{k*2+4});
                        if len > 0
                            send(NSIDENTIFIER,id(1:4),code,uint16(len),val);
                        end
                    end
                    if strcmpi(varargin{1},'event')
                        rep = receive(NSIDENTIFIER,1);
                    end
                    status=0;
                end
            end
        case 'flushreadbuffer'
            data='1';
            bufferCount=0;
            while ~isempty(data),
                data=pnet(NSIDENTIFIER,'read',1 ,'char',[],[],'noblock');
                bufferCount = bufferCount+1;
            end;
            status = 0;
        otherwise
            status = 7;
    end
end

error=nserr(status);



function send(con,varargin)
    i=1;
    while i <= nargin-1
        % pnet(con,'write',varargin{i},'native'); % original
        pnet(con,'write',varargin{i},'network');
        i = i+1;
    end
end


function rep=receive(con,len)
    rep=pnet(con,'read',len,'char');
end

% Receives 8 byte ntp timestamp from EGI system.
function ntpTimestamp=receiveNtpTimestamp(con)
    if noNetstationAvailable
        ntpTimestamp(1) = 1494145042+2208988800;
        ntpTimestamp(2) = 0;
    else
        ntpTimestamp=double(pnet(con,'read',2,'uint32'));
    end
end

function errstr=nserr(status)
    switch status
        case -1
            errstr='NetStation has not been initialized';
        case 0
            errstr='No error';
        case 1
            errstr='NetStation host not connected';
        case 2
            errstr='NS connect: NetStation host name must be specified';
        case 3
            errstr='NS connect: Unable to connect to host';
        case 4
            errstr='NS connect: ECI error';
        case 5
            errstr='NS connect: Unknown ECI version';
        case 6
            errstr='NS event: Unsuccesful';
        case 7
            errstr='Unknown NetStation command';
        otherwise
            errstr='NS unknown error';
    end
end

function [len, code, val] = keycodedata(data)

    len = 0;
    d = data(1);

    switch class(d)
        case 'logical'
            val = int8(0);
            if(d) val = int8(1); end
            code = 'bool';
            len = 1;
        case {'int8' 'uint8' 'int16' }
            val = int16(d);
            code = 'shor';
            len = 2;
        case {'uint16' 'int32' }
            val = int32(d);
            code = 'long';
            len = 4;
        case 'single'
            val = single(d);
            code = 'sing';
            len = 4;
        case {'double' 'uint32' 'int64' 'uint64'}
            val = double(d);
            code = 'doub';
            len = 8;
        case 'char'
            val = data(1:numel(data));
            code = 'TEXT';
            len = length(val);
    end

end
end