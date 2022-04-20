% DKLDemo
%
% Demonstrates the DKL color space routines.
% Produces a picture of the DKL isoluminant plane
% in a figure window.  This picture is not calibrated.
%
% You can chooose the image size and what cone/luminance
% data you want to use to define the space.
%
% 4/9/05	dhb		Wrote it.
% 5/26/11   dhb     Updated to use PTB3TestCal, SetSensorColorSpace
%           dhb     Remove OS9 option.
% 4/13/17   dhb     Switch default cone type to Stockman-Sharpe.
%                   Demonstrate conversion from cone contrast coords to
%                   DKL, code at end.
% 8/21/21   dhb     Added a few more checks and comments.  Made some of the
%                   numbers less generic, on the view that testing is more
%                   robust in such cases.
% 4/13/22   dhb     Fixed unfinished comment, added numerical check of
%                   agreement at end.

%% Clear
clear; close all

%% Define parameters.  Image size should be an odd number.
imageSize = 513;
whichCones = 'StockmanSharpe';

% Load spectral data and set calibration file
switch (whichCones)
	case 'SmithPokorny'
		load T_cones_sp
		load T_xyzJuddVos
		S_cones = S_cones_sp;
		T_cones = T_cones_sp;
		T_Y = 683*T_xyzJuddVos(2,:);
		S_Y = S_xyzJuddVos;
		T_Y = SplineCmf(S_Y,T_Y,S_cones);
	case 'StockmanSharpe'
		load T_cones_ss2
		load T_ss2000_Y2
		S_cones = S_cones_ss2;
		T_cones = T_cones_ss2;
		T_Y = 683*T_ss2000_Y2;
		S_Y = S_ss2000_Y2;
		T_Y = SplineCmf(S_Y,T_Y,S_cones);
    otherwise
        error('Unknown cone fundamentals specified')
end
cal = LoadCalFile('PTB3TestCal');
calLMS = SetSensorColorSpace(cal,T_cones,S_cones);
calLMS = SetGammaMethod(calLMS,1);
calLum = SetSensorColorSpace(cal,T_Y,S_Y);

%% Define background.  Here we just take the
% something near the monitor mid-point.
bgLMS = PrimaryToSensor(calLMS,[0.4 0.5 0.3]');

%% Basic transformation matrices.  ComputeDKL_M() does the work.
%
% Get matrix that transforms between incremental
% cone coordinates and DKL coordinates 
% (Lum, RG, S).
[M_ConeIncToDKL,LMLumWeights] = ComputeDKL_M(bgLMS,T_cones,T_Y);
M_DKLToConeInc = inv(M_ConeIncToDKL);

%% Find incremental cone directions corresponding to DKL  directions.
lumConeInc = M_DKLToConeInc*[1 0 0]';
rgConeInc = M_DKLToConeInc*[0 1 0]';
sConeInc = M_DKLToConeInc*[0 0 1]';

% These directions should (and do last I checked) have unit pooled cone contrast,
% the way that the matrix M is scaled by ComputeDKL_M.
lumPooled = norm(lumConeInc ./ bgLMS);
rgPooled = norm(rgConeInc ./ bgLMS);
sPooled = norm(sConeInc ./ bgLMS);
fprintf('Pooled cone contrast for unit DKL directions with initial scaling: %0.3g %0.3g %0.3g\n', ...
    lumPooled, rgPooled, sPooled);

% The pooled cone contrast scaling convention implemented by ComputeDKL_M
% is fine, but one might also want to scale in some other way, say so that
% a unit step was just within the monitor gamut.  Here we compute such
% scale factors for the rg and s directions.  The work is done by routine
% MaximizeGamutContrast.
bgPrimary = SensorToPrimary(calLMS,bgLMS);
rgPrimaryInc = SensorToPrimary(calLMS,rgConeInc+bgLMS)-bgPrimary;
sPrimaryInc = SensorToPrimary(calLMS,sConeInc+bgLMS)-bgPrimary;
rgScale = MaximizeGamutContrast(rgPrimaryInc,bgPrimary);
sScale = MaximizeGamutContrast(sPrimaryInc,bgPrimary);
rgConeInc = 0.95*rgScale*rgConeInc;
sConeInc = 0.95*sScale*sConeInc;

% If we find the RGB values corresponding to unit excursions
% in rg and s directions, we should find a) that the luminance
% of each is the same and b) that they are all within gamut.
% In gamut means that the primary coordinates are all bounded
% within [0,1].
rgPlusLMS = bgLMS+rgConeInc;
rgMinusLMS = bgLMS-rgConeInc;
sPlusLMS = bgLMS+sConeInc;
sMinusLMS = bgLMS-sConeInc;
rgPlusPrimary = SensorToPrimary(calLMS,rgPlusLMS);
rgMinusPrimary = SensorToPrimary(calLMS,rgMinusLMS);
sPlusPrimary = SensorToPrimary(calLMS,sPlusLMS);
sMinusPrimary = SensorToPrimary(calLMS,sMinusLMS);
if (any([rgPlusPrimary(:) ; rgMinusPrimary(:) ; ...
		sPlusPrimary(:) ; sMinusPrimary(:)] < 0))
	fprintf('Something out of gamut low that shouldn''t be.\n');
end
if (any([rgPlusPrimary(:) ; rgMinusPrimary(:) ; ...
		sPlusPrimary(:) ; sMinusPrimary(:)] > 1))
	fprintf('Something out of gamut high that shouldn''t be.\n');
end
bgLum = PrimaryToSensor(calLum,bgPrimary);
rgPlusLum = PrimaryToSensor(calLum,rgPlusPrimary);
rgMinusLum = PrimaryToSensor(calLum,rgMinusPrimary);
sPlusLum = PrimaryToSensor(calLum,sPlusPrimary);
sMinusLum = PrimaryToSensor(calLum,sMinusPrimary);
lums = sort([bgLum rgPlusLum rgMinusLum sPlusLum sMinusLum]);
fprintf('Luminance range in isoluminant plane is %0.2f to %0.2f\n',...
	lums(1), lums(end));

%% Make a picture
%
% Now we have the coordinates we desire, make a picture of the
% isoluminant plane.
[X,Y] = meshgrid(0:imageSize-1,0:imageSize-1);
X = X-(imageSize-1)/2; Y = Y-(imageSize-1)/2;
X = X/max(abs(X(:))); Y = Y/max(abs(Y(:)));
XVec = reshape(X,1,imageSize^2); YVec = reshape(Y,1,imageSize^2);
imageLMS = bgLMS*ones(size(XVec))+rgConeInc*XVec+sConeInc*YVec;
[imageRGB,badIndex] = SensorToSettings(calLMS,imageLMS);
bgRGB = SensorToSettings(calLMS,bgLMS);
imageRGB(:,find(badIndex == 1)) = bgRGB(:,ones(size(find(badIndex == 1))));
rPlane = reshape(imageRGB(1,:),imageSize,imageSize);
gPlane = reshape(imageRGB(2,:),imageSize,imageSize);
bPlane = reshape(imageRGB(3,:),imageSize,imageSize);
theImage = zeros(imageSize,imageSize,3);
theImage(:,:,1) = rPlane;
theImage(:,:,2) = gPlane;
theImage(:,:,3) = bPlane;

% Show the image for illustrative purposes
figure; clf; image(theImage);

%% What if we want to convert between cone contrast coordinates and DKL?

% Also, let's optionally scale the cones so that they sum to luminance
SCALE_LMSUMTOLUM = false;
if (SCALE_LMSUMTOLUM)
    T_cones(1,:) = T_cones(1,:)*LMLumWeights(1);
    T_cones(2,:) = T_cones(2,:)*LMLumWeights(2);
end

% Specify LMS coordiantes of the background.  Here equal LMS excitations
% but you can try different values here.  Note that if you're mucking
% with the scaling of the cones, then holding this vector fixed across
% two different cone scalings is not holding the actual background fixed.
bgLMS = [0.2 0.3 0.25]';

% Compute matrix that goes into DKL
[M_ConeIncToDKL,LMLumWeights] = ComputeDKL_M(bgLMS,T_cones,T_Y);

% Set up modulations that should isolate each DKL direction.  Isochromatic
% is easy, because that is equal contrasts for all three cone types.
theBaseConeContrast = 0.5;
theIsochromaticConeContrast = [theBaseConeContrast theBaseConeContrast theBaseConeContrast]';
M_ConeContrastToConeInc = diag(bgLMS);
M_ConeIncToConeContrast = diag(1./bgLMS);
theIsochromaticDKL = M_ConeIncToDKL*M_ConeContrastToConeInc*theIsochromaticConeContrast;

% Red green is a little tricker.  We need red/green increments that have
% equal and opposite luminances.  Since increment depends on the
% background, we have to work backwards from increments to the appropriate
% contrasts.  These will not be equal to each other, in general.
relMContrast = bgLMS(1)/bgLMS(2)*LMLumWeights(1)/LMLumWeights(2);
meanRedGreenAbsConeContrast = mean([theBaseConeContrast relMContrast*theBaseConeContrast]);
theRedGreenConeContrast = [theBaseConeContrast -relMContrast*theBaseConeContrast 0]';
theRedGreenDKL = M_ConeIncToDKL*M_ConeContrastToConeInc*theRedGreenConeContrast;

% The third DKL isolating direction is also easy, because it's just S cone
% contrast.
theBYConeContrast = [0 0 theBaseConeContrast]';
theBYDKL = M_ConeIncToDKL*M_ConeContrastToConeInc*theBYConeContrast;

% Choose a scaling matrix so that the first DKL length is the luminance
% contrast, the second is the average of the LM contrasts for the
% isoluminant red-green direciton, and the third is the S cone contrast.
% You may have detected by now that there are lots of ways to choose the
% scaling of these directions. 
M_scaleDKLForContrastInput = diag(1./[theIsochromaticDKL(1)/theBaseConeContrast theRedGreenDKL(2)/meanRedGreenAbsConeContrast theBYDKL(3)/theBaseConeContrast]);
M_coneContrastToDKL = M_scaleDKLForContrastInput*M_ConeIncToDKL*M_ConeContrastToConeInc;

% Test full scaling.  This should be diagonal with the first and third
% entries equal to the base contrast and the middle one equal to the mean
% of the isoluminant red green cone contrasts.
theDKLFromConeContrasts = M_coneContrastToDKL*[theIsochromaticConeContrast theRedGreenConeContrast theBYConeContrast];

% Get the isolating directions.
M_DKLToConeContrast = inv(M_coneContrastToDKL);

% Check that regression on the isolating directions does what we expect.
% The output here should match what we get just above, in variable
% theDKLFromConeConrasts.  And it does to excellent tolerance.
theDKLFromConeContrastsCheck = M_DKLToConeContrast\[theIsochromaticConeContrast theRedGreenConeContrast theBYConeContrast];
if (max(abs(theDKLFromConeContrastsCheck(:)-theDKLFromConeContrasts(:))) > 1e-6)
    error('Unexpected difference in check values');
end


