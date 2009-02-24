% varargout = ColorCal(command, [varargin])
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
