function corr = FindNeighborCorr(X)
% corr = FindNeighborCorr(X)
% 
% Find the nearest neighbor correlation in a data set X

% 8/26/94		dhb		Wrote it.
% 7/24/04       awi     Cosmetic.


% Find the sample covariance matrix
K = cov(X');

% Convert to matrix of correlations
C = CovToCorr(K);

% Get the nearest neighbor correlations
neighborCorr = diag(C,1);

% Return the mean
corr = mean(neighborCorr);
