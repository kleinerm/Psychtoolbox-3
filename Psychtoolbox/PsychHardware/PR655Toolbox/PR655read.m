function serialData = PR655read
% serialData = PR655read
%
% Reads data chars from PR655 until there is nothing left.  Returns an
% empty matrix if there is nothing to read.
% 
% 01/16/09    tbc   Adapted from PR650Toolbox for use with PR655
%

global g_serialPort;

% Look for any data on the serial port.
serialData = char(IOPort('read', g_serialPort));

% If data exists keep reading off the port until there's nothing left.
if ~isempty(serialData)
    tmpData = 1;
    while ~isempty(tmpData)
        WaitSecs(0.050);
        tmpData = char(IOPort('read', g_serialPort));
        serialData = [serialData, tmpData]; %#ok<AGROW>
    end
end

return
