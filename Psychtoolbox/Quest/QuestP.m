function p=QuestP(q,x)
% p=QuestP(q,x)
%
% The probability of a correct (or yes) response at intensity x, assuming
% threshold is at x=0.
%
% See Quest.

% 7/25/04   awi     cosmetic (help text layout).

% Copyright (c) 1996-2004 Denis Pelli

if x<q.x2(1)
	p=q.p2(1)
elseif x>q.x2(end)
	p=q.p2(end)
else
	p=interp1(q.x2,q.p2,x);
end
if ~isfinite(p)
	q
	error(sprintf('psychometric function %g at %.2g',p,x))
end
