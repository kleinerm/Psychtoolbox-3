function cal = CalibrateFitYoked(cal)
% cal = CalibrateFitYoked(cal)
%
% Fit the gamma data from the yoked measurements.
%
% 4/30/10  dhb, kmo, ar  Wrote it.
% 5/24/10  dhb           Update comment.

%% Check that this is possible
if (~isfield(cal.rawdata,'yokedSpd'))
    error('You can''t fit the yoked data if it wasn''t measured.');
end
if (cal.nPrimaryBases == 0)
    error('Need to specify an explicit linear model dimension for this procedure to be well-defined.');
end

%% Average yoked measurements for this primary
yokedSpds = cal.rawdata.yokedSpd;
[nAverage,nPrimaries] = size(yokedSpds);
averageYokedSpd = 0;
for i = 1:nPrimaries
    for a = 1:nAverage
        averageYokedSpd = averageYokedSpd + yokedSpds{a,i};
    end
        
end
averageYokedSpd = averageYokedSpd/(nAverage*nPrimaries);

%% Fit each spectrum with the linear model for all three primaries
% and project down onto this
projectedYokedSpd = cal.P_device*(cal.P_device\averageYokedSpd);

%% Now we have to adjust the linear model so that it has our standard 
% properties.

% Make first three basis functions fit maxSpd exactly
maxSpd = projectedYokedSpd(:,end);
weights = cal.P_device\maxSpd;
currentLinMod = zeros(size(cal.P_device));
for i = 1:nPrimaries
    tempLinMod = 0;
    for j = 1:cal.nPrimaryBases
        tempLinMod = tempLinMod + cal.P_device(:,i+(j-1)*nPrimaries)*weights(i+(j-1)*nPrimaries);
    end
    currentLinMod(:,i) = tempLinMod;
end
weights = currentLinMod(:,1:nPrimaries)\maxSpd;
for i = 1:nPrimaries
    currentLinMod(:,i) = currentLinMod(:,i)*weights(i);
end
maxPow = max(max(currentLinMod(:,1:nPrimaries)));

% Now find the rest of the linear model
clear tempLinMod
for i = 1:nPrimaries
    for j = 1:cal.nPrimaryBases
        tempLinMod(:,j) = cal.P_device(:,i+(j-1)*nPrimaries); %#ok<AGROW>
    end
    residual = tempLinMod - currentLinMod(:,i)*(currentLinMod(:,i)\tempLinMod);
    restOfLinMod = FindLinMod(residual,nPrimaries-1);
    for j = 2:cal.nPrimaryBases
        tempMax = max(abs(restOfLinMod(:,j-1)));
        currentLinMod(:,i+(j-1)*nPrimaries) = maxPow*restOfLinMod(:,j-1)/tempMax;
    end
end

% Express yoked spectra in terms of model weights
gammaTable = currentLinMod\projectedYokedSpd;

% Stash info in calibration structure
cal.P_device = currentLinMod;
cal.rawdata.rawGammaTable = gammaTable';
cal.describe.yokedGamma = 1;

return


