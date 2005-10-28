function x = InvertLogistic(y,a,b);
% x = InvertLogistic(y,a,b);
%
% Compute the inverse of a logistic equation.
% The form of the logistic equation is y = 1/(1+10^-(ax+b)).
%
% See also: ComputeLogistic, FitLogistic, FitLogitYN.
%
% 2/15/95		dhb		Wrote it.

% Enforce range sanity
if (y == 1)
	y = 0.99;
elseif (y == 0)
  y = 0.01;
end

% Now invert the equation
axb = -1*log10( (1-y)/y );
x = (axb-b)/a;
