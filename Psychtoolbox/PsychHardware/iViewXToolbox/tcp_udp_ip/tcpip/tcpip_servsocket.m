function fid=tcpip_servsocket(port)
% TCPIP_SERVSOCKET(port) - Creates an listning socket.
% Returns a handler that can be used with TCPIP_LISTEN
% To then get each clients connection to this socket.
% 
% This runction returns handler to listning socket if
% successfull else it returns -1.

fid=tcpipmex(20,-1,port);

