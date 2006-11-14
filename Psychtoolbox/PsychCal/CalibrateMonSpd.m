% CalibrateMonSpd
%
% Calling script for monitor calibration.  Assumes
% you have CMCheckInit/MeasSpd functions that initialize
% measurement hardware and return a measured spectral
% power distribution respectively.
%
% 7/7/98  dhb  Wrote from generic.
%         dhb  dacsize/driver filled in by hand, Screen fails to return it.
% 4/7/99  dhb  NINEBIT -> NBITS.
%         dhb  Wrote version for Radius 10 bit cards.
% 4/23/99 dhb  Change wavelength sampling to 380 4 101, PR-650 native.
% 9/22/99 dhb, mdr  Define boxSize.
% 8/11/00 dhb  Save mon in rawdata.
% 8/18/00 dhb  More descriptive information saved.
% 8/20/00 dhb  Automatic check for RADIUS and number of DAC bits.
% 9/10/00 pbe  Added option to blank another screen while measuring. 
% 2/27/02 dhb  Various small fixes, including Radeon support.
%         dhb  Change noMeterAvail to whichMeterType.
% 11/08/06 cgb, dhb  OS/X.

% Create calibration structure;
cal = [];

% Blank other screens?
blankOtherScreen = 1;

% Script parameters
whichScreen = max(Screen('Screens'));
whichMeterType = 1;
cal.describe.leaveRoomTime = 10;
cal.describe.nAverage = 2;  
cal.describe.nMeas = 30;
cal.describe.boxSize = 400;
cal.nDevices = 3;
cal.nPrimaryBases = 2;
switch whichMeterType
	case {0,1}
		cal.describe.S = [380 4 101];
	case 2
		cal.describe.S = [380 1 401];
    otherwise
		cal.describe.S = [380 4 101];
end
cal.manual.use = 0;
		
% Enter screen
defaultScreen = whichScreen;
whichScreen = input(sprintf('Which screen to calibrate [%g]: ', defaultScreen));
if isempty(whichScreen)
	whichScreen = defaultScreen;
end
cal.describe.whichScreen = whichScreen;

% Blank screen
defaultBlankOtherScreen = 0;
blankOtherScreen = input(sprintf('Do you want to blank another screen? (1 for yes, 0 for no) [%g]: ', defaultBlankOtherScreen));
if isempty(blankOtherScreen)
	blankOtherScreen = defaultBlankOtherScreen;
end
if blankOtherScreen
	defaultBlankScreen = 2;
	whichBlankScreen = input(sprintf('Which screen to blank [%g]: ', defaultBlankScreen));
	if isempty(whichBlankScreen)
		whichBlankScreen = defaultBlankScreen;
	end
	cal.describe.whichBlankScreen = whichBlankScreen;
end

% Find out about screen
cal.describe.dacsize = ScreenDacBits(whichScreen);
nLevels = 2^cal.describe.dacsize;

% Prompt for background values.  The default is a guess as to what
% produces one-half of maximum output for a typical CRT.
defBgColor = [190 190 190]'/255;
thePrompt = sprintf('Enter RGB values for background (range 0-1) as a row vector [%0.3f %0.3f %0.3f]: ',...
                    defBgColor(1), defBgColor(2), defBgColor(3));
while 1
	cal.bgColor = input(thePrompt)';
	if isempty(cal.bgColor)
		cal.bgColor = defBgColor;
	end
	[m, n] = size(cal.bgColor);
	if m ~= 3 || n ~= 1
		fprintf('\nMust enter values as a row vector (in brackets).  Try again.\n');
    elseif (any(defBgColor > 1) || any(defBgColor < 0))
        fprintf('\nValues must be in range (0-1) inclusive.  Try again.\n');
    else
		break;
	end
end

% Get distance from meter to screen.
defDistance = .80;
theDataPrompt = sprintf('Enter distance from meter to screen (in meters): [%g]: ', defDistance);
cal.describe.meterDistance = input(theDataPrompt);
if isempty(cal.describe.meterDistance)
  cal.describe.meterDistance = defDistance;
end

% Fill in descriptive information
computerInfo = Screen('Computer');
hz = Screen('NominalFrameRate', cal.describe.whichScreen);
cal.describe.caltype = 'monitor';
cal.describe.computer = sprintf('%s''s %s, %s', computerInfo.consoleUserName, computerInfo.machineName, computerInfo.system);
cal.describe.monitor = input('Enter monitor name: ','s');
cal.describe.driver = sprintf('%s %s','unknown_driver','unknown_driver_version');
cal.describe.hz = hz;
cal.describe.who = input('Enter your name: ','s');
cal.describe.date = sprintf('%s %s',date,datestr(now,14));
cal.describe.program = sprintf('CalibrateMonSpd, background set to [%g,%g,%g]',...
                               cal.bgColor(1), cal.bgColor(2), cal.bgColor(3));
cal.describe.comment = input('Describe the calibration: ','s');

% Enter save code
fprintf(1, '\nSave codes:\n\t0 - screenX.mat\n\t1 - string.mat\n\t2 - default.mat\n');
saveCode = input('	Enter save code [0]: ');
if isempty(saveCode)
	saveCode = 0;
end
if saveCode == 1
	defaultFileName = 'monitor';
	thePrompt = sprintf('Enter calibration filename [%s]: ',defaultFileName);
	newFileName = input(thePrompt,'s');
	if isempty(newFileName)
        newFileName = defaultFileName;
	end
end

% Fitting parameters
cal.describe.gamma.fitType = 'crtPolyLinear';
cal.describe.gamma.contrastThresh = 0.001;
cal.describe.gamma.fitBreakThresh = 0.02;

% Initialize
switch whichMeterType
	case 0
	case 1
		CMCheckInit;
	case 2
		CVIOpen;
    otherwise
		error('Invalid meter type');
end
ClockRandSeed;
%ScreenSaver(0);

% Calibrate monitor
USERPROMPT = 1;
cal = CalibrateMonDrvr(cal, USERPROMPT, whichMeterType, blankOtherScreen);

% Calibrate ambient
USERPROMPT = 0;
cal = CalibrateAmbDrvr(cal, USERPROMPT, whichMeterType, blankOtherScreen);

% Signal end 
Snd('Play', sin(0:10000)); WaitSecs(.75); Snd('Play', sin(0:10000)); WaitSecs(.75); Snd('Play', sin(0:20000));

% Save the structure
if saveCode == 0
	screenNumber = cal.describe.whichScreen;
	fprintf(1, '\nSaving to screen%g.mat\n', screenNumber);
	SaveCalFile(cal, screenNumber);
elseif saveCode == 1
	fprintf(1, '\nSaving to %s.mat\n', newFileName);
	SaveCalFile(cal, newFileName);
elseif saveCode == 2
	fprintf(1, '\nSaving to default.mat\n');
	SaveCalFile(cal);
else
	error('Illegal value for save code entered');
end

% Put up a plot of the essential data
figure(1); clf;
plot(SToWls(cal.S_device), cal.P_device);
xlabel('Wavelength (nm)', 'Fontweight', 'bold');
ylabel('Power', 'Fontweight', 'bold');
title('Phosphor spectra', 'Fontsize', 13, 'Fontname', 'helvetica', 'Fontweight', 'bold');
axis([380, 780, -Inf, Inf]);

figure(2); clf;
plot(cal.rawdata.rawGammaInput, cal.rawdata.rawGammaTable, '+');
xlabel('Input value', 'Fontweight', 'bold');
ylabel('Normalized output', 'Fontweight', 'bold');
title('Gamma functions', 'Fontsize', 13, 'Fontname', 'helvetica', 'Fontweight', 'bold');
hold on
plot(cal.gammaInput, cal.gammaTable);
hold off
figure(gcf);
drawnow;

% Reenable screen saver.
%ScreenSaver(1);

% Close down meter
switch whichMeterType
	case 0
	case 1
		CMClose;
	case 2
		CVIClose;
    otherwise
		error('Invalid meter type');
end

