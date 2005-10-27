function prob = MultiNormalPDF(x,u,K)
% prob = NormalPDF(x,u,K)
%
% Computes the p(x) according to N(u,K). Vectors x and u should be column
% vectors.
%
% Formula taken from Duda and Hart, page 23.
%
% Does not work properly if K is singular. Does not check for dimensional
% correctness.

% 12/31/93  dhb  Wrote it.
% 7/24/04   awi  cosmetic.

% Find dimension
[m,null] = size(u);

% Compute the exponential part
factor1 = exp(-0.5*(x-u)'*inv(K)*(x-u));

% Compute the leading constant
factor2 = ((2*pi)^(m/2))*sqrt(det(K));
prob = factor1/factor2;
