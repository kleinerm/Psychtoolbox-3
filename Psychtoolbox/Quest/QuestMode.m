function [t,p]=QuestMode(q)
% [t,p]=QuestMode(q)
%
% "t" is the mode threshold estimate
% "p" is the value of the (unnormalized) pdf at t.
% 
% See Quest.

% Denis Pelli, 6/8/96
% 3/1/97 dgp updated to use Matlab 5 structs.
% 4/12/99 dgp dropped support for Matlab 4.

% Copyright (c) 1996-2004 Denis Pelli

if nargin~=1
	error('Usage: t=QuestMode(q)')
end
if length(q)>1
	t=zeros(size(q));
	for i=1:length(q(:))
		t(i)=QuestMode(q(i));
	end
	return
end

[p,iMode]=max(q.pdf);
t=q.x(iMode)+q.tGuess;
