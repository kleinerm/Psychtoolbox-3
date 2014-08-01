% CalDemo
%
% Demonstrates basic use of the PsychCal calibraiton structure and routines.
%
% See also PsychCal, DKLDemo, RenderDemo, DumpMonCalSpd
%
% 1/15/07	dhb		Wrote it.
% 9/27/08   dhb     Prompt for filename.  Clean up plot labels
%           dhb     Prompt for gamma method.
% 5/08/14   npc     Modifications for accessing calibration data using a @CalStruct object.
% 7/9/14    dhb     Made this work with PTB original or new object oriented
%                   calibration code (available in the BrainardLabToolbox on gitHub).

% Clear
% clear; close all

% Load
% Load a calibration file. You can make this with CalibrateMonSpd if
% you have a supported radiometer.
defaultFileName = 'PTB3TestCal';
thePrompt = sprintf('Enter calibration filename [%s]: ',defaultFileName);
newFileName = input(thePrompt,'s');
if (isempty(newFileName))
    newFileName = defaultFileName;
end
fprintf(1,'\nLoading from %s.mat\n',newFileName);
commandwindow;
cal = LoadCalFile(newFileName);
fprintf('Calibration file %s read\n\n',newFileName);
    
%% Pull out what we need to run this in a fashion that is portable
% across PTB and the way the Brainard Lab does things (see 
% BrainardLabToolbox on gitHub if you'd like our code).
% Specify @CalStruct object that will handle all access to the calibration data.
if (exist('ObjectToHandleCalOrCalStruct','file'))
    [calStructOBJ, inputArgIsACalStructOBJ] = ObjectToHandleCalOrCalStruct(cal); clear 'cal';
    S               = calStructOBJ.get('S');
    P_device        = calStructOBJ.get('P_device');
    gammaInput      = calStructOBJ.get('gammaInput');
    rawGammaInput   = calStructOBJ.get('rawGammaInput');
    gammaTable      = calStructOBJ.get('gammaTable');
    rawGammaTable   = calStructOBJ.get('rawGammaTable');
    OBJStyle = true;
    DescribeMonCal(calStructOBJ);
else
    S               = cal.S_device;
    P_device        = cal.P_device;
    gammaInput      = cal.gammaInput;
    rawGammaInput   = cal.rawdata.rawGammaInput;
    gammaTable      = cal.gammaTable;
    rawGammaTable   = cal.rawdata.rawGammaTable;
    OBJStyle = false;
    calStructOBJ = cal;
end
DescribeMonCal(calStructOBJ);

% Plot underlying spectral data of the three device primaries
wls = SToWls(S);
figure; clf; hold on
plot(wls,P_device(:,1),'r');
plot(wls,P_device(:,2),'g');
plot(wls,P_device(:,3),'b');
xlabel('Wavelength (nm)');
ylabel('Power');
title('Device Primary Spectra');

% Plot gamma functions together with raw gamma data.  The smooth fit is
% performed at calibration time, but can be redone with RefitCalGamma.
figure; clf;
subplot(1,3,1); hold on
plot(gammaInput,gammaTable(:,1),'r');
plot(rawGammaInput,rawGammaTable(:,1),'ro','MarkerFaceColor','r','MarkerSize',3);
axis([0 1 0 1]); axis('square');
xlabel('Input value');
ylabel('Linear output');
title('Device Gamma');
subplot(1,3,2); hold on
plot(gammaInput,gammaTable(:,2),'g');
plot(rawGammaInput,rawGammaTable(:,2),'go','MarkerFaceColor','g','MarkerSize',3);
axis([0 1 0 1]); axis('square');
xlabel('Input value');
ylabel('Linear output');
title('Device Gamma');
subplot(1,3,3); hold on
plot(gammaInput,gammaTable(:,3),'b');
plot(rawGammaInput,rawGammaTable(:,3),'bo','MarkerFaceColor','b','MarkerSize',3);
axis([0 1 0 1]); axis('square');
xlabel('Input value');
ylabel('Linear output');
title('Device Gamma');

%% Gamma correction without worrying about color
% Show how to linearize a gamma table.  If there were no
% quantization in the DAC, these values would linearize perfectly.
% Actually linearization will be affected by the precision of the DACs.

% Set inversion method.  See SetGammaMethod for information on available
% methods.
defaultGammaMethod = 0;
commandwindow;
gammaMethod = input(sprintf('Enter gamma method [%d]:',defaultGammaMethod));
if (isempty(gammaMethod))
    gammaMethod = defaultGammaMethod;
end
if (OBJStyle)
    SetGammaMethod(calStructOBJ,gammaMethod);
else
    calStructOBJ = SetGammaMethod(calStructOBJ,gammaMethod);
end
             
% Make the desired linear output, then convert.
linearValues = ones(3,1)*linspace(0,1,256);
clutValues = PrimaryToSettings(calStructOBJ,linearValues);
predValues = SettingsToPrimary(calStructOBJ,clutValues);

% Make a plot of the inverse lookup table.
figure; clf;
subplot(1,3,1); hold on
plot(linearValues,clutValues(1,:)','r');
axis([0 1 0 1]); axis('square');
xlabel('Linear output');
ylabel('Input value');
title('Inverse Gamma');
subplot(1,3,2); hold on
plot(linearValues,clutValues(2,:)','g');
axis([0 1 0 1]); axis('square');
xlabel('Linear output');
ylabel('Input value');
title('Inverse Gamma');
subplot(1,3,3); hold on
plot(linearValues,clutValues(3,:)','b');
axis([0 1 0 1]); axis('square');
xlabel('Linear output');
ylabel('Input value');
title('Inverse Gamma');

% Make a plot of the obtained linear values.
figure; clf;
subplot(1,3,1); hold on
plot(linearValues,predValues(1,:)','r');
axis([0 1 0 1]); axis('square');
xlabel('Desired value');
ylabel('Predicted value');
title('Gamma Correction');
subplot(1,3,2); hold on
plot(linearValues,predValues(2,:)','g');
axis([0 1 0 1]); axis('square');
xlabel('Desired value');
ylabel('Predicted value');
title('Gamma Correction');
subplot(1,3,3); hold on
plot(linearValues,predValues(3,:)','b');
axis([0 1 0 1]); axis('square');
xlabel('Desired value');
ylabel('Predicted value');
title('Gamma Correction');

%% Color space conversions  - CIE 1931
% Let's see how to do some standard color conversions

% Choose color matching functions.  Here CIE 1931, with a unit
% constant so that luminance is in cd/m2.
load T_xyz1931
T_xyz = 683*T_xyz1931;
if (OBJStyle)
   SetSensorColorSpace(calStructOBJ,T_xyz,S_xyz1931);
else
    calStructOBJ = SetSensorColorSpace(calStructOBJ,T_xyz,S_xyz1931);
end

% Dump out min, mid, and max XYZ settings.  In general
% the calibration structure records the ambient light so
% that the min output is not necessarily zero light.
minXYZ = PrimaryToSensor(calStructOBJ,[0 0 0]'); minxyY = XYZToxyY(minXYZ);
midXYZ = PrimaryToSensor(calStructOBJ,[0.5 0.5 0.5]'); midxyY = XYZToxyY(midXYZ);
maxXYZ = PrimaryToSensor(calStructOBJ,[1 1 1]'); maxxyY = XYZToxyY(maxXYZ);
fprintf('Device properties in XYZ\n');
fprintf('\tMin xyY = %0.3g, %0.3g, %0.2f\n',minxyY(1),minxyY(2),minxyY(3));
fprintf('\tMid xyY = %0.3g, %0.3g, %0.2f\n',midxyY(1),midxyY(2),midxyY(3));
fprintf('\tMax xyY = %0.3g, %0.3g, %0.2f\n',maxxyY(1),maxxyY(2),maxxyY(3));

% Find actual settings to produce a desired colorimetric response.   Note
% that there are two ways to think about this.
%   a) If you've linearized the display using the lookup table, then pass
%   to the framebuffer the desiredPrimary triplet computed below.  This is
%   then mapped via the clut to produce the desired effect.
%   b) If you're maninpulating the clut directly, then you care what goes
%   into the clut entry corresponding to the region you're displaying.  In
%   that case, use the desiredSettings triplet computed below and stick it
%   into the clut.
% If you don't understand the distinction between a frame buffer and a
% lookup table, you might want to read Brainard, D. H., Pelli, D.G., and
% Robson, T. (2002). Display characterization. In the Encylopedia of Imaging
% Science and Technology. J. Hornak (ed.), Wiley. 172-188.  You can
% download a PDF from http://color.psych.upenn.edu/brainard/characterize.pdf.
desiredxyY = [0.4 0.3 40]';
desiredXYZ = xyYToXYZ(desiredxyY);
desiredPrimary = SensorToPrimary(calStructOBJ,desiredXYZ);
desiredSettings = SensorToSettings(calStructOBJ,desiredXYZ);
fprintf('To get xyY = %0.3g, %0.3g, %0.2f\n',desiredxyY(1),desiredxyY(2),desiredxyY(3))
fprintf('\tLinear weights on primaries should be %0.2g, %0.2g, %0.2g\n',desiredPrimary(1),desiredPrimary(2),desiredPrimary(3));
fprintf('\tActual settings values passed to driver (via clut) should be %0.2g, %0.2g, %0.2g\n',desiredSettings(1),desiredSettings(2),desiredSettings(3));

%% Color space conversions - Cone space and isoluminance
% Now let's do some examples in cone space.  See DKLDemo for more
% colorimetric stuff.

% Load cone spectral sensitivities
load T_cones_ss2
T_cones = T_cones_ss2;
if (OBJStyle)
    SetSensorColorSpace(calStructOBJ,T_cones,S_cones_ss2);
else
    calStructOBJ = SetSensorColorSpace(calStructOBJ,T_cones,S_cones_ss2);
end

% Choose monitor white point as a background around which to modulate
bgPrimary = [0.55 0.45 0.5]';
bgLMS = PrimaryToSensor(calStructOBJ, bgPrimary);

% Choose a modulation direction. [0 1 0] isolates the M cones.
directionLMS = [0 1 0]';

% Figure out maximum within gamut modulation in this direction.  This
% is best done in primary color space.  The calculation as executed below
% works even with non-zero ambient light and when the background is not
% in the middle of the device space.
%
% The resulting modulation +/- gamutScalar*directionLMS is symmetric around
% the background and takes us from the background exactly to the edge of the gamut.  
targetLMS       = bgLMS+directionLMS;             
targetPrimary   = SensorToPrimary(calStructOBJ, targetLMS);
directionPrimary= targetPrimary-bgPrimary;
gamutScalar     = MaximizeGamutContrast(directionPrimary,bgPrimary);
minLMS          = bgLMS-gamutScalar*directionLMS;
maxLMS          = bgLMS+gamutScalar*directionLMS;

minPrimary      = SensorToPrimary(calStructOBJ,  minLMS);
maxPrimary      = SensorToPrimary(calStructOBJ,  maxLMS);
minSettings     = SensorToSettings(calStructOBJ, minLMS);
maxSettings     = SensorToSettings(calStructOBJ, maxLMS);

contrastLMS1    = (maxLMS-bgLMS)./bgLMS;
contrastLMS2    = (maxLMS-minLMS)./(maxLMS+minLMS);
fprintf('Primary values at low edge of moduluation: %0.2f %0.2f %0.2f\n',minPrimary(1),minPrimary(2),minPrimary(3));
fprintf('Primary values at high edge of moduluation: %0.2f %0.2f %0.2f\n',maxPrimary(1),maxPrimary(2),maxPrimary(3));
fprintf('Cone contrast of modulation: %0.2f, %0.2f %0.2f\n',contrastLMS1(1),contrastLMS1(2),contrastLMS1(3));
if (max(abs(contrastLMS1-contrastLMS2)) > 1e-12)
    fprintf('Uh-oh, two ways of computing contrast don''t agree.\n');
end





