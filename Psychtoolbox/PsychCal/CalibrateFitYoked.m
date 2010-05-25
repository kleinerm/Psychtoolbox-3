function cal = CalibrateFitYoked(cal)
% cal = CalibrateFitYoked(cal)
%
% Fit the gamma data from the yoked measurements.
%
% 4/30/10  dhb, kmo, ar  Wrote it.
% 5/24/10  dhb           Update comment.
% 5/25/10  dhb, ar       New input format.

%% Check that this is possible
if (~isfield(cal.describe,'yoked') || cal.describe.yoked == 0)
    error('Need yoked data to be present for this routine to work');
end
if (~isfield(cal,'yoked') || ~isfield(cal.yoked,'spectra'))
    error('You can''t fit the yoked data if it wasn''t measured.');
end
if (cal.nPrimaryBases == 0)
    error('Need to specify an explicit linear model dimension for this procedure to be well-defined.');
end

%% Average yoked measurements for this primary
yokedSpds = cal.yoked.spectra;

%% Fit each spectrum with the linear model for all three primaries
% and project down onto this
projectedYokedSpd = cal.P_device*(cal.P_device\averageYokedSpd);

%% Now we have to adjust the linear model so that it has our standard 
% properties.

% Make first three basis functions fit maxSpd exactly
maxSpd = projectedYokedSpd(:,end);
weights = cal.P_device\maxSpd;
currentLinMod = zeros(size(cal.P_device));
for i = 1:cal.nDevice
    tempLinMod = 0;
    for j = 1:cal.nPrimaryBases
        tempLinMod = tempLinMod + cal.P_device(:,i+(j-1)*cal.nDevice)*weights(i+(j-1)*cal.nDevice);
    end
    currentLinMod(:,i) = tempLinMod;
end
weights = currentLinMod(:,1:cal.nDevice)\maxSpd;
for i = 1:cal.nDevice
    currentLinMod(:,i) = currentLinMod(:,i)*weights(i);
end
maxPow = max(max(currentLinMod(:,1:cal.nDevice)));

% Now find the rest of the linear model
clear tempLinMod
for i = 1:cal.nDevice
    for j = 1:cal.nPrimaryBases
        tempLinMod(:,j) = cal.P_device(:,i+(j-1)*cal.nDevice); %#ok<AGROW>
    end
    residual = tempLinMod - currentLinMod(:,i)*(currentLinMod(:,i)\tempLinMod);
    restOfLinMod = FindLinMod(residual,cal.nDevice-1);
    for j = 2:cal.nPrimaryBases
        tempMax = max(abs(restOfLinMod(:,j-1)));
        currentLinMod(:,i+(j-1)*cal.nDevice) = maxPow*restOfLinMod(:,j-1)/tempMax;
    end
end

% Express yoked spectra in terms of model weights
gammaTable = currentLinMod\projectedYokedSpd;

% Stash info in calibration structure
cal.P_device = currentLinMod;

% Store correct gamma input settings based on type of yoking.
%
% When R=G=B, we just use the common settings.
if (cal.describe.yoked == 1)
    cal.rawdata.rawGammaTable = cal.yoked.settings(1,:)';
    
% When measurements are at a specified chromaticity, need to interpolate gamma
% functions so that we have them for each device on a common scale.
elseif (cal.describe.yoked == 2)
    error('Need to implement an interoplation procedure to bring all devices on common input scale.');
end

return


