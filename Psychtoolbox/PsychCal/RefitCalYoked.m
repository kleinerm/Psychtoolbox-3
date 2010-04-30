% RefitCalYoked
%
% Refit a calibration file to make gamma curves based on the raw yoked measurements.
% This requires, of course, that the yoked measurements were taken during calibration,
% which in turn requires the correct settings.
%
% 4/29/10  dhb, kmo, ar  Wrote it.

% Enter load code
defaultFileName = 'HDRFront';
thePrompt = sprintf('Enter calibration filename [%s]: ',defaultFileName);
newFileName = input(thePrompt,'s');
if (isempty(newFileName))
    newFileName = defaultFileName;
end
fprintf(1,'\nLoading from %s.mat\n',newFileName);
cal = LoadCalFile(newFileName);
fprintf('Calibration file %s read\n\n',newFileName);

% Print out some information from the calibration.
DescribeMonCal(cal);

% Provide information about gamma measurements
% This is probably not method-independent.
fprintf('Gamma measurements were made at %g levels\n',...
	size(cal.rawdata.rawGammaInput,1));
fprintf('Gamma table available at %g levels\n',...
	size(cal.gammaInput,1));

% Fit yoked measurements
cal.describe.yokedGamma = 1;
cal = CalibrateFitLinMod(cal);
cal = CalibrateFitGamma(cal,2^cal.describe.dacsize);

% Put up a plot of the essential data
figure(1); clf;
plot(SToWls(cal.S_device),cal.P_device);
xlabel('Wavelength (nm)', 'Fontweight', 'bold');
ylabel('Power', 'Fontweight', 'bold');
title('Phosphor spectra', 'Fontsize', 13, 'Fontname', 'helvetica', 'Fontweight', 'bold');
axis([380,780,-Inf,Inf]);

figure(2); clf;
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
saveIt = input('Save new fit data (0->no, 1 -> yes)? [0]: ');
if (isempty(saveIt))
	saveIt = 0;
end
if (saveIt)
    % Prompt for new file name if we're saving to a name.
    defaultFileName = newFileName;
    thePrompt = sprintf('Enter calibration filename [%s]: ',defaultFileName);
    saveFileName = input(thePrompt,'s');
    if (isempty(saveFileName))
        saveFileName = defaultFileName;
    end
    fprintf(1,'\nSaving to %s.mat\n',saveFileName);
    SaveCalFile(cal,saveFileName);
end





