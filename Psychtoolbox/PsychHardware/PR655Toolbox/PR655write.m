function PR655write(cmdStr)
% Write a string of chars to PR655. Add [CR] terminator if necessary
% 
% 01/16/09    tbc   Wrote it.
%

global g_serialPort;

% Check for Terminator and add if necessary
if cmdStr(end)~=char(13)
    cmdStr = [cmdStr char(13)];
end

% Write sequence of chars to PR655
for i = 1:length(cmdStr)
    IOPort('write', g_serialPort, upper(cmdStr(i)));
    pause(0.05)
end

return
