function out = RandLim(n,lower,upper)
% returns pseudo-random values drawn from a uniform distribution with lower
% and upper limits LOWER and UPPER.
% LOWER and UPPER can also be matrices of the same shape as rand(N) would
% create
%
% N is a vector indicating the dimensions of the output:
%   N = [3]      creates a    3x3 matrix
%   N = [1,3]    creates a    1x3 matrix
%   N = [17,1,3] creates a 17x1x3 matrix
%
% LOWER and UPPER can be any number

% DN 2008-07-21 Wrote it
% DN 2008-09-19 Support for vector lower and upper limits

error(nargchk(3, 3, nargin, 'struct'))

r = rand(n);
if (~isscalar(lower) && any(size(lower)~=size(r))) || (~isscalar(upper) && any(size(upper)~=size(r)))
    error('LOWER and UPPER must each be scalar or have the same shape as rand(N)')
else
    out = lower + r.*(upper-lower);
end
