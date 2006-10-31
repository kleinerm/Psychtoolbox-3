function data=tcpip_read(varargin)
  
% str=tcpip_read(fid,len,datatype)
% str=tcpip_read(fid,len)
%
% fid       is file id.
% len       is maximum length to be read.
% datatype  optional char array argument specifying what datatype to read.
%
% Read given number of characters (bytes) OR elements of other datatype
% if specified. Data vill be returned as specified datatype.
% Se  fwrite   for more about names of different datatypes.
%
% Operation will be interupted before len is reached if their
% is no more character to read at the moment. (non blocking)
%
%
% Problems:
%  
% Type conversion from the tcpipstream of 'char' is stream is slow!!!!
% If you use this functionality you schuold know their is better
% ways of doing it! u8read()   and u8write() are available at mathworks  
% user contributed ftp site and are probably better way of convert
% this stream of char to other data types afterwords in your script.
%    
% Somethink similar will be included in future inside the toolbox....
% 
% 
% EXAMPLE 1:
%
%    data=tcpip_read(fid,20,'double');
%
%          Reads (max) 20 elements of type 'double' from tcpip connection.
%
%
% EXAMPLE 2:
%
%    data=tcpip_read(fid,20);
%
%             Reads characters (bytes) from tcpip connection.
%
%
% EXAMPLE 3:
%
%    data=tcpip_read(fid,1024,'uint16');
%
%             Reads 1024 elements of datatype uint16.
%
  
  fid=-2;
  
  if nargin>1,
    fid=varargin{1};
    len=varargin{2};
  else
    len=varargin{1};
  end
  
  if nargin>2,
    % If other datatype then char. . .
    dt=varargin{3};
    tmpn=tempname;
    dfid=fopen(tmpn,'w+','ieee-be');
    if dfid==-1,
      error(['Cant open tmp file:',tmpn]);
    end
    % Get size of data type.
    fwrite(dfid,1,dt);
    sz=ftell(dfid);
    frewind(dfid);
    
    % Detect available size
    data=tcpipmex(5,fid,len*sz);
    len=floor(length(data)/sz);
    
    % Read available number of elements as chars.
    data=tcpipmex(3,fid,len*sz);
    
    % Convert datatype and byte order with help of a
    % file.
    fwrite(dfid,data(1:len*sz));
    frewind(dfid);
    data=fread(dfid,len,dt);
    fclose(dfid);
    delete(tmpn);
  else
    % Simple read with char as input data type.
    data=tcpipmex(3,fid,len);
  end
  return;
