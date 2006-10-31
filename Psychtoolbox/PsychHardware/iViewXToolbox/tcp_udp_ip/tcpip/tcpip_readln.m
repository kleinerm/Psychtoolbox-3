function [str,ok]=tcpip_readln(fid,len)
  
% [str,ok]=tcpip_readln(fid,len)
%
% fid   is file id.
% len   is maximum length to be read.
%
% Reads a line of charters terminated by newline character (LF).
% If the a full line (until LF) isn available at the
% moment a empty string will be returned.
% CR and LF characters will not be returned in the string.
%
% Returns ok set to 1 if a complete line whas recived.
%  
  ok=0;
  str=tcpipmex(4,fid,len);  
  if(length(str)),
    idx=find(str~=10 & str~=13); % Remove all 10 & 13
    if(length(idx)>0),
      str=str(idx);
      ok=1;
    end
  end
  return;
  
  
  
  
  
  
  
  
  
  
  
  

