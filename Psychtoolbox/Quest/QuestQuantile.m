function t=QuestQuantile(q,quantileOrder)
% intensity=QuestQuantile(q,[quantileOrder])
% 
% Gets a quantile of the pdf in the struct q. You may specify the desired
% quantileOrder, e.g. 0.5 for median, or, making two calls, 0.05 and 0.95
% for a 90% confidence interval. If the "quantileOrder" argument is not
% supplied, then it's taken from the "q" struct. QuestCreate uses
% QuestRecompute to compute the optimal quantileOrder and saves that in the
% "q" struct; this quantileOrder yields a quantile  that is the most
% informative intensity for the next trial.
% 
% This is based on work presented at a conference, but otherwise unpublished:
% Pelli, D. G. (1987). The ideal psychometric procedure. Investigative 
% Ophthalmology & Visual Science, 28(Suppl), 366.
% 
% See Quest.

% Denis Pelli, 6/9/96
% 6/17/96 dgp, worked around "nonmonotonic" (i.e. not strictly monotonic)
%				interp1 error.
% 3/1/97 dgp updated to use Matlab 5 structs.
% 4/12/99 dgp removed support for Matlab 4.
%
% Copyright (c) 1996-1999 Denis Pelli

if nargin>2
	error('Usage: intensity=QuestQuantile(q,[quantileOrder])')
end
if length(q)>1
	if nargin>1
		error('Can''t accept quantileOrder for q vector. Set each q.quantileOrder instead.')
	end
	t=zeros(size(q));
	for i=1:length(q(:))
		t(i)=QuestQuantile(q(i));
	end
	return
end
if nargin<2
	quantileOrder=q.quantileOrder;
end
p=cumsum(q.pdf);
if ~isfinite(p(end))
	error('pdf is not finite')
end
if p(end)==0
	error('pdf is all zero')
end
index=find(diff([-1 p])>0);
if length(index)<2
	error(sprintf('pdf has only %g nonzero point(s)',length(index)))
end
t=q.tGuess+interp1(p(index),q.x(index),quantileOrder*p(end)); % 40 ms
