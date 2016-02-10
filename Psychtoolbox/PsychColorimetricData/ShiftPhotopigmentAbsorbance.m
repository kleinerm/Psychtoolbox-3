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
% 2/10/16 dhb, ms  Do shifting on a fine (0.25 nm) wavelength spacing.

% Check
if (length(lambdaMaxShift) ~= size(absorbance,1))
    error('Number of absorbances and length of shift vector do not match');
end

% Do the shifting on a fine wavelength spacing scale, so as to avoid
% unexpected wavelength quantization artifacts as a result of the shift.
wls = MakeItWls(S);
fineSpacing = 0.25;
if (S(2) < fineSpacing)
    error('Not a good idea to run this function with an input wavelength spacing less than 0.25 nm');
end
wlsFine = (wls(1):fineSpacing:wls(end))';
SFine = WlsToS(wlsFine);
absorbanceFine = SplineCmf(S,absorbance,SFine);

% Normalize the wave number and log wavelength
[~, maxIdx] = max(absorbanceFine, [], 2);
for ii = 1:size(absorbanceFine,1);
	logWavelengthsNorm = log10(wlsFine) - log10(wlsFine(maxIdx(ii)));
    logWavelengthsNew = logWavelengthsNorm + log10(wlsFine(maxIdx(ii))+lambdaMaxShift(ii));
    wlsFineNew = 10.^logWavelengthsNew;
    absorbanceFine(ii, :) = interp1(wlsFineNew, absorbanceFine(ii, :), wlsFine, 'linear','extrap');
end

% Spline the shifted function back to the original wavelength spacing
absorbance = SplineCmf(SFine,absorbanceFine,S);

