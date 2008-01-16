function PortNumber = FindSerialPort(PortString)
% Syntax: PortNumber = FindSerialPort([NameOfIDString])
%
% Purpose: Find port number associated with a particular input string.
%
% History: 11/21/07   mpr   listed to port
%           1/16/08   mpr   added support for keyspan adapter
%
% Function is slightly more general than name suggests because name reflects the
% origin of this function for finding the usb to serial port used for driving a
% device like the PR650 spectroradiometer I wrote this for.  NameOfIDString
% defaults to 'usbserial'.  I think any USB to RS-232 adapter that makes
% use of the 2303 chip (which seems to cover all of them on the market
% right now) will have the device name usbserial.  But if I'm wrong, then
% someone else will probably find out before I do.  -- mpr

% When I started writing this function I knew about PR650init, and I wanted
% to be able to use that function without knowing the port number.  I also
% found that calling that function directly leads to inconsistent
% (generally ineffective) behavior.  For some reason wrapping CMCheckInit
% solves the inconsistent/ineffective problem.  The place where I put the
% call to this function in CMCheckInit is inside a condition testing that
% user is on a machine using OS X, so the error checking here isn't
% necessary.   I also didn't know about IsOSX, IsWin, and IsOS9 when I
% wrote this -- I used the checks in DownloadPsychToolbox as my guide, and
% obviously that function can't make use of PsychToolbox functions...  So
% this could be improved even without making it work for other OSs, but I
% don't have an incentive to make improvements. -- mpr 11/21/07

if ~nargin | isempty(PortString)
	% if user doesn't know or doesn't care whether we find PL-2303 or keyspan
	% adapter:
	AllowAlternateStrings = 1;
	PortString = 'usbserial';
	AltPortString = 'usa19';
else
	AllowAlternateStrings = 0;
end

if ~isempty(strfind(computer,'PCWIN'))
	fprintf('It appears that you are using a Windows operating system.\n');
	fprintf('Among many other problems that will cause you is that you cannot\n');
	fprintf('use ''FindSerialPort'' because that function was written for Mac OS X.\n\n');
	error('You are using the wrong operating system.');
end

if ~isempty(strfind(computer,'MAC2'))
	fprintf('I think that you are using Mac OS 9.  If so, you should not be trying\n');
	fprintf('to use FindSerialPort.  In fact, you shouldn''t need to.  I think that\n');
	fprintf('PsychSerial is your friend.\n\n');
	error('Please upgrade to OS X or use some other method to establish serial communication.');
end

if isempty(strfind(computer,'MAC'))
	fprintf('It appears that you are using an operating system other than Mac OS\n');
	fprintf('or Windows.  I hope that you are using Linux.  If so, FindSerialPort\n');
	fprintf('may work for you.  I have not tested it.  You are about to...\n');
end

ThePortDevices = dir('/dev/cu*');

PortTypeFound = 0;
FoundIndices = [];
for k=1:length(ThePortDevices)
	if ~isempty(regexpi(ThePortDevices(k).name,PortString)) || (AllowAlternateStrings && ~isempty(regexpi(ThePortDevices(k).name,AltPortString)))
		PortTypeFound = PortTypeFound+1;
		FoundIndices = k;
	end
end

if ~PortTypeFound
	fprintf('I could not find any ports with names that match %s.  Sorry!\n',PortString);
	if isempty(ThePortDevices)
		fprintf('I''m afraid I didn''t find any serial ports.  Apparently my assumption about\n');
		fprintf('how ports will be named is not correct.  It is up to you to fix that.\n\n');
	else
		fprintf('This is a list of the port (or ports) I found:\n\n');
		for k=1:length(ThePortDevices)
			ThisDevice = sscanf(ThePortDevices(k).name,'cu.%s');
			fprintf('\t\t%s\n',ThisDevice);
		end
		fprintf('\n\n');
		fprintf('Is your USB to RS-232 adapter connected to your computer?\n');
	end
	error('No matching port found');
elseif PortTypeFound > 1
	fprintf('I found more than one port name that matched your input string.  I fear\n');
	fprintf('you will have to edit code or command line to be more specific.  These\n');
	fprintf('are the names of the devices that matched:\n\n');
	for k=1:PortTypeFound
		ThisDevice = sscanf(ThePortDevices(FoundIndices(k)),'cu.%s');
		fprintf('\t\t%s\n',ThisDevice);
	end
	fprintf('\n');
	error('Too many port name matches.');
end

% Some of the logic below could probably be thought out better... Within the
% while loop, I want software to bump up PortNumber if it appears that the
% PortNumber being evaluated is not the one we're looking for.  But if the
% port is already open when I try to get that information, then I (may) want to
% close it and go back through the loop again with the same PortNumber.  This
% code does that, but it could probably do it better.  I'm also a bit afraid
% about configuring ports (9600,n,8,1) when I don't know what they are.  I
% suspect most ports can and will be run at higher baud rates here in the
% 21st century.  But so far I have not had any problems opening ports this
% way even when I suspect it is inappropriate. -- MPR  11/21/07

NotAllPortsTested = 1;
PortNumber = 1;
while NotAllPortsTested
	StdOutString  =[];
	try
		StdOutString = lower(evalc(['SerialComm(''open'',' int2str(PortNumber) ',''9600,n,8,1'');']));
	catch
		if strfind(lasterr,'port number out of range')
			NotAllPortsTested = 0;
		end
	end
	if ~isempty(StdOutString)
		if ~isempty(strfind(StdOutString,'already open'))
			% found that automatically closing port broke established communication,
			% so this is an effort to test if we are talking to the device we want
			% to talk to...
			try
				evalc(['SerialComm(''write'', ' int2str(PortNumber) ', [''b3'' char(10)]);']);
				StartTime = GetSecs;
				retval = [];
				while isempty(retval) & GetSecs-StartTime < 10
					retval = PR650serialread;
				end

				if isempty(retval)
					evalc(['SerialComm(''close'',' int2str(PortNumber) ');']);
				else
					return;
				end
			catch
				evalc(['SerialComm(''close'',' int2str(PortNumber) ');']);
			end

		elseif ~isempty(strfind(StdOutString, PortString)) || ...
				~isempty(strfind(StdOutString, AltPortString))
			return;
		else
			if strfind(StdOutString, 'opened device')
				evalc(['SerialComm(''close'',' int2str(PortNumber) ');']);
			end
			PortNumber = PortNumber+1;
		end
	else
		PortNumber = PortNumber+1;
	end
end

% return upon finding of PortString means function should not get here unless it
% looked through all possible ports and failed to find one that matched.
fprintf('I''m afraid that I could not find a port number associated with the \n');
fprintf('port string.  I did not think this was possible, so there must be\n');
fprintf('something that I don''t know about SerialComm or your operating system.\n');
error('Could not match port number to port string.  Sorry!');
