function q=QuestRecompute(q, plotIt)
% q=QuestRecompute(q [,plotIt=0])
%
% Call this immediately after changing a parameter of the psychometric
% function. QuestRecompute uses the specified parameters in "q" to
% recompute the psychometric function. It then uses the newly computed
% psychometric function and the history in q.intensity and q.response
% to recompute the pdf. (QuestRecompute does nothing if q.updatePdf is
% false.)
%
% QuestCreate saves in struct q the parameters for a Weibull psychometric function:
% p2=delta*gamma+(1-delta)*(1-(1-gamma)*exp(-10.^(beta*(x-xThreshold))));
% where x represents log10 contrast relative to threshold. The Weibull
% function itself appears only in QuestRecompute, which uses the
% specified parameter values in q to compute a psychometric function
% and store it in q. All the other Quest functions simply use the
% psychometric function stored in "q". QuestRecompute is called solely
% by QuestCreate and QuestBetaAnalysis (and possibly by a few user
% programs). Thus, if you prefer to use a different kind of
% psychometric function, called Foo, you need only create your own
% QuestCreateFoo, QuestRecomputeFoo, and (if you need it)
% QuestBetaAnalysisFoo, based on QuestCreate, QuestRecompute, and
% QuestBetaAnalysis, and you can use them with the rest of the Quest
% package unchanged. You would only be changing a few lines of code,
% so it would quite easy to do.
% 
% "dim" is the number of distinct intensities that the internal tables in q can store,
% e.g. 500. This vector, of length "dim", with increment size "grain",
% will be centered on the initial guess tGuess, i.e.
% tGuess+[-range/2:grain:range/2]. QUEST assumes that intensities outside
% of this interval have zero prior probability, i.e. they are impossible
% values for threshold. The cost of making "dim" too big is some extra
% storage and computation, which are usually negligible. The cost of
% making "dim" too small is that you prejudicially exclude what are
% actually possible values for threshold. Getting out-of-range warnings
% from QuestUpdate is one possible indication that your stated range is
% too small.
% 
% If you set the optional parameter 'plotIt' to 1, the function will plot
% the psychometric function in use.
%
% See QuestCreate, QuestUpdate, QuestQuantile, QuestMean, QuestMode,
% QuestSd, and QuestSimulate.

% 4/29/99   dgp  Wrote it.
% 8/15/99   dgp  Explain how to use other kind of psychometric function.
% 9/11/04   dgp  Explain why supplied "dim" should err on the high side.
% 10/31/10   mk  Allocate q.intensity and q.response in chunks of 10000
%                trials to reduce memory fragmentation problems.
% 03/10/12   mk  Optionally plot psychometric function for debugging.
%                Also some Matlab M-Lint warning cleanup.

% Copyright (c) 1996-2004 Denis Pelli
if nargin < 1
	error('Usage: q=QuestRecompute(q [,plotIt=0])')
end

if length(q)>1
	for i=1:length(q(:))
		q(i).normalizePdf=0; % any norming must be done across the whole set of pdfs, because it's actually one big multi-dimensional pdf.
		q(i)=QuestRecompute(q(i));
	end
	return
end

if ~q.updatePdf
	return
end

if q.gamma>q.pThreshold
	warning(sprintf('reducing gamma from %.2f to 0.5',q.gamma)) %#ok<SPWRN>
	q.gamma=0.5;
end

% Don't visualize functions by default:
if nargin < 2 || isempty(plotIt)
    plotIt = 0;
end

% prepare all the arrays
q.i=-q.dim/2:q.dim/2;
q.x=q.i*q.grain;
q.pdf=exp(-0.5*(q.x/q.tGuessSd).^2);
q.pdf=q.pdf/sum(q.pdf);
i2=-q.dim:q.dim;
q.x2=i2*q.grain;
q.p2=q.delta*q.gamma+(1-q.delta)*(1-(1-q.gamma)*exp(-10.^(q.beta*q.x2)));

% Plot Psychometric function if requested:
if plotIt > 0
    figure;
    plot(q.x2, q.p2);
end

if q.p2(1)>=q.pThreshold || q.p2(end)<=q.pThreshold
	error(sprintf('psychometric function range [%.2f %.2f] omits %.2f threshold',q.p2(1),q.p2(end),q.pThreshold))
end
if any(~isfinite(q.p2))
	error('psychometric function p2 is not finite')
end
index=find(diff(q.p2)); 		% subset that is strictly monotonic
if length(index)<2
	error(sprintf('psychometric function has only %g strictly monotonic point(s)',length(index)))
end
q.xThreshold=interp1(q.p2(index),q.x2(index),q.pThreshold);
if ~isfinite(q.xThreshold)
	q %#ok<NOPRT>
	error(sprintf('psychometric function has no %.2f threshold',q.pThreshold))
end
q.p2=q.delta*q.gamma+(1-q.delta)*(1-(1-q.gamma)*exp(-10.^(q.beta*(q.x2+q.xThreshold))));
if any(~isfinite(q.p2))
	q %#ok<NOPRT>
	error('psychometric function p2 is not finite')
end
q.s2=fliplr([1-q.p2;q.p2]);
if ~isfield(q,'intensity') || ~isfield(q,'response')
    % Preallocate for 10000 trials, keep track of real useful content in
    % q.trialCount. We allocate such large chunks to reduce memory
    % fragmentation that would be caused by growing the arrays one element
    % per trial. Fragmentation has been shown to cause severe out-of-memory
    % problems if one runs many interleaved quests. 10000 trials require/
    % waste about 157 kB of memory, which is basically nothing for todays
    % computers and likely sufficient for even the most tortorous experiment
    % sessions.
    q.trialCount = 0;
    q.intensity=zeros(1,10000);
    q.response=zeros(1,10000);
end

if any(~isfinite(q.s2(:)))
	error('psychometric function s2 is not finite')
end

% Best quantileOrder depends only on min and max of psychometric function.
% For 2-interval forced choice, if pL=0.5 and pH=1 then best quantileOrder=0.60
% We write x*log(x+eps) in place of x*log(x) to get zero instead of NaN when x is zero.
pL=q.p2(1);
pH=q.p2(size(q.p2,2));
pE=pH*log(pH+eps)-pL*log(pL+eps)+(1-pH+eps)*log(1-pH+eps)-(1-pL+eps)*log(1-pL+eps);
pE=1/(1+exp(pE/(pL-pH)));
q.quantileOrder=(pE-pL)/(pH-pL);

if any(~isfinite(q.pdf))
	error('prior pdf is not finite')
end
% recompute the pdf from the historical record of trials
for k=1:q.trialCount
	inten=max(-1e10,min(1e10,q.intensity(k))); % make intensity finite
	ii=size(q.pdf,2)+q.i-round((inten-q.tGuess)/q.grain);
	if ii(1)<1
		ii=ii+1-ii(1);
	end
	if ii(end)>size(q.s2,2)
		ii=ii+size(q.s2,2)-ii(end);
	end
	q.pdf=q.pdf.*q.s2(q.response(k)+1,ii); % 4 ms
	if q.normalizePdf && mod(k,100)==0
		q.pdf=q.pdf/sum(q.pdf);	% avoid underflow; keep the pdf normalized	% 3 ms
	end
end
if q.normalizePdf
	q.pdf=q.pdf/sum(q.pdf);		% keep the pdf normalized	% 3 ms
end
if any(~isfinite(q.pdf))
	error('pdf is not finite')
end
