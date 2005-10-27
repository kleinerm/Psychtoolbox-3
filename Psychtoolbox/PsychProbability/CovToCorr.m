function corr = CovToCorr(K)
% corr = CovToCorr(K)
% 
% Compute the matrix of correlations from the covariance matrix.

% 8/23/94		dhb		Wrote it.
% 7/24/04       awi     Cosmetic.


vars = diag(K);
denom = vars*vars';
corr = K ./ (denom.^0.5);
