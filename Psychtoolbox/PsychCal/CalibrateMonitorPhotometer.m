function [ gammaTable1, gammaTable2, displayBaseline, displayRange, displayGamma ] = CalibrateMonitorPhotometer( numMeasures )
% [gammaTable1, gammaTable2, displayRange. displayGamma] =
%       CalibrateMonitorPhotometer(numMeasures)
% A simple calibration script for analog photometers. numMeasures (default: 9) 
% readings are taken manually, and the readings are fit with a gamma function 
% and piecewise cubic splines. numMeasures - 1 should be a power of 2, ideally (9, 17, 33, etc.).
% The corresponding linearized gamma tables (1 -> gamma,
% 2 -> splines) are returned, as well as the display baseline, display range in cd/m^2 and
% display gamma. Plots of the two fits are created as well. Requires fit
% tools.
%
% If the normalized gamma table is not loaded, then the cd/m^2 value of a
% screen value can be figured out by the formula: cdm2 =
% displayRange*(screenval/255).^(1/displayGamma) + displayBaseline.
%
% Generally, you will want to load the normalized gamma tables and use them
% in Screen('LoadNormalizedGammaTable'). For example:
%
% [gammaTable1, gammaTable2] = CalibrateMonitorPhotometer;
% %Look at the outputted graphs to see which one gives a better fit
% %Then save the corresponding gamma table for later use
% gammaTable = gammaTable1;
% save MyGammaTable gammaTable
% 
% %Then when you're ready to use the gamma table:
% load MyGammaTable
% Screen('LoadNormalizedGammaTable', win, gammaTable*[1 1 1]);
% 
% Version 1.0: Patrick Mineault (patrick.mineault@gmail.com)
    if(nargin < 1)
        numMeasures = 9;
    end

    input(sprintf(['When black screen appears, point photometer, \n' ...
           'get reading in cd/m^2, input reading using numpad and press enter. \n' ...
           'A screen of higher luminance will be shown. Repeat %d times. ' ...
           'Press enter to start'], numMeasures));
       


    origclut = repmat([0:255]'/255,1,3);

    [win rect] = Screen('OpenWindow', 0, 0);
    Screen('LoadNormalizedGammaTable', win, origclut );
    
    vals = [];
    inputV = [0:256/(numMeasures - 1):256];
    inputV(end) = 255;
    for i = inputV
        Screen('FillRect',win,i);
        Screen('Flip',win);
        
        resp = input('Value?');
        vals = [vals resp];
    end
    Screen('CloseAll');
    
    displayRange = (max(vals) - min(vals));
    displayBaseline = min(vals);
    
    %Normalize values
    vals = (vals - displayBaseline)/(max(vals) - min(vals));
    inputV = inputV/255;
    
    %Gamma function fitting
    g = fittype('x^g');
    fittedmodel = fit(inputV',vals',g);
    displayGamma = fittedmodel.g;
    gammaTable1 = ((([0:255]'/255))).^(1/fittedmodel.g);
    
    firstFit = fittedmodel([0:255]/255);
    
    %Spline interp fitting
    fittedmodel = fit(inputV',vals','splineinterp');
    secondFit = fittedmodel([0:255]/255);
    
    figure;
    plot(inputV, vals, '.', [0:255]/255, firstFit, '--', [0:255]/255, secondFit, '-.');
    legend('Measures', 'Gamma model', 'Spline interpolation');
    title(sprintf('Gamma model x^{%.2f} vs. Spline interpolation', displayGamma));
    
    %Invert interpolation
    fittedmodel = fit(vals',inputV','splineinterp');
    
    gammaTable2 = fittedmodel([0:255]/255);