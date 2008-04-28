function choice = ChooseKFromN(n,k,unique)
% choice = ChooseKFromN(n,k,[unique])
%
% Choose k distinct integers out of n.   The indices come back in
% randomized order if unique is absent or 0.  They come back
% sorted if unique is equal to 1.
%
% 4/11/94	dhb		Wrote it.
% 2/8/98    dhb     Renamed to avoid name conflict with Matlab 5 nchoosek.m
%			dhb		Eliminate obsolete call to rand('uniform').
% 7/24/04   awi     Cosmetic.
% 1/29/05   dgp     Cosmetic.

% Handle optional arg
if nargin == 2
  unique = 0;
end

% Handle k > n case.
if k > n
  k = n;
end

% Sort random numbers to get indices
[null,index] = sort(rand(n,1));

% Pull out first k
choice = index(1:k);

% If unique is set, return them sorted.
if unique
	choice = sort(choice);
end
