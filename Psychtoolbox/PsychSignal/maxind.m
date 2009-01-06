function [m,inds] = maxind(mat)
% [m,inds] = maxind(mat)
% returns maximum of an N-D matrix and the indices at which this maximum
% occurs
% indices are returned in a [#maxs x ndims] matrix

% DN 2008-07-21 wrote it

m                   = max(mat(:));
inds                = findind(mat==m);