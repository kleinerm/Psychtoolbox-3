function serialData = PR650serialread
% serialData = PR650serialread
%
% Reads data off the serial port until there is nothing left.  Returns an
% empty matrix if there is nothing to read.

global g_serialPort g_useIOPort;

if g_useIOPort
	data = IOPort('Read', g_serialPort);
	serialData = char(data);
	
	% If data exists keep reading off the port until there's nothing left.
	if ~isempty(serialData)
		tmpData = 1;
		while ~isempty(tmpData)
			WaitSecs(0.050);
			tmpData = IOPort('Read', g_serialPort);
			serialData = [serialData, char(tmpData)]; %#ok<AGROW>
		end
	end
else
	% Look for any data on the serial port.
	serialData = char(SerialComm('read', g_serialPort))';

	% If data exists keep reading off the port until there's nothing left.
	if ~isempty(serialData)
		tmpData = 1;
		while ~isempty(tmpData)
			WaitSecs(0.050);
			tmpData = char(SerialComm('read', g_serialPort))';
			serialData = [serialData, tmpData]; %#ok<AGROW>
		end
	end
end
