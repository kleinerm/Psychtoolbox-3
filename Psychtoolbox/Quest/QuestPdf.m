function p=QuestPdf(q,t)
% p=QuestPdf(q,t)
% 
% The (possibly unnormalized) probability density of candidate threshold "t".
% q and t may be vectors of the same size, in which case the returned p is a vector of that size.
% 
% See Quest.

% Denis Pelli
% 5/6/99 dgp wrote it
% Copyright (c) 1996-1999 Denis Pelli

if nargin~=2
	error('Usage: p=QuestPdf(q,t)')
end
if size(q)~=size(t)
	error('both arguments must have the same dimensions')
end
if length(q)>1
	p=zeros(size(q));
	for i=1:length(q(:))
		p(i)=QuestPdf(q(i),t(i));
	end
	return
end
i=round((t-q.tGuess)/q.grain)+1+q.dim/2;
i=min(length(q.pdf),max(1,i));
p=q.pdf(i);
