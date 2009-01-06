function s = stdall(in)
% [s] = stdall(in)
% returns the standard deviation of input IN. IN can be anything from a
% scalar to an N-D matrix
%
% DN 2008

s = std(in(:));