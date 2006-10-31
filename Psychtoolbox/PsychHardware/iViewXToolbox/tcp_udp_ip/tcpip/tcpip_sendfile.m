function ret=tcpip_sendfile(ip_fid,name)
  
%
%  ret = tcpip_sendfile(ip_fid,name)
%
%  ip_fid     Number for tcpip channel.
%  name    Name of file to send
%  ret     Return value
%
% Send a file over network with tcpip.
% Using a own protocol. se also tcpip_getfile
%
  
  NL=char(10);  %Define newline character
  
  fh=dir(name);
  
  fid=fopen(name,'rb');
  
  if fid==-1,
    ret=-1;
    return;
  end
  
  dump=tcpip_write(ip_fid,NL,'==BINFILE==',NL,fh.name,NL,num2str(fh.bytes),NL,'DATA:');
  
  buff='';
  while ~feof(fid),
    buff=char(fread(fid,10000));
    while length(buff)>0,
      buff=tcpip_write(ip_fid,buff);
    end
  end
  dump=tcpip_write(ip_fid,'==END==',NL);
  fclose(fid);
  return;
