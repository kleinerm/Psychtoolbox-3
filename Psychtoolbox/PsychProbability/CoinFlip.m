function x = CoinFlip(ndraws,p)
% x = CoinFlip(ndraws,p)
%
% Generate a list x of zeros and ones according to coin flipping (i.e.
% Bernoulli) statistics with probability p of getting a 1.

% 1/20/97  dhb  Delete obsolet rand('uniform').
% 7/24/04  awi  Cosmetic.


% Generate ndraws random variables on the real
% interval [0,1).
unif = rand(ndraws,1);

% Find all of the ones that are less than p.
% On average, this proportion will be p.
index = find(unif < p);
[nones,m] = size(index);

% Generate an array of zeros and then set
% the ones found in the previous step to 1.
x = zeros(ndraws,1);
if (nones ~= 0)
  x(index) = ones(nones,m);
end



