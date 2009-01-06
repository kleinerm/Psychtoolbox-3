function out = randlim(n,lower,upper)
% returns pseudo-random values drawn from a uniform distribution with lower
% and upper limits LOWER and UPPER.
%
% N is a vector indicating the dimensions of the output:
%   N = [3]      creates a    3x3 matrix
%   N = [1,3]    creates a    1x3 matrix
%   N = [17,1,3] creates a 17x1x3 matrix
%
% LOWER and UPPER can be any number

% DN 2008-07-21 Wrote it

if nargin~=3
    error('Three input arguments expected')
else
    out = lower + rand(n)*(upper-lower);
end