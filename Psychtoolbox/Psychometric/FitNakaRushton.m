function [params,f] = FitNakaRushton(contrast,response,params0)
% [params,f] = FitNakaRushton(contrast,response [, params0])
%
% Find the parameters that best fit the data.  If the initial guess
% isn't good, this can get stuck in local minima.  
%
% 8/1/05    dhb, pr     Wrote it.
% 8/2/07    dhb         Change argument order.

% If params0 isn't passed, try to pick a good default.
if (nargin < 3 || isempty(params0))
    params0(1) = max(response);
    params0(2) = mean(contrast);
    params0(3) = 2;
end

% Set up minimization options
options = optimset;
options = optimset(options,'Display','off');
if ~IsOctave
    options = optimset(options,'LargeScale','off');
end

vlb = [0      0.001     0.001]';
vub = [1000   100000    100]';
params = fmincon(@FitNakaRushtonFun,params0,[],[],[],[],vlb,vub,[],options); 
f = FitNakaRushtonFun(params);

    % Nested target function to optimize:
    function [f] = FitNakaRushtonFun(params)
    % [f] = FitNakaRushtonFun(params)
    % 
    % Evaluate model fit and return measure of goodness of fit (f).
    %
    % 8/1/05    dhb, pr     Wrote it.
    % 8/2/07    dhb         Get rid of silly call to ComputeNakaRushtonError.

    % Unpack paramters, make predictions
    prediction = ComputeNakaRushton(params,contrast);
    nPoints = length(prediction);
    error = prediction-response;
    f = 10000*sqrt(sum(error.^2)/nPoints);

    % Handle bizarre parameter values.
    if (isnan(f))
        f = 2000;
    end
end
