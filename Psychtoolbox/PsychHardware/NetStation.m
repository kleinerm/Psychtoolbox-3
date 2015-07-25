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
%%

persistent NSIDENTIFIER;
persistent NSSTATUS;
persistent NSRECORDING;
persistent SYNCHEPOCH; %This is to fix the bug that large epochs (like unix) overflows the int32 value --JMA

DefaultSynchLimit = 2.5;  % The accuracy of synchronization in milliseconds

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
                if nargin < 2
                    event = 'EVEN';
                else
                    event = [char(varargin{2}) '    '];
                end

                if nargin < 3
                    start = GetSecs();
                else
                    start = varargin{3};
                end
                start = start - SYNCHEPOCH;

                if nargin < 4
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
return


function send(con,varargin)
    i=1;
    while i <= nargin-1
        % pnet(con,'write',varargin{i},'native'); % original
        pnet(con,'write',varargin{i},'network');
        i = i+1;
    end
return



function rep=receive(con,len)
  rep=pnet(con,'read',len,'char');
return

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
return

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

return
