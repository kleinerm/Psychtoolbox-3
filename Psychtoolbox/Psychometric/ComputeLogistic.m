function y = ComputeLogistic(x,a,b)
% y = ComputeLogistic(x,a,b)
%
% Compute a logistic equation.
% The form of the logistic equation is y = 1/(1+10^-(ax+b)).
%
% See also: FitLogistic, InvertLogistic, FitLogitYN.
%
% 2/15/95		dhb		Wrote it.

y = 1 ./ (1 + (10.0 .^ (-1*(a*x+b))));
