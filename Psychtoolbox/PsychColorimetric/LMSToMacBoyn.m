function ls = LMSToMacBoyn(LMS,T_cones,T_lum)
% ls = LMSToMacBoyn(LMS,[T_cones,T_lum])
%
% Compute MacLeod-Boynton chromaticity from cone exciation coordinates.
%
% If T_cones and T_lum are not passed, we assume that the passed LMS
% values were computed with respect to the Smith-Pokorny
% fundamentals normalized to a peak of 1.  That is, using the fundamentals
% stored in PTB's T_cones_sp. This is old usage and preserved for backwards
% compatibility, but the three argument usage as described below is
% preferred for clarity.
%
% If the LMS values were computed with respect to some other fundamentals,
% pass the cone fundamentals and the photopic luminance sensitiity
% function. This routine will scale passed L and M values so that they sum
% to the best linear approximation of luminance, and the S cone value to be
% as if it were computed with respect to an S cone fundamentals with a max
% of 1.
%
% 10/30/97  dhb  Wrote it.
% 7/9/02    dhb  T_cones_sp -> T_cones on line 20.  Thanks to Eiji Kimura.
% 1/23/18   dhb  Scale S cone value to make it as if fundamental had a max
%                of 1. This shouldn't break old code because this would
%                typically be called with fundamentals normalized to a max
%                of 1, since that's how most PTB data are provided. Thanks
%                to DG for pointing out that this scaling was not being
%                provided.

% Scale LMS so that L+M = luminance and S cone value corresponds to a
% fundamental with a max of 1.
if (nargin == 1)
	LMS = diag([0.6373 0.3924 1]')*LMS;
elseif (nargin == 3)
	factorsLM = (T_cones(1:2,:)'\T_lum');
    factorS = 1/max(T_cones(3,:));
	LMS = diag([factorsLM ; factorS])*LMS;
else
    error('Number of input arguments should be either 1 or 3');
end

% Compute ls coordinates from LMS
n = size(LMS,2);
ls = zeros(2,n);
denom = [1 1 0]*LMS;
ls = LMS([1 3],:) ./ ([1 1]'*denom);
