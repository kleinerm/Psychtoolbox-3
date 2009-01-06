function [q] = allall(in)
% [q] = allall(in)
% returns true if all element of in are non-zeros, false otherwise
% returns a scalar boolean. IN can be anything from a scalar to an N-D
% matrix
%
% DN 2008

q = all(in(:));