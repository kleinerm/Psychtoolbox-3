function prob = NormalCumulative(x,u,var)
% function prob = NormalCumulative(x,u,var)
% 
% Compute the probability that a draw from a N(u,var) distribution is less
% than x.
%
% 6/25/96  dhb  Fixed for new erf convention.
% 7/24/04  awi  Cosmetic

[m,n] = size(x);
z = (x - u*ones(m,n))/sqrt(var);
prob = 0.5 + erf(z/sqrt(2))/2;
