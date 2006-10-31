function [varargout]=tcpip_getvar(ip_fid)
%
%  ret = tcpip_getvar(ip_fid)
%
%  ip_fid    File id for tcpip channel.
%  ret       Variable to return
%
% Get a file over network with tcpip.
% Using a own protocol for this system.
% se tcpip_sendvar()
  
  
  name=tempname;
  tcpip_getfile(ip_fid,[name,'.mat']);
  VARS={};
  load(name);
  delete([name,'.mat']);
  varargout=VARS;
  return;
