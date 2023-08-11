function [a,b] = FitLogistic(x,y)
% [a,b] = FitLogistic(x,y)
% 
% Fit a logistic function to the data pairs x,y.
%
% The form of the logistic equation is y = 1/(1+10^-(ax+b)).
% The method used here is to regres on transformed coordinates.
% One could use search to do a maximum likelihood function for
% psychometric data, but if you are going to do that, you
% should probably fit a cumulative normal or Weibull function.
%
% See also: ComputeLogistic, InvertLogistic, FitLogitYN, FitWeibTAFC, 
%   FitWeibYN, FitAlphaWeibTAFC.
%
% 2/15/95		dhb		Wrote it.

% Get rid of bad values
index = find( y == 0 );
y(index) = 0.001*ones(size(index));
index = find( y == 1 );
y(index) = 0.999*ones(size(index));

regresY = log10( (1-y) ./ y );
ab = [x ones(size(x))]\regresY;
a = -ab(1);
b = -ab(2);

