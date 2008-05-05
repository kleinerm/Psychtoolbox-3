function [scal] = fit(scal)

% Generate the polynomial fitting coefficients and save them in the
% calibration file. Fit the pixel settings to the real position that the
% dots have in centimeters.

% the real position in centimeters of the dot selection:
selxcm = scal.xcm(scal.FITDOTLIST);
selycm = scal.ycm(scal.FITDOTLIST);

% do the fitting
scal.xFitCoef = poly2fit(selxcm, selycm, scal.SELECTXCALIBDOTS, scal.NOrderPoly);
scal.yFitCoef = poly2fit(selxcm, selycm, scal.SELECTYCALIBDOTS, scal.NOrderPoly);
