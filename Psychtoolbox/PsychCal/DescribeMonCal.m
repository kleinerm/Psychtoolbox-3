function same = DescribeMonCal(cal,file,whichScreen)
% same = DescribeMonCal(cal,[file],[whichScreen])
% 
% Print descriptive information about a calibration 
% to the command window or file.
%
% Argument file is a standard Matlab file descriptor,
% see fopen.  If file arg is omitted or empty, printout
% goes to command window.
%
% If argument whichScreen is passed, a description of
% the current hardware is also printed.  In this case,
% returned boolean same indicates whether the calibration
% is consistent with the current hardware.  Boolean
% same is empty if whichScreen is not provided.
%
% 8/25/97  dhb, pbe  Wrote it.
% 7/3/98   dhb, pbe  Updated for cal.describe.
% 12/3/99  dhb, mpr  Fix check for calibration desription field.
% 8/1800   dhb       Add whichScreen arg, same return.
% 6/29/02  dgp       Use new version of Screen VideoCard.
% 9/23/02  dhb, jms  Fix small bug in way driver is compared, presumably introduced 6/29/02.

% Default args
if (nargin < 2 | isempty(file))
	file = 1;
end
if (nargin < 3 | isempty(whichScreen))
	file = 1;
	whichScreen = [];
end
same = [];

if (~isfield(cal,'describe'))
	error('Calibration structure has no description');
end

fprintf('Calibration:\n');
fprintf(file,'\tComputer: %s\n',cal.describe.computer);
fprintf(file,'\tScreen: %d\n',cal.describe.whichScreen);
fprintf(file,'\tMonitor: %s\n',cal.describe.monitor);
fprintf(file,'\tVideo driver: %s\n',cal.describe.driver);
fprintf(file,'\tDac size: %g\n',cal.describe.dacsize);
fprintf(file,'\tFrame rate: %g hz\n',cal.describe.hz);
fprintf(file,'\tCalibration performed by %s\n',cal.describe.who);
fprintf(file,'\tCalibration performed on %s\n',cal.describe.date);
fprintf(file,'\tCalibration program: %s\n',cal.describe.program);
fprintf(file,'\tComment: %s\n',cal.describe.comment);
fprintf(file,'\tCalibrated device has %g primaries\n',cal.nDevices);
fprintf(file,'\n');

% Current configuration
if (~isempty(whichScreen))
	[computerName,owner,system,processor,cache,fpu,Hz,busHz,vm,pci]=Screen('Computer');
	computer = sprintf('%s''s %s, %s',owner,computerName,system);
	card=Screen(whichScreen,'VideoCard');
	driver = sprintf('%s %s',card.driverName,card.driverVersion);
	dacsize = Screen(whichScreen,'Preference','ClutDacSize');
	hz = FrameRate(whichScreen);
	same = 1;
	fprintf('Current configuration:\n');
	fprintf(file,'\tComputer: %s\n',computer);
	if (~streq(computer,cal.describe.computer))
		same = 0;
	end
	fprintf(file,'\tScreen: %d\n',whichScreen);
	if (whichScreen ~= cal.describe.whichScreen)
		save = 0;
	end
	fprintf(file,'\tVideo driver: %s\n',driver);
	if (~streq(driver,cal.describe.driver))
		same = 0;
	end
	fprintf(file,'\tDac size: %g\n',dacsize);
	if (dacsize ~= cal.describe.dacsize)
		same = 0;
	end
	fprintf(file,'\tFrame rate: %g hz\n',hz);
	if (abs(hz-cal.describe.hz) > 0.5)
		same = 0;
	end
end
