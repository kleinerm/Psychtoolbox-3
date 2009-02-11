function PR650close
% PR650close
%
% Close serial port used to talk to colorimeter.  Reset
% serial global.
%

global g_serialPort g_useIOPort;

if g_useIOPort
	if ~isempty(g_serialPort)
		IOPort('Close', g_serialPort);
		g_serialPort = [];
	end
else
	if ~isempty(g_serialPort)
		SerialComm('close', g_serialPort);
		g_serialPort = [];
	end
end
