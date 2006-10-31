function tcpip_close(varargin)
  
% tcpip_close(fid) closes a open tcpip connection. No arg.=>  Close default.
%
% fid is file id for the open tcpip connection returned from tcpip_open.
%
  
  if nargin== 0,
    tcpipmex(0,-2);   % Close current tcpip connection
  else
    if strcmp(char(varargin{1}),'all'),
      tcpipmex(-1,0);             %Close all files
    else
      tcpipmex(0,varargin{1});
    end
  end
  
  
  

