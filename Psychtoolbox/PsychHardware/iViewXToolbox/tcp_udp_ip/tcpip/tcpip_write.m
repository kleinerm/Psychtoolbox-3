function [varargout]=tcpip_write(varargin)
  
% unsent = tcpip_write(fid, arg2, arg3, . . .)
% unsent = tcpip_write(str, arg2 . . .)
%
% fid          is file id number returned from tcpip_open.
% arg2, arg2   Arrays of any type to send throw
% str          char array must be first argument if fid
%              is not the first. Chould be  str=''
%
% unsent   is returned string with usent charecters.
%          On success an empty char array is returned.
%
% Outputs arrays to tcpip channel. All variables is sent as the
% datatype it is but in network byte order. Notice that a char
% array is sent as one data byte per matlab char.
% If first argument is a scalar double it's used as file id.
% If a char array is given as first argument then last used
% fid (current) is used.
% Usually all character is sent but if their is trouble only
% some or no character is sent, non sent characters is returned.
%
% NOTE: changed from version 1.0 to 1.1 of tcpip toolbox.
%
% EXAMPLE 1:
%
%  V=23;
%  tcpip_write(fid,V);
%
%        Sends V as double to tcpip connection in network byte order.
%
%
% EXAMPLE 2:
%
%  V='Hello World!';
%  tcpip_write(fid,V);
%
%        Sends text string as array of bytes to tcpip connection;
%
%
% EXAMPLE 3:
%
%  V=uint16([1 3 0 1024]);
%  tcpip_write(fid,V);
%
%        Sends 4 uint16 to tcpip connection in network byte order;
%
  
% fid is set to current as default.
  fid=-2;
  
  % buff is set to empty string as default.
  buff='';
  
  if nargin<1,
    return;
  end
  
  if ischar(varargin{1})==0,
    if nargin<2,
      return;
    end
    fid=varargin{1};
    startx=2;
  else
    startx=1;
  end
  
  % Check if all input argument is char....
  allchar=1;
  for x=startx:nargin,
    if ischar(varargin{x})==0,
      allchar=0;
      break;
    end
  end
  
  
  % If all is char the do simple send and return
  if allchar,
    for x=startx:nargin,
      buff=[buff,varargin{x}];
    end
    retmsg=tcpipmex(2,fid,buff);
    
    if nargout>0,
      if retmsg<length(buff),
        varargout{1}=buff(retmsg+1:end);
      else
        varargout{1}='';
      end
    end
    return;
  end
  
  tmpn=tempname;
  dfid=fopen(tmpn,'w+','ieee-be');
  if dfid==-1,
    error ['Cant create tmp file: ',tmpn];
  end
  for x=startx:nargin,
    cl=class(varargin{x});
    fwrite(dfid,double(varargin{x}),cl);
  end
  frewind(dfid);
  while feof(dfid)==0,
    buff=[buff,char(fread(dfid,100000))];
    buff=tcpip_write(fid,buff);
  end
  fclose(dfid);
  delete(tmpn);
  if nargout>0,
    varargout{1}=buff;
  end
  return;
