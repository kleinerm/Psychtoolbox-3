function cal = CalibrateFitGamma(cal,nInputLevels)
% cal = CalibrateFitGamma(cal,[nInputLevels])
%
% Fit the gamma function to the calibration measurements.  Options for field
% cal.describe.gamma.fitType are:
%    simplePower
%    crtPolyLinear
%    crtGamma
%    sigmoid
%    weibull
%
% Underlying fit routine is FitGamma.
%
% See also PsychGamma.
%
% 3/26/02  dhb  Pulled out of CalibrateMonDrvr.
% 11/14/06 dhb  Define nInputLevels and pass to underlying fit routine.
% 07/22/07 dhb  Add simplePower fitType.
% 08/02/07 dhb  Optional pass of nInputLevels.
%          dhb  Don't allow a long string of zeros at the start.
%          dhb  Reduce redundant code for higher order terms by pulling out of switch

% Set nInputLevels
if (nargin < 2 | isempty(nInputLevels))
    nInputLevels = 1024;
end

% Fit gamma functions.
switch(cal.describe.gamma.fitType)
    
     case 'simplePower',
        mGammaMassaged = cal.rawdata.rawGammaTable(:,1:cal.nDevices);
        for i = 1:cal.nDevices
            mGammaMassaged(:,i) = MakeMonotonic(HalfRect(mGammaMassaged(:,i)));
        end
        fitType = 1;
        [mGammaFit1a,cal.gammaInput,mGammaCommenta] = FitDeviceGamma(...
            mGammaMassaged,cal.rawdata.rawGammaInput,fitType,nInputLevels);
        mGammaFit1 = mGammaFit1a;
        
    case 'crtPolyLinear',
        % For fitting, we set to zero the raw data we
        % believe to be below reliable measurement threshold (contrastThresh).
        % Currently we are fitting both with polynomial and a linear interpolation,
        % using the latter for low measurement values.  The fit break point is
        % given by fitBreakThresh.   This technique was developed
        % through bitter experience and is not theoretically driven.
        mGammaMassaged = cal.rawdata.rawGammaTable(:,1:cal.nDevices);
        massIndex = find(mGammaMassaged < cal.describe.gamma.contrastThresh);
        mGammaMassaged(massIndex) = zeros(length(massIndex),1);
        for i = 1:cal.nDevices
            mGammaMassaged(:,i) = MakeMonotonic(HalfRect(mGammaMassaged(:,i)));
        end
        fitType = 7;
        [mGammaFit1a,cal.gammaInput,mGammaCommenta] = FitDeviceGamma(...
            mGammaMassaged,cal.rawdata.rawGammaInput,fitType,nInputLevels);
        fitType = 6;
        [mGammaFit1b,cal.gammaInput,mGammaCommentb] = FitDeviceGamma(...
            mGammaMassaged,cal.rawdata.rawGammaInput,fitType,nInputLevels);
        mGammaFit1 = mGammaFit1a;
        for i = 1:cal.nDevices
            indexLin = find(mGammaMassaged(:,i) < cal.describe.gamma.fitBreakThresh);
            if (~isempty(indexLin))
                breakIndex = max(indexLin);
                breakInput = cal.rawdata.rawGammaInput(breakIndex);
                inputIndex = find(cal.gammaInput <= breakInput);
                if (~isempty(inputIndex))
                    mGammaFit1(inputIndex,i) = mGammaFit1b(inputIndex,i);
                end
            end
        end
        
    case 'crtGamma',
        mGammaMassaged = cal.rawdata.rawGammaTable(:,1:cal.nDevices);
        for i = 1:cal.nDevices
            mGammaMassaged(:,i) = MakeMonotonic(HalfRect(mGammaMassaged(:,i)));
        end
        fitType = 2;
        [mGammaFit1a,cal.gammaInput,mGammaCommenta] = FitDeviceGamma(...
            mGammaMassaged,cal.rawdata.rawGammaInput,fitType,nInputLevels);
        mGammaFit1 = mGammaFit1a;
        
    case 'sigmoid',
        mGammaMassaged = cal.rawdata.rawGammaTable(:,1:cal.nDevices);
        for i = 1:cal.nDevices
            mGammaMassaged(:,i) = MakeMonotonic(HalfRect(mGammaMassaged(:,i)));
        end
        fitType = 3
        [mGammaFit1a,cal.gammaInput,mGammaCommenta] = FitDeviceGamma(...
            mGammaMassaged,cal.rawdata.rawGammaInput,fitType,nInputLevels);
        mGammaFit1 = mGammaFit1a;
        
    case 'weibull',
        mGammaMassaged = cal.rawdata.rawGammaTable(:,1:cal.nDevices);
        for i = 1:cal.nDevices
            mGammaMassaged(:,i) = MakeMonotonic(HalfRect(mGammaMassaged(:,i)));
        end
        fitType = 4;
        [mGammaFit1a,cal.gammaInput,mGammaCommenta] = FitDeviceGamma(...
            mGammaMassaged,cal.rawdata.rawGammaInput,fitType,nInputLevels);
        mGammaFit1 = mGammaFit1a;

    otherwise
        error('Unsupported gamma fit string passed');

end

% Fix contingous zeros at start problem
mGammatFit1 = FixZerosAtStart(mGammaFit1);

% Handle higher order terms, which are just fit with a polynomial
if (cal.nPrimaryBases > 1)
    [m,n] = size(mGammaFit1);
    mGammaFit2 = zeros(m,cal.nDevices*(cal.nPrimaryBases-1));
    for j = 1:cal.nDevices*(cal.nPrimaryBases-1)
        mGammaFit2(:,j) = ...
            FitGammaPolyR(cal.rawdata.rawGammaInput,cal.rawdata.rawGammaTable(:,cal.nDevices+j), ...
            cal.gammaInput);
    end
    mGammaFit = [mGammaFit1 , mGammaFit2];
else
    mGammaFit = mGammaFit1;
end
        
% Save information in form for calibration routines.
cal.gammaFormat = 0;
cal.gammaTable = mGammaFit;

return

% output = FixZerosAtStart(input)
%
% The OS/X routines need the fit gamma function to be monotonically
% increasing.  One way that sometimes fails is when a whole bunch of
% entries at the start are zero.  This routine fixes that up.
function output = FixZerosAtStart(input)

output = input;
for i = 1:size(input,2)
    for j = 1:size(input,1)
        if (input(i,j) > 0)
            break;
        end
    end
    if (j == size(input,1))
        error('Entire passed gamma function is zero');
    end
    output(1:j,i) = linspace(0,min([0.0001 input(j+1,i)/2]),j)';
end

return


