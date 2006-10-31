function str=tcpip_viewbuff(fid,len)
% str=tcpip_viewbuff(fid,len)
%
% fid   file id for connection.
% len   Maximum length of returned string.
% str   Returned string as char.
%
%
% Returns what's in incomming buffert but it will not
% remove data from the buffert. A following tcpip_read()
% will return the same string.
  
  str=tcpipmex(5,fid,len);
  return;
