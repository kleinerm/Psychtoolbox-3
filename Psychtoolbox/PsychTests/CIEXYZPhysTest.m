% CIEXYZPhysTest
%
% This program tests the CIE Physiologically based XYZ data.
%
% See also
%   ComputeCIEConeFundamentals, CIEConeFundamentalsTest.
%
% 6/26/16  dhb  Wrote it.

%% Clear
clear; close all;

%% Load 2 degree functions
% 
% As it happens, these are all on the same wavelength sampling.
load T_xyzCIEPhys2
load T_CIE_Y2
load T_cones_ss2
if (max(abs(S_xyzCIEPhys2-S_CIE_Y2)) > 0)
    error('Surprising wavelength sampling inconsistency');
end
if (max(abs(S_xyzCIEPhys2-S_cones_ss2)) > 0)
    error('Surprising wavelength sampling inconsistency');
end

%% Start 2-degree plot
figure; clf; hold on
plot(SToWls(S_xyzCIEPhys2),T_xyzCIEPhys2','k','LineWidth',5);

%% Check the consistency of XYZ and cones up to linear transformation
M2 = (T_cones_ss2'\T_xyzCIEPhys2')';
T_xyzPred = M2*T_cones_ss2;
plot(SToWls(S_xyzCIEPhys2),T_xyzPred','g','LineWidth',3);

check1 = T_xyzPred-T_xyzCIEPhys2;
tolerance = 1e-5;
if (max(abs(check1(:))) > tolerance)
    error('2-degree cones and XYZ and Y direct do not agree within a linear transformation')
end

fprintf('2-degree transform, applied from left to cones as column vector');
M2
expectedM2 = [1.94735469 -1.41445123 0.36476327 ; 0.68990272 0.34832189 0 ; 0 0 1.93485343];
check3 = M2-expectedM2;
tolerance = 1e-6;
if (max(abs(check3(:))) > tolerance)
    error('2-degree transformation matrix is off')
end

%% Check consistency of xyz and Y
% 
% These are given to a different number of places on CVRL,
% so we check within the expected rounding error.
plot(SToWls(S_CIE_Y2),T_CIE_Y2','r','LineWidth',2);
xlabel('Wavelength (nm)');
ylabel('CMF value')
title('2-degree functions');
ylim([0 2.5]);

check2 = T_xyzCIEPhys2(2,:)-T_CIE_Y2;
tolerance = 1e-6;
if (max(abs(check2(:))) > tolerance)
    error('2-degree Y in XYZ and Y direct are not the same')
end

%% Add 1931 functions, just to have a peek
load T_xyz1931
plot(SToWls(S_xyz1931),T_xyz1931','b:','LineWidth',1)
legend({'XYZ', '', '', 'Fit from cones', '', '', 'CIE Y' '1931 XYZ'});

%% Load 10 degree functions
% 
% As it happens, these are all on the same wavelength sampling.
load T_xyzCIEPhys10
load T_CIE_Y10
load T_cones_ss10
if (max(abs(S_xyzCIEPhys10-S_CIE_Y10)) > 0)
    error('Surprising wavelength sampling inconsistency');
end
if (max(abs(S_xyzCIEPhys10-S_cones_ss10)) > 0)
    error('Surprising wavelength sampling inconsistency');
end

%% Start 10-degree plot
figure; clf; hold on
plot(SToWls(S_xyzCIEPhys10),T_xyzCIEPhys10','k','LineWidth',5);

%% Check the consistency of XYZ and cones up to linear transformation
M10 = (T_cones_ss10'\T_xyzCIEPhys10')';
T_xyzPred = M10*T_cones_ss10;
plot(SToWls(S_xyzCIEPhys10),T_xyzPred','g','LineWidth',3);

check1 = T_xyzPred-T_xyzCIEPhys10;
tolerance = 1e-5;
if (max(abs(check1(:))) > tolerance)
    error('10-degree ones and XYZ and Y direct do not agree within a linear transformation')
end

fprintf('10-degree transform, applied from left to cones as column vector');
M10
expectedM10 = [1.93986443 -1.34664359 0.43044935 ; 0.69283932 0.34967567 0 ; 0 0 2.14687945];
check3 = M10-expectedM10;
tolerance = 1e-6;
if (max(abs(check3(:))) > tolerance)
    error('10-degree transformation matrix is off')
end

%% Check consistency of xyz and Y
% 
% These are given to a different number of places on CVRL,
% so we check within the expected rounding error.
plot(SToWls(S_CIE_Y10),T_CIE_Y10','r','LineWidth',2);
xlabel('Wavelength (nm)');
ylabel('CMF value')
title('10-degree functions');
ylim([0 2.5]);
legend({'XYZ', '', '', 'Fit from cones', '', '', 'Y'});

check2 = T_xyzCIEPhys10(2,:)-T_CIE_Y10;
tolerance = 1e-6;
if (max(abs(check2(:))) > tolerance)
    error('10-degree Y in XYZ and Y direct are not the same')
end

%% Add 1964 functions, just to have a peek
load T_xyz1964
plot(SToWls(S_xyz1964),T_xyz1964','b:','LineWidth',1)
legend({'XYZ', '', '', 'Fit from cones', '', '', 'CIE Y' '1964 XYZ'});


