function tcpip_test()
  disp ' '
  disp '=========== START TCPIP TEST ========================='
  disp 'If this test stops or gives any other error it failed!!'
  tcpip_close all;
  local_talk([999 1025 2000:2010 2777 21777 55123]);
  disp '=========== END              ========================='
  return
  
function local_talk(port)
  if length(port)>1,
    for n=1:length(port),
      local_talk(port(n));
    end
    return;
  end
  sock=tcpip_servsocket(port);
  if sock==-1,
    disp(sprintf('Cant use local socket no:%d as server',port));
    return;
  end
  cli=tcpip_open('localhost',port);
  if cli==-1,
    disp(sprintf('Cant connect to local socket no:%d as client',port));
    tcpip_close all;
    return;
  end
  serv=tcpip_listen(sock);
  if serv==-1,
    disp(sprintf('Cant get connection on socket no:%d as server',port));
    tcpip_close all;
    return;
  end
  tcpip_write(serv,'Hi!',char(10));
  str='';
  while length(str)<3,
    str=[str , tcpip_readln(cli,20)];
  end
  
  tcpip_write(cli,'Hello!',char(10));
  str='';
  while length(str)<6,
    str=[str , tcpip_readln(serv,20)];
  end
  
  VARa=rand(3,3);
  VARb=rand(3,3);
  
  tcpip_sendvar(cli,VARa,VARb);
  [VARa2,VARb2]=tcpip_getvar(serv);
  
  if sum(abs(VARb(:)~=VARb2(:)))>0,
    disp(sprintf('Cant transmit variable proper! port no:%d',port));
    tcpip_close all;
    return;
  end
    
  tcpip_close(cli);
  tcpip_close(serv);
  tcpip_close(sock);
  disp(sprintf('Client-server talk via port no:% 6d is OK!',port));
  return;
