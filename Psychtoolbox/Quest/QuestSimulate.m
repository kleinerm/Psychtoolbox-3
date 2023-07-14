function response=QuestSimulate(q,tTest,tActual,plotIt)
% response=QuestSimulate(q,intensity,tActual [,plotIt])
%
% Simulate the response of an observer with threshold tActual when exposed
% to a stimulus tTest.
%
% 'plotIt' is optional: If set to a non-zero value, the simulated Quest
% session is visualized in a plot which shows the psychometric function of
% the simulated observer, where Quest placed test trials and what the
% observers response was. plotIt == 1 shows past trials in black, the
% current trial in green or red for a positive or negative response. plotIt
% == 2 color-codes all trials in red/green for negative or positive
% responses. By default, nothing is plotted.
% 
% See Quest.

% Denis Pelli, 6/8/96
% 3/1/97 dgp restrict intensity parameter to range of x2.
% 3/1/97 dgp updated to use Matlab 5 structs.
% 4/12/99 dgp dropped support for Matlab 4.
% 2/24/2018 dgp Allow psychometric function to be monotonically decreasing.

% Copyright (c) 1996-2018 Denis Pelli

if nargin < 3
	error('Usage: response=QuestSimulate(q,tTest,tActual[,plotIt])')
end
if length(q)>1
	error('can''t deal with q being a vector')
end
x2min=min(q.x2([1 end]));
x2max=max(q.x2([1 end]));
t=min(max(tTest-tActual,x2min),x2max);
response=interp1(q.x2,q.p2,t) > rand(1);

% Visualize if requested:
if (nargin >= 4) && (plotIt > 0)
    tc = t;
    col = {'*r', '*g'};
    
    t = min(max(q.intensity(1:q.trialCount) - tActual, x2min), x2max);
    
    if plotIt == 2
        positive = find(q.response(1:q.trialCount) > 0);
        negative = find(q.response(1:q.trialCount) <= 0);
        pcol = 'og';
    else
        positive = 1:q.trialCount;
        negative = [];
        pcol = 'ok';
    end
    
    plot(q.x2 + tActual, q.p2, 'b', ...
        t(positive) + tActual, interp1(q.x2,q.p2,t(positive)), pcol, ...
        t(negative) + tActual, interp1(q.x2,q.p2,t(negative)), 'or', ...
        tActual, interp1(q.x2 + tActual,q.p2,tActual), 'x', ...
        tc + tActual, interp1(q.x2,q.p2,tc), col{response + 1});
end
