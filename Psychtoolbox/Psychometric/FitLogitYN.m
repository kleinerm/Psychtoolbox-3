function [a,b,thresh50] = FitLogistic(inputs,nYes,nNo)
% [a,b,thresh50] = FitLogistic(inputs,nYes,nNo)
% 
% Fit a logistic function to YN psychometric data.
% Returns logistic parameters and 50% threshold. 
%
% The form of the logistic equation is pYes = 1/(1+10^-(a*inputs+b))
% 
% The logistic is not a good function to use for serious work,
% but you can do a quick and dirty fit analytically and
% you can explain it to students more easily than some
% other candidate models for psychometric functions.
%
% See also: FitLogistic, FitWeibYN, FitCumNormYN, 
%  InvertLogistic, ComputeLogistic.
%
% 2/8/97		dhb		Wrote it.

% Make sure there are trials passed at all input levels.
% Ignore any input levels with no trials
pYes = nYes ./ (nYes + nNo);
index = find(~isnan(pYes));
if (isempty(index))
	error('FItLogitYN: no input trials passed');
end
pYes = pYes(index);
inputs = inputs(index);

[a,b] = FitLogistic(inputs,pYes);
thresh50 = InvertLogistic(0.5,a,b);
