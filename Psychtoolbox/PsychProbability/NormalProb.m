function probs = NormalProb(center,width,u,var)
% probs = NormalProb(center,width,u,var)
%
% Compute the probability that a N(u,var) random variable lies between
% center-width/2 and center+width/2

% 7/24/04  awi  Cosmetic
% 1/29/05  dgp  Cosmetic

% Find low and high values of each bin
[m,n] = size(center);
widths = width*ones(m,n);
lows = center - widths/2;
highs = center + widths/2;

probs=abs(NormalCumulative(lows,u,var)-NormalCumulative(highs,u,var));

