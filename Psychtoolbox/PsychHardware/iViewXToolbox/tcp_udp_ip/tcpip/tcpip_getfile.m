function ret=tcpip_getfile(ip_fid,name)
%
%  ret = tcpip_getfile(ip_fid,name)
%
%  ip_fid    file id for tcpip channel.
%  name      Name of file to save as.
%  ret       Return value.
%
% Get a file over network with tcpip.
% Using a own protocol for this system.
% se tcpip_sendfile()
  
  NL=char(10);        %Define newline character
  
  fid=fopen(name,'wb');
  if fid==-1,
    ret=-1;
    return;
  end
  
  ver='';
  while strcmp(ver,'==BINFILE==')==0,
    ver=tcpip_readln(ip_fid,15);
  end
  
  srcname='';
  while length(srcname)==0,
    srcname=tcpip_readln(ip_fid,20);
  end
  
  buff='';
  while length(buff)==0,
    buff=tcpip_readln(ip_fid,20);
  end
  
  filelen=sscanf(buff,'%d');
  
  buff='x';
  while buff~=':',
    buff=tcpip_read(ip_fid,1);      %Wait for ':' char.
    if isempty(buff),
      buff='x';
    end
  end
  
  getlen=0;
  while getlen<filelen,
    blocksize=min(filelen-getlen,2000);
    buff=tcpip_read(ip_fid,blocksize);
    getlen=getlen+length(buff); 
    fwrite(fid,buff);
  end
  fclose(fid);
  dump=tcpip_readln(ip_fid,10);
  ret=getlen;
  return;
