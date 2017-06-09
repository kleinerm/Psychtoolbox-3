function varargout = ColorCal2(command, varargin)
% varargout = ColorCal2(command, [varargin])
%
% Description:
% Interface function to communicate with the ColorCal2 USB device.
%
%
% WINDOWS: On MS-Windows this driver needs to use a virtual COM port method
% to communicate, instead of the more feature-rich USB method on Linux or
% OSX. As a consequence, only a subset of functions is supported, as listed
% below, and device detection may be less reliable if you have other serial
% port devices or virtual COM port devices connected, unless you help the
% driver by telling it the right COM port: The device will show up as an
% additional serial port on the system. This driver will communicate with
% the device by establishing a serial port connection via that serial port.
% The driver will check for the existence of a configuration file named
% [PsychtoolboxConfigDir 'ColorCal2Config.txt']. If the file exists and is
% readable, then presence of a serial port name in the first line of that
% text configuration file will use the serial port with that name for
% communication, otherwise the driver will try to auto-detect the proper
% serial port for communication. E.g., if the file would contain one line
% with the text COM3 then virtual COM port 3 would be used for serial port
% communication.
%
%
% OSX: OSX 10.12 or later is required for this function to work. For older
% OSX versions, look for instructions on the CRS Ltd. website.
%
%
% LINUX: If you want to use this function without the need to run Matlab or
% Octave as root user (i.e., without need for root login or the sudo
% command), please run PsychLinuxConfiguration once (this will happen
% automatically if you install, update or setup Psychtoolbox via
% DownloadPsychtoolbox, UpdatePsychtoolbox or SetupPsychtoolbox), or
% alternatively copy the file
% Psychtoolbox/PsychHardware/ColorCal2/60-cambridgeresearch-permissions.rules
% into the folder /etc/udev/rules.d/ on your system. This one-time copy
% will require administrator privileges, but after that, any user should be
% able to use the ColorCal devices or other CRS Ltd. devices without
% special permissions.
%
%
% Required Input:
% command (string) - Command to send to the ColorCal2 device. Commands are
%                    case insensitive.
%
% Optional Input:
% varargin - Argument(s) required for a subset of the ColorCal2
%            commands. Varies depending on the command.
%
% Optional Output:
% varargout - Value(s) returned for a subset of the ColorCal2 commands.
%
% Command List for all operating systems:
%
% 'DeviceInfo' - Retrieves the following device information in a struct:
%      firmware version number, 8 digit serial number, and firmware build
%      number. The struct's fields are romVersion, serialNumber,
%      buildNumber.
%
%      Example:
%      devInfo = ColorCal2('DeviceInfo');
%
% 'MeasureXYZ' - Measures the tri-stimulus value of the current light.
%      Returns a struct with x, y, and z in floating point format.  These
%      values should be corrected by multiplying them against the calibration
%      matrix typically stored in the 1st calibration matrix in the device.
%
%      Example: Retrieve the xyz values and correct them with the 1st
%               calibration matrix.
%      cMatrix = ColorCal2('ReadColorMatrix');
%      s = ColorCal2('MeasureXYZ');
%      correctedValues = cMatrix(1:3,:) * [s.x s.y s.z]';
%
% 'ReadColorMatrix' or 'ReadColourMatrix' - Retrieves all 3 color
%      calibration matrices from the device and returns them as a 9x3 matrix.
%      Each set of 3 rows represents a single calibration matrix.  All
%      values will be in floating point format.
%
% 'ZeroCalibration' - Removes small zero errors in the electronic system of
%      the ColorCal2 device.  It reads the current light level and stores
%      the readings in a zero correction array.  All subsequent light
%      readings have this value subtracted from them before being returned.
%      This command is intended to be issued when the ColorCal2 is in the
%      dark.  Returns 1 if the command succeeds, 0 if it fails.  This
%      command must be run after every power cycle of the device.
%
% 'Close' - Close connection to device. A 'clear all' or 'clear ColorCal2' or
%      quitting Octave or Matlab will also close the connection, so this is
%      not strictly needed.
%
% All the following commands are not supported on MS-Windows, only on Linux and OSX:
%
%
% 'GetRawData' - Returns the raw data for all three light channels, the
%      contents of the zero correction array for all three channels, and
%      the current reading of the trigger ADC.  Returns a single struct
%      containing the following fields: Xdata, Xzero, Ydata, Yzero, Zdata,
%      Zzero, Trigger.  All values are unformatted.
%
% 'LEDOn' - Turns the LED on.
%
% 'LEDOff' - Turns the LED off.
%
% 'SetLEDFunction' - Controls whether the LED is illuminated when the
%      trigger signal is generated.  This state is stored in non-volatile
%      memory and will survive a power cycle.  Takes 1 additional argument:
%      0 or 1.  0 = LED not active when triggered, 1 = LED active when
%      triggered.
%
% 'SetTriggerThreshold' - Sets the threshold which must be exceeded by the
%      first derivative of the trigger ADC before a trigger pulse is
%      generated.  It is stored in non-volatile memory and will survive a
%      power cycle.  Takes 1 additional argument which is the trigger
%      threshold value.
%
% 'StartBootloader' - Causes the ColorCal2 to start its internal bootloader
%      in preparation for a firmware upgrade.
%

% *** These functions are for internal use only.  Using these functions may
% *** brick your device.  These functions are commented out below, so they
% *** must be uncommented before they can be run.
%
% 'ResetEEProm' - Resets the non-volatile memory on the ColorCal2 to its
%      default value.  ALL CALIBRATION DATA IS ERASED.
%
% 'SetColorMatrix' or 'SetColourMatrix' - Writes data into one of the
%      calibration matrices.  Takes 2 additional arguments: matrix number,
%      and a color matrix.  The ColorCal2 has 3 calibration matrices
%      indexed in the range [0,2].  The color matrix must be 3x3 in
%      floating point format.  The values contained within the color matrix
%      will be converted to a Minolta format internally.
%
%      Example: Set the first color matrix to some random values.
%      ColorCal2('SetColorMatrix', 0, rand(3,3));

% History:
% 03.04.2009  Written (Chris Broussard).
% 18.08.2010  Make useBigEndian persistent. Reported by Ian A. ("tatooed.tentackle@yahoo.com")
% 19.01.2013  Make compatible with GNU/Octave-3 (Mario Kleiner).
% 06.06.2017  Make compatible with Windows via serial port implementation (Mario Kleiner).
%             This uses code derived from the Matlab sample code by CRS, incorporated in a
%             modified form with permission of CRS under our usual MIT license.

persistent usbHandle;
persistent portHandle;
persistent useBigEndian;

varargout = {};

if nargin == 0
    error('Usage: varargout = ColorCal2(command, [varargin])');
end

if ~ischar(command)
    error('command must be a string.');
end

% Connect to the ColorCal2 if we haven't already.
if isempty(usbHandle) && isempty(portHandle)
    if ~IsWin
        % Not Windows: Can use efficient USB control transfers:
        LoadPsychHID;
        usbHandle = PsychHID('OpenUSBDevice', 2145, 4097);
    else
        % Windows: Need to use limited virtual COM port method:
        portString = [];
        configfile = [PsychtoolboxConfigDir 'ColorCal2Config.txt'];
        if ~exist(configfile, 'file')
            fprintf('ColorCal2: Could not find a ColorCal2 config file under [%s]. Trying to probe correct virtual COM port.\n', configfile);
        else
            % File exists -> We want to access a Bits#. Parse file for a port name string:
            fid = fopen(configfile);
            fileContentsWrapped = fgets(fid);
            fclose(fid);

            % Port spec available?
            if ~isempty(fileContentsWrapped) && ischar(fileContentsWrapped)
                % Yes: Assign namestring for port.
                portString = deblank(fileContentsWrapped);
                fprintf('ColorCal2: Connecting to ColorCal2 device via serial port [%s], as provided by configuration file [%s].\n', portString, configfile);
            end
        end

        % On Windows we use virtual COM ports, aka serial port
        % communication instead, to work around the lack of a
        % proper USB HID driver.
        verb = IOPort('Verbosity', 1);
        portString = FindSerialPort(portString, 1);
        portHandle = IOPort('OpenSerialPort', portString, 'ReceiveTimeout=5.0');
        IOPort('Verbosity', verb);
    end
    disp('- ColorCal2 connected');

    % Determine the byte order of the machine.
    x = typecast(uint32(1), 'uint8');
    if x(4) == 1
        useBigEndian = true;
    else
        useBigEndian = false;
    end
end

% Currently, we don't vary this value.
bRequest = 0;

switch lower(command)
    case {'close', 'cls'}
        if ~isempty(usbHandle)
            PsychHID('CloseUSBDevice', usbHandle);
        end

        if ~isempty(portHandle)
            IOPort('Close', portHandle);
        end

        disp('- ColorCal2 closed');
        usbHandle = [];
        portHandle = [];

    case {'ledon', 'lon'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('40');
            wValue = 2;
            wIndex = 0;
            wLength = 0;
            PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength);
        else
            warning('ColorCal2(''LEDOn''): LED control unsupported on this platform.');
        end
    case {'ledoff', 'lof'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('40');
            wValue = 3;
            wIndex = 0;
            wLength = 0;
            PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength);
        else
            warning('ColorCal2(''LEDOff''): LED control unsupported on this platform.');
        end

    case {'measurexyz', 'mes'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('40');
            wValue = 1;
            wIndex = 0;
            wLength = 3;
            PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength, uint8('MES'));
            bmRequestType = hex2dec('C0');
            wLength = 32;
            outString = char(PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength));
            % Parse the output string.
            a = sscanf(outString, 'OK00,%6f,%6f,%6f');
        else
            a = ColorCALIIGetValues(portHandle);
        end

        s.x = a(1);
        s.y = a(2);
        s.z = a(3);

        varargout(1) = {s};

    case {'zerocalibration', 'uzc'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('40');
            wValue = 1;
            wLength = 3;
            wIndex = 0;
            PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength, uint8('UZC'));
            bmRequestType = hex2dec('C0');
            wLength = 32;
            outString = char(PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength));
        else
            IOPort('Write', portHandle, uint8(['UZC' 13]));
            outString = readSerial(portHandle);
            outString = readSerial(portHandle);
        end

        % Parse the output string.
        switch outString(1:4)
            case 'OK00'
                varargout(1) = {true};
            case 'ER11'
                varargout(1) = {false};
            otherwise
                error('Failed to parse output string from the ColorCal2');
        end

    case {'getrawdata', 'grd'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('C0');
            wValue = 4;
            wLength = 28;
            wIndex = 0;
            outData = PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength);

            % Each 4 bytes represents one value, so we must concatenate each
            % group of 4 bytes to get number we want.  On big endian systems we
            % must swap the byte order because the USB bus works in little
            % endian mode.
            extractedData = [];
            for i = 1:4:28
                % Read 4 bytes of data and convert it into an uint32 value.
                y = typecast(outData(i:i+3), 'uint32');

                % Swap the byte order if on a big endian machine.
                if useBigEndian
                    y = swapbytes(y);
                end

                extractedData(end+1) = double(y); %#ok<NASGU>
            end
        else
            error('ColorCal2(''GetRawData''): Function unsupported on this platform.');
        end

        % Create a struct to hold the results.
        d.Xdata = extractedData(1);
        d.Xzero = extractedData(2);
        d.Ydata = extractedData(3);
        d.Yzero = extractedData(4);
        d.Zdata = extractedData(5);
        d.Zzero = extractedData(6);
        d.Trigger = extractedData(7);

        varargout(1) = {d};

    case {'readcolormatrix', 'readcolourmatrix', 'rcm'}
        colorMatrix = zeros(9, 3);

        if ~isempty(usbHandle)
            % Get data for each row in the color matrices.  There are 9 rows, 3
            % per matrix.
            for i = 1:9
                bmRequestType = hex2dec('40');
                wValue = 1;
                wLength = 3;
                wIndex = 0;

                % Grab row 'i' of the color matrix.
                PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength, uint8(sprintf('r0%d', i)));
                bmRequestType = hex2dec('C0');
                wLength = 32;
                outString = char(PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength));

                % Parse the output string and store the values returned.
                colorMatrix(i, :) = sscanf(outString, 'OK00,%d,%d,%d')';
            end
        else
            colorMatrix = getColorCALIICorrectionMatrix(portHandle);
        end

        % Convert the matrix values from Minolta format to floating point.
        varargout(1) = {Minolta2Float(colorMatrix)};

    case {'deviceinfo', 'idr'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('40');
            wValue = 1;
            wLength = 3;
            wIndex = 0;

            % Get the ColorCal2 device info.
            PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength, uint8('IDR'));
            bmRequestType = hex2dec('C0');
            wLength = 32;
            outString = char(PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength));
        else
            IOPort('Write', portHandle, uint8(['IDR' char(13)]));
            readSerial(portHandle);
            outString = readSerial(portHandle);
        end

        % Parse the device info string.
        x = sscanf(outString, 'OK00,1,%d,100.10,%d,%d');

        % Set the output: rom version, serial number, and build number.
        dInfo.romVersion = x(1);
        dInfo.serialNumber = x(2);
        dInfo.buildNumber = x(3);
        varargout(1) = {dInfo};

%    case {'reseteeprom', 'rse'}
%        if ~isempty(usbHandle)
%             bmRequestType = hex2dec('40');
%             wValue = 7;
%             wLength = 0;
%             wIndex = 0;
%
%             % Send the reset command.
%             PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength);
%        else
%            warning('ColorCal2(''ResetEEProm''): Function unsupported on this platform.');
%        end

    case {'startbootloader', 'sbl'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('40');
            wValue = 99;
            wLength = 0;
            wIndex = 0;

            % Send the start bootloader command.
            PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength);
        else
            warning('ColorCal2(''StartBootloader''): Function unsupported on this platform.');
        end
    case {'settriggerthreshold', 'stt'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('40');
            wValue = 8;
            wLength = 0;

            % Make sure a trigger value was passed.
            if nargin ~= 2
                error('Usage: ColorCal2(''SetTriggerThreshold'', triggerValue)');
            end

            % Make sure the trigger value is scalar.
            wIndex = varargin{1};
            if ~isscalar(wIndex)
                error('triggerValue must be scalar.');
            end

            % Send the set trigger threshold command.
            PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength);
        else
            warning('ColorCal2(''SetTriggerThreshold''): Function unsupported on this platform.');
        end
    case {'setledfunction', 'slf'}
        if ~isempty(usbHandle)
            bmRequestType = hex2dec('40');
            wValue = 9;
            wLength = 0;

            % Make sure a LED function value was passed.
            if nargin ~= 2
                error('Usage: ColorCal2(''SetLEDFunction'', ledFunctionValue)');
            end

            % Make sure that the function value is 0 or 1.
            wIndex = varargin{1};
            if ~isscalar(wIndex)
                error('ledFunctionValue must be a scalar.');
            end
            if ~any(wIndex == [0 1])
                error('ledFunctionValue must be 0 or 1.');
            end

            % Send the new LED function value.
            PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength);
        else
            warning('ColorCal2(''SetLEDFunction''): Function unsupported on this platform.');
        end

%     case {'setcolormatrix', 'setcolourmatrix', 'scm'}
%         if isempty(usbHandle)
%             error('SetColorMatrix is not supported on this platform.');
%         end
%
%         bmRequestType = hex2dec('40');
%         wValue = 6;
%         wLength = 18;
%
%         % Check the number of input arguments.
%         if nargin ~= 3
%             error('Usage: ColorCal2(''SetColorMatrix'', matrixNumber, colorMatrix)');
%         end
%
%         % Make sure that the 2nd argument is a valid matrix number.
%         matrixNumber = varargin{1};
%         if isscalar(matrixNumber)
%             if ~any(matrixNumber == [0 1 2])
%                 error('matrixNumber must be an integer in the range [0,2].');
%             end
%         else
%             error('matrixNumber must be a scalar value in the range [0,2].');
%         end
%         wIndex = matrixNumber;
%
%         % Make sure that argument 3 is a valid 3x3 color matrix.
%         colorMatrix = varargin{2};
%         if isnumeric(colorMatrix)
%             if ndims(colorMatrix) ~= 2 || ~all(size(colorMatrix) == [3 3])
%                 error('colorMatrix must be a 3x3 matrix.');
%             end
%         else
%             error('colorMatrix must be a numeric matrix');
%         end
%
%         % Convert the color matrix from floating point to Minolta format.
%         colorMatrix = Float2Minolta(colorMatrix);
%
%         % Repackage the color matrix into a 8 bit vector where each matrix
%         % value is represented by 2 bytes.
%         newColorMatrix = uint16([colorMatrix(1,:) colorMatrix(2,:) colorMatrix(3,:)]);
%         if useBigEndian
%             newColorMatrix = swapbytes(newColorMatrix);
%         end
%         newColorMatrix = typecast(newColorMatrix, 'uint8');
%
%         % Send over the new color matrix.
%         PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, ...
%             wValue, wIndex, wLength, newColorMatrix);

    otherwise
        error('Invalid command: %s', command);
end

end

function readString = readSerial(portHandle)
    vals = [];
    while length(vals) < 2 || vals(end-1) ~= 10 || vals(end) ~= 13
        curval = IOPort('Read', portHandle, 1, 1);
        vals = [vals, curval];
    end

    readString = char(vals(1:end-2));
end

function myCorrectionMatrix = getColorCALIICorrectionMatrix(portHandle)
    % Obtains the individual correction matrix for the ColorCAL II, to be used
    % to translate measured readings to calibrated XYZ values
    try
        % Cycle through the 3 times 3 rows of the 3 vertically concatenated correction matrices.
        for j = 1:9
            % whichColumn is to indicate the column the current value is to be
            % written to.
            whichColumn = 1;

            % Commands are passed to the ColorCAL II as though they were being
            % written to a text file, using fprintf. The commands 'r01', 'r02'
            % and 'r03' will return the 1st, 2nd and 3rd rows of the correction
            % matrix respectively. Note the '13' represents the terminator
            % character. 13 represents a carriage return and should be included
            % at the end of every command to indicate when a command is
            % finished.
            IOPort('Write', portHandle, uint8(['r0' num2str(j) char(13)]));

            % This command returns a blank character at the start of each line
            % by default that can confuse efforts to read the values. Therefore
            % use dummy read once to remove this character.
            readSerial(portHandle);

            % Read the returned data:
            dataLine = readSerial(portHandle);

            % The returned dataLine will be returned as a string of characters
            % in the form of 'OK00, 8053,52040,50642'. Therefore loop through
            % each character until a O is found to be sure of the start
            % position of the data.
            for k = 1:length(dataLine)
                % Once an O has been found, assign the start position of the
                % numbers to 5 characters beyond this (i.e. skipping the
                % 'OKOO,').
                if dataLine(k) == 'O'
                    myStart = k+5;

                    % A comma (,) indicates the start of a value. Therefore if
                    % this is found, the value is the number formed of the next
                    % 5 characters.
                elseif dataLine(k) == ','
                    myEnd = k+5;

                    % Using j to indicate the row position and whichColumn to
                    % indicate the column position, convert the 5 characters to
                    % a number and assign it to the relevant position.
                    myCorrectionMatrix(j, whichColumn) = str2num(dataLine(myStart:myEnd));

                    % reset myStart to k+6 (the first value of the next number)
                    myStart = k+6;

                    % Add 1 to the whichColumn value so that the next value
                    % will be saved to the correct location.
                    whichColumn = whichColumn + 1;

                end
            end
        end
    catch
        disp('Error');
    end
end

function myMeasureMatrix = ColorCALIIGetValues(portHandle)
    % Takes a reading. These values need to be transformed by above correction
    % matrix to obtain XYZ values
    whichColumn = 1;

    try
        IOPort('Write', portHandle, uint8(['MES' 13]));

        readSerial(portHandle);
        dataLine = readSerial(portHandle);

        % The returned dataLine will be returned as a string of characters in
        % the form of 'OK00,242.85,248.11, 89.05'. In case of additional blank
        % characters before or after the relevant information, loop through
        % each character until a O is found to be sure of the start position of
        % the data.
        for k = 1:length(dataLine)
            % Once an O has been found, assign the start position of the
            % numbers to 5 characters beyond this (i.e. skipping th 'OKOO,')
            if dataLine(k) == 'O'
                myStart = k+5;

                % A comma (,) indicates the start of a value. Therefore if this
                % is found, the value is the number formed of the next 6
                % characters
            elseif dataLine(k) == ','
                myEnd = k+6;

                % Using k to indicate the row position and whichColumn to
                % indicate the column position, convert the 5 characters to a
                % number and assign it to the relevant position.
                myMeasureMatrix(whichColumn) = str2num(dataLine(myStart:myEnd));

                % reset myStart to k+7 (the first value of the next number)
                myStart = k+7;

                % Add 1 to the whichColumn value so that the next value will be
                % saved to the correct location.
                whichColumn = whichColumn + 1;

            end
        end
    catch
        disp('Error');
    end
end
