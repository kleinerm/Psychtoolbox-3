function PortNumber = FindSerialPort(PortString, forIOPort)
% Syntax: PortNumber = FindSerialPort([NameOfIDString] [, forIOPort=0])
%
% Purpose: Find port number associated with a particular input string.
%
% If optional parameter 'PortString' is omitted, the routine will use a
% built-in list of default serial port names to search for. Otherwise it
% will try to find a serial port with a name matching 'PortString'.
%
% If optional flag 'forIOPort' is zero or left out, the routine will return
% a 'PortNumber' compatible with use of the SerialComm() serial port driver
% for MacOS/X with Matlab. If 'forIOPort' is non-zero, it will return a
% serial port device string in 'PortNumber' suitable for use with the new
% IOPort serial port driver which works on all operating systems and
% runtime environments.
%
% History: 11/21/07   mpr   listed to port
%           1/16/08   mpr   added support for keyspan adapter
%           2/07/09   mk    Add code for use with IOPort instead of SerialComm.

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

if ~nargin || isempty(PortString)	
	% List of serial port devices to look for.  These should all be lower
	% case.
    if ~IsWin
        PortDB = { lower('usa19'), lower('keyserial1')};
    else
        PortDB = { 'COM5' };
    end
    selectionType = 'default';
else	
	% Make the port string lowercase for easier string comparison later.
	PortDB = {lower(PortString)};
	selectionType = 'requested';
end

if nargin < 2 || isempty(forIOPort)
    forIOPort = 0;
end

if IsOS9
	fprintf('I think that you are using Mac OS 9.  If so, you should not be trying\n');
	fprintf('to use FindSerialPort.  In fact, you shouldn''t need to.  I think that\n');
	fprintf('PsychSerial is your friend.\n\n');
	error('Please upgrade to OS X or use some other method to establish serial communication.');
end

if ~(IsOSX || IsWin || IsLinux)
	fprintf('It appears that you are using an operating system other than Mac OS/X,\n');
	fprintf('Linux or Windows. I hope that you are using some other Unix variant.\n');
	fprintf('If so, FindSerialPort may work for you.  I have not tested it.  You are about to...\n');
end

if IsWin
    % Don't have enumeration code yet. Just pass through what we got:
    PortNumber = upper(char(PortDB));
    return;
end

% Either Linux, OS/X or (hopefully) some other compatible Posix OS:

% Get a list of any serial devices attached to the computer.
if IsOSX
    ThePortDevices = dir('/dev/cu*');
else
    % Linux or some other Posix OS. Use search pattern for Linux:
    ThePortDevices = dir('/dev/tty*');
end

% Check to see if any serial devices were found.
if isempty(ThePortDevices)
	error('\n%s\n%s\n', ...
		'No serial devices were found.  Make sure the serial device is plugged', ...
		'in and the drivers installed.');
end

% For each serial type in the PortDB cell array, see if any attached serial
% devices match.
FoundIndices = [];
for i = 1:length(PortDB)
	for j = 1:length(ThePortDevices)
		if ~isempty(strfind(lower(ThePortDevices(j).name), PortDB{i}))
			FoundIndices(end+1) = j; %#ok<AGROW>
		end
	end
end
numPortsFound = length(FoundIndices);

if numPortsFound == 0 % No port matches.
	error('\n%s%s%s\n%s\n%s\n%s\n%s', ...
		'No ports found that match following ', selectionType, ' types(s)', ...
		makeDesiredPortTypesString(PortDB), ...
		'This is a list of the port(s) found:', ...
		makeFoundDevicesString(ThePortDevices), ...
		'Is your RS-232 adapter connected to your computer?');
elseif numPortsFound > 1 % Too many port matches.
	error('\n%s%s%s\n%s\n\n%s\n%s', ...
		'More than one port name matched the ', selectionType, ' type(s)', ...
		'A more specific port type is required.', ...
		'This is a list of the port(s) found:', ...
		makeFoundDevicesString(ThePortDevices)); %#ok<SPERR>
end

if ~forIOPort
    % Old-Style, using SerialComm:
    % Convert the port name into a port number.
    if IsOctave
        error('This routine is not supported for use with SerialComm under Octave. Use forIOPort=1 flag instead!');
    end
    portString = sprintf('/dev/%s', ThePortDevices(FoundIndices).name);
    PortNumber = SerialComm('name2number', portString);
else
    % New-Style: Path name of port itself for use with IOPort:
    if IsOctave
        PortNumber = sprintf('%s', ThePortDevices(FoundIndices).name);
    else
        PortNumber = sprintf('/dev/%s', ThePortDevices(FoundIndices).name);
    end
end

return;

function s = makeDesiredPortTypesString(PortDB)
s = [];
for i = 1:length(PortDB)
	s = [char(s) sprintf('\t\t%s\n', PortDB{i})]; %#ok<AGROW>
end

function s = makeFoundDevicesString(ThePortDevices)
s = [];

if IsOSX
    prefix = 'cu.';
else
    prefix = 'tty';
end

if IsOctave
    prefix = ['/dev/' prefix '%s'];
else
    prefix = [prefix '%s'];
end

for k = 1:length(ThePortDevices)
	ThisDevice = sscanf(ThePortDevices(k).name,prefix);
	s = [char(s) sprintf('\t\t%s\n',ThisDevice)]; %#ok<AGROW>
end
