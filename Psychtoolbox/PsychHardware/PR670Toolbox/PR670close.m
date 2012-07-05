function PR670close
% PR670close - Closes the PR-670 connection.
%
% Syntax:
% PR670close
%
% Description:
% Exits PR-670 remote mode, then closes the serial port connection.

global g_serialPort;

% Exit Remote Mode
PR670write('Q', 0);

% Close and clear serial port
if ~isempty(g_serialPort)
   IOPort('close', g_serialPort);
   g_serialPort = [];
end
