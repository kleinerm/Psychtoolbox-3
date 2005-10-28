% RefitCalGamma
%
% Refit the gamma data from a calibration.
%
% 3/27/02  dhb  Wrote it.
% 8/26/03  dhb, jl  Allow changing dacsize of calibraiton.  Useful for dropping from 10 to 8.
% 2/2/05	 dhb, dam Ask for filename to save to, rather than automatically overwrite.

% Enter load code
fprintf(1,'\nLoad codes:\n\t0 - screenX.mat\n\t1 - string.mat\n\t2 - default.mat\n');
loadCode = input('	Enter load code [0]: ');
if (isempty(loadCode))
	loadCode = 0;
end
if (loadCode == 1)
	defaultFileName = 'monitor';
	thePrompt = sprintf('Enter calibration filename [%s]: ',defaultFileName);
	newFileName = input(thePrompt,'s');
	if (isempty(newFileName))
  	newFileName = defaultFileName;
	end
end

% Load the structure
if (loadCode == 0)
	defaultScreen = 1;
	whichScreen = input(sprintf('Which screen to calibrate [%g]: ',defaultScreen));
	if (isempty(whichScreen))
		whichScreen = defaultScreen;
	end
	fprintf(1,'\nLoading for screen%g.mat\n',whichScreen);
	cal = LoadCalFile(whichScreen);
	fprintf('Calibration file read for screen %g\n\n',whichScreen);
elseif (loadCode == 1)
	fprintf(1,'\nLoading from %s.mat\n',newFileName);
	cal = LoadCalFile(newFileName);
	fprintf('Calibration file %s read\n\n',newFileName);
elseif (loadCode == 2)
	fprintf(1,'\nLoading from default.mat\n');
	cal = LoadCalFile;
	fprintf('Read default calibration file %s read\n\n',newFileName);
else
	error('Illegal value for save code entered');
end

% Print out some information from the calibration.
DescribeMonCal(cal);

% Provide information about gamma measurements
% This is probably not method-independent.
fprintf('Gamma measurements were made at %g levels\n',...
	size(cal.rawdata.rawGammaInput,1));
fprintf('Gamma table available at %g levels\n',...
	size(cal.gammaInput,1));

% Get new fit type
fprintf('Old gamma fit type was: %s\n',cal.describe.gamma.fitType);
oldType = cal.describe.gamma.fitType;
cal.describe.gamma.fitType = input('Enter new fit type: ','s');
if (isempty(cal.describe.gamma.fitType))
	cal.describe.gamma.fitType = oldType;
end
cal = CalibrateFitGamma(cal);

% Get new number of bits
fprintf('Old DAC bits was: %d\n',cal.describe.dacsize);
oldDacsize = cal.describe.dacsize;
cal.describe.dacsize = input('Enter new dacsize: ');
if (isempty(cal.describe.dacsize))
	cal.describe.dacsize;
end

% If we're changing dacsize, need to muck around with
% the way the raw gamma data are represented.  Here
% we simply recreate the inputs we would have used had
% we calibrated at the new dac depth.  This seems as close
% as anything else we can think of.
if (cal.describe.dacsize ~= oldDacsize)
	bits = cal.describe.dacsize;
	nInputLevels = 2^bits;
	cal.rawdata.rawGammaInput  = round(linspace(nInputLevels/cal.describe.nMeas,nInputLevels-1,cal.describe.nMeas))';
end

% Now refit
cal = CalibrateFitGamma(cal);

% And some plots
figure(1); clf;
plot(cal.rawdata.rawGammaInput,cal.rawdata.rawGammaTable,'+');
xlabel('Input value', 'Fontweight', 'bold');
ylabel('Normalized output', 'Fontweight', 'bold');
title('Gamma functions', 'Fontsize', 13, 'Fontname', 'helvetica', 'Fontweight', 'bold');
hold on
plot(cal.gammaInput,cal.gammaTable);
hold off
figure(gcf);
drawnow;

% Option to save the refit file
saveIt = input('Save new fit data? [0]: ');
if (isempty(saveIt))
	saveIt = 0;
end
if (saveIt)
	% Prompt for new file name if we're saving to a name.
	saveCode = loadCode;
	if (isempty(saveCode))
		saveCode = 0;
	end
	if (saveCode == 1)
		defaultFileName = newFileName;
		thePrompt = sprintf('Enter calibration filename [%s]: ',defaultFileName);
		saveFileName = input(thePrompt,'s');
		if (isempty(saveFileName))
  		saveFileName = defaultFileName;
		end
	end
	
	if (saveCode == 0)
		screenNumber = cal.describe.whichScreen;
		fprintf(1,'\nSaving to screen%g.mat\n',screenNumber);
		SaveCalFile(cal,screenNumber);
	elseif (saveCode == 1)
		fprintf(1,'\nSaving to %s.mat\n',saveFileName);
		SaveCalFile(cal,saveFileName);
	elseif (saveCode == 2)
		fprintf(1,'\nSaving to default.mat\n');
		SaveCalFile(cal);
	else
		error('Illegal value for save code entered');
	end
end



