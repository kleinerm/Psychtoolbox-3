udp=pnet('udpsocket',1111)
%udp2=pnet('udpsocket',2111)

pnet(udp,'udpconnect','192.168.1.2',4444);
i=0;
while i<10
 %   pnet(udp,'write','hello');              % Write to write buffer
    for k=1:1
        for p=1:1
            pnet(udp,'write','boeh');              % Write to write buffer
            pnet(udp,'write', char(10));
            
   %         pnet(udp,'write','n');
            
%             fprintf('%d%d\n',k,p);
%             pnet(udp,'write',k);              % Write to write buffer
%             pnet(udp,'write',p);              % Write to write buffer
            pnet(udp,'writepacket');   % Send buffer as UDP packet
        end
    end
    pnet(udp,'writepacket');   % Send buffer as UDP packet
    i=i+1;
end
[ip,port]=pnet(udp,'gethost')
stat=pnet(udp,'status')
pnet(udp,'close');
