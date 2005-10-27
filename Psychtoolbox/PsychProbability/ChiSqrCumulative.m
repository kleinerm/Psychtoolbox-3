function prob = ChiSqrCumulative(X2,v)
% prob = ChiSqrCumulative(X2,v)
%
% Computes the chi-squared probability function. ChiSqrCumulative(X2,v)
% returns P(X2|v), the probability of observing a chi-squared value <= X2
% with v degrees of freedom. This is the probability that the sum of
% squares of v unit-variance normally-distributed random variables is <=
% X2. X2 and v may be matrices of the same size size, or either may be a
% scalar.
%
% e.g., ChiSqrCumulative(5.99,2) returns 0.9500, verifying the 95%
% confidence bound for 2 degrees of freedom. This is also cross-checked in,
% e.g., Abramowitz & Stegun Table 26.8
%
% References: Press et al., Numerical Recipes, Cambridge, 1986;
% Abramowitz & Stegun, Handbook of Mathematical Functions, Dover, 1972.
%
% Peter R. Shaw, Woods Hole Oceanographic Institution
% Woods Hole, MA 02543
% (508) 457-2000 ext. 2473  pshaw@aqua.whoi.edu
% March, 1990
%
% Computed using the Incomplete Gamma function, as given by 
% Press et al. (Recipes) eq. (6.2.17)


% 9/28/00  dhb  Updated call to gamma to an explicit call to gammainc to match
%               newer Matlab conventions.
% 7/24/04  awi  Cosmetic.  Changed Contents.m where this was named
%               "ChiSquareCumulative" not ChiSqrCumulative

prob = gammainc(X2/2,v/2);
