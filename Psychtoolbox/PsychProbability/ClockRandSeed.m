function [seed,whichGen] = ClockRandSeed(seed,whichGen)
% [seed,whichGen] = ClockRandSeed([seed],[whichGen])
%
% ClockRandSeed seeds the random number from the time-of-day clock. If seed
% is passed this is used instead.
% 
% The multiplier 1e6 in fix(1e6*sum(clock)) is bigger than the 100
% suggested by Mathworks (see help RAND).  They suggest 100*sum(clock), but
% this would only change the seed once every 1/100 sec, which might
% correspond to many iterations of a loop. With the bigger  multiplier it's
% pretty sure (depending only on clock grain) that two successive calls
% will generate different seeds.
% 
% Also unlike the Mathworks suggestion (see help RAND), we call FIX. This
% has no effect on RAND and RANDN, but makes it easier to correctly print
% the seed to a file. When a random seed generates an interesting result
% one would like to be able to recreate the conditions that generated the
% run. If the seed 1e6*sum(clock) is printed to a file, '%.0f' will often
% round it differently than RAND would.
%
% Note in that in Matlab 5 there are two random number generators, the
% Matlab 5 generator and the Matlab 4 generator.  (Matlab 4
% has only the version 4 generator.)  ClockRandSeed checks what version of
% Matlab you're running and uses the version 5 generator if it can.  The
% string whichGen contains 'seed' for the version 4 generator and 'state'
% for the version 5 generator.
%
% See also: rand, randn.

% 10/15/93	dhb	Made this a function.
% 4/11/94	dhb	Added randn seeding.
% 8/9/97	dgp	Update for more reliable seed.
% 	        dgp Gutted it to make version-dependence explicit.
% 8/9/97    dhb Ungutted it to hide version-dependence.
% 8/13/97   dhb Optional return of which generator used and passing of seed/generator.
% 7/24/04   awi Cosmetic.
% 1/29/05   dgp Cosmetic.


if (nargin == 0)
	seed = fix(1e6*sum(clock));
	if sscanf(version,'%f',1)<5
		whichGen = 'seed';
	else
		whichGen = 'state';
	end
elseif (nargin == 1)
	if (isempty(seed))
		seed = fix(1e6*sum(clock));
	end
	if sscanf(version,'%f',1)<5
		whichGen = 'seed';
	else
		whichGen = 'state';
	end
else
	if (isempty(seed))
		seed = fix(1e6*sum(clock));
	end
	if (isempty(whichGen))
		if sscanf(version,'%f',1)<5
			whichGen = 'seed';
		else
			whichGen = 'state';
		end
	end
end

rand(whichGen,seed);randn(whichGen,seed);
