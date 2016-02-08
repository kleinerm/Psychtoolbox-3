function absorbance = ShiftPhotopigmentAbsorbance(S,absorbance,lambdaMaxShift)
% absorbance = ShiftPhotopigmentAbsorbance(S,absorbance,lambdaMaxShift)
% 
% Shift a photopigment absorbance along a log wavelength axis to give it
% a new lambda max while plausibly retaining its shape appropriately.  
% This is probabably very reasonable for small shifts and less so for big
% ones.
%
% Linearly extrapolates as necessary to preserve wavelength sampling of
% input.
%
% 2/8/16  dhb, ms  Wrote it.

% Check
if (length(lambdaMaxShift) ~= size(absorbance,1))
    error('Number of absorbances and length of shift vector do not match');
end

% Normalize the wave number and log wavelength
wls = MakeItWls(S);
[~, maxIdx] = max(absorbance, [], 2);
for ii = 1:size(absorbance,1);
	logWavelengthsNorm = log10(wls) - log10(wls(maxIdx(ii)));
    logWavelengthsNew = logWavelengthsNorm + log10(wls(maxIdx(ii))+lambdaMaxShift(ii));
    wlsNew = 10.^logWavelengthsNew;
    absorbance(ii, :) = interp1(wlsNew, absorbance(ii, :), wls, 'linear','extrap');
end

