function x = NormalDraw(ndraws,u,var);
% x = NormalDraw(ndraws,u,var)
%
% Generate ndraws from a N(u,var) distribution.

% 7/24/04  awi  Added this line.


uvec = u .* ones(ndraws,1);
x = sqrt(var) .* randn(ndraws,1) + u;
