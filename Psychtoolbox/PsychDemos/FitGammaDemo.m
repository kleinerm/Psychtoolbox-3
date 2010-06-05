% FitGammaDemo.
%
% Shows how function FitGamma may be used to fit various functions through measured gamma data.
% We typically call FitGamma in scripts that perform calibrations to generate a full gamma table.
%
% NOTE: FitGamma uses the MATLAB optimization toolbox.
%
% NOTE: FitGamma is now a little obsolete, as we typically call it through
% the function CalibrateFitGamma. That function has more fit options, but
% is also more tied to our calibration structure.  
%
% Also see RefitCalGamma, CalibrateFitGamma, FitGamma.
%
% 8/7/00  dhb  Wrote it.
% 3/5/05  dhb  A few more tests.
% 6/5/10  dhb  Made sure this still runs OK.  A few small tweaks.

% Clear
clear; close all

% Here is some typical gammma function data.
% Here we only take a portion of the data
% to make the fit more interesting.  Note
% that FitGamma assumes that a measurement
% was made of the output for maximum device
% input (255 in this case).  The meausurements
% are also assumed to be dark subtracted, so
% that the output for input 0 is zero by
% definition.  Correction for ambient (flare)
% is handled separately from gamma correction
% in our code.
typicalGammaInput = [7	15	22	29	36	44	51	58	66	73	80	87 ...
    95	102	109	117	124	131	138	146	153	160	168	175	...
    182	189	197	204	211	219	226	233	240	248	255]'/255;
typicalGammaData = [0.00185045	0.00487599	0.00887364	0.0139455	0.0198809	...
    0.028367	0.0375484	0.0478407	0.0618163	0.0750891	0.090624	...
    0.106427	0.127097	0.146463	0.167736	0.193906	0.21841	...
    0.245344	0.272638	0.307377	0.338154	0.373123	0.411352	...
    0.449246	0.487839	0.528577	0.576434	0.621044	0.668951	...
    0.722544	0.772606	0.824269	0.880795	0.939194	1]';

% Plot the data
figure; clf; hold on
plot(typicalGammaInput,typicalGammaData,'+');

% Fit simple gamma power function
output = linspace(0,1,100)';
[simpleFit,simpleX] = FitGamma(typicalGammaInput,typicalGammaData,output,1);
plot(output,simpleFit,'r');
fprintf(1,'Found exponent %g\n\n',simpleX(1));

% Fit extended gamma power function.
% Here the fit is the same as for the simple function.
[extendedFit,extendedX] = FitGamma(typicalGammaInput,typicalGammaData,output,2);
plot(output,extendedFit,'g');
fprintf(1,'Found exponent %g, offset %g\n\n',extendedX(1),extendedX(2));

% By passing other values of fitType (last arg to FitGamma), you can
% fit other parametric forms or spline the data.  See "help FitGamma"
% for information.  FitGamma can also be called with fitType set to
% zero to cause it to fit all of its forms and let you know which
% one produces the lowest fit error.
% Plot the data
figure; clf; hold on
plot(typicalGammaInput,typicalGammaData,'+');
for i = 1:7
    theFit(:,i) = FitGamma(typicalGammaInput,typicalGammaData,output,i); %#ok<SAGROW>
end
plot(output,theFit,'.','MarkerSize',2);
fprintf('\n');
theFit0 = FitGamma(typicalGammaInput,typicalGammaData,output,0);
plot(output,theFit0,'r');

% For grins, we can use the parameters from the extended fit to
% invert the gamma function.  The inversion isn't perfect,
% particularly at the low end.  This is because the fit isn't
% perfect there.  Because the gamma function is so flat, a
% small error in fit is amplified on the inversion, when
% the inversion is examined in the input space.  On the
% other hand, the same flatness means that the actual display
% error resulting from the inversion error is small.
%
% Analytial inverse functions are not currently implemented
% for other parametric fits.  Typically we do the inversion
% by inverse table lookup in the actual calibration routines.
maxInput = max(typicalGammaInput);
invertedInput = InvertGammaExtP(extendedX,maxInput,typicalGammaData);
figure; clf; hold on
plot(typicalGammaInput,invertedInput,'r+');
axis('square');
axis([0 maxInput 0 maxInput]);
plot([0 maxInput],[0 maxInput],'r');

% Under OS 9, we had a function CopyText that would copy a matrix to
% the clipboard.  That went away in OS/X, but I left the code here
% just to document what the interesting variables are.
% CopyText([typicalGammaInput,typicalGammaData]);
% CopyText([output,extendedFit]);
