function out = altround(nr,factor)
% out = altround(nr,factor)
% rounds the elements in NR to the nearest multiple
% of FACTOR.
% Thus:
%   altround(12.26,.25) = 12.25;
%   altround(12.2 ,0.6) = 12,
%   but also
%   altround(1449 ,100) = 1400.
%
% DN 2008


if isscalar(factor) && isnumeric(factor) && isfinite(factor)
    factor = 1/factor;
else
    error('Factor argument must be a numeric and finite scalar. See help');
end

out = round(nr.*factor)./factor;