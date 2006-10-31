function [fid]=tcpip_open(varargin)
% fid = tcpip_open(adress_string,portnumber) Open remote tcp/ip connection.
%
% opens tcp ip network connection.
  
% Localhost adress as default adress.
  adress='127.0.0.1';
  
  % File ID set to "whant new" as default.
  fid=-1;
  
  if nargin<1,
    disp('Two or at least one parameter needed.');
    return;
  end
  
  if nargin<2,
    port=varargin{1};
  else
    port=varargin{2};
    adress=varargin{1};
  end
  
  fid=tcpipmex(1,fid,adress,port);
  return;

