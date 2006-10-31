function ret=tcpip_sendvar(ip_fid,varargin)
%
%  ret = tcpip_sendvar(ip_fid,var)
%
%  ip_fid    File id for tcpip channel.
%  var       Variable to send
%  ret       Return value.
%
% Get a file over network with tcpip.
% Using a own protocol for this system.
% se tcpip_sendfile()
  
  ret=0;
  
  VARS=varargin;
  name=[tempname,'.mat'];
  save(name,'VARS');
  tcpip_sendfile(ip_fid,name);
  delete(name);
