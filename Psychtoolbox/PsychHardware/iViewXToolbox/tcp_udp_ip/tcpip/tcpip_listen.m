function fid=tcpip_listen(socket)
% TCPIP_LISTEN(socket) - Listning for connetions on open socket.
% Socket must first be opened with TCPIP_SERVSOCKET, both sockets
% and connections are closed with TCPIP_CLOSE().
% 
% This frunction returns a hander to connection if ther is one
% waiting, else it returns -1.

fid=tcpipmex(21,socket);
