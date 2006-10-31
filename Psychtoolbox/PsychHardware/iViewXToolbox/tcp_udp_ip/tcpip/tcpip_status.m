function status=tcpip_status(fid)
  
% status=tcpip_status(fid) Return a status value for tcpip connection.
%
% Mainly usefull when you whant to detect that a connection is broken.
% (return value 0)
%
% Possible status values:
%
% TCPIP_NOCONNECT   0 Not connected. Perhaps it has been broken. Close it!
% TCPIP_SERVSOCKET  1   Server socket waiting for connections.
% TCPIP_STDIO       5   Connected to file (not implemented)
% TCPIP_CLIENT      10  Connected as client. Used tcpip_open.
% TCPIP_SERVER      12  Connected as server  Used tcpip_servopen or tcpip_listen
  
  status=tcpipmex(6,fid);
  return;
