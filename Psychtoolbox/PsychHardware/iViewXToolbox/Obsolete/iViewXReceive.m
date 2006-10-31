function data=iViewXReceive(ivx)

data=[];
switch ivx.connection
    case 1, % ethernet

        % open (and close) udp connection for receiving data from iView computer
        % we could split opening, checking, receiving and closing of course

        % Open  udpsocket and bind udp port adress to it.
        udp=pnet('udpsocket',ivx.localport);
        pnet(udp,'setreadtimeout',ivx.udpreadtimeout);

        % Wait/Read udp packet to read buffer
        len=pnet(udp,'readpacket');
        %len=pnet(udp,'readpacket',[],'noblock');
        %         if len>0 fprintf('Len: %d\n', len); end


        if len>0,
            [ip,port]=pnet(udp,'gethost')
            % if packet larger then 1 byte then read maximum of 1000 doubles in network byte order
            %  data=pnet(udp,'read',ivx.udpmaxread,'double');
            data=pnet(udp,'read',ivx.udpmaxread);
            data
            % "translate" received data into commands and parameters
            % following may work for gaze data
            data=str2num(data);
            data
            x=data(1);
            y=data(2);
            x
            y
        end
        pnet(udp,'close');

    case 2,
        % assume we use serial communication
        error([mfilename ': serial communication not implemented yet']);


    otherwise,
        error([mfilename ': unknown connection type']);
end

