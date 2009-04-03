function varargout = ColorCal2(command, varargin)
% varargout = ColorCal2(command, [varargin])
%
% Description:
% Interface function to communicate with the ColorCal II USB device.
%
% Required Input:
% command (string) - Command to send to the ColorCal device.  Commands are
%                    case insensitive.
%
% Optional Input:
% varargin - Argument(s) required for a subset of the ColorCal
%            commands.  Varies depending on the command.
%
% Optional Output:
% varargout - Value(s) returned for a subset of the ColorCal commands.
%
% Command List:
% 'DeviceInfo' - Retrieves the following device information: firmware
%      version number, 8 digit serial number, and firmware build number.
%
%      Example:
%      [firmwareVersion, serialNumber, buildNumber] = ColorCal('DeviceInfo');
% 'GetRawData' - Returns the raw data for all three light channels, the
%      contents of the zero correction array for all three channels, and
%      the current reading of the trigger ADC.  Returns a single struct
%      containing the following fields: Xdata, Xzero, Ydata, Yzero, Zzero,
%      Trigger.  All values are unformatted.
% 'LEDOn' - Turns the LED on.
% 'LEDOff' - Turns the LED off.
% 'MeasureXYZ' - Measures the tri-stimulus value of the current light.
%      Returns [X, Y, Z] in floating point format.  These values should be
%      corrected by multiplying them against the calibration matrix
%      typically stored in the 1st calibration matrix in the device.
%
%      Example: Retrieve the xyz values and correct them with the 1st
%               calibration matrix.
%      cMatrix = ColorCal('ReadColorMatrix');
%      [x y z] = ColorCal('MeasureXYZ');
%      correctedValues = cMatrix(1:3,:) * [x y z]';
% 'ReadColorMatrix' or 'ReadColourMatrix' - Retrieves all 3 color
%      calibration matrices from the device and returns them as a 9x3 matrix.
%      Each set of 3 rows represents a single calibration matrix.  All
%      values will be in floating point format.
% 'ResetEEProm' - Resets the non-volatile memory on the ColorCal to its
%      default value.  ALL CALIBRATION DATA IS ERASED.
% 'SetColorMatrix' or 'SetColourMatrix' - Writes data into one of the
%      calibration matrices.  Takes 2 additional arguments: matrix number, 
%      and a color matrix.  The ColorCal II has 3 calibration matrices
%      indexed in the range [0,2].  The color matrix must be 3x3 in
%      floating point format.  The values contained within the color matrix
%      will be converted to a Minolta format internally.
%
%      Example: Set the first color matrix to some random values.
%      ColorCal('SetColorMatrix', 0, rand(3,3));
% 'SetLEDFunction' - Controls whether the LED is illuminated when the
%      trigger signal is generated.  This state is stored in non-volatile
%      memory and will survive a power cycle.  Takes 1 additional argument:
%      0 or 1.  0 = LED not active when triggered, 1 = LED active when
%      triggered.
% 'SetTriggerThreshold' - Sets the threshold which must be exceeded by the
%      first derivative of the trigger ADC before a trigger pulse is
%      generated.  It is stored in non-volatile memory and will survive a
%      power cycle.  Takes 1 additional argument which is the trigger
%      threshold value.
% 'StartBootloader' - Causes the ColorCal to start its internal bootloader
%      in preparation for a firmware upgrade.
% 'ZeroCalibration' - Removes small zero errors in the electronic system of
%      the ColorCal device.  It reads the current light level and stores
%      the readings in a zero correction array.  All subsequent light
%      readings have this value subtracted from them before being returned.
%      This command is intended to be issued when the ColorCal is in the
%      dark.  Returns 1 if the command succeeds, 0 if it fails.  This
%      command must be run after every power cycle of the device.

persistent usbHandle;

varargout = {};

if nargin == 0
	error('Usage: varargout = ColorCal2(command, [varargin])');
end

if ~ischar(command)
	error('command must be a string.');
end

% Connect to the ColorCal2 if we haven't already.
if isempty(usbHandle)
	usbHandle = PsychHID('OpenUSBDevice', 2145, 4097);
	disp('- ColorCal 2 connected');
end

switch lower(command)
	case {'close', 'cls'}
		PsychHID('CloseUSBDevice', usbHandle);
		disp('- ColorCal2 closed');
		usbHandle = [];
		
	case {'ledon', 'lon'}
		bmRequestType = hex2dec('40');
		wValue = 2;
		wIndex = 0;
		wLength = 0;
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength);

	case {'ledoff', 'lof'}
		bmRequestType = hex2dec('40');
		wValue = 3;
		wIndex = 0;
		wLength = 0;
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength);
		
	case {'measurexyz', 'mes'}
		bmRequestType = hex2dec('40');
		wValue = 1;
		wIndex = 0;
		wLength = 32;
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength, uint8('MES'));
		bmRequestType = hex2dec('C0');
		outString = char(PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength));
		
		% Parse the output string.
		a = sscanf(outString, 'OK00,%6f,%6f,%6f');
		
		for i = 1:3
			varargout(i) = {a(i)};
		end
		
	case {'zerocalibration', 'uzc'}
		bmRequestType = hex2dec('40');
		wValue = 1;
		wLength = 32;
		wIndex = 0;
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength, uint8('UZC'));
		bmRequestType = hex2dec('C0');
		outString = char(PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength));
		
		% Parse the output string.
		switch outString(1:4)
			case 'OK00'
				varargout(1) = {true};
			case 'ER11'
				varargout(1) = {false};
			otherwise
				error('Failed to parse output string from the ColorCal 2');
		end
		
	case {'getrawdata', 'grd'}
		bmRequestType = hex2dec('C0');
		wValue = 4;
		wLength = 28;
		wIndex = 0;
		outData = PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength);
		
		% Each 4 bytes represents one value, so we must concatenate each
		% group of 4 bytes to get number we want.  We also need to swap the byte
		% order because Matlab will want big Endian numbers and the USB bus
		% operates in little Endian.
		extractedData = [];
		for i = 1:4:28
			x = uint32(0);
			for j = 0:3
				b = uint32(outData(i+j));
				b = bitshift(b, 8*j);
				x = bitor(x, b);
			end
			extractedData(end+1) = double(x); %#ok<NASGU>
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
		
		% Get data for each row in the color matrices.  There are 9 rows, 3
		% per matrix.
		for i = 1:9
			bmRequestType = hex2dec('40');
			wValue = 1;
			wLength = 32;
			wIndex = 0;
			
			% Grab row 'i' of the color matrix.
			PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength, uint8(sprintf('r0%d', i)));
			bmRequestType = hex2dec('C0');
			outString = char(PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength));
			
			% Parse the output string and store the values returned.
			colorMatrix(i, :) = sscanf(outString, 'OK00,%5d,%5d,%5d')';
		end

		% Convert the matrix values from Minolta format to floating
		% point.
		varargout(1) = {Minolta2Float(colorMatrix)};
		
	case {'deviceinfo', 'idr'}
		bmRequestType = hex2dec('40');
		wValue = 1;
		wLength = 32;
		wIndex = 0;
		
		% Get the ColorCal 2 device info.
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength, uint8('IDR'));
		bmRequestType = hex2dec('C0');
		outString = char(PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength));
		
		% Parse the device info string.
		x = sscanf(outString, 'OK00,1,%3d,100.10,%8d,%3d');
		
		% Set the output: rom version, serial number, and build number.
		varargout(1) = {x(1)};
		varargout(2) = {x(2)};
		varargout(3) = {x(3)};
		
	case {'reseteeprom', 'rse'}
		bmRequestType = hex2dec('40');
		wValue = 7;
		wLength = 0;
		wIndex = 0;
		
		% Send the reset command.
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength);
		
	case {'startbootloader', 'sbl'}
		bmRequestType = hex2dec('40');
		wValue = 99;
		wLength = 0;
		wIndex = 0;
		
		% Send the start bootloader command.
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength);
		
	case {'settriggerthreshold', 'stt'}
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
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength);
		
	case {'setledfunction', 'slf'}
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
		PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength);

	case {'setcolormatrix', 'setcolourmatrix', 'scm'}
		bmRequestType = hex2dec('40');
		wValue = 6;
		wLength = 18;
		
		% Check the number of input arguments.
		if nargin ~= 3
			error('Usage: ColorCal2(''SetColorMatrix'', matrixNumber, colorMatrix)');
		end
		
		% Make sure that the 2nd argument is a valid matrix number.
		matrixNumber = varargin{1};
		if isscalar(matrixNumber)
			if ~any(matrixNumber == [0 1 2])
				error('matrixNumber must be an integer in the range [0,2].');
			end
		else
			error('matrixNumber must be a scalar value in the range [0,2].');
		end
		
		% Make sure that argument 3 is a valid 3x3 color matrix.
		colorMatrix = varargin{2};
		if isnumeric(colorMatrix)
			if ndims(colorMatrix) ~= 2 || ~all(size(colorDims) == [3 3])
				error('colorMatrix must be a 3x3 matrix.'); 
			end
		else
			error('colorMatrix must be a numeric matrix');
		end
		
		% Convert the color matrix from floating point to Minolta format.
		colorMatrix = Float2Minolta(colorMatrix);
		
		% Send over the new color matrix.
		
		
	otherwise
		error('Invalid command: %s', command);
end
